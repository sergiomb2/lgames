/***************************************************************************
                          file.c  -  description
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

#include "tools.h"
#include "file.h"
#include <stdlib.h>
#include <string.h>

int f_ln = 0; /* file line counter */

/* open file and reset line counter */
FILE *fileOpen(const char *fname, const char *mode)
{
	FILE *fh = fopen(fname, mode);
	if (fh == NULL)
		_logerr(_("cannot access file %s (%s)\n"), fname, mode);
	f_ln = 0;
	return fh;
}
void fileClose(FILE *fh)
{
	if (fh)
		fclose(fh);
}

/* read an entry to @str (max size MAXSTRLEN including \0) of
 * type @flags (value, subsection, comment).
 * ignore corrupted entries and continue reading until valid entry
 * or end of file (in which case entry might be corrupted).
*/
void fileGetEntry(FILE *fh, char *str, int flags)
{
	int pos = 0;
	char c;

	str[0] = 0;

	while (fread(&c, 1, 1, fh)) { /* read exactly one byte */
		/* ignore return carrier but count lines */
		if (c == 10) {
			f_ln++;
			continue;
		}
		/* add character */
		str[pos++] = c;
		str[pos] = 0;
		if (pos == MAXSTRLEN - 1)
			break;
		/* check valid end character */
		if ( (c == ';' && (flags & F_VAL)) ||
				(c == '>' && (flags & F_SUB)) ||
				(c == ')' && (flags & F_COM)) )
			break;
		/* ignore corrupted entries; try to read next one */
		if (c == ';' || c == '>' || c == ')')
			pos = 0;
	}
}

/* returns first non-blank character */
static char strGetFirstChar(char *str, char **n)
{
	int i = 0;
	while (str[i] <= 32) {
		i++;
		if (i >= strlen(str))
			return 0;
	}
	*n = str + i;
	return str[i];
}

/* returns last non-blank character */
static char strGetLastChar(char *str)
{
	int i = strlen(str) - 1;
	while (str[i] <= 32) {
		i--;
		if (i <= 0)
			return 0;
	}
	return str[i];
}

/* read value to @val; return 0 if no value, 1 otherwise */
static int strGetValue(char *str, char *val)
{
	int i;
	char *n;
	for (i = 0; i < strlen(str); i++)
		if (str[i] == '=' && strGetFirstChar(str + i + 1, &n)) {
			strcpy(val, n);
			if (val[strlen(val)-1] == ';')
				val[strlen(val)-1] = 0; // remove semicolon
			return 1;
		}
	return 0;
}

/* check entry for type @t and target name @nm and copy
 * value to @v if not NULL. Return 0 on failure, 1 on success.
*/
int fileCheckEntry(char *str, int type, const char *name, char *val)
{
	char *n;

	if (strlen(str) == 0)
		return 0;

	if (type & F_VAL) {
		if (!strGetFirstChar(str, &n) || strncmp(name, n, strlen(name)))
			return 0;
		if (val != 0 )
			return strGetValue(str, val);
		else
			return 0;
	}
	else if (type & F_SUB) {
		if (strGetFirstChar(str, &n) && !strncmp(name, n, strlen(name)))
			return 1;
	} else if ((type & F_COM) &&
			strGetFirstChar(str, 0) == '(' && strGetLastChar(str) == ')')
		return 1;
	return 0;
}

/** Read next entry from file and verify it is a value item
 * with the correct @id. Copy value to @val. Return 0 if not found,
 * 1 if found.
 */
int fileReadString(FILE *fh, const char *id, char *val)
{
	char entry[MAXSTRLEN]; /* FIXME fileGetEntry does not check length */
	fileGetEntry(fh, entry, F_VAL);
	if (!fileCheckEntry(entry, F_VAL, id, val))
		return 0;
	return 1;
}

/** Read next entry from file and verify it is a value item
 * with the correct @id. Copy value to @val. Return 0 if not found,
 * 1 if found.
 */
int fileReadInt(FILE *fh, const char *id, int *val)
{
	char str[MAXSTRLEN];
	if (!fileReadString(fh, id, str))
		return 0;
	*val = atoi(str);
	return 1;
}

/** Read list of @size integers to @arr.
 * Return 1 on success, 0 on failure */
int fileReadIntList(FILE *fh, const char *id, int *arr, int size)
{
	char str[MAXSTRLEN];
	int first;

	if (size <= 0)
		return 0;

	/* to check id, read first element with fileReadInt */
	if (!fileReadInt(fh, id, &first))
		return 0;
	arr[0] = first;

	/* remaining elements have no extra ids */
	for (int i = 1; i < size; i++) {
		fileGetEntry(fh, str, F_VAL);
		if (str[strlen(str)-1] == ';')
			str[strlen(str)-1] = 0; /* remove semicolon */
		arr[i] = atoi(str);
	}

	return 1;
}
