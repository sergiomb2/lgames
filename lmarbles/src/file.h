/***************************************************************************
                          file.h  -  description
                             -------------------
    begin                : Tue Sep 26 2000
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

#ifndef __FILE_H
#define __FILE_H

#include <stdio.h>

#define F_SUB   0x0001
#define F_COM   0x0002
#define F_VAL   0x0004

FILE *fileOpen(const char *fname, const char *mode);
void fileClose(FILE *fh);

void fileGetEntry(FILE *fh, char *str, int flags);
int fileCheckEntry(char *str, int type, const char *name, char *val);

int fileReadString(FILE *fh, const char *id, char *val);
int fileReadInt(FILE *fh, const char *id, int *val);
int fileReadIntList(FILE *fh, const char *id, int *arr, int size);

#endif
