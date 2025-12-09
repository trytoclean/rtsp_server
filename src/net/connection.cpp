#include <synthesizer/net/connection.hpp>
#include <synthesizer/net/epoll.hpp>

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

namespace synthesizer::net {

Connection::Connection(int fd, Epoll *ep) : fd_(fd), epoll_(ep) {}

Connection::~Connection() {
  if (fd_ >= 0)
    ::close(fd_);
}

void Connection::onReadable() {
  char buf[4096];

  while (true) {
    ssize_t n = ::read(fd_, buf, sizeof(buf));

    if (n > 0) {
      input_.append(buf, n);
    } else if (n == 0) { // 对端关闭写
      close();
      return;
    } else { // n < 0
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      close();
      return;
    }
  }

  // 读取完毕，交给上层处理
  if (msg_cb_) {
    msg_cb_(this, input_.retrieveAllAsString());
  }
}

void Connection::onWritable() {
  while (output_.readableBytes() > 0) {
    ssize_t n = ::write(fd_, output_.readBegin(), output_.readableBytes());

    if (n > 0) {
      input_.hasRead(n);
    } else if (n < 0) {
      if (errno == EAGAIN)
        break;
      close();
      return;
    }
  }

  if (output_.readableBytes() == 0) {
    disableWriting(); // 发送完毕
  }
}

void Connection::send(const std::string &data) {
  output_.append(data);
  enableWriting(); // 触发 EPOLLOUT
}

void Connection::close() {
  if (fd_ < 0)
    return;

  ::close(fd_);
  fd_ = -1;

  if (close_cb_)
    close_cb_(this);
}

void Connection::enableWriting() { epoll_->mod(fd_, EPOLLIN | EPOLLOUT, this); }

void Connection::disableWriting() { epoll_->mod(fd_, EPOLLIN, this); }

} // namespace synthesizer::net
