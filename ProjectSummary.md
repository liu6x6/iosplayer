# 项目总结: iosplayer

## 1. 项目简介

`iosplayer` 是一个轻量级的视频流播放器，其灵感来源于 `scrcpy`。它专为播放来自 iOS 设备的 H.264 视频流而设计，功能上类似于 macOS 的 QuickTime 播放器。

该项目需要与 [WebDriverAgent](https://github.com/liu6x6/WebDriverAgent) 配合使用，`WebDriverAgent` 在 iOS 设备上启动一个 TCP 服务（默认端口 10001），提供实时的 H.264 视频流。`iosplayer` 则连接到此服务，解码并显示视频。

项目支持跨平台运行，包括 macOS, Linux, 和 Windows。

## 2. 技术栈

*   **核心语言**: C
*   **多媒体处理**: [FFmpeg](https://ffmpeg.org/) (libavformat, libavcodec, libswscale, libavutil) - 用于网络流的接收、解封装和视频解码。
*   **图形渲染**: [SDL2 (Simple DirectMedia Layer)](https://www.libsdl.org/) - 用于创建窗口、处理用户输入以及将解码后的视频帧渲染到屏幕上。

## 3. 核心功能

代码主要逻辑位于 `src/main.c` 文件中，其执行流程如下：

1.  **参数解析**: 程序启动时，会解析命令行参数，允许用户指定IP地址 (`-i` 或 `--ip`)、端口号 (`-p` 或 `--port`) 以及一个可选的输出文件路径 (`-o` 或 `--out`)，用于将原始视频流保存到本地。
2.  **网络连接**: 使用 FFmpeg 的 `avformat_network_init` 和 `avformat_open_input` 函数，连接到由 `WebDriverAgent` 在 iOS 设备上提供的 TCP 视频流。
3.  **流信息检索**: 查找并定位到视频流（`AVMEDIA_TYPE_VIDEO`）。
4.  **解码器初始化**: 根据视频流的编码格式（应为 H.264），找到并初始化对应的 FFmpeg 解码器。
5.  **SDL环境设置**: 初始化 SDL 视频子系统，创建一个标题为 "iOS iPhone" 的窗口，并设置渲染器和用于显示视频的流式纹理（`SDL_Texture`）。
6.  **主循环 (解码与渲染)**:
    *   通过 `av_read_frame` 从 TCP 流中循环读取数据包（`AVPacket`）。
    *   如果用户指定了输出文件，则将原始数据包写入该文件。
    *   使用 `avcodec_send_packet` 和 `avcodec_receive_frame` 将数据包送入解码器并获取解码后的视频帧（`AVFrame`）。
    *   使用 `sws_scale` 将解码后的视频帧从其原生像素格式转换为渲染所需的 YUV420P 格式。
    *   通过 `SDL_UpdateYUVTexture` 更新 SDL 纹理内容。
    *   调用 `SDL_RenderCopy` 和 `SDL_RenderPresent` 将纹理渲染到窗口上。
7.  **事件处理**: 监听 SDL 事件，如 `SDL_QUIT`，以便在用户关闭窗口时优雅地退出程序。
8.  **资源清理**: 在程序退出前，释放所有由 FFmpeg 和 SDL 分配的资源，包括上下文、帧、数据包、纹理等，防止内存泄漏。

## 4. 如何使用

1.  在 iOS 设备上启动 `WebDriverAgent`。
2.  通过 `iproxy` 将设备的端口转发到本地：
    ```bash
    ./iproxy 10001 10001
    ```
3.  运行 `iosplayer` 并指定端口：
    ```bash
    ./iosplayer -p 10001
    ```

## 5. 如何构建

### macOS & Linux

1.  执行 `./autogen.sh` 生成配置脚本。
2.  执行 `make && make install` 进行编译和安装。

### Windows

1.  安装 `msys2`。
2.  在 `msys2` 环境中，使用 `pacman` 安装 FFmpeg 依赖：
    ```bash
    pacman -S mingw-w64-ucrt-x86_64-ffmpeg
    ```

### 通用 GCC 构建方法

也可以直接使用 `gcc` 和 `pkg-config` 进行编译：

```bash
gcc -o iosplayer src/main.c \
    `pkg-config --cflags --libs libavformat libavcodec libavutil libswscale sdl2`
```
