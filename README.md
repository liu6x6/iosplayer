# iosplayer

[中文文档](README_CN.md)

## Overview

Inspired by `scrcpy`, `iosplayer` is a lightweight player designed to stream an iOS device's screen to a computer using FFmpeg and SDL2. It supports remote control functionalities like tap, swipe, and keyboard input by sending HTTP requests to a running WebDriverAgent instance on the device.

This project automatically detects and connects to the iOS device, removing the need for manual proxying.

## Features

- **Screen Mirroring**: Streams the device screen over USB.
- **Auto-Connection**: Automatically finds the connected iOS device and establishes a connection.
- **Remote Control**:
    - **Tap**: Click on the video window to simulate a tap on the device.
    - **Swipe/Pan**: Click, drag, and release to simulate a swipe gesture.
    - **Keyboard Input**: Type directly into the window to send keystrokes to the device. Supports regular text, backspace, and enter.

## Prerequisites

1.  **WebDriverAgent**: A running WebDriverAgent server on the iOS device to handle control commands. The player assumes it's accessible at `http://localhost:8100`.
2.  **usbmuxd**: The `usbmuxd` daemon must be running on the host machine to handle USB communication with the device.

## Build Instructions

### Dependencies

You need the development libraries for:
- FFmpeg (libavformat, libavcodec, libavutil, libswscale)
- SDL2
- libusbmuxd (version 2.0 or higher)
- libcurl

On macOS, you can install them using Homebrew:
```bash
brew install ffmpeg sdl2 libusbmuxd curl
```

### Compiling

Once the dependencies are installed, you can build the project using the standard autotools workflow:

```bash
./autogen.sh
./configure
make
```

## Usage

After a successful build, simply run the executable:

```bash
./src/iosplayer
```

The application will automatically connect to the first available iOS device on the default port (10001).

### Command-line Options

- `-p, --port <port_number>`: Specify a custom device port to connect to.
  ```bash
  ./src/iosplayer -p 12345
  ```
- `-o, --out <filename>`: Save the incoming H.264 stream to a file.
  ```bash
  ./src/iosplayer -o stream.h264
  ```
