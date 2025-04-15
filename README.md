# iosplayer
a small stream play using ffmpeg and SDL2, which i use it to test the iOS h264 stream 


# how to use
```
# you need have a tcp h264 stream on you iOS devices
./iproxy 10001 10001
./iosplayer -p 10001  # play the tcp stream on 10001
```

# build
## mac & linux
* run ./autogen.sh
* and then make && make install

## windows
* install msys2 first
* pacman -S mingw-w64-ucrt-x86_64-ffmpeg


## use gcc to build
```
gcc -o iosplayer iosplayer.c \
    `pkg-config --cflags --libs libavformat libavcodec libavutil libswscale sdl2`

```
