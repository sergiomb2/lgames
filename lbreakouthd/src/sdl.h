/*
 * sdl.h
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

#ifndef SDL_H_
#define SDL_H_

#include <string>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class MainWindow {
public:
	SDL_Window* mw;
	SDL_Renderer* mr;
	int w, h;

	MainWindow(const char *title, int _w, int _h, int _full = 0);
	~MainWindow();
	int getWidth() { return w; }
	int getHeight() { return h; }
	void refresh();
};

class Geom {
	int gx, gy, gw, gh;
public:
	static int sw; /* screen geometry, only available after MainWindow created */
	static int sh;

	Geom(int _x, int _y, int _w, int _h) : gx(_x), gy(_y), gw(_w), gh(_h) {
		if (_w <= 0)
			gw = Geom::sw;
		if (_h <= 0)
			gh = Geom::sh;
	}
	Geom(float _x, float _y, float _w, float _h) {
		gx = _x*Geom::sw;
		gy = _y*Geom::sh;
		gw = _w*Geom::sw;
		gh = _h*Geom::sh;
	}
	int width() {return gw;}
	int height() {return gh;}
	int xpos() {return gx;}
	int ypos() {return gy;}
	static int rwidth(float r) {return r*Geom::sw;}
	static int rheight(float r) {return r*Geom::sh;}
};

class Image {
protected:
	SDL_Texture *tex;
	int w, h;
public:
	static bool useColorKeyBlack;
	static int getHeight(const string& fname) {
		Image img;
		if (!img.load(fname))
			return -1;
		return img.getHeight();
	}
	static int getWidth(const string& fname) {
		Image img;
		if (!img.load(fname))
			return -1;
		return img.getWidth();
	}
	static Uint32 getSurfacePixel(const SDL_Surface *src, int sx, int sy) {
		Uint32 pixel = 0;
		memcpy( &pixel, (char*)src->pixels + sy * src->pitch +
				sx * src->format->BytesPerPixel,
				src->format->BytesPerPixel );
		return pixel;
	}
	static void setRenderScaleQuality(int level) {
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,to_string(level).c_str());
	}

	Image() : tex(NULL), w(0), h(0) {}
	Image(int _w, int _h) : tex(NULL), w(0), h(0) {
		create(_w,_h);
	}
	~Image() {
		if (tex)
			SDL_DestroyTexture(tex);
	}
	int create(int w, int h);
	int createFromScreen();
	int load(const string& fname);
	int load(SDL_Surface *s);
	int load(Image *s, int x, int y, int w, int h);
	SDL_Texture *getTex();
	void copy();
	void copy(int dx, int dy);
	void copy(int dx, int dy, int dw, int dh);
	void copy(int sx, int sy, int sw, int sh, int dx, int dy);
	int getWidth() {return w;}
	int getHeight() {return h;}
	void setAlpha(int alpha) { SDL_SetTextureAlphaMod(tex, alpha); }
	void clearAlpha() { setAlpha(255); }
	void setBlendMode(int on) {
		SDL_SetTextureBlendMode(tex,
				on?SDL_BLENDMODE_BLEND:SDL_BLENDMODE_NONE);
	}
	void fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
	void scale(int nw, int nh);
};

class GridImage : public Image
{
	int gw, gh; /* grid cell geometry */
public:
	GridImage() : gw(0), gh(0) {};
	int load(const string& fname, int _gw, int _gh);
	int load(SDL_Surface *s, int _gw, int _gh);
	int getGridSizeX() {return w/gw;}
	int getGridSizeY() {return h/gh;}
	int getGridWidth() {return gw;}
	int getGridHeight() {return gh;}
	void copy(int gx, int gy, int dx, int dy);
	void copy(int gx, int gy, int dx, int dy, int dw, int dh);
	void copy(int gx, int gy, int sx, int sy, int sw, int sh, int dx, int dy);
	void scale(int ncw, int nch);
};

#define ALIGN_X_LEFT	1
#define ALIGN_X_CENTER	2
#define ALIGN_X_RIGHT	4
#define ALIGN_Y_TOP	8
#define ALIGN_Y_CENTER	16
#define ALIGN_Y_BOTTOM	32

class Font {
protected:
	TTF_Font *font;
	SDL_Color clr;
	int align;
	int size;
public:
	Font();
	~Font();
	void free() {
		if (font) {
			TTF_CloseFont(font);
			font = 0;
		}
	}
	void load(const string& fname, int size);
	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void setColor(SDL_Color c);
	void setAlign(int a);
	int getSize() {return size;};
	int getLineHeight() {return TTF_FontLineSkip(font); }
	int getTextSize(const string& str, int *w, int *h) {return TTF_SizeText(font,str.c_str(),w,h);};
	int getCharWidth(char c) {
		int w, h;
		char str[2] = {c, 0};
		TTF_SizeText(font,str,&w,&h);
		return w;
	}
	void write(int x, int y, const string &str);
	void writeText(int x, int y, const string &text, int width);
};

/** easy log sdl error */
#define _logsdlerr() \
	fprintf(stderr,"ERROR: %s:%d: %s(): %s\n", \
			__FILE__, __LINE__, __FUNCTION__, SDL_GetError())

#endif /* SDL_H_ */