# RTSP 高性能高并发服务器
## RTSP 方法
    对RTSP支持 OPTIONS SUCRIBE SETUP PLAY PAUSE TEARDOWN

## 如何构建/运行  
    CMake 3.xx
    linux 环境

    ```bash
    #configure
    cmake -B build
    #complie
    cmake --build build
    #./build/Server
    ```
## 目录结构说明
```txt
    .
├── build
├── doc
├── include
├── src
└── temp
```
### 待补充：
一次完整交互流程示例（ffplay命令 + 抓包截图/文本）
