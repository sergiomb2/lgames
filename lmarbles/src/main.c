/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Sam Aug  5 12:36:32 MEST 2000
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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef _WIN32
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "menu.h"
#include "cfg.h"
#include "levels.h"
#include "tools.h"
#include "sdl.h"
#include "audio.h"
#include "profile.h"
#include "game.h"

// timer -- timer.c //
extern struct timeb t_c, t_lc;
// quit game quickly ? //
int     trm_gm = 0;
// menu config -- cfg.c //
extern  Config config;
// menu manager -- menu.c //
extern  MMng mm;
// sdl struct //
extern Sdl sdl;
// levelset list and number of entries -- levels.c //
extern char **ls_lst;
extern int  ls_n;
// profiles //
extern DLst prfs;
// string list for the menu -- profile.c //
extern char **prf_lst;
extern int  prf_n;
// menu entry containing the profile list //
MEnt *me_prf, *me_del, *me_clr;

// menu actions MUST BE GREATER THAN 0 //
#define MA_NONE 0
#define MA_QUT 1
#define MA_PLY 2
#define MA_HSC 3

/*
====================================================================
Clear the SDL event key (keydown events)
====================================================================
*/
int all_filter( const SDL_Event *event )
{
    return 0;
}
void event_clear_sdl_queue()
{
    SDL_Event event;
    SDL_SetEventFilter( all_filter );
    while ( SDL_PollEvent( &event ) );
    SDL_SetEventFilter( 0 );
}

// menu callbacks //

/*
    activate / deactivate sound
*/
void CB_Snd()
{
    sound_enable(config.sound);
}

/*
    set sound volume
*/
void CB_StV()
{
    sound_volume(config.volume * 16);
}

/*
    init menu
    add all entries
    check them
*/
void MM_CrtE()
{
    SDL_Surface *ss_bk, *ss_lg;
    SFnt        *ft_y, *ft_w, *ft_t;
    Menu        *_main, *opts, *snd, *new, *gfx, *ctrl;
    MEnt        *e;
    char        *str_limit[] = {_("Time"), _("Moves")};
    char        *str_mode[] = {_("Normal"), _("5 Stars only")};

    // load and assign gfx //
    ss_bk = SS_Ld("gfx/title.bmp", SDL_SWSURFACE);
    ss_lg = SS_Ld("gfx/logo.bmp", SDL_SWSURFACE);
    ft_y = SF_LdFxd("gfx/f_yellow.bmp", 32, 96, 10);
    ft_w = SF_LdFxd("gfx/f_white.bmp", 32, 96, 10);
    MM_Ini(ss_bk->w / 2, ss_bk->h - 100, 50, ss_bk, ss_lg, ft_y, ft_w);

    // add copyright //
    ft_t = SF_Ld("gfx/tiny.sdlfnt");
	ft_t->algn = TA_X_R | TA_Y_B;
	SF_Wrt(ft_t, ss_bk, ss_bk->w-3, ss_bk->h-1, "(C) 2000-2012 Michael Speck", 0);
	ft_t->algn = TA_X_L | TA_Y_B;
	SF_Wrt(ft_t, ss_bk, 0, ss_bk->h-1, "http://lgames.sourceforge.net", 0);
    SF_Fr(ft_t);

    // create and add entrys //
    _main = M_Crt(); MM_Add(_main);
    opts = M_Crt(); MM_Add(opts);
    snd = M_Crt(); MM_Add(snd);
    new = M_Crt(); MM_Add(new);
    gfx = M_Crt(); MM_Add(gfx);
    ctrl = M_Crt(); MM_Add(ctrl);
    // main //
    M_Add(_main, ME_CrtSub(_("New Game"), new));
    M_Add(_main, ME_CrtSub(_("Options"), opts));
    M_Add(_main, ME_CrtAct(_("Quit"), MA_QUT));
    // options //
    M_Add(opts, ME_CrtSub(_("Controls"), ctrl));
    M_Add(opts, ME_CrtSub(_("Graphics"), gfx));
    M_Add(opts, ME_CrtSub(_("Audio"), snd));
    M_Add(opts, ME_CrtSep(""));
    M_Add(opts, ME_CrtSub(_("Back"), _main));
    // sound //
    e = ME_CrtSw2(_("Sound: "), &config.sound, _("Off"), _("On"));
    e->cb = CB_Snd;
    M_Add(snd, e);
    e = ME_CrtRng(_("Volume: "), &config.volume, 1, 8, 1);
    e->cb = CB_StV;
    M_Add(snd, e);
    M_Add(snd, ME_CrtSep(""));
    M_Add(snd, ME_CrtSub(_("Back"), opts));
    // new game //
    M_Add(new, ME_CrtAct(_("Start"), MA_PLY));
    M_Add(new, ME_CrtSep(""));
    if (config.ls >= ls_n) config.ls = 0; // maybe someone deleted some level sets //
    M_Add(new, ME_CrtSwX(_("Levelset:"), &config.ls, ls_lst, ls_n));
    M_Add(new, ME_CrtSwX(_("Limit:"), &config.limitType, str_limit, 2));
    M_Add(new, ME_CrtSwX(_("Mode:"), &config.hardcore, str_mode, 2));
    M_Add(new, ME_CrtSep(""));
    M_Add(new, ME_CrtSub(_("Back"), _main));
    // graphics //
    M_Add(gfx, ME_CrtSw2(_("Animations:"), &config.animations, _("Off"), _("On")));
    M_Add(gfx, ME_CrtSw2(_("Fullscreen:"), &config.fullscreen, _("Off"), _("On")));
    M_Add(gfx, ME_CrtSw2(_("Dim Effect:"), &config.dim, _("Off"), _("On")));
    M_Add(gfx, ME_CrtSep(""));
    M_Add(gfx, ME_CrtSub(_("Back"), opts));
    // controls
    M_Add(ctrl, ME_CrtKey(_("Up"), &config.k_up));
    M_Add(ctrl, ME_CrtKey(_("Down"), &config.k_down));
    M_Add(ctrl, ME_CrtKey(_("Left"), &config.k_left));
    M_Add(ctrl, ME_CrtKey(_("Right"), &config.k_right));
    M_Add(ctrl, ME_CrtKey(_("Undo"), &config.k_undo));
    M_Add(ctrl, ME_CrtSep(""));
    M_Add(ctrl, ME_CrtSub(_("Back"), opts));

    // default valid keys
    MM_DfVKys();

    // adjust position of all entries in all menus //
    MM_AdjP();

    // call all callbacks //
    MM_CB();

    // check for errors
    MM_Ck();
}

/*
    init sound, menu, profiles, game
    main loop for the menu
*/
int main(int argc, char *argv[])
{
    SDL_Event   e;
    int         go_on = 1;
    int         ms;
    char aux[64];
    
#ifdef _WIN32
    /* Get Windows to open files in binary mode instead of default text mode */
    _fmode = _O_BINARY;
#endif
    
    /* i18n */
#ifdef ENABLE_NLS
    setlocale (LC_ALL, "");
    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#endif

    // just query version? //
    if ( argc > 1 && !strncmp( "--version", argv[1], 9 ) ) {

        printf( "%s\n", VERSION );
        return 0;

    }

    // random init
    srand(time(NULL));

    // init SDL
    Sdl_Ini(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    sprintf( aux, "LMarbles %s", VERSION );
    SDL_WM_SetCaption(aux, 0);
    Sdl_StVdMd(512, 384, 16, SDL_SWSURFACE);

    /* load config (and create config dir which is also used for profiles) */
    configSetPath();
    configLoad();

    /* load profiles */
    profileInit();
    profileLoad();
            
    // create levelset list and reset config's levelset index if nescessary //
    L_CrtLst();
    if (config.ls >= ls_n)
        config.ls = 0;

    audio_open();
    sound_enable( config.sound );
    sound_volume( config.volume * 16 );
		
    // game init //
    G_Ini();

    // init and show menu//
    MM_CrtE();
    MM_Shw(MM_RSZ);

    event_clear_sdl_queue();
    //menu loop
    T_Rst(); // reset time //
    while (go_on && !trm_gm) {
        M_Hd(mm.c_mn);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                trm_gm = 1;
            if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                go_on = 0;
            switch (MM_Evt(&e)) {
                case MA_QUT:
                    go_on = 0;
                    break;
                case MA_PLY:
                    if (G_Opn()) {
                        G_Run();
                        G_Cls();
                    }
                    break;
            }
        }
        ms = T_Gt();
        M_CmA(mm.c_mn, ms);
        M_Shw(mm.c_mn);
        Sdl_UpdR();
        SDL_Delay(5);
    }

    /* finalize */
    MM_Trm();
    G_Trm();
    audio_close();
    configSave();
    L_DelLst();
    profileSave();
    Sdl_Qut();
    printf(_("Bye, bye!\n"));

    return 0;
}
