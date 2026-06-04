AUTOMAKE_OPTIONS = foreign

SUBDIRS = po src

EXTRA_DIST = config.rpath \
	     AUTHORS COPYING ChangeLog INSTALL README \
	     README-SDL.txt TODO lmarbles.6 \
	     lmarbles.png lmarbles.desktop

desktopdir = $(datadir)/applications
desktop_DATA = lmarbles.desktop
icondir = $(datadir)/icons/hicolor/256x256/apps
icon_DATA = lmarbles.png

man_MANS = lmarbles.6

ACLOCAL_AMFLAGS = -I m4
