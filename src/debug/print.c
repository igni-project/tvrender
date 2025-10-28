#include "print.h"

#include <stdio.h>

int (*db_print_vb)(const char *restrict format, ...) = &db_print_none;

void db_verbose_enable()
{
	db_print_vb = &printf;
}

int db_print_none(const char *restrict format, ...)
{
	return 0;
}

