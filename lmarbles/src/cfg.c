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

char configDir[512]; /* config directory also used for profiles */
char c_pth[1024];
Cfg cfg;

/*
    get the full path of the cfg file
*/
void C_StPth()
{
	DIR *dir;
	/* build config dir by expanding home directory if needed,
	 * use c_pth as auxiliary var */
	snprintf(configDir, sizeof(configDir), CONFIGDIR);
	if (configDir[0] == '~')
		snprintf(c_pth, sizeof(c_pth), "%s/%s", getenv( "HOME" ), configDir+1);
	else
		snprintf(c_pth, sizeof(c_pth), "%s", configDir);
	snprintf(configDir, sizeof(configDir), c_pth);
	printf(_("config directory: %s\n"), configDir);
	/* create directory if not found */
	if ((dir = opendir(configDir)) == 0) {
		fprintf(stderr, _("  not found, creating it\n"));
		MKDIR(configDir, S_IRWXU);
	} else {
		closedir(dir);
	}
	/* get full path of config file */
	snprintf(c_pth, sizeof(c_pth), "%s/lmarbles.conf", configDir);
}

/*
    load it
*/
void C_Ld()
{
    char str[256];
    FILE	*f;
    struct stat dir_stat;

    printf(_("loading configuration...\n"));

    // load init //
    if ((f = fopen(c_pth, "r")) == 0) {
        printf(_("cfg file '%s' not found; using defaults\n"), c_pth);
        C_Def();
	}
    else {
        stat( c_pth, &dir_stat );
        if ( dir_stat.st_size != sizeof( Cfg ) ) {
            fprintf( stderr, _("config file '%s' corrupted... using defaults\n"), c_pth );
            C_Def();
        }
        else
            
        {

        fileGetEntry(f, str, F_VAL);
        if (strncmp(str,"ascii",5)) {

            printf(_("cfg file '%s' not in ascii; using defaults\n"), c_pth);
            C_Def();

        }
        else {

            fileGetEntry(f, cfg.prf_nm, F_VAL); cfg.prf_nm[strlen(cfg.prf_nm) - 1] = 0;
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.prf);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.s_vol);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.s_on);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.ani);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.trp);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.fscr);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.dim);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.diff);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.k_up);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.k_down);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.k_left);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.k_right);
            fileGetEntry(f, str, F_VAL); F_ValToInt(str, &cfg.k_undo);

        }
        }
        
		fclose(f);
	}
}

/*
    save it
*/
void C_Sv()
{
    //save init //
    FILE	*f = fopen(c_pth, "w");
    char    str[256];

    fileWriteEntry(f, "ascii");
    fileWriteEntry(f, cfg.prf_nm);
    F_IntToStr(str, cfg.prf); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.s_vol); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.s_on); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.ani); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.trp); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.fscr); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.dim); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.diff); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.k_up); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.k_down); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.k_left); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.k_right); fileWriteEntry(f, str);
    F_IntToStr(str, cfg.k_undo); fileWriteEntry(f, str);

    fclose(f);
}

/*
    default values
*/
void C_Def()
{
    strcpy(cfg.prf_nm, "Michael");
    cfg.prf = 0;
    // sound //
    cfg.s_vol = 6;
    cfg.s_on = 1;
    // gfx //
    cfg.ani = 1;
    cfg.trp = 1;
    cfg.fscr = 0;
    cfg.dim = 1;
    cfg.diff = DIFF_NORMAL;
    // controls
    cfg.k_up = SDLK_UP;
    cfg.k_down = SDLK_DOWN;
    cfg.k_left = SDLK_LEFT;
    cfg.k_right = SDLK_RIGHT;
    cfg.k_undo = SDLK_SPACE;
}
