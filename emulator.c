#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>  // sysconf
#include <sys/times.h> // times

#include <netinet/in.h> // socket

#include "emulate.h"

static void print_state(State *state) {
    printf(" o: %02x\n", state->o);
    printf(" s: %02x\n", state->s);
    printf(" t: %02x\n", state->t);
    printf("ml: %02x\n", state->ml);
    printf("mh: %02x\n", state->mh);
    printf(" f: %x\n", state->f);
    printf(" c: %x\n", state->c);

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

int main(void) {
    uint8_t control[CONTROL_ROM_SIZE];
    uint8_t alu[ALU_ROM_SIZE];

    if (!read_rom("./custom-cpu_control.bin", CONTROL_ROM_SIZE, control) ||
        !read_rom("./custom-cpu_alu.bin",     ALU_ROM_SIZE,     alu)) return 1;

    State state = {0};

    size_t cycles = 0;

    printf("running init\n");

    for (; !(state.f & F_I); ++cycles)
        emulate_next_cycle(false, control, alu, &state);

    printf("init done after %zd cycles\n", cycles);

    print_state(&state);

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

    uint8_t recv_buffer[2] = {0};
    uint8_t send_buffer[2] = {0};

    size_t max_cycles = 10000000;

    for (;;) {
        clock_t start = times(&tms);

        bool print_next_instruction = true;

        for (cycles = 0; cycles < max_cycles; ++cycles) {
            // if ((cycles & 63) == 63) usleep(4);
            if ((cycles & 127) == 127) usleep(9);

            if (print_next_instruction) {
                print_next_instruction = false;

                // printf("%04x: \n", (state.mh << 8) | state.ml);
                // usleep(1000 * 100);
            }

            bool instr_done = emulate_next_cycle(false, control, alu, &state);

            if (instr_done) {
                print_next_instruction = true;

                if (state.tx_bits == 9) {
                    // printf("sending '%c'\n", state.tx);
                    send_buffer[0] = state.tx;
                    send(clientfd, send_buffer, 1, 0);

                    state.tx_bits = 0;
                }

                if (state.rx_bits == 0 && state.mem[(uint16_t)(state.mh << 8) | state.ml] == 0x04) {

                    ssize_t bytes_read = recv(clientfd, recv_buffer, 1, MSG_PEEK | MSG_DONTWAIT);

                    if (bytes_read == 0) {
                        printf("disconnected\n");
                        exit(1);
                    }
                    else if (bytes_read < -1) {
                        perror("recv failed");
                        exit(1);
                    } else if (bytes_read > 0) {

                        // printf("bytes_read: %ld\n", bytes_read);

                        bytes_read = recv(clientfd, recv_buffer, 1, MSG_DONTWAIT);

                        if (bytes_read < 1) {
                            fprintf(stderr, "expected bytes from recv, got %ld\n", bytes_read);
                            exit(1);
                        }

                        state.rx = recv_buffer[0];
                        state.rx_bits = 1;

                        // printf("recv: '%c'\n", state.rx);
                    }
                }
            }
        }

        clock_t end = times(&tms);
        printf("elapsed ticks: %lu, elapsed seconds: %5.2f s\n", end - start, (double)(end - start) / freq);
    }

    printf("done\n");

    printf("closing Serial connection\n");
    shutdown(listenfd, 2);

    return 0;
}
