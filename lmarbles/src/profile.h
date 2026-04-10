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

#include "tools.h"
#include "levels.h"
#include "dynlist.h"

typedef struct {
    char nm[MAXSTRLEN];
    int num;
    int l_num; // levels per chapter //
    int c_num;
    int c_opn[MAX_CHPTRS];
    int cmp[MAX_LVLS * MAX_CHPTRS];
} SInf;

typedef struct {
    char    nm[MAXSTRLEN];
    int     lvls; // number of levels cleared
    int     scr; // total score gained
    float   pct; // percentage of time needed to complete a level
    DLst    sts;
} Profile;

void Profile_Ini();
int Profile_Ld();
void Profile_Sv();
void Profile_Reset();
SInf* Profile_RegLS(LSet *l_st);
void Profile_Upd(Profile *p, SInf *inf, int l_id, float pct, int scr);

#endif
