#include "rflag.h"

/* A newly created window without any draw calls is rather unsightly, so the
 * redraw flag is set to 1. Start the window out right with a solid backround
 * or what-have-you. */
int redraw_flag = 1;

void flag_redraw()
{
	redraw_flag = 1;
}

void unflag_redraw()
{
	redraw_flag = 0;
}

int needs_redraw()
{
	return redraw_flag;
}

