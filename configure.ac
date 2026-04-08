AC_INIT([scrcpy-like-player], [1.0], [you@example.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for required libraries
PKG_CHECK_MODULES([FFMPEG], [libavformat libavcodec libavutil libswscale])
PKG_CHECK_MODULES([SDL2], [sdl2])
PKG_CHECK_MODULES([LIBUSBMUXD], [libusbmuxd-2.0])
PKG_CHECK_MODULES([CURL], [libcurl])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
