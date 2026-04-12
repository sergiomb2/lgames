/***************************************************************************
                          cfg.c  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000 by Michael Speck
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

#include "cfg.h"
#include "tools.h"
#include "sdl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef _WIN32
 #define MKDIR(d, p) mkdir(d)
#else
 #define MKDIR mkdir
#endif

char configDir[MAXSTRLEN/2]; /* config directory also used for profiles */
char configPath[MAXSTRLEN];
Config config;

/* Set configDir (directory for config, create if needed) and
 * configPath (including file name)
 */
void configSetPath()
{
	DIR *dir;
	char aux[MAXSTRLEN/2];

	/* build config dir by expanding home directory if needed */
	snprintf(aux, sizeof(aux), "%s", CONFIGDIR);
	if (aux[0] == '~')
		snprintf(configDir, sizeof(configDir), "%s/%s", getenv( "HOME" ), aux+1);
	else
		snprintf(configDir, sizeof(configDir), "%s", aux);
	printf(_("config directory: %s\n"), configDir);

	/* create directory if not found */
	if ((dir = opendir(configDir)) == 0) {
		fprintf(stderr, _("  not found, creating it\n"));
		MKDIR(configDir, S_IRWXU);
	} else {
		closedir(dir);
	}

	/* get full path of config file */
	snprintf(configPath, sizeof(configPath), "%s/lmarbles.conf", configDir);
}

void configLoad()
{
	FILE	*fh;

	configSetDefaults();

	printf(_("loading configuration %s\n"), configPath);

	if ((fh = fileOpen(configPath,"r")) == NULL)
		return;

	if (!fileReadInt(fh, "sound", &config.sound)) {
		_logerr("old or corrupted config, using defaults\n");
		fileClose(fh);
		return;
	}
	fileReadInt(fh, "volume", &config.volume);
	fileReadInt(fh, "animations", &config.animations);
	fileReadInt(fh, "fullscreen", &config.fullscreen);
	fileReadInt(fh, "dim", &config.dim);
	fileReadInt(fh, "k_up", &config.k_up);
	fileReadInt(fh, "k_down", &config.k_down);
	fileReadInt(fh, "k_left", &config.k_left);
	fileReadInt(fh, "k_right", &config.k_right);
	fileReadInt(fh, "k_undo", &config.k_undo);
	fileReadInt(fh, "limittype", &config.limitType);

	fileClose(fh);
}

void configSave()
{
	FILE *fh = fileOpen(configPath, "w");

	if (fh == NULL)
		return;

	fprintf(fh, "sound = %d;\n", config.sound);
	fprintf(fh, "volume = %d;\n", config.volume);
	fprintf(fh, "animations = %d;\n", config.animations);
	fprintf(fh, "fullscreen = %d;\n", config.fullscreen);
	fprintf(fh, "dim = %d;\n", config.dim);
	fprintf(fh, "k_up = %d;\n", config.k_up);
	fprintf(fh, "k_down = %d;\n", config.k_down);
	fprintf(fh, "k_left = %d;\n", config.k_left);
	fprintf(fh, "k_right = %d;\n", config.k_right);
	fprintf(fh, "k_undo = %d;\n", config.k_undo);
	fprintf(fh, "limittype = %d;\n", config.limitType);

	fileClose(fh);
}

void configSetDefaults()
{
	// game
	config.limitType = LT_TIME;
	// sound
	config.volume = 6;
	config.sound = 1;
	// gfx
	config.animations = 1;
	config.fullscreen = 0;
	config.dim = 1;
	// controls
	config.k_up = SDLK_UP;
	config.k_down = SDLK_DOWN;
	config.k_left = SDLK_LEFT;
	config.k_right = SDLK_RIGHT;
	config.k_undo = SDLK_SPACE;
}
