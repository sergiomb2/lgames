EXTRA_DIST = ABOUT \
back0.png back1.png back2.png back3.png back4.png back5.png \
ball.png bricks.png explosions_dark.png explosions.png \
extras.png f_frame.png f_game.png fr_left.png fr_right.png \
fr_top.png f_small_red.png f_small_white.png f_small_yellow.png \
f_tiny_black.png f_white.png f_yellow.png life.png \
menuback.png paddle.png shine.png shot.png warp.png weapon.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datdir)/themes/Classic
	@for file in $(EXTRA_DIST); do\
	  $(INSTALL_DATA) $$file $(DESTDIR)$(datdir)/themes/Classic/$$file;\
	done

