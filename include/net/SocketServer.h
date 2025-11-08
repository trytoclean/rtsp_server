#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>

class SocketServer {
public:
  SocketServer(int port);
  ~SocketServer();

  bool init();
  void start();

private:
  int server_socket_;
  int port_;
  sockaddr_in server_addr_;

  bool createSocket();
  bool bindSocket();
  bool listenSocket();
  void handleClient(int client_socket);
};

#endif // SOCKET_SERVER_H
