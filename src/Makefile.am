bin_PROGRAMS = iosplayer playerServer playerClient

iosplayer_SOURCES = main.c cli.c cli.h mux.c mux.h player.c player.h http_client.c http_client.h
iosplayer_CFLAGS = $(FFMPEG_CFLAGS) $(SDL2_CFLAGS) $(LIBUSBMUXD_CFLAGS) $(CURL_CFLAGS)
iosplayer_LDADD = $(FFMPEG_LIBS) $(SDL2_LIBS) $(LIBUSBMUXD_LIBS) $(CURL_LIBS)

playerServer_SOURCES = server_main.c cli.c cli.h mux.c mux.h net_server.c net_server.h http_client.c http_client.h
playerServer_CFLAGS = $(FFMPEG_CFLAGS) $(LIBUSBMUXD_CFLAGS) $(CURL_CFLAGS)
playerServer_LDADD = $(FFMPEG_LIBS) $(LIBUSBMUXD_LIBS) $(CURL_LIBS)

playerClient_SOURCES = client_main.c cli.c cli.h player.c player.h net_client.c net_client.h
playerClient_CFLAGS = $(FFMPEG_CFLAGS) $(SDL2_CFLAGS) -DPLAYER_CLIENT
playerClient_LDADD = $(FFMPEG_LIBS) $(SDL2_LIBS)
