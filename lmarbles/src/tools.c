/***************************************************************************
                          tools.c  -  description
                             -------------------
    begin                : 2026/04/05
    copyright            : (C) by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/time.h>
#include <time.h>
#include "tools.h"
#include "sdl.h"

int t_c, t_lc;

/*
    get milliseconds since last call
*/
int T_Gt()
{
    int ms;
    t_c = SDL_GetTicks();
    ms = t_c - t_lc;
    t_lc = t_c;
    if (ms == 0) {
        ms = 1;
        SDL_Delay(1);
    }
    return ms;
}

/*
    reset timer
*/
void T_Rst()
{
    t_lc = SDL_GetTicks();
}

/** Return static string using stars for rating (0 to 5).
 * For an illegal rating an empty string is returned. */
char *getRatingStr(int rating)
{
	static char str[6];

	if (rating < 0 || rating > 5) {
		str[0] = 0;
		return str;
	}

	for (int i = 0; i < 5; i++)
		str[i] = '`'; /* unlit star */
	str[5] = 0;

	for (int i = 1; i <= rating; i++)
		str[i-1] = 127; /* star */

	return str;
}
