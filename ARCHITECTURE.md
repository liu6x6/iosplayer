# 项目架构说明

本文档旨在说明 `iosplayer` 项目的软件架构、模块划分以及执行流程。

## 1. 模块关系图 (Component Diagram)

项目被划分为四个核心模块和一个主入口：

- **`main`**: 程序的入口，负责初始化和协调其他模块。
- **`cli`**: 命令行接口模块，负责解析用户输入的参数。
- **`mux`**: USB多路复用模块，封装了 `libusbmuxd` 的功能，用于与iOS设备建立连接。
- **`player`**: 播放器核心模块，封装了 `FFmpeg` 和 `SDL` 的功能，负责视频流的解码、渲染以及用户交互（点击、滑动和键盘输入）。
- **`http_client`**: HTTP客户端模块，封装了 `libcurl` 的功能，用于向 WebDriverAgent 发送控制命令（如 `tap`, `drag`, `keys`）。

```mermaid
graph TD
    subgraph Application
        A[main] --> B[cli]
        A --> C[mux]
        A --> D[player]
        D -- "用户手势和键盘输入" --> E[http_client]
    end

    subgraph Dependencies
        C --> F[libusbmuxd]
        D --> G[FFmpeg]
        D --> H[SDL2]
        E --> I[libcurl]
    end

    B -- "解析参数" --> A
    C -- "返回文件描述符 (fd)" --> A
    A -- "传递 fd" --> D

    style Application fill:#f9f,stroke:#333,stroke-width:2px
    style Dependencies fill:#ccf,stroke:#333,stroke-width:2px
```

## 2. 执行流程图 (Sequence Diagram)

下图展示了程序从启动到响应用户各种输入的完整执行流程。

```mermaid
sequenceDiagram
    participant User
    participant main
    participant player
    participant http_client
    participant WDA

    User->>main: ./iosplayer
    main->>player: run_player(fd)
    Note right of player: 启动视频解码和渲染循环

    alt 手势输入 (点击/滑动)
        User->>player: 按下/释放鼠标
        player->>player: 判断手势类型 (tap/swipe)
        player->>http_client: send_tap_request() 或 send_swipe_request()
        http_client->>WDA: POST /wda/tap 或 /wda/drag
        WDA-->>http_client: HTTP 200 OK
    end

    alt 键盘输入
        User->>player: 在窗口中输入文本或按键
        player->>player: 捕获 SDL_TEXTINPUT 或 SDL_KEYDOWN 事件
        player->>http_client: send_keys_request(text)
        http_client->>WDA: POST /wda/keys
        WDA-->>http_client: HTTP 200 OK
    end

    User->>player: 关闭窗口 (SDL_QUIT event)
    player-->>main: 播放循环结束
    main-->>User: 程序退出
```
