#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h> // memcpy
#include <unistd.h>  // sysconf
#include <sys/times.h> // times
#include <netinet/in.h> // socket

#include "emulate.h"

#define PROGRAM_START 0x1000
#define PROGRAM_SIZE (0x10000 - PROGRAM_START)

static void print_state(State *state) {
    printf(" o: %02x\n", state->o);
    printf(" s: %02x\n", state->s);
    printf(" t: %02x\n", state->t);
    printf("ml: %02x\n", state->ml);
    printf("mh: %02x\n", state->mh);
    printf(" f: %x\n", state->f);
    printf("zf: %x\n", state->f & F_Z ? 1 : 0);
    printf("cf: %x\n", state->f & F_C ? 1 : 0);
    printf(" c: %x\n", state->c);
    printf("ra: %02x\n", state->mem[0xfff0 | 0]);
    printf("rb: %02x\n", state->mem[0xfff0 | 1]);
    printf("rc: %02x\n", state->mem[0xfff0 | 2]);
    printf("rd: %02x\n", state->mem[0xfff0 | 3]);
    printf("re: %02x\n", state->mem[0xfff0 | 4]);
    printf("rt: %02x\n", state->mem[0xfff0 | 5]);

    printf("\n");

    /*for (int i = 0; i < 0x1000; ++i) {
        printf("%02x ", state.mem[i]);
        if ((i & 15) == 15) printf("\n");
    }
    printf("\n");*/
}

static bool read_rom(const char *filepath, size_t rom_size, uint8_t rom[rom_size]) {
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        fprintf(stderr, "Failed to open rom %s\n", filepath);
        return false;
    }

    size_t read_bytes = fread(rom, sizeof(rom[0]), rom_size, file);
    fclose(file);

    if (read_bytes != rom_size) {
        fprintf(stderr, "Only read %zd byte out of expected %zd bytes\n", rom_size, read_bytes);
        return false;
    }

    return true;
}

static size_t read_program(const char *filepath, uint8_t program[PROGRAM_SIZE]) {
    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        fprintf(stderr, "Failed to open program %s\n", filepath);
        return 0;
    }

    size_t read_bytes = fread(program, sizeof(program[0]), PROGRAM_SIZE, file);
    fclose(file);

    return read_bytes;
}

int main(int argc, char **argv) {
    uint8_t control[CONTROL_ROM_SIZE];
    uint8_t alu[ALU_ROM_SIZE];

    if (!read_rom("./build/custom-cpu_control.bin", CONTROL_ROM_SIZE, control) ||
        !read_rom("./build/custom-cpu_alu.bin",     ALU_ROM_SIZE,     alu)) return 1;

    State state = {0};

    printf("running init\n");

    size_t cycles = 0;
    for (; !(state.f & F_I); ++cycles)
        emulate_next_cycle(false, control, alu, &state);

    printf("init done after %zd cycles\n", cycles);

    print_state(&state);

    if (argc > 1) {
        uint8_t program[PROGRAM_SIZE];
        size_t program_size = read_program(argv[1], program);

        if (program_size == 0) return 1;

        // jmp {i:i16} => 0x1c @ i;
        state.mem[0] = 0x1c;
        state.mem[1] = PROGRAM_START >> 8;
        state.mem[2] = PROGRAM_START & 0xff;

        memcpy(state.mem + PROGRAM_START, program, program_size);

        printf("boot program skipped, running %s (%ld) directly\n", argv[1], program_size);
    }

    struct tms tms;
    double freq = (double)sysconf(_SC_CLK_TCK);

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // accept connections from any network interface
    serv_addr.sin_port = htons(2323); // port

    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP

    const int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if (listenfd < 0) {
        perror("socket failed");
        exit(1);
    }

    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    if (listen(listenfd, 1) < 0) {
        perror("listen failed");
        exit(1);
    }

    struct sockaddr_in client_addr = {0};
    socklen_t client_socklen = sizeof(client_addr);

    printf("waiting for Serial connection\n");

    int clientfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_socklen);

    if (clientfd < 0) {
        perror("accept failed\n");
        exit(1);
    }

    printf("starting emulation\n");

    size_t max_cycles = 10000000;
    uint8_t recv_byte;

    for (;;) {
        clock_t start = times(&tms);

        for (cycles = 0; cycles < max_cycles; ++cycles) {
            // if ((cycles & 63) == 63) usleep(4);
            if ((cycles & 127) == 127) usleep(8);

            bool instr_done = emulate_next_cycle(false, control, alu, &state);

            if (instr_done) {
                if (state.o == 0xfe) {
                    print_state(&state);
                    getchar();
                }

                if (state.tx_bits == 9) {
                    // printf("sending '%c'\n", state.tx);

                    send(clientfd, &state.tx, 1, 0);

                    state.tx_bits = 0;
                }

                if (state.rx_bits == 0 && state.mem[(uint16_t)(state.mh << 8) | state.ml] == 0x04) {

                    ssize_t bytes_read = recv(clientfd, &recv_byte, 1, MSG_PEEK | MSG_DONTWAIT);

                    if (bytes_read > 0) {
                        bytes_read = recv(clientfd, &recv_byte, 1, MSG_DONTWAIT);

                        if (bytes_read < 1) {
                            fprintf(stderr, "expected bytes from recv, got %ld\n", bytes_read);
                            goto done;
                        }

                        state.rx = recv_byte;
                        state.rx_bits = 1;

                        // printf("recv: '%c'\n", state.rx);
                    }
                }
            }
        }

        clock_t end = times(&tms);

        if (recv(clientfd, &recv_byte, 1, MSG_PEEK | MSG_DONTWAIT) == 0) {
            perror("disconnected");
            break;
        }

        if (false) printf("elapsed ticks: %lu, elapsed seconds: %5.2f s\n", end - start, (double)(end - start) / freq);
    }
done:
    printf("done\n");

    printf("closing Serial connection\n");
    shutdown(listenfd, 2);

    return 0;
}
