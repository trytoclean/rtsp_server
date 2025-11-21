#include <RtspDispatcher.h>
#include <RtspParser.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
void onRtspMessage(const std::string &raw, int client_fd) {}

int main() {
  // socket
  const int port = 8554;
  int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  int reuse = 1;
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  auto ret = ::bind(server_fd, reinterpret_cast<sockaddr *>(&address),
                    sizeof(address));
  if (ret < 0) {
    std::cerr << "bind failed" << std::endl;
    return -1;
  }

  std::cout << "listening on port " << port << std::endl;
  sockaddr_in client_address{};

  socklen_t client_len = sizeof(client_address);
  int client_fd = ::accept(
      server_fd, reinterpret_cast<sockaddr *>(&client_address), &client_len);
  if (client_fd < 0) {
    std::cerr << "accept failed" << std::endl;
    return -1;
  }
  std::cout << "client connected and fd is :" << client_fd << std::endl;
  char buffer[4096];
  ssize_t n = ::recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (n < 0) {
    std::cerr << "recv failed" << std::endl;
    return -1;
  }
  buffer[n] = '\0';
  // 处理buffer；
  std::string raw_request(buffer);
  RtspParser parser;
  RtspRequest req = parser.parse(raw_request);
  RtspDispatcher dispatcher;
  RtspResponse resp = dispatcher.dispatch(req);
  auto out = resp.serialize();
  ::send(client_fd, out.data(), out.size(), 0);
  ::close(client_fd);
  ::close(server_fd);
  return 0;
}