AUTOMAKE_OPTIONS = foreign

SUBDIRS = po src

EXTRA_DIST = config.rpath \
	     AUTHORS COPYING ChangeLog INSTALL README \
	     README-SDL.txt TODO lmarbles.6 \
	     lmarbles.png lmarbles.desktop.in

desktopdir = $(datadir)/applications
desktop_DATA = lmarbles.desktop
icondir = $(datadir)/icons
icon_DATA = lmarbles.png

man_MANS = lmarbles.6

# We can't use static desktop file because of icon location. Is this 
# the right way to handle it? At least it works... as long as $datadir
# does not contain plus signs. :-)
lmarbles.desktop:
	@sed -e "s+Icon=+Icon=$(datadir)/icons/lmarbles.png+" \
	< lmarbles.desktop.in > lmarbles.desktop
	
ACLOCAL_AMFLAGS = -I m4
