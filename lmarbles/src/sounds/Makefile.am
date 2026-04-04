EXTRA_DIST = alarm.wav arrow.wav click.wav explode.wav \
select.wav stop.wav teleport.wav score.wav

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datdir)/sounds
	@for file in $(EXTRA_DIST); do\
	  $(INSTALL_DATA) $$file $(DESTDIR)$(datdir)/sounds/$$file;\
	done
