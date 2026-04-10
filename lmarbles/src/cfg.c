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
	FILE	*f;

	configSetDefaults();

	printf(_("loading configuration %s\n"), configPath);

	if ((f = fopen(configPath, "r")) == 0) {
		printf(_("config file not found, using defaults\n"));
		return;
	}

	fileReadString(f, "profile", config.prf_nm);
	fileReadInt(f, "profileid", &config.prf);
	fileReadInt(f, "sound", &config.sound);
	fileReadInt(f, "volume", &config.volume);
	fileReadInt(f, "animations", &config.animations);
	fileReadInt(f, "fullscreen", &config.fullscreen);
	fileReadInt(f, "dim", &config.dim);
	fileReadInt(f, "diff", &config.diff);
	fileReadInt(f, "k_up", &config.k_up);
	fileReadInt(f, "k_down", &config.k_down);
	fileReadInt(f, "k_left", &config.k_left);
	fileReadInt(f, "k_right", &config.k_right);
	fileReadInt(f, "k_undo", &config.k_undo);

	fclose(f);
}

void configSave()
{
    FILE *f = fopen(configPath, "w");

    if (f == NULL) {
	    _logerr("no write access to config file %s\n",configPath);
	    return;
    }

    fprintf(f, "profile = %s;\n", config.prf_nm);
    fprintf(f, "profileid = %d;\n", config.prf);
    fprintf(f, "sound = %d;\n", config.sound);
    fprintf(f, "volume = %d;\n", config.volume);
    fprintf(f, "animations = %d;\n", config.animations);
    fprintf(f, "fullscreen = %d;\n", config.fullscreen);
    fprintf(f, "dim = %d;\n", config.dim);
    fprintf(f, "diff = %d;\n", config.diff);
    fprintf(f, "k_up = %d;\n", config.k_up);
    fprintf(f, "k_down = %d;\n", config.k_down);
    fprintf(f, "k_left = %d;\n", config.k_left);
    fprintf(f, "k_right = %d;\n", config.k_right);
    fprintf(f, "k_undo = %d;\n", config.k_undo);

    fclose(f);
}

/*
    default values
*/
void configSetDefaults()
{
    strcpy(config.prf_nm, "Michael");
    config.prf = 0;
    // sound //
    config.volume = 6;
    config.sound = 1;
    // gfx //
    config.animations = 1;
    config.fullscreen = 0;
    config.dim = 1;
    config.diff = DIFF_NORMAL;
    // controls
    config.k_up = SDLK_UP;
    config.k_down = SDLK_DOWN;
    config.k_left = SDLK_LEFT;
    config.k_right = SDLK_RIGHT;
    config.k_undo = SDLK_SPACE;
}
