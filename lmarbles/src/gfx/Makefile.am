EXTRA_DIST = \
board.bmp f_red.bmp f_small.bmp f_white.bmp \
f_yellow.bmp figure.bmp lights.bmp logo.bmp \
m_frame.bmp m_sframe.bmp marbles.bmp tiny.sdlfnt  \
title.bmp win_icon.bmp \
stone/arrow_d.bmp stone/arrow_l.bmp stone/arrow_r.bmp \
stone/arrow_u.bmp stone/background.bmp stone/barrier_lr.bmp \
stone/barrier_ud.bmp stone/floor.bmp stone/teleport0.bmp \
stone/teleport1.bmp stone/teleport2.bmp \
stone/teleport3.bmp stone/wall.bmp \
stone/wall_crumble.bmp \
metal/arrow_d.bmp metal/arrow_l.bmp metal/arrow_r.bmp \
metal/arrow_u.bmp metal/background.bmp metal/barrier_lr.bmp \
metal/barrier_ud.bmp metal/floor.bmp metal/teleport0.bmp \
metal/teleport1.bmp metal/teleport2.bmp \
metal/teleport3.bmp metal/wall.bmp \
metal/wall_crumble.bmp \
wood/arrow_d.bmp wood/arrow_l.bmp wood/arrow_r.bmp \
wood/arrow_u.bmp wood/background.bmp wood/barrier_lr.bmp \
wood/barrier_ud.bmp wood/floor.bmp wood/teleport0.bmp \
wood/teleport1.bmp wood/teleport2.bmp \
wood/teleport3.bmp wood/wall.bmp \
wood/wall_crumble.bmp 

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datdir)/gfx
	$(mkinstalldirs) $(DESTDIR)$(datdir)/gfx/metal
	$(mkinstalldirs) $(DESTDIR)$(datdir)/gfx/stone
	$(mkinstalldirs) $(DESTDIR)$(datdir)/gfx/wood
	@for file in $(EXTRA_DIST); do\
	  $(INSTALL_DATA) $$file $(DESTDIR)$(datdir)/gfx/$$file;\
	done

