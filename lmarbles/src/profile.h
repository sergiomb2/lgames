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

#include "levels.h"
#include "dynlist.h"

typedef struct {
    char    nm[32];
    int     num;
    int     l_num; // levels per chapter //
    int     c_num;
    char    c_opn[MAX_CHPTRS];
    char    cmp[MAX_LVLS * MAX_CHPTRS];
} SInf;

typedef struct {
    char    nm[12];
    int     lvls;
    int     scr; // total score gained
    float   pct; // percentage of time needed to complete a level
    DLst    sts;
} Profile;

void Profile_Ini();
void Profile_Trm();
int Profile_Ld();
void Profile_Sv();
void Profile_Crt(char *nm);
void Profile_Del(void *p);
SInf* Profile_RegLS(Profile *p, LSet *l_st);
void Profile_CrtLst();
void Profile_DelLst();
void Profile_Srt();
void Profile_Upd(Profile *p, SInf *inf, int l_id, float pct, int scr);

#endif
