#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <unistd.h>

#define PORT 2323

#define TX_CLOCKS 13
#define RX_CLOCKS 13

typedef enum {
    WAITING_FOR_TX_START_BIT,
    SAMPLING_TX,
    WAITING_FOR_TX_STOP_BIT,
} State;

static struct termios orig_termios;

static int socket_fd = 0;

static int nfailed = -1;

#define SIZE (64*1024)
static char input_buffer[SIZE];

static void restore_termios(void) {
    printf("restoring terminal\n");
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    if (socket_fd) {
        printf("closing socket\n");
        close(socket_fd);
    }

    printf("nfailed: %d\n", nfailed);
}

static void enable_terminal_raw_mod(void) {
    setbuf(stdout, NULL);

    if (tcgetattr(STDIN_FILENO, &orig_termios) < 0) {
        perror("tcgetattr()");
        exit(1);
    }

    atexit(restore_termios);

    struct termios raw = orig_termios;

    raw.c_iflag &= (tcflag_t) ~(BRKINT | /*ICRNL |*/ INPCK | ISTRIP | IXON);
    // raw.c_oflag &= (tcflag_t) ~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= (tcflag_t) ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0; // min bytes of input before read() can return
    raw.c_cc[VTIME] = 0; // number of 100 milliseconds

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        perror("tcsetattr()");
        exit(1);
    }
}

static void send_char(char c) {
    uint32_t out_c = (uint16_t)(0x1000 | ((uint16_t)c << 1) | 0);

    uint8_t buf[RX_CLOCKS];

    uint8_t out_c_bit = 0;

    // start bit
    out_c_bit = 0;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS - 1, 0); // -1 is the key here.

    out_c_bit = (out_c >> 1) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS - 1, 0);

    out_c_bit = (out_c >> 2) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS - 1, 0);

    out_c_bit = (out_c >> 3) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS - 1, 0);

    out_c_bit = (out_c >> 4) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS, 0);

    out_c_bit = (out_c >> 5) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS, 0);

    out_c_bit = (out_c >> 6) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS , 0);

    out_c_bit = (out_c >> 7) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS, 0);

    out_c_bit = (out_c >> 8) & 1;
    for (int j = 0; j < RX_CLOCKS; ++j) {
        buf[j] = out_c_bit;
    }
    send(socket_fd, &buf, RX_CLOCKS, 0);

    // stop bit
}

int main(void) {
    enable_terminal_raw_mod();

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        perror("socket()");
        return 1;
    }

    // socket read timeout
    struct timeval tv = {
        .tv_sec = 2,
        .tv_usec = 0
    };

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
        perror("setsockopt()");
        return 1;
    }

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    servaddr.sin_port = htons(PORT);

    if (connect(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("connect()");
        return 1;
    }

    printf("connected to the Digital TCP serve\n");

    State state = WAITING_FOR_TX_START_BIT;
    int n_clocks = 0;
    int n_bits = 0;

    uint8_t tx = 0;
    uint8_t gpo = 0;

    bool rts_enabled = false;
    int rts_clocks = 0;

    int nbuffer = 0;
    int sbuffer = 0;

    uint8_t tx_bit = 1;
    uint8_t prev_tx_bit = 1;

    int c_clocks = 0;

    while (true) {
        char input_c = '\0';

        if (read(STDIN_FILENO, &input_c, 1) < 0) {
            perror("read()");
            return 1;
        }

        if (input_c) {
            // if (iscntrl(input_c)) {
            //   printf("%\n", input_c);
            // } else {
            //   printf("%d ('%c')\n", input_c, input_c);
            // }

            if (input_c == ('q' & 0x1f)) exit(0); // ctrl + q

            input_buffer[nbuffer] = input_c;
            if (++nbuffer >= SIZE) nbuffer = 0;
        }

        ++n_clocks;
        ++rts_clocks;

        ssize_t socket_nread = read(socket_fd, &gpo, 1);

        if (socket_nread != 1) {
            printf("\nsocket probably closed\n");
            return 1;
        }

        prev_tx_bit = tx_bit;

        if (rts_enabled) {
            // printf("RTS disabled!\n");
            if (sbuffer != nbuffer) {
                char c_send = input_buffer[sbuffer];

                // printf(" RTS enabled, sending '%c'\n", c_send);
                send_char(c_send);

                rts_clocks = 0;
                if (++sbuffer >= SIZE) sbuffer = 0;
            }

            rts_enabled = false;
        }

        if (socket_nread > 0) {
            tx_bit = gpo & 1;

            uint8_t rts_bit = (~gpo) & 2;

            if (rts_bit && !rts_enabled && rts_clocks > (RX_CLOCKS + 4)) {
                rts_enabled = true;
            }
        }

        if (prev_tx_bit != tx_bit) {
            // printf("  tx bit changed from '%d' to '%d' at clock - took %d - clocks %d\n", prev_tx_bit, tx_bit, n_clocks, c_clocks);
            c_clocks = 0;
        } else {
            ++c_clocks;
        }

        switch (state) {
        case WAITING_FOR_TX_START_BIT: {
            if (tx_bit == 0) {
                // printf("got start bit, %d\n", n_clocks);
                state = SAMPLING_TX;
                n_clocks = -1;
                n_bits = 0;
            }
        } break;

        case SAMPLING_TX: {
            if (n_clocks >= TX_CLOCKS) {
                // printf("%d sampling tx bit %d (%d)\n", n_bits, tx_bit, n_clocks);

                tx >>= 1;
                tx |= (tx_bit << 7);

                n_clocks = 0;

                if (++n_bits >= 8) {
                    if (tx > 0) {
                        fputc(tx, stdout);

                        // if (tx != 'U') ++nfailed;
                    }

                    if (nfailed < 0) nfailed = 0;

                    state = WAITING_FOR_TX_STOP_BIT;
                }
            }
        } break;

        case WAITING_FOR_TX_STOP_BIT:
            if (n_clocks >= TX_CLOCKS) {
                // printf("\ngot stop bit '%d' - %d\n", tx_bit, n_clocks);
                state = WAITING_FOR_TX_START_BIT;
            }

            break;
        }

        if (nfailed > 1) {
            printf("nope :/\n");
            exit(3);
        }
    }

    return 0;
}
