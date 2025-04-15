AC_INIT([scrcpy-like-player], [1.0], [you@example.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for required libraries
PKG_CHECK_MODULES([FFMPEG], [libavformat libavcodec libavutil libswscale])
PKG_CHECK_MODULES([SDL2], [sdl2])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
