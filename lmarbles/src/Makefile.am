bin_PROGRAMS = lmarbles
lmarbles_SOURCES = audio.c cfg.c dynlist.c file.c game.c levels.c main.c menu.c \
profile.c sdl.c tools.c audio.h cfg.h dynlist.h file.h game.h levels.h \
menu.h profile.h sdl.h tools.h	

EXTRA_DIST = empty.prfs

SUBDIRS = gfx levels manual sounds

install-data-local:
	$(mkinstalldirs) $(DESTDIR)/$(datdir)
	$(mkinstalldirs) $(DESTDIR)$(profdir)
	if ! test -f $(DESTDIR)$(profdir)/lmarbles.prfs; then \
		$(INSTALL_DATA) -m 666 empty.prfs $(DESTDIR)$(profdir)/lmarbles.prfs; \
	fi;

uninstall-local:
	rm -rf $(DESTDIR)$(datdir)
	rm -f $(DESTDIR)$(profdir)/lmarbles.prfs

clean:
	rm -f *.o lmarbles

