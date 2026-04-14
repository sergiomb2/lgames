/***************************************************************************
                          profile.h  -  description
                             -------------------
    begin                : Sun Sep 17 2000
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

#ifndef __PROFILE_H
#define __PROFILE_H

#include "cfg.h"
#include "tools.h"
#include "levels.h"
#include "dynlist.h"

typedef struct {
    char name[MAXSTRLEN];
    int score[LT_COUNT]; /* total score gained for this set */
    int numLevels; /* total number of levels */
    int numChapters; /* number of chapters */
    int chapterSize; /* levels per chapter */
    int chapterOpen[LT_COUNT][MAXCHAPTERS];
    int completed[LT_COUNT][MAXCHAPTERS * MAXCHAPTERSIZE];
} SInf;

typedef struct {
    char    name[MAXSTRLEN];
    DLst    sts;
} Profile;

void profileInit();
int profileLoad();
void profileSave();
void profileReset();
SInf* profileRegisterSet(LSet *l_st);
void profileUpdate(SInf *inf, int lvl, int scr);

#endif
