# iosplayer

[中文文档](README_CN.md)

## Overview

This project provides tools to stream an iOS device's screen and control it remotely. It consists of three main programs:

- **`iosplayer`**: A standalone application for mirroring and controlling the device on the same machine.
- **`playerServer`**: A server application that connects to the iOS device, grabs the video stream, and forwards it over TCP. It also receives control commands from a client and sends them to the device's WebDriverAgent.
- **`playerClient`**: A client application that connects to `playerServer` to receive the video stream and display it. It captures user input (mouse, keyboard) and sends it back to the server for execution.

## Features

- **Screen Mirroring**: Streams the device screen over USB or TCP.
- **Auto-Connection**: The standalone player and server automatically find the connected iOS device.
- **Client/Server Architecture**: Allows running the player on a separate machine from the one connected to the device.
- **Remote Control**:
    - **Tap**: Click on the video window to simulate a tap.
    - **Swipe/Pan**: Click, drag, and release to simulate a swipe.
    - **Keyboard Input**: Type directly into the window to send keystrokes.

## Prerequisites

1.  **WebDriverAgent**: A running WebDriverAgent server on the iOS device. Required for `iosplayer` and `playerServer`.
2.  **usbmuxd**: The `usbmuxd` daemon must be running on the host machine for USB communication.

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

```bash
./autogen.sh
./configure
make
```

## Usage

### Standalone Mode

Run `iosplayer` for an all-in-one solution on the machine connected to the device.

```bash
./src/iosplayer
```

### Client/Server Mode

**1. Start the Server:**
On the machine connected to the iOS device, run `playerServer`.
```bash
./src/playerServer
```

**2. Start the Client:**
On another machine, run `playerClient`. It will connect to the server at `127.0.0.1:12345` by default. You can use command-line options to specify a different IP.
```bash
./src/playerClient
```

### Command-line Options

- `-p, --port <port>`: (For `iosplayer`) Specify a custom device port.
- `-o, --out <filename>`: (For `iosplayer`) Save the stream to a file.
- *Client-specific options can be added to `cli.c` to specify server IP and port.*
