bin_PROGRAMS = iosplayer
iosplayer_SOURCES = main.c cli.c cli.h mux.c mux.h player.c player.h http_client.c http_client.h

iosplayer_CFLAGS = $(FFMPEG_CFLAGS) $(SDL2_CFLAGS) $(LIBUSBMUXD_CFLAGS) $(CURL_CFLAGS)
iosplayer_LDADD = $(FFMPEG_LIBS) $(SDL2_LIBS) $(LIBUSBMUXD_LIBS) $(CURL_LIBS)
