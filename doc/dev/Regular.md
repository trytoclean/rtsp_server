# 注释
-  本项目的所有命名、均符合 google c++ code style 
    (有.clang-tidy)
- 本项目所有的注释规范是：
1. 文件头注释
2. 类注释（说明“是什么 + 负责什么 + 不负责什么”）
3. 函数注释（Doxygen 风格）
4. 实现内注释：只解释“为什么”，不复述代码

## 具体细节
-   头文件注释
```cpp
/**
 * @file Dispatcher.h
 * @brief RTSP请求分发器：根据 Method 路由到对应 Handler
 *
 * 职责边界：
 * 1) 不做业务决策，只做路由
 * 2) 不持有 socket / 不做网络 I/O
 *
 * 典型调用：
 *   RtspResponse res = dispatcher.dispatch(req);
 *
 * @author trytoclean
 * @date 2025-11-23
 */
 ```
-  类注释
```cpp
/**
 * @class RtspDispatcher
 * @brief 将解析后的 RtspRequest 分派给对应的 IRtspHandler。
 *
 * 线程安全：
 * - 注册阶段只在初始化调用（单线程）
 * - dispatch 阶段只读 handler 表，可多线程调用
 */
class RtspDispatcher { ... };
```

-   函数注释
```cpp
/**
 * @brief 分发一个请求到对应 Handler 并返回响应
 * @param req 已完成解析的 RTSP 请求
 * @return RtspResponse（总是包含CSeq）
 *
 * 错误约定：
 * - Method 未注册 → 405 Method Not Allowed
 * - 解析字段缺失 → 400 Bad Request
 */
RtspResponse dispatch(const RtspRequest& req);
```

-   实现内注释
```cpp
//只解释“为什么”，不复述代码

// RTSP允许header重复出现，这里保留最后一个值，与ffplay行为一致
headers_[key] = value;
```

