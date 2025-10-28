#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H 1

extern int (*db_print_vb)(const char *restrict format, ...);

void db_verbose_enable();
int db_print_none(const char *restrict format, ...);

#endif

