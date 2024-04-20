AUTOMAKE_OPTIONS = foreign
SUBDIRS = po libgame src 
EXTRA_DIST = m4  gettext.h config.rpath \
	TODO COPYING README Changelog CROSSCOMPILE \
	lbreakouthd.desktop lbreakouthd256.png lbreakouthd48.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/icons
	$(mkinstalldirs) $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) lbreakouthd256.png $(DESTDIR)$(datadir)/icons/lbreakouthd.png
	$(INSTALL_DATA) lbreakouthd.desktop $(DESTDIR)$(datadir)/applications
	

uninstall-local:
	rm $(DESTDIR)$(datadir)/icons/lbreakouthd.png
	rm $(DESTDIR)$(datadir)/applications/lbreakouthd.desktop

ACLOCAL_AMFLAGS = -I m4
