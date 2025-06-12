# iosplayer
[中文文档](README_CN.md)

this project get inspiration from scrcpy.

a small stream player using ffmpeg and SDL2, which i use it to test the iOS h264 stream. support Mac/linux/windows.
need wotk with [WebDriverAgent](https://github.com/liu6x6/WebDriverAgent)

there is a h264 stream server on port 10001. 


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
