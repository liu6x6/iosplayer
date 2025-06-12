# iosplayer
[English](README.md)

本项目灵感来自 scrcpy.

一个非常轻量的 iOS h264 player. 类似于 QuickTime. 使用 ffmpeg 和 SDL2. 支持 Mac/linux/Windows,需要和[WebDriverAgent](https://github.com/liu6x6/WebDriverAgent)一起使用.

WebDriverAgent 在端口 10001 上启动了一个 H264 的 TCP server. 


# 使用流程
```
# 需要首先启动 WebdriverAgent
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
