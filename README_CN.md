# iosplayer

[English](README.md)

## 项目概览

本项目提供了流式传输和远程控制iOS设备的工具。它包含三个主要程序：

- **`iosplayer`**: 一个独立的应用程序，用于在与设备连接的同一台机器上进行屏幕镜像和控制。
- **`playerServer`**: 一个服务器应用程序，它连接到iOS设备，获取视频流，并通过TCP转发出去。它还接收来自客户端的控制命令，并将其发送到设备的WebDriverAgent。
- **`playerClient`**: 一个客户端应用程序，它连接到`playerServer`以接收和显示视频流。它会捕捉用户的输入（鼠标、键盘）并将其发送回服务器执行。

## 功能特性

- **屏幕镜像**: 通过USB或TCP流式传输设备屏幕。
- **自动连接**: 独立播放器和服务器能自动查找已连接的iOS设备。
- **客户端/服务器架构**: 允许在与设备分离的另一台机器上运行播放器。
- **远程控制**:
    - **点击**: 在视频窗口上单击以模拟点击。
    - **滑动/拖拽**: 点击、拖动并释放以模拟滑动。
    - **键盘输入**: 直接在窗口中输入以发送按键。

## 环境要求

1.  **WebDriverAgent**: 在iOS设备上运行的WebDriverAgent服务。`iosplayer` 和 `playerServer` 都需要它。
2.  **usbmuxd**: 主机上必须运行`usbmuxd`守护进程以进行USB通信。

## 构建指南

### 依赖库

您需要安装以下库的开发版本：
- FFmpeg (libavformat, libavcodec, libavutil, libswscale)
- SDL2
- libusbmuxd (2.0 或更高版本)
- libcurl

在 macOS 上，您可以使用 Homebrew 来安装它们：
```bash
brew install ffmpeg sdl2 libusbmuxd curl
```

### 编译

```bash
./autogen.sh
./configure
make
```

## 使用方法

### 单体模式

在与设备连接的机器上运行 `iosplayer`，这是一个一体化的解决方案。

```bash
./src/iosplayer
```

### 客户端/服务器模式

**1. 启动服务器:**
在与iOS设备连接的机器上，运行 `playerServer`。
```bash
./src/playerServer
```

**2. 启动客户端:**
在另一台机器上，运行 `playerClient`。它默认会连接到 `127.0.0.1:12345` 的服务器。您可以使用命令行选项来指定不同的IP。
```bash
./src/playerClient
```

### 命令行选项

- `-p, --port <端口号>`: (用于 `iosplayer`) 指定一个自定义的设备端口。
- `-o, --out <文件名>`: (用于 `iosplayer`) 将视频流保存到文件。
- *客户端的特定选项（如指定服务器IP和端口）可以后续添加到 `cli.c` 中。*
