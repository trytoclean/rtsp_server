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
```
### 待补充：
一次完整交互流程示例（ffplay命令 + 抓包截图/文本）

### 三个版本：
- v1:
在单机 + 虚拟机环境下，实现一个“能稳定播放”的 RTSP 服务器：
支持基础 RTSP 流程（OPTIONS/DESCRIBE/SETUP/PLAY），从本地文件拉取 H.264/AAC 媒体，封装成 RTP 推给客户端（如 ffplay），并支持多个客户端同时播放。
架构图（文字描述）：
> epoll 事件驱动 → Connection 收发字节流 → RTSP 模块解析/生成信令 → Media 模块提供帧数据 → RTP builder 封装并通过 UDP/TCP 发送给客户端。
- v2: 待实现的
性能版本（单机能力最大化）的项目级规划
在单机环境下，把 RTSP 服务器的“吞吐能力”和“资源利用率”做到可观测、可优化：在固定硬件上，支持尽可能多的并发播放会话，同时保持 CPU/内存占用在合理范围内，无明显卡顿。
1. 指标:
    1. 单机最大并发会话数：如 N 路 720p@30fps 播放
    2. CPU 占用：在 N 路并发下 < 80%
    3. 内存占用：控制在 M GB 内
    4. 平均帧发送间隔误差：如 ≤ 5ms（和理想 33ms 或 40ms 相比）
2. 核心方向
    1. 事件模型 & 线程模型 
    > 目标：避免所有阻塞操作卡住主 reactor。
    2. 内存与 Buffer 优化
    > 目标：降低内存分配和拷贝次数。
    3. 系统调用与 I/O 调优 
    > 目标：减少系统调用次数、提升数据通路效率。
    4. 性能测试与 Profiling 体系
    > 目标：不是“感觉变快了”，而是用数据证明。
3. 里程碑
    1. M1：搭好性能测试脚本，跑出“当前 v1 上的 baseline 数据”
    2. M2：完成线程模型 + Buffer 优化，baseline 提升一轮
    3. M3：完成系统调用与参数调优，形成第一版性能报告
- v3: 版本目标
> 将服务器部署到真实物理/云服务器上，在复杂网络环境（跨公网、NAT、丢包、抖动、弱网）下，保持播放体验可接受，并为后续运维与扩展打下基础。

1. 网络健壮性与协议完善
超时与心跳
- RTSP 会话超时（Session Timeout）
- 空闲连接回收
RTCP支持
- 发送 SR（Sender Report），正确绑定 NTP 时间戳和 RTP 时间戳

- 解析客户端 RR（Receiver Report），感知丢包、抖动
重传与错误处理策略
    对 UDP 丢包的态度：是完全忽略，还是做简单的“关键帧重发”?
多种传输模式
- v1的 UDP、RTP
-  TCP interleaved (弱网，nat环境更常见)



