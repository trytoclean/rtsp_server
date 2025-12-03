// src/rtsp/synthesizer.cc
#include "synthesizer/synthesizer.h"

#include <utility>

#include <synthesizer/rtsp/rtsp_dispatcher.h>

#include "synthesizer/net/tcp_server.h"

namespace synthesizer {

class RtspServer::Impl {
public:
  explicit Impl(ServerConfig config)
      : config_(std::move(config)),
        tcp_(std::make_unique<net::TcpServer>(config_.bind_ip, config_.port)),
        dispatcher_(std::make_unique<rtsp::RtspDispatcher>()) {

    // net -> rtsp 的唯一连接点：回调
    tcp_->setOnMessage([this](net::Connection *conn, const std::string &data) {
      auto res =
          dispatcher_->onMessage(data); // 你 dispatcher 返回 RtspResponse
      conn->send(res.serialize());
    });
  }

  bool start() { return tcp_->start(); }
  void stop() { tcp_->stop(); }

private:
  ServerConfig config_;
  std::unique_ptr<net::TcpServer> tcp_;
  std::unique_ptr<rtsp::RtspDispatcher> dispatcher_;
};

RtspServer::RtspServer(ServerConfig config)
    : impl_(std::make_unique<Impl>(std::move(config))) {}

RtspServer::~RtspServer() = default;

bool RtspServer::start() { return impl_->start(); }
void RtspServer::stop() { impl_->stop(); }

} // namespace synthesizer
