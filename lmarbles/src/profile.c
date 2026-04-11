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

/* initialize profile */
void profileInit()
{
	snprintf(prf_pth, MAXSTRLEN, "%s/lmarbles.prf", configDir);
	snprintf(profile.name, MAXSTRLEN, "Profile");
	DL_Ini(&profile.sts);
	profile.sts.flgs = DL_AUTODEL | DL_NOCB;
}

/* load profile; return 0 on error (and create standard profile), 1 otherwise */
int profileLoad()
{
	FILE    *fh = 0;
	char    str[MAXSTRLEN];
	int     i;

	profileReset();

	_loginfo(_("loading profile...\n"));

	/* open file */
	if ((fh = fopen(prf_pth, "r")) == 0) {
		_logerr(_("file %s does not exist\n"), prf_pth);
		return 0;
	}

	/* read profile */
	fileReadString(fh, "name", profile.name);
	while (fileReadString(fh, "setname", str)) {
		SInf *st = calloc(1,sizeof(SInf));
		snprintf(st->name, MAXSTRLEN, "%s", str);
		fileReadInt(fh, "score", &st->score);
		fileReadInt(fh, "numlevels", &st->numLevels);
		fileReadInt(fh, "numchapters", &st->numChapters);
		fileReadInt(fh, "chaptersize", &st->chapterSize);
		for (i = 0; i < st->numChapters; i++)
			fileReadInt(fh, "chapteropen", &st->chapterOpen[i]);
		for (i = 0; i < st->numLevels; i++)
			fileReadInt(fh, "cmp", &st->completed[i]);
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

	fprintf(fh, "name=%s;\n", profile.name);
	le = profile.sts.hd.n;
	while (le != &profile.sts.tl) {
		st = (SInf*)le->d;
		fprintf(fh, "setname=%s;\n", st->name);
		fprintf(fh, "score=%d;\n", st->score);
		fprintf(fh, "numlevels=%d;\n", st->numLevels);
		fprintf(fh, "numchapters=%d;\n", st->numChapters);
		fprintf(fh, "chaptersize=%d;\n", st->chapterSize);
		for (i = 0; i < st->numChapters; i++)
			fprintf(fh, "chapteropen=%d;\n", st->chapterOpen[i]);
		for (i = 0; i < st->numLevels; i++)
			fprintf(fh, "cmp=%d;\n", st->completed[i]);
		le = le->n;
	}

	fclose(fh);
}

/* reset profile */
void profileReset()
{
	DL_Clr(&profile.sts);
}

/* register or find a levelset with @name */
SInf* profileRegisterSet(LSet *l_st)
{
    int i;
    DL_E *e = profile.sts.hd.n;
    SInf  *s;
    /* maybe it already exists */
    while (e != &profile.sts.tl) {
        s = (SInf*)e->d;
        if (!strcmp(s->name, l_st->nm)) {
            if (l_st->c_num != s->numChapters || l_st->l_num != s->chapterSize) {
                // seems to have changed; clear it
                s->numLevels = l_st->c_num * l_st->l_num;
                s->chapterSize = l_st->l_num;
                s->numChapters = l_st->c_num;
                for (i = 0; i < s->numChapters; i++)
                    s->chapterOpen[i] = l_st->ch[i].opn;
                memset(s->completed, 0, sizeof(s->completed));
                _loginfo("WARNING: profile '%s': set info '%s' seems to be out of date\n",
                	profile.name, l_st->nm);
            }
            return s;
        }
        e = e->n;
    }
    /* must be registered */
    s = calloc(1, sizeof(SInf));
    strcpy(s->name, l_st->nm);
    s->numLevels = l_st->c_num * l_st->l_num;
    s->chapterSize = l_st->l_num;
    s->numChapters = l_st->c_num;
    for (i = 0; i < s->numChapters; i++)
        s->chapterOpen[i] = l_st->ch[i].opn;
    memset(s->completed, 0, sizeof(s->completed));
    DL_Add(&profile.sts, s);
    return s;
}

/* update profile's score and info
   s is rem_time / max_time of that level
*/
void profileUpdate(SInf *inf, int lvl, int scr)
{
	if (!inf->completed[lvl]) {
		/* mark as completed */
		inf->completed[lvl] = 1;
		/* update percentage */
		inf->score += scr;
	}
}
