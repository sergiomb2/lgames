/*
 * view.cpp
 * (C) 2018 by Michael Speck
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tools.h"
#include "sdl.h"
#include "mixer.h"
#include "theme.h"
#include "sprite.h"
#include "game.h"
#include "menu.h"
#include "view.h"

extern SDL_Renderer *mrc;

View::View(Renderer &r, Config &cfg, Game &gm)
	: renderer(r), config(cfg), menuActive(true),
	  curMenu(NULL), graphicsMenu(NULL),
	  lblCredits1(true), lblCredits2(true), noGameYet(true),
	  game(gm), quitReceived(false),
	  lblScore(true), lblTime(true), lblErrors(true),
	  mcx(-1), mcy(-1),
	  fpsCycles(0), fpsStart(0), fps(0)
{
	mixer.open(config.channels, config.audiobuffersize);
	mixer.setVolume(config.volume);
	mixer.setMute(!config.sound);

	/* load theme names */
	readDir(string(DATADIR)+"/themes", RD_FOLDERS, themeNames);
	if ((uint)config.themeid >= themeNames.size())
		config.themeid = 0;
	config.themecount = themeNames.size();

	init(themeNames[config.themeid], config.fullscreen);
}

/** (Re)Initialize window, theme and menu.
 * t is theme name, r=0 means window, otherwise fullscreen. */
void View::init(string t, uint f)
{
	_loginfo("Initializing View (Theme=%s, Fullscreen=%d)\n",t.c_str(),f);

	/* determine resolution */
	int sw = 640, sh = 480;
	SDL_DisplayMode mode;
	SDL_GetCurrentDisplayMode(0,&mode);
	if (f) {
		sw = mode.w;
		sh = mode.h;
		_loginfo("Using fullscreen resolution %dx%d\n",mode.w,mode.h);
	} else {
		sw = mode.w / 2;
		sh = 3 * sw / 4;
		_loginfo("Using window resolution %dx%d\n",sw,sh);
	}

	/* (re)create main window */
	renderer.create("LPairs2", sw, sh, f );

	/* load theme */
	theme.load(t,renderer);

	/* create menu structure */
	createMenus();

	/* set label stuff */
	lblCredits1.setText(theme.fSmall, "http://lgames.sf.net");
	lblCredits2.setText(theme.fSmall, string("v")+PACKAGE_VERSION);
	lblRestart.setText(theme.fNormal, _("Press R to restart (or ESC for menu)."));

	/* create render images and positions */
	imgBackground.create(sw,sh);
	imgBackground.setBlendMode(0);

	/* game */
	if (noGameYet) {
		changeWallpaper();
		game.init(renderer.rx2sx(1), renderer.ry2sy(0.90),
				GM_HUGE, config.fullscreen, theme.numCards*2);
	} else
		startGame();
	cxoff = renderer.rx2sx(0.00);
	cyoff = renderer.ry2sy(0.05);
}

/** Main game loop. Handle events, update game and render view.
 *  Handle menu overlay if active. */
void View::run()
{
	SDL_Event ev;
	int flags;
	Ticks ticks;
	Ticks renderTicks;
	int maxDelay, delay = 0;
	Uint32 ms;
	vector<string> text;
	string str;

	fpsStart = SDL_GetTicks();
	fpsCycles = 0;

	if (config.fps == 1)
		maxDelay = 5;
	else if (config.fps == 2)
		maxDelay = 10;
	else
		maxDelay = 0;

	render();

	while (!quitReceived) {
		renderTicks.reset();

		/* handle events */
		if (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				quitReceived = true;
			else if (ev.type == SDL_KEYUP) {
				switch (ev.key.keysym.scancode) {
				case SDL_SCANCODE_F:
					if (!menuActive)
						config.showfps = !config.showfps;
					break;
				case SDL_SCANCODE_R:
					if (!menuActive && game.gameover)
						startGame();
					break;
				case SDL_SCANCODE_ESCAPE:
					if (!noGameYet)
						menuActive = !menuActive;
					break;
				default:
					if (!menuActive)
						game.handleClick(-1, -1);
					break;
				}
			}
			if (menuActive)
				handleMenuEvent(ev);
			else if (ev.type == SDL_MOUSEBUTTONUP) {
				if (game.handleClick(ev.button.x - cxoff,
						ev.button.y - cyoff))
					mixer.play(theme.sClick);
			}
		}

		/* get input state */
		SDL_GetMouseState(&mcx, &mcy);

		/* get passed time */
		ms = ticks.get();

		/* update animations and particles */
		for (auto it = begin(sprites); it != end(sprites); ++it) {
			if ((*it).get()->update(ms))
				it = sprites.erase(it);
		}

		/* update game and menu */
		flags = game.update(ms);
		if (menuActive)
			curMenu->update(ms);
		if (flags & GF_TIMECHANGED) {
			strprintf(str, _("Time: %d:%02d"),
						game.gtime/60000,
						(game.gtime/1000)%60);
			lblTime.setText(theme.fNormal, str);
		}
		if (flags & GF_SCORECHANGED) {
			strprintf(str, _("Score: %d"),game.score);
			lblScore.setText(theme.fNormal, str);
			mixer.play(theme.sRemove);
		}
		if (flags & GF_ERRORSCHANGED) {
			strprintf(str, _("Errors: %d"),game.errors);
			lblErrors.setText(theme.fNormal, str);
		}
		if (flags & GF_CARDSCLOSED)
			mixer.play(theme.sFail);
		if (flags & GF_CARDSREMOVED) {
			for (uint i = 0; i < game.numMaxOpenCards; i++) {
				FadeAnimation *a;
				Card &c = game.cards[game.lastMatchIds[i]];
				a = new FadeAnimation(
					theme.cards[c.id], cxoff+c.x, cyoff+c.y,
							0, renderer.ry2sy(1), c.w, c.h, 1000);
				sprites.push_back(unique_ptr<FadeAnimation>(a));
			}
		}

		/* render */
		render();
		SDL_RenderPresent(mrc);

		/* stats */
		fpsCycles++;
		if (fpsStart < SDL_GetTicks())
			fps = 1000 * fpsCycles / (SDL_GetTicks() - fpsStart);
		if (fpsCycles > 100) {
			fpsCycles = 0;
			fpsStart = SDL_GetTicks();
		}

		/* limit frame rate */
		delay = maxDelay - renderTicks.get(true);
		if (delay > 0)
			SDL_Delay(delay);
		SDL_FlushEvent(SDL_MOUSEMOTION); /* prevent event loop from dying */
	}

	if (!quitReceived)
		dim();
}

/** Render current game state. */
void View::render()
{
	/* background */
	imgBackground.copy();

	/* shadows */
	for (uint i = 0; i < game.numCards; i++) {
		Card &c = game.cards[i];
		if (c.removed)
			continue;
		/* FIXME shadow should be relative to gap not hardcoded */
		theme.cardShadow.copy(cxoff + c.x + 0.05*c.w,
					cyoff + c.y + 0.05*c.h, c.w, c.h);
	}

	/* cards */
	for (uint i = 0; i < game.numCards; i++) {
		Card &c = game.cards[i];
		if (c.removed)
			continue;
		if (c.open || noGameYet)
			theme.cards[c.id].copy(cxoff + c.x,cyoff + c.y,c.w,c.h);
		else if (!menuActive && c.hasFocus(mcx - cxoff, mcy - cyoff))
			theme.cardFocus.copy(cxoff + c.x,cyoff + c.y,c.w,c.h);
		else
			theme.cardBack.copy(cxoff + c.x,cyoff + c.y,c.w,c.h);
	}

	/* labels */
	lblTime.copy(renderer.rx2sx(0.5),renderer.ry2sy(0.025));
	lblScore.copy(renderer.rx2sx(0.01),renderer.ry2sy(0.975),
			ALIGN_X_LEFT | ALIGN_Y_CENTER);
	lblErrors.copy(renderer.rx2sx(0.99),renderer.ry2sy(0.975),
			ALIGN_X_RIGHT | ALIGN_Y_CENTER);

	if (game.gameover && !menuActive)
		lblRestart.copy(renderer.rx2sx(0.5),renderer.ry2sy(0.5),
				ALIGN_X_CENTER | ALIGN_Y_CENTER);

	/* sprites */
	for (auto& s : sprites)
		s->render();

	/* menu */
	if (menuActive) {
		theme.menuBackground.copy();
		lblCredits1.copy(renderer.getWidth()-2,renderer.getHeight(),
					ALIGN_X_RIGHT | ALIGN_Y_BOTTOM);
		lblCredits2.copy(renderer.getWidth()-2,renderer.getHeight() - theme.fSmall.getLineHeight(),
					ALIGN_X_RIGHT | ALIGN_Y_BOTTOM);
		curMenu->render();
	}

	/* stats */
	if (config.showfps) {
		theme.fSmall.setAlign(ALIGN_X_LEFT | ALIGN_Y_TOP);
		theme.fSmall.write(0,0,to_string((int)fps));
	}
}

/* Dim screen to black. As game engine is already set for new state
 * we cannot use render() but need current screen state. */
void View::dim()
{
	Texture img;
	img.createFromScreen();

	for (uint8_t a = 250; a > 0; a -= 10) {
		SDL_SetRenderDrawColor(mrc,0,0,0,255);
		SDL_RenderClear(mrc);
		img.setAlpha(a);
		img.copy();
		SDL_RenderPresent(mrc);
		SDL_Delay(10);
	}

	/* make sure no input is given yet for next state */
	waitForInputRelease();
}

bool View::showInfo(const string &line, int type)
{
	vector<string> text;
	text.push_back(line);
	return showInfo(text, type);
}

/* Darken current screen content and show info text.
 * If confirm is false, wait for any key/click.
 * If confirm is true, wait for key y/n and return true false. */
bool View::showInfo(const vector<string> &text, int type)
{
	Font &font = theme.fSmall;
	bool ret = true;
	uint h = text.size() * font.getLineHeight();
	int tx = renderer.getWidth()/2;
	int ty = (renderer.getHeight() - h)/2;

	darkenScreen();

	font.setAlign(ALIGN_X_CENTER | ALIGN_Y_TOP);
	for (uint i = 0; i < text.size(); i++) {
		font.write(tx,ty,text[i]);
		ty += font.getLineHeight();
	}

	SDL_RenderPresent(mrc);

	ret = waitForKey(type);
	return ret;
}

void View::createMenus()
{
	Menu *mNewGame, *mOptions, *mAudio, *mGraphics;
	const char *diffNames[] = {_("Small"),_("Medium"),_("Large"),_("Huge") } ;
	const char *fpsLimitNames[] = {_("No Limit"),_("200 FPS"),_("100 FPS") } ;
	const int bufSizes[] = { 256, 512, 1024, 2048, 4096 };
	const int channelNums[] = { 8, 16, 32 };

	/* XXX too lazy to set fonts for each and every item...
	 * use static pointers instead */
	MenuItem::fNormal = &theme.fMenuNormal;
	MenuItem::fFocus = &theme.fMenuFocus;
	MenuItem::fTooltip = &theme.fSmall;
	MenuItem::tooltipWidth = 0.3 * theme.menuBackground.getWidth();

	rootMenu.reset(); /* delete any old menu ... */

	rootMenu = unique_ptr<Menu>(new Menu(theme)); /* .. or is assigning a new object doing it? */
	mNewGame = new Menu(theme);
	mOptions = new Menu(theme);
	mAudio = new Menu(theme);
	mGraphics = new Menu(theme);
	graphicsMenu = mGraphics; /* needed to return after mode/theme change */

	mNewGame->add(new MenuItem(_("Start Game"),
			_("Start game."),
			AID_STARTGAME));
	mNewGame->add(new MenuItemSep());
	mNewGame->add(new MenuItemList(_("Size"),
			_("In fullscreen: Small=6x4, Medium=8x4, Large=10x5, Huge=12x6.\nNote that it's slightly different in window mode to match the different ratio."),
			AID_NONE,config.gamemode,diffNames,4));
	mNewGame->add(new MenuItemSep());
/*	mNewGame->add(new MenuItemRange(_("Players"),
			_("Number and names of players. Players alternate whenever a life is lost."),
			AID_NONE,config.playercount,1,MAX_PLAYERS,1));
	mNewGame->add(new MenuItemEdit(_("1st"),config.playernames[0]));
	mNewGame->add(new MenuItemEdit(_("2nd"),config.playernames[1]));
	mNewGame->add(new MenuItemEdit(_("3rd"),config.playernames[2]));
	mNewGame->add(new MenuItemEdit(_("4th"),config.playernames[3]));
	mNewGame->add(new MenuItemSep());*/
	mNewGame->add(new MenuItemBack(rootMenu.get()));

	mGraphics->add(new MenuItemList(_("Theme"),
			_("Select theme. (not applied yet)"),
			AID_NONE,config.themeid,themeNames));
	mGraphics->add(new MenuItemList(_("Mode"),
			_("Select mode. (not applied yet)"),
			AID_NONE,config.fullscreen,_("Window"),_("Fullscreen")));
	mGraphics->add(new MenuItem(_("Apply Theme&Mode"),
			_("Apply the above settings."),AID_APPLYTHEMEMODE));
	mGraphics->add(new MenuItemSep());
	mGraphics->add(new MenuItemList(_("Frame Limit"),
			"Maximum number of frames per second.\nBe careful: The higher the limit the more insensitive your mouse might become to slow movements (because relative motion is used and program cycles are shorter).\n200 FPS should be a good value.",
			AID_NONE,config.fps,fpsLimitNames,3));
	mGraphics->add(new MenuItemSep());
	mGraphics->add(new MenuItemBack(mOptions));

	mAudio->add(new MenuItemSwitch(_("Sound"),"",AID_SOUND,config.sound));
	mAudio->add(new MenuItemRange(_("Volume"),"",AID_VOLUME,config.volume,0,100,10));
	mAudio->add(new MenuItemSwitch(_("Speech"),"",AID_NONE,config.speech));
	mAudio->add(new MenuItemSep());
	mAudio->add(new MenuItemIntList(_("Buffer Size"),
			_("Reduce buffer size if you experience sound delays. Might have more CPU impact though. (not applied yet)"),
			config.audiobuffersize,bufSizes,5));
	mAudio->add(new MenuItemIntList(_("Channels"),
			_("More channels gives more sound variety, less channels less (not applied yet)"),config.channels,
			channelNums,3));
	mAudio->add(new MenuItem(_("Apply Size&Channels"),
			_("Apply above settings"),AID_APPLYAUDIO));
	mAudio->add(new MenuItemSep());
	mAudio->add(new MenuItemBack(mOptions));

	mOptions->add(new MenuItemSub(_("Graphics"),mGraphics));
	mOptions->add(new MenuItemSub(_("Audio"),mAudio));
	mOptions->add(new MenuItemBack(rootMenu.get()));

	rootMenu->add(new MenuItemSub(_("New Game"), mNewGame));
	rootMenu->add(new MenuItemSep());
	rootMenu->add(new MenuItemSub(_("Settings"), mOptions));
	//rootMenu->add(new MenuItem(_("Help"), "", AID_HELP));
	//rootMenu->add(new MenuItemSep());
	rootMenu->add(new MenuItem(_("Quit"), "", AID_QUIT));

	rootMenu->adjust();

	curMenu = rootMenu.get();
}

int View::waitForKey(int type)
{
	SDL_Event ev;
	bool ret = true;
	bool leave = false;

	SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
	while (!leave) {
		/* handle events */
		if (SDL_WaitEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				quitReceived = leave = true;
			if (type == WT_ANYKEY && (ev.type == SDL_KEYUP ||
						ev.type == SDL_MOUSEBUTTONUP))
				leave = true;
			else if (ev.type == SDL_KEYUP) {
				int sc = ev.key.keysym.scancode;
				if (type == WT_YESNO) {
					if (sc == SDL_SCANCODE_Y || sc == SDL_SCANCODE_Z)
						ret = leave = true;
					if (sc == SDL_SCANCODE_N || sc == SDL_SCANCODE_ESCAPE) {
						ret = false;
						leave = true;
					}
				} else if (type == WT_PAUSE)
					if (sc == SDL_SCANCODE_P)
						ret = leave = true;
			}
		}
		SDL_FlushEvent(SDL_MOUSEMOTION);
	}
	SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);
	return ret;
}

void View::darkenScreen(int alpha)
{
	Texture img;
	img.createFromScreen();
	SDL_SetRenderDrawColor(mrc,0,0,0,255);
	SDL_RenderClear(mrc);
	img.setAlpha(alpha);
	img.copy();
}

/* wait up to 1s for release of all buttons and keys.
 * Clear SDL event loop afterwards.
 */
void View::waitForInputRelease()
{
	SDL_Event ev;
	bool leave = false;
	int timeout = 500, numkeys;
	Ticks ticks;

	SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
	while (!leave) {
		if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
			quitReceived = leave = true;
		leave = true;
		if (SDL_GetMouseState(NULL, NULL))
			leave = false;
		const Uint8 *keystate = SDL_GetKeyboardState(&numkeys);
		for (int i = 0; i < numkeys; i++)
			if (keystate[i]) {
				leave = false;
				break;
			}
		timeout -= ticks.get();
		if (timeout <= 0)
			leave = true;
		SDL_Delay(10);
	}
	SDL_PumpEvents();
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

void View::handleMenuEvent(SDL_Event &ev)
{
	int aid = AID_NONE;
	MenuItemSub *subItem;
	MenuItemBack *backItem;

	if (ev.type == SDL_QUIT)
		quitReceived = true;
	else if (ev.type == SDL_KEYDOWN &&
			ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		Menu *lm = curMenu->getLastMenu();
		if (lm)
			curMenu = lm;
	} else if ((aid = curMenu->handleEvent(ev))) {
		if (aid != AID_FOCUSCHANGED)
			mixer.play(theme.sMenuClick);
		switch (aid) {
		case AID_FOCUSCHANGED:
			mixer.play(theme.sMenuMotion);
			break;
		case AID_QUIT:
			quitReceived = true;
			break;
		case AID_ENTERMENU:
			subItem = dynamic_cast<MenuItemSub*>(curMenu->getCurItem());
			if (subItem) { /* should never fail */
				curMenu = subItem->getSubMenu();
			} else
				_logerr("Oops, submenu not found...\n");
			break;
		case AID_LEAVEMENU:
			backItem = dynamic_cast<MenuItemBack*>(curMenu->getCurItem());
			if (backItem) { /* should never fail */
				curMenu = backItem->getLastMenu();
			} else
				_logerr("Oops, last menu not found...\n");
			break;
		case AID_SOUND:
			mixer.setMute(!config.sound);
			break;
		case AID_VOLUME:
			mixer.setVolume(config.volume);
			break;
		case AID_APPLYAUDIO:
			mixer.close();
			mixer.open(config.channels, config.audiobuffersize);
			break;
		case AID_APPLYTHEMEMODE:
			/* XXX workaround for SDL bug: clear event
			 * loop otherwise left mouse button event is
			 * screwed for the first click*/
			waitForInputRelease();
			init(themeNames[config.themeid],config.fullscreen);
			curMenu = graphicsMenu;
			break;
		case AID_HELP:
			//showHelp();
			break;
		case AID_STARTGAME:
			startGame();
			menuActive = false;
			waitForInputRelease();
			break;
		}
	}
}

void View::startGame()
{
	changeWallpaper();
	game.init(renderer.rx2sx(1), renderer.ry2sy(0.90),
			config.gamemode, config.fullscreen, theme.numCards*2);
	noGameYet = false;
	lblScore.setText(theme.fNormal, _("Score: 0"));
	lblTime.setText(theme.fNormal, _("Time: 0:00"));
	lblErrors.setText(theme.fNormal, _("Errors: 0"));
}

void View::changeWallpaper()
{
	curWallpaperId = rand() % theme.numWallpapers;
	renderer.setTarget(imgBackground);
	theme.wallpapers[curWallpaperId].copy();
	renderer.clearTarget();
}