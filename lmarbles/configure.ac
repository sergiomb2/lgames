#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([lmarbles],[1.0.9])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT([external])

# Checks for libraries.
AC_CHECK_LIB(m, main,, AC_MSG_ERROR(lib math is needed))
AC_CHECK_LIB(SDL, main,, AC_MSG_ERROR(lib SDL is needed))
AC_CHECK_LIB(SDL_mixer, main,, AC_MSG_ERROR(lib SDL mixer is needed))

CFLAGS="$CFLAGS -DSDL_1_1_5"

dnl check ascii-flag
ascii_flag=""
AC_ARG_ENABLE( ascii,
[  --enable-ascii          Enables 'save as ascii' option (for big-endian)], ascii_flag="-DASCII")
AC_SUBST(ascii_flag)

# Flags
datdir=$datadir/$PACKAGE
profdir=$localstatedir
configdir="~"
debugflags=""
AC_ARG_ENABLE( install,
[  --disable-install       No installation. Start from source directory.], 
datdir="." configdir=".")
AC_ARG_ENABLE( debug,
[  --enable-debug       Add debug symbols, no optimization.], 
debugflags="-O0 -g")
AC_SUBST(datdir)
AC_SUBST(profdir)

SDL_CFLAGS=$(sdl-config --cflags)
SDL_LIBS=$(sdl-config --libs)

CFLAGS="$CFLAGS $SDL_CFLAGS -Wall $debugflags"
CFLAGS="$CFLAGS -DLOCALEDIR=\\\"$localedir\\\" -DCONFIGDIR=\\\"$configdir\\\" -DSRC_DIR=\\\"$datdir\\\" -DPRF_DIR=\\\"$profdir\\\""
LIBS="$LIBS $SDL_LIBS"

# Makefiles
AC_CONFIG_FILES([Makefile po/Makefile.in
src/Makefile
src/gfx/Makefile
src/levels/Makefile
src/manual/Makefile
src/sounds/Makefile
])
AC_OUTPUT



