# 项目架构说明

本文档旨在说明 `iosplayer` 项目的软件架构、模块划分以及执行流程。

## 1. 单体模式 (Standalone Mode)

这是 `iosplayer` 程序的默认工作模式。

### 1.1 模块关系图 (Component Diagram)

项目被划分为四个核心模块和一个主入口：

- **`main`**: 程序的入口，负责初始化和协调其他模块。
- **`cli`**: 命令行接口模块，负责解析用户输入的参数。
- **`mux`**: USB多路复用模块，封装了 `libusbmuxd` 的功能，用于与iOS设备建立连接。
- **`player`**: 播放器核心模块，封装了 `FFmpeg` 和 `SDL` 的功能，负责视频流的解码、渲染以及用户交互（点击、滑动和键盘输入）。
- **`http_client`**: HTTP客户端模块，封装了 `libcurl` 的功能，用于向 WebDriverAgent 发送控制命令（如 `tap`, `drag`, `keys`）。

```mermaid
graph TD
    subgraph "`iosplayer` (Standalone)"
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

### 1.2 执行流程图 (Sequence Diagram)

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

---

## 2. 客户端/服务器模式 (Client/Server Mode)

此模式将功能拆分为两个独立的程序：`playerServer` 和 `playerClient`。

- **`playerServer`**: 运行在与iOS设备直连的机器上。它负责从设备获取视频流，并将其通过TCP网络转发给客户端。同时，它接收来自客户端的控制命令，并将其转化为对 WebDriverAgent 的HTTP请求。
- **`playerClient`**: 运行在任意机器上。它连接到 `playerServer`，接收并播放视频流，同时将用户的输入操作（点击、滑动、键盘）发送回服务器进行处理。

### 2.1 模块关系与复用

```mermaid
graph TD
    subgraph "`playerServer`"
        PS_Main[server_main] --> Mux[mux]
        PS_Main --> Net_S[net_server]
        Net_S -- "控制命令" --> HTTP[http_client]
    end

    subgraph "`playerClient`"
        PC_Main[client_main] --> Player[player]
        PC_Main --> Net_C[net_client]
        Player -- "用户输入" --> Net_C
    end

    subgraph "Shared Modules"
        Mux
        Player
        HTTP
    end

    Net_C -- "TCP连接" --> Net_S
    Net_S -- "H.264 视频流" --> Net_C
    Net_C -- "控制命令" --> Net_S

    Mux --> Dep_Mux[libusbmuxd]
    Player --> Dep_FFmpeg[FFmpeg]
    Player --> Dep_SDL[SDL2]
    HTTP --> Dep_Curl[libcurl]

    style `playerServer` fill:#f9f,stroke:#333,stroke-width:2px
    style `playerClient` fill:#ccf,stroke:#333,stroke-width:2px
    style `Shared Modules` fill:#9cf,stroke:#333,stroke-width:2px
```

### 2.2 C/S 交互流程图

```mermaid
sequenceDiagram
    participant User
    participant playerClient
    participant playerServer
    participant WDA
    participant iOS_Device

    playerServer->>iOS_Device: 通过 libusbmuxd 获取 H.264 视频流
    playerServer->>playerServer: 启动 TCP 服务器，等待连接

    User->>playerClient: ./playerClient --ip <server_ip>
    playerClient->>playerServer: 建立 TCP 连接
    playerServer-->>playerClient: 开始转发 H.264 视频流

    loop 视频播放
        playerClient->>playerClient: 接收视频数据，解码并渲染 (FFmpeg, SDL2)
    end

    User->>playerClient: 在窗口中进行操作 (点击/滑动/键盘)
    playerClient->>playerClient: 捕获 SDL 事件
    playerClient->>playerServer: 通过 TCP 发送控制命令 (例如, "TAP 100 200")
    
    playerServer->>http_client: 调用相应的请求函数
    http_client->>WDA: 发送 HTTP 请求 (POST /wda/tap, etc.)
    WDA-->>http_client: HTTP 200 OK

    User->>playerClient: 关闭窗口
    playerClient->>playerServer: 断开 TCP 连接
    playerServer->>playerServer: 停止推流，继续等待新连接
```
