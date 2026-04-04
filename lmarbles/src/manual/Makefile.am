EXTRA_DIST = manual.html arrow.jpg arrow2.jpg \
crumble.jpg teleport.jpg marbles.jpg

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datdir)/manual
	@for file in $(EXTRA_DIST); do\
	  $(INSTALL_DATA) $$file $(DESTDIR)$(datdir)/manual/$$file;\
	done
