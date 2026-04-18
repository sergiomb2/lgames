/***************************************************************************
                          tools.h  -  description
                             -------------------
    begin                : 2026/04/05
    copyright            : (C) Michael Speck
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

#ifndef __TOOLS_H
#define __TOOLS_H

/* i18n */
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif
#include "gettext.h"
#if ENABLE_NLS
#define _(str) gettext (str)
#else
#define _(str) (str)
#endif

#define MAXSTRLEN 1024

#define DEBUGLEVEL 0
#define _logerr(...) do { \
		fprintf(stderr,"ERROR: %s:%d: %s(): ", __FILE__, __LINE__, __FUNCTION__); \
		fprintf (stderr, __VA_ARGS__); \
	} while (0)
#define _loginfo(...) fprintf (stdout, __VA_ARGS__)
#define _logdebug(level,...) \
	if (level <= DEBUGLEVEL) \
		fprintf (stderr, __VA_ARGS__);

int T_Gt();
void T_Rst();

char *getRatingStr(int rating);

#endif
