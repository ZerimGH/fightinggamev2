#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define INDENT "    "

int file2c(char *path, int header) {
    /* Get the name of the array */
    char *name = path;
    char *slash = strchr(path, '/');
    while (slash) {
        name = slash + 1;
        slash = strchr(name, '/');
    }

    FILE *file = fopen(path, "rb");
    if (!file) return 1;

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size == 0) {
        fclose(file);
        return 1; /* Can't define an empty array in C */
    }

    /* Print full file name as a comment */
    printf("/* %s */\n", path);
    /* Print array declaration */
    /* Declare as extern if header */
    if (header) printf("extern ");
    printf("unsigned char ");
    while (*name) {
        if (isalnum(*name)) putchar(*name);
        else
            putchar('_');
        name++;
    }
    printf("[%zu]", size);

    /* Print semicolon and exit early if printing the header */
    if (header) {
        putchar(';');
        putchar('\n');
        fclose(file);
        return 0;
    }

    /* Print file contents as array */
    printf(" = {\n%s", INDENT);
    unsigned char c;
    for (size_t i = 0; i < size; i++) {
        if (!fread(&c, sizeof(unsigned char), 1, file)) {
            fclose(file);
            return 1; /* Read failed */
        }
        /* Print character as a hex byte */
        printf("0x%02x%s", c, i == size - 1 ? "" : ", ");
        /* Print a newline every ~80 characters */
        if (i != size - 1) {
            if (i % 13 == 12) printf("\n%s", INDENT);
        } else {
            putchar('\n');
        }
    }
    printf("};\n");
    fclose(file);

    return 0;
}

void print_help(char *exec_name) {
    printf("Usage: %s [options] [file1, file2...]\n", exec_name);
    printf("Options:\n");
    printf("-h, --header        Output header instead of raw bytes\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_help(argv[0]);
        return 0;
    }

    int header = 0;

    for (size_t i = 1; i < (size_t)argc; i++) {
        /* Parse arguments */
        if (argv[i][0] == '-') {
            if (argv[i][1] == '-') {
                /* Full word argument */
                char *scan = argv[i] + 2;
                if (strcmp(scan, "header") == 0) header = 1;
                else if (strcmp(scan, "help") == 0) {
                    print_help(argv[0]);
                    return 0;
                } else {
                    fprintf(stderr, "Unrecognised option %s\n", argv[i]);
                    return 1;
                }
            } else {
                /* One or more single character flags */
                char *scan = argv[i] + 1;
                while (*scan) {
                    char c = *scan;
                    switch (c) {
                        case 'h': header = 1; break;
                        default: fprintf(stderr, "Unrecognised flag -%c\n", c); return 1;
                    }
                    scan++;
                }
            }
        }
    }

    for (size_t i = 1; i < (size_t)argc; i++) {
        /* Process files */
        if (argv[i][0] == '-') continue;
        if (file2c(argv[i], header)) {
            fprintf(stderr, "Failed to process file %s\n", argv[i]);
            return 1;
        }
    }

    return 0;
}
