#ifndef LOG_H
#define LOG_H

/* #include <stdio.h> */
#include "console.h"

#ifdef LOGS

/* #define PINFO(fmt, ...) fprintf(stdout, "(%s): " fmt, __func__, ##__VA_ARGS__) */
#define PINFO(fmt, ...) console_printf("(%s): " fmt, __func__, ##__VA_ARGS__)

#else

#define PINFO(fmt, ...) ((void)0)

#endif

/* #define PERROR(fmt, ...) fprintf(stderr, "%s:%d(%s): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__) */
#define PERROR(fmt, ...) console_printf("%s:%d(%s): " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif
