/***************************************************************************
                          profile.c  -  description
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

#include "tools.h"
#include "profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg.h"
#include "file.h"

/* only one profile for user */
Profile profile;

/* profile path */
char prf_pth[MAXSTRLEN];

/* configuration */
extern char configDir[MAXSTRLEN/2];
extern Config config;

/* initialize source path */
void profileInit()
{
    snprintf(prf_pth, MAXSTRLEN, "%s/lmarbles.prf", configDir);
}

/* load profile; return 0 on error (and create standard profile), 1 otherwise */
int profileLoad()
{
	FILE    *fh = 0;
	char    str[MAXSTRLEN];
	int     i;

	profileReset(); /* default fallback */

	_loginfo(_("loading profile...\n"));

	/* open file */
	if ((fh = fopen(prf_pth, "r")) == 0) {
		_logerr(_("file %s does not exist\n"), prf_pth);
		return 0;
	}

	/* read profile */
	fileReadString(fh, "name", profile.nm);
	fileReadInt(fh, "levels", &profile.lvls);
	fileReadInt(fh, "score", &profile.scr);
	profile.pct = 0; /* TODO remove or properly read */
	while (fileReadString(fh, "setname", str)) {
		SInf *st = calloc(1,sizeof(SInf));
		snprintf(st->nm, MAXSTRLEN, "%s", str);
		fileReadInt(fh, "num", &st->num);
		fileReadInt(fh, "l_num", &st->l_num);
		fileReadInt(fh, "c_num", &st->c_num);
		for (i = 0; i < st->c_num; i++)
			fileReadInt(fh, "c_open", &st->c_opn[i]);
		for (i = 0; i < st->num; i++)
			fileReadInt(fh, "cmp", &st->cmp[i]);
		DL_Add(&profile.sts, st);
	}

	fclose(fh);

	return 1;
}

/* save profiles */
void profileSave()
{
	DL_E *le = 0;
	SInf *st;
	FILE *fh;
	int i;

	_loginfo(_("saving profile...\n"));

	if ((fh = fopen(prf_pth, "w")) == 0) {
		_logerr("no write access to %s\n", prf_pth);
		return;
	}

	fprintf(fh, "name=%s;\n", profile.nm);
	fprintf(fh, "levels=%d;\n", profile.lvls);
	fprintf(fh, "score=%d;\n", profile.scr);
	/* TODO profile.pct is not saved right now */
	le = profile.sts.hd.n;
	while (le != &profile.sts.tl) {
		st = (SInf*)le->d;
		fprintf(fh, "setname=%s;\n", st->nm);
		fprintf(fh, "num=%d;\n", st->num);
		fprintf(fh, "l_num=%d;\n", st->l_num);
		fprintf(fh, "c_num=%d;\n", st->c_num);
		for (i = 0; i < st->c_num; i++)
			fprintf(fh, "c_open=%d;\n", st->c_opn[i]);
		for (i = 0; i < st->num; i++)
			fprintf(fh, "cmp=%d;\n", st->cmp[i]);
		le = le->n;
	}

	fclose(fh);
}

/* reset profile */
void profileReset()
{
    snprintf(profile.nm,MAXSTRLEN,"Profile");
    profile.lvls = 0;
    profile.scr = 0;
    profile.pct = 0;
    DL_Ini(&profile.sts);
    profile.sts.flgs = DL_AUTODEL | DL_NOCB;
}

/* register or find a levelset with name nm */
SInf* profileRegisterSet(LSet *l_st)
{
    int i;
    DL_E *e = profile.sts.hd.n;
    SInf  *s;
    /* maybe it already exists */
    while (e != &profile.sts.tl) {
        s = (SInf*)e->d;
        if (!strcmp(s->nm, l_st->nm)) {
            if (l_st->c_num != s->c_num || l_st->l_num != s->l_num) {
                // seems to be changed; clear it //
                s->num = l_st->c_num * l_st->l_num;
                s->l_num = l_st->l_num;
                s->c_num = l_st->c_num;
                for (i = 0; i < s->c_num; i++)
                    s->c_opn[i] = l_st->ch[i].opn;
                memset(s->cmp, 0, sizeof(s->cmp));
                printf("WARNING: profile '%s': set info '%s' seems to be out of date\n", profile.nm, l_st->nm);
            }
            return s;
        }
        e = e->n;
    }
    /* must be registered */
    s = calloc(1, sizeof(SInf));
    strcpy(s->nm, l_st->nm);
    s->num = l_st->c_num * l_st->l_num;
    s->l_num = l_st->l_num;
    s->c_num = l_st->c_num;
    for (i = 0; i < s->c_num; i++)
        s->c_opn[i] = l_st->ch[i].opn;
    memset(s->cmp, 0, sizeof(s->cmp));
    DL_Add(&profile.sts, s);
    return s;
}

/*
    update Profile p's score and info
    s is rem_time / max_time of that level
*/
void profileUpdate(SInf *inf, int l_id, float pct, int scr)
{
	float new_p;

	if (!inf->cmp[l_id]) {
		/* mark as completed */
		inf->cmp[l_id] = 1;
		/* update percentage */
		if (profile.lvls == 0)
			profile.pct = pct;
		else {
			new_p = (profile.pct * profile.lvls + pct) / (profile.lvls + 1);
			profile.pct = new_p;
		}
		profile.lvls++;
		profile.scr += scr;
	}
}
