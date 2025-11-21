
## 一、RTSP 请求报文结构

典型 RTSP 请求 = **起始行 + 首部行 + 空行 + 可选消息体**

### 1. 起始行（Request-Line）

格式：

```text
<METHOD> <RTSP-URI> RTSP/1.0\r\n
```

例如：

```text
DESCRIBE rtsp://127.0.0.1:8554/test.sdp RTSP/1.0\r\n
```

- `METHOD`：常见有 `OPTIONS` / `DESCRIBE` / `SETUP` / `PLAY` / `PAUSE` / `TEARDOWN` / `ANNOUNCE` / `RECORD` / `GET_PARAMETER` / `SET_PARAMETER` 等
- `RTSP-URI`：一般是 `rtsp://ip:port/stream_name`
- `RTSP/1.0`：版本

### 2. 首部行（Headers）

每行一个 `Key: Value`，结尾 `\r\n`，类似 HTTP。比如：

```text
CSeq: 2\r\n
User-Agent: FFplay/6.1\r\n
Accept: application/sdp\r\n
```

常见请求里你会遇到的 header：

- `CSeq`：**序列号**，客户端每发一个请求 CSeq+1，服务器响应要原样带回。
- `Transport`（主要出现在 SETUP）
  - 描述客户端想怎么收 RTP：UDP 还是 TCP interleaved，端口是多少。
- `Session`（出现在除 SETUP 外的状态相关请求）
  - 用于标识一个 RTSP 会话，由 server 在 SETUP 响应里分配；之后 PLAY/PAUSE/TEARDOWN 都要带。
- `Range`（PLAY 等）
  - 指定播放区间，例如 `Range: npt=0.000-`。
- `User-Agent`、`Accept` 等可先不用太在意。

### 3. 空行

`"\r\n"` 一行，标记**首部结束**。后面如果有 body，就在空行之后。

### 4. 消息体（Body，可选）

- 对于 `DESCRIBE` 的响应，body 是 **SDP** 描述；
- 对于 `ANNOUNCE`/`SET_PARAMETER` 等，body 也可能携带 SDP 或参数。

请求里一般你可以**先假设没有 body**，早期实现可以只处理 header 部分。

------

## 二、RTSP 响应报文结构

响应 = **状态行 + 首部行 + 空行 + 可选消息体**

### 1. 状态行（Status-Line）

格式：

```text
RTSP/1.0 <Status-Code> <Reason-Phrase>\r\n
```

例如：

```text
RTSP/1.0 200 OK\r\n
```

常见状态码：

- `200 OK`：成功
- `400 Bad Request`
- `401 Unauthorized`
- `404 Not Found`
- `454 Session Not Found`
- `455 Method Not Valid in This State`
- `461 Unsupported Transport`
- `500 Internal Server Error`

早期你可以只实现 `200` + 少量错误码。

### 2. 首部行（Headers）

和请求类似，也是 `Key: Value`。常见：

```text
CSeq: 2\r\n
Date: Wed, 21 Nov 2025 10:00:00 GMT\r\n
Server: MySimpleRTSP/0.1\r\n
```

需要重点关注：

- **必须带回请求中的 `CSeq`**（一一对应）
- `Session: xxxx`（SETUP 之后的响应，分配 session id）
- `Transport: ...`（SETUP 响应时，确认服务器采用的传输参数）
- 对于 DESCRIBE 响应：
  - `Content-Type: application/sdp`
  - `Content-Length: <len>`

### 3. 空行

同样 `\r\n` 一行。

### 4. 消息体（Body）

- **DESCRIBE 响应**：SDP 文本
- 少数其他响应可能有 body，一开始先只管 DESCRIBE 的 SDP。

------

## 三、不同请求类型，服务器要做的事情

下面按你现在最需要的那几个方法来讲：

### 1. OPTIONS

**作用**：告诉客户端这个 URL / 服务器支持哪些方法。

**请求示例：**

```text
OPTIONS rtsp://127.0.0.1:8554/test RTSP/1.0
CSeq: 1
User-Agent: LibVLC/3.0.18
```

**服务器需要做什么：**

1. 解析起始行，识别是 `OPTIONS`。
2. 读到 `CSeq`。
3. 构造响应，至少包含：

```text
RTSP/1.0 200 OK
CSeq: 1
Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE
```

> 逻辑上比较简单，是你可以用来“打通请求解析到响应构造”的第一步。

------

### 2. DESCRIBE

**作用**：客户端向服务器要 SDP，了解媒体信息（codec、端口、payload type 等）。

**请求示例：**

```text
DESCRIBE rtsp://127.0.0.1:8554/test.sdp RTSP/1.0
CSeq: 2
Accept: application/sdp
```

**服务器需要做什么：**

1. 解析 URI，找到对应的“流”（比如名字 `test.sdp` 或 `test`）。

2. 如果不存在，返回 `404 Not Found`。

3. 存在的话，准备一段 SDP 文本（可以先写死）：

   ```text
   v=0
   o=- 0 0 IN IP4 127.0.0.1
   s=No Name
   c=IN IP4 0.0.0.0
   t=0 0
   m=video 0 RTP/AVP 96
   a=rtpmap:96 H264/90000
   ```

4. 计算 `Content-Length`，构造响应：

   ```text
   RTSP/1.0 200 OK
   CSeq: 2
   Content-Type: application/sdp
   Content-Length: <len>
   
   v=0
   o=- ...
   ...
   ```

> DESCRIBE 让你练习：**根据 URL 查找资源 + 生成 SDP + 带 Content-Length 的响应**。

------

### 3. SETUP

**作用**：为某个 track 建立传输通道（UDP 端口或 TCP interleaved），并创建 Session。

**请求示例（UDP）：**

```text
SETUP rtsp://127.0.0.1:8554/test.sdp/trackID=1 RTSP/1.0
CSeq: 3
Transport: RTP/AVP;unicast;client_port=5000-5001
```

**服务器需要做什么（你现在可以先做简化版）：**

1. 解析 `Transport` 字段：

   - 看是 `RTP/AVP;unicast;client_port=5000-5001` → RTP over UDP
   - 如果未来要支持 interleaved，会看到 `RTP/AVP/TCP;interleaved=0-1` 之类

2. 给这个请求创建一个 **Session**：

   - 生成 session id（比如简单用递增整数或随机数转字符串）
   - 在服务器内部维护 `session_id -> 会话信息`（客户端地址、端口、媒体信息、状态等）

3. UDP 模式：

   - 记录客户端的 `client_port`，准备往这两个端口发 RTP/RTCP。

4. 构造响应：

   ```text
   RTSP/1.0 200 OK
   CSeq: 3
   Session: 12345678;timeout=60
   Transport: RTP/AVP;unicast;client_port=5000-5001;server_port=6000-6001
   ```

   其中 `server_port` 可以由你分配。

> SETUP 让你开始设计：**Session 管理** + **传输参数（UDP/TCP）配置**。

------

### 4. PLAY

**作用**：告诉服务器，开始按某个 Range 发送 RTP。

**请求示例：**

```text
PLAY rtsp://127.0.0.1:8554/test.sdp RTSP/1.0
CSeq: 4
Session: 12345678
Range: npt=0.000-
```

**服务器需要做什么：**

1. 根据 `Session` 找到会话。

   - 找不到 → `454 Session Not Found`

2. 修改会话状态为 `PLAYING`。

3. 如果你已经有一个线程 / 定时器负责按 `Session` 向客户端持续发 RTP，则这里可以：

   - 为该 session 启动推流
   - 或者设置一个“开始发送”的标志

4. 构造响应：

   ```text
   RTSP/1.0 200 OK
   CSeq: 4
   Session: 12345678
   Range: npt=0.000-
   ```

> 在最初版本，可以只做**状态切换 + 响应**；真正发 RTP 可以下一阶段实现。

------

### 5. PAUSE

**作用**：暂停播放（停止发 RTP，但 Session 保留）。

**请求示例：**

```text
PAUSE rtsp://127.0.0.1:8554/test.sdp RTSP/1.0
CSeq: 5
Session: 12345678
```

**服务器需要做什么：**

1. 找到 Session。
2. 将状态从 `PLAYING` → `PAUSED`，并停止发 RTP。
3. 回复 `200 OK`。

------

### 6. TEARDOWN

**作用**：结束会话，释放资源。

**请求示例：**

```text
TEARDOWN rtsp://127.0.0.1:8554/test.sdp RTSP/1.0
CSeq: 6
Session: 12345678
```

**服务器需要做什么：**

1. 找到 Session。
2. 停止所有 RTP 发送、关闭 UDP/TCP 相关资源。
3. 删除 Session。
4. 回复 `200 OK`。

------

## 四、实现优先级 & 一步一步的路线

结合你现在的进度（有基本 TCP server + 想支持 interleaved/UDP），我建议一个**非常具体的顺序**（先不管 RTP 数据，只管“能说话”）：

### 第 1 步：统一解析请求起始行 + CSeq

目标：**任何 RTSP 请求进来，你都能解析出：方法 + URI + CSeq**，然后简单回一个固定 200 OK（不用区分方法）。

- 写一个 `RtspRequest` 结构体：
  - `method`（枚举 `OPTIONS`/`DESCRIBE`/…）
  - `uri`（std::string）
  - `version`
  - `headers`（std::unordered_map<std::string, std::string>）
- 写一个简单解析函数：
  - 按 `\r\n` 切行
  - 第一行拆成 3 个 token
  - 后面行按 `:` 切成 key/value
- 先不处理 body。

### 第 2 步：按方法分发 + 实现 OPTIONS / DESCRIBE

建立一个简单的“方法分发”逻辑：

```cpp
switch (request.method) {
  case RtspMethod::OPTIONS: handleOptions(request, connection); break;
  case RtspMethod::DESCRIBE: handleDescribe(request, connection); break;
  // 其他暂时返回 501 Not Implemented
}
```

此阶段目标：

- OPTIONS：
  - 返回带 `Public` 的 200 OK
- DESCRIBE：
  - 为固定几个 URL（例如 `/test.sdp`）返回写死的 SDP
  - 其他返回 404

这一步结束后，你的 RTSP server 已经能被 `ffplay` 用 DESCRIBE 探测到（虽然还不能播放）。

### 第 3 步：引入 Session 管理 + 实现 SETUP

设计一个简单的 `RtspSession` 结构：

- session_id（std::string）
- client_addr / client_ports
- server_ports
- state（INIT / READY / PLAYING / PAUSED）
- 关联的 media info（从 SDP 里来，先写死也行）

实现 `handleSetup()`：

- 解析 `Transport`，先只支持：
  - `RTP/AVP;unicast;client_port=xxxx-xxxx`
- 生成 session_id，创建一个 `RtspSession`，状态设为 `READY`
- 构造带 `Session` 和 `Transport` 的响应

这时，`ffplay` 应该能完成 OPTIONS → DESCRIBE → SETUP 三连。

### 第 4 步：实现 PLAY / PAUSE / TEARDOWN 的状态机逻辑

先不必真的发送 RTP（或者先写一个假发包的线程），只要把状态机打通：

- PLAY：`READY -> PLAYING`
- PAUSE：`PLAYING -> PAUSED`
- TEARDOWN：删除 Session

你可以在日志里打出：“session 12345678 state: READY -> PLAYING”。

### 第 5 步：开始向客户端发 RTP（先支持 UDP）

在有了 Session 的基础上，再单独开一个“推流模块”：

- 给每个 `PLAYING` 的 Session：
  - 周期性构造 RTP 包（可以先 dummy payload）
  - 调用 `sendto()` 发到 `client_port`
- 先不考虑 TCP interleaved、RTCP，只要 ffplay 能“看到”在收包，就算一个小里程碑。

### 第 6 步：再往后才是 TCP interleaved、多个 track、多客户端并发（epoll）等

这些可以作为后续特性，一点点叠上去。
