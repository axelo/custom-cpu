#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv) {

    if (argc < 2) {
        fprintf(stderr, "Missing program file path\n");
        return 1;
    }

    const char *filepath = argv[1];

    FILE *file = fopen(filepath, "r");

    if (file == NULL) {
        fprintf(stderr, "Failed to open program\n");
        return 1;
    }

    uint8_t program[0x1000] = {0};

    size_t read_bytes = fread(program + 2, sizeof(program[0]), sizeof(program) - 2, file);
    fclose(file);

    int64_t sum_of_bytes = 0;

    for (size_t i = 0; i < read_bytes; ++i) {
        sum_of_bytes += program[i];
    }

    uint8_t checksum = (uint8_t)((~sum_of_bytes) + 1);

    printf("read %zd bytes, sum of bytes is 0x%02llx, checksum: 0x%02x\n", read_bytes, sum_of_bytes,checksum);

    char output_path[0x100] = {0};

    snprintf(output_path, sizeof(output_path), "%s.sized", filepath);

    FILE *sized_file = fopen(output_path, "w");

    if (sized_file == NULL) {
        fprintf(stderr, "Failed to create %s\n", output_path);
        return 1;
    }

    //program[0] = 0xfa;
    //program[1] = checksum;
    program[0] = (uint8_t)(read_bytes >> 8);
    program[1] = read_bytes & 0xff;

    size_t wrote_bytes = fwrite(program, read_bytes + 2, sizeof(program[0]), sized_file);

    if (wrote_bytes != 1) {
        fprintf(stderr, "Failed to write program with size and checksum\n");
    } else {
        printf("Wrote %s\n", output_path);
    }

    fclose(sized_file);

    return 0;
}
