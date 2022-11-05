/*
 * editor.h
 * (C) 2022 by Michael Speck
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SRC_EDITOR_H_
#define SRC_EDITOR_H_

class Editor;

class EditorLevel {
	friend Editor;

	string title, author;
	int bricks[EDIT_WIDTH][EDIT_HEIGHT]; /* -1 or brick id */
	int extras[EDIT_WIDTH][EDIT_HEIGHT]; /* -1 or extra id */
public:
	EditorLevel() { reset(); }
	void reset();
	void clear();
};

enum {
	EB_FIRST = 0,
	EB_PREV,
	EB_NEXT,
	EB_LAST,
	EB_ADDAFTER,
	EB_ADDBEFORE,
	EB_CLEAR,
	EB_DELETE,
	EB_MOVEUP,
	EB_MOVEDOWN,
	EB_LOAD,
	EB_SAVE,
	EB_QUIT,
	EB_TEST,
	EB_NUMBER
};

class Editor {
	Theme &theme;
	Mixer &mixer;
	bool quitReceived; /* close app entirely */
	bool leaveRequested; /* close editor and return to menu */

	Image background;
	Image selFrame;
	Image buttons;
	SDL_Rect rMap, rBricks, rExtras, rTitle, rAuthor, rVersion, rButtons;
	uint brickWidth, brickHeight;
	uint numBrickCols, numExtraCols;
	uint numBricks, numExtras;

	string fpath; /* edited file */
	vector<EditorLevel> levels; /* level data */
	string version; /* version of level set */
	uint numLevels; /* number of levels in set */
	uint curLevelId; /* current level id */
	EditorLevel *curLevel; /* pointer to current level in array */
	int selBrickId, selExtraId; /* selected brick/extra */
	uint editWidth, editHeight; /* size of editable part */

	void init(const string &setname);
	void render();
	bool inRect(int x, int y, const SDL_Rect &r) {
		if (x >= r.x && y >= r.y && x < r.x+r.w && y < r.y+r.h)
			return true;
		return false;
	};
	void addNewLevel(uint pos);
	void gotoLevel(uint pos);
	void handleButton(uint id);
	void handleClick(int x, int y, int b);
	void runEditDlg(const string &c, string &v);
	bool runConfirmDlg(const string &c);
	void load();
	void save();
	char brickId2Char(int id);
	char extraId2Char(int id);
	int brickChar2Id(char c);
	int extraChar2Id(char c);
public:
	Editor(Theme &t, Mixer &m)
			: theme(t), mixer(m),
			  quitReceived(false), leaveRequested(false),
			  brickWidth(0), brickHeight(0),
			  numBrickCols(0), numExtraCols(0),
			  numBricks(0), numExtras(0),
			  version("1.00"), numLevels(0), curLevelId(0), curLevel(NULL),
			  selBrickId(0), selExtraId(0),
			  editWidth(EDIT_WIDTH), editHeight(EDIT_HEIGHT) {};
	bool quitRcvd() { return quitReceived; }
	void run(const string &setname);
};

#endif /* SRC_EDITOR_H_ */
