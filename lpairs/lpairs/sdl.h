/***************************************************************************
                          sdl.h  -  description
                             -------------------
    begin                : Thu Apr 20 2000
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

#ifndef SDL_H
#define SDL_H

#ifdef __cplusplus
extern "C" {
#endif

/* for development -- done by config later */
//#define SDL_1_1_5

// draw region //
#define DEST(p, i, j, k, l) {sdl.d.s = p; sdl.d.r.x = i; sdl.d.r.y = j; sdl.d.r.w = k; sdl.d.r.h = l;}
#define SOURCE(p, i, j) {sdl.s.s = p; sdl.s.r.x = i; sdl.s.r.y = j; sdl.s.r.w = sdl.d.r.w; sdl.s.r.h = sdl.d.r.h;}
#define FULL_DEST(p) {sdl.d.s = p; sdl.d.r.x = 0; sdl.d.r.y = 0; sdl.d.r.w = (p)->w; sdl.d.r.h = (p)->h;}
#define FULL_SOURCE(p) {sdl.s.s = p; sdl.s.r.x = 0; sdl.s.r.y = 0; sdl.s.r.w = sdl.d.r.w; sdl.s.r.h = sdl.d.r.h;}
typedef struct {
    SDL_Surface *s;
    SDL_Rect    r;
} DrawRgn;

// Sdl Surface //
#define SDL_NONFATAL 0x10000000
SDL_Surface* load_surf(char *fname, int f);
SDL_Surface* create_surf(int w, int h, int f);
int  disp_format(SDL_Surface *sur);
void lock_surf(SDL_Surface *sur);
void unlock_surf(SDL_Surface *sur);
void blit_surf(void);
void alpha_blit_surf(int alpha);
void fill_surf(int c);
void set_surf_clip( SDL_Surface *surf, int x, int y, int w, int h );

// Sdl Font //
#ifdef SDL_1_1_5
enum {
    OPAQUE = 0
};
#else
enum {
    OPAQUE = 255
};
#endif
enum {
    ALIGN_X_LEFT	= (1L<<1),
    ALIGN_X_CENTER	= (1L<<2),
    ALIGN_X_RIGHT	= (1L<<3),
    ALIGN_Y_TOP	    = (1L<<4),
    ALIGN_Y_CENTER	= (1L<<5),
    ALIGN_Y_BOTTOM	= (1L<<6)
};

typedef struct {
    SDL_Surface *pic;
    int         align;
    int         color;
    int         height;
    char        char_width[256];
    int         char_offset[256];
    char        keys[256];
    char        offset;
    char        length;
    //last written rect
    int     	last_x;
    int         last_y;
    int	        last_width;
    int	        last_height;
} Font;
Font* load_font(char *fname);
Font* load_fixed_font(char *fname, int off, int len, int w);
void free_font(Font *sfnt);
int  write_text(Font *sfnt, SDL_Surface *dest, int x, int y, char *str, int alpha);
void lock_font(Font *sfnt);
void unlock_font(Font *sfnt);
SDL_Rect last_write_rect(Font *fnt);
int  text_width(Font *fnt, char *str);

/* mouse buttons */
enum {
    LEFT_BUTTON = 1,
    MIDDLE_BUTTON = 2,
    RIGHT_BUTTON = 3
};

/* Sdl */
enum {
    RECT_LIMIT = 200,
    DIM_STEPS = 8,
    DIM_DELAY = 20
};
#define DIM_SCREEN()   dim_screen(DIM_STEPS, DIM_DELAY, 255)
#define UNDIM_SCREEN() undim_screen(DIM_STEPS, DIM_DELAY, 255)
typedef struct {
    SDL_Surface *screen;
    DrawRgn     d, s;
    int         rect_count;
    SDL_Rect    rect[RECT_LIMIT];
} Sdl;
void init_sdl( int f );
void quit_sdl();
int  set_video_mode( int w, int h, int d, int f );
void hardware_cap();
void refresh_screen( int x, int y, int w, int h );
void refresh_rects();
void add_refresh_rect(int x, int y, int w, int h);
void dim_screen(int steps, int delay, int trp);
void undim_screen(int steps, int delay, int trp);
int  wait_for_key();
void wait_for_click();
void lock_screen();
void unlock_screen();
void flip_screen();

/* cursor */
/* creates cursor */
SDL_Cursor* create_cursor( int width, int height, int hot_x, int hot_y, char *source );

/* timer */
int get_time();
void reset_timer();

/* Simple Animation: linear movement of image in front of "frozen" screen */
typedef struct {
	SDL_Surface *image; /* surface (is deleted after animation) */
	double px,py,pa; /* current position/alpha */
	double vx,vy,va; /* change per millisecond */
	double runtime; /* remaining runtime of animation in milliseconds */
} SimpleAnimation;

void sa_get_background();
void sa_free_background();
void sa_draw_background();

void sa_init(SimpleAnimation *anim, SDL_Surface *img, int sx, int sy, int sa, int dx, int dy, int da, int time);
void sa_finalize(SimpleAnimation *anim);
int sa_update(SimpleAnimation *anim, int ms);
void sa_draw(SimpleAnimation *anim);

#ifdef __cplusplus
};
#endif

#endif
