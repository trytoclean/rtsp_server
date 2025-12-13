#include <fcntl.h>
#include <synthesizer/net/acceptor.hpp>
#include <synthesizer/net/socket.hpp>
#include <sys/socket.h>
#include <unistd.h>

namespace synthesizer::net {

Acceptor::Acceptor(int port) {
  listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);

  int on = 1;
  setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  InetAddress addr(port);
  Socket socket;
  socket.bindSocket(addr);
  socket.listenSocket(1024);

  // 设置非阻塞
  fcntl(listen_fd_, F_SETFL, O_NONBLOCK);
}

Acceptor::~Acceptor() { close(listen_fd_); }

void Acceptor::handleAccept() {
  sockaddr_in caddr{};
  socklen_t len = sizeof(caddr);

  int client_fd = ::accept(listen_fd_, (sockaddr *)&caddr, &len);
  if (client_fd < 0)
    return;

  if (callback_)
    callback_(client_fd); // 把新连接交给上层
}

} // namespace synthesizer::net
