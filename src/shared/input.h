#ifndef INPUT_H

#define INPUT_H

#include <stdint.h>

typedef union {
    struct {
        uint64_t left : 1, right : 1, punch : 1, kick : 1, unused : 60;
    } __attribute__((packed)) fields;

    uint64_t raw;
} Input;

typedef struct __attribute__((packed)) {
    uint64_t frame;
    Input input;
} TimedInput;

#endif /* INPUT_H */
