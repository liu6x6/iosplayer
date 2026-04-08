# iosplayer

[English](README.md)

## 项目概览

`iosplayer` 的灵感来源于 `scrcpy`，是一个轻量级的播放器。它使用 FFmpeg 和 SDL2 将 iOS 设备的屏幕内容流式传输到电脑上，并通过向设备上运行的 WebDriverAgent 实例发送 HTTP 请求，来支持远程控制功能，如点击、滑动和键盘输入。

本项目能够自动检测并连接到 iOS 设备，无需手动设置端口转发。

## 功能特性

- **屏幕镜像**: 通过 USB 流式传输设备屏幕。
- **自动连接**: 自动查找已连接的 iOS 设备并建立通信。
- **远程控制**:
    - **点击**: 在视频窗口上单击，以模拟在设备上的点击操作。
    - **滑动/拖拽**: 在窗口中按下、拖动并释放鼠标，以模拟滑动操作。
    - **键盘输入**: 直接在窗口中输入，即可将文本发送到设备。支持常规文本、退格键和回车键。

## 环境要求

1.  **WebDriverAgent**: iOS 设备上需要运行一个 WebDriverAgent 服务，用于处理控制命令。本项目假定其服务地址为 `http://localhost:8100`。
2.  **usbmuxd**: 主机上必须运行 `usbmuxd` 守护进程，以处理与设备的 USB 通信。

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

安装完依赖后，您可以使用标准的 autotools 流程来构建项目：

```bash
./autogen.sh
./configure
make
```

## 使用方法

成功编译后，直接运行可执行文件即可：

```bash
./src/iosplayer
```

程序将会自动连接到第一个可用的 iOS 设备，并使用默认端口（10001）。

### 命令行选项

- `-p, --port <端口号>`: 指定一个自定义的设备端口进行连接。
  ```bash
  ./src/iosplayer -p 12345
  ```
- `-o, --out <文件名>`: 将输入的 H.264 视频流保存到文件。
  ```bash
  ./src/iosplayer -o stream.h264
  ```
