#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <net/SocketServer.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

SocketServer::SocketServer(int port) : port_(port), server_socket_(-1) {
  std::memset(&server_addr_, 0, sizeof(server_addr_));
}

SocketServer::~SocketServer() {
  if (server_socket_ >= 0) {
    close(server_socket_);
  }
}

bool SocketServer::init() {
  return createSocket() && bindSocket() && listenSocket();
}

void SocketServer::start() {
  std::cout << "Server listening on port " << port_ << "\n";
  sockaddr_in client_addr{};
  socklen_t client_len = sizeof(client_addr);
  while (true) {
    int client_socket =
        accept(server_socket_, (sockaddr *)&client_addr, &client_len);
    if (client_socket < 0) {
      std::cerr << "Accept failed\n";
      continue;
    }
    handleClient(client_socket);
    close(client_socket);
  }
}

bool SocketServer::createSocket() {
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    std::cerr << "Socket creation failed\n";
    return false;
  }
  return true;
}

bool SocketServer::bindSocket() {
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = INADDR_ANY;
  server_addr_.sin_port = htons(port_);

  if (bind(server_socket_, (sockaddr *)&server_addr_, sizeof(server_addr_)) <
      0) {
    std::cerr << "Bind failed\n";
    return false;
  }
  return true;
}

bool SocketServer::listenSocket() {
  if (listen(server_socket_, 3) < 0) {
    std::cerr << "Listen failed\n";
    return false;
  }
  return true;
}

void SocketServer::handleClient(int client_socket) {
  char buffer[1024] = {0};
  ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0'; // Null-terminate the received message
    std::cout << "Received: " << buffer << "\n";
    const char *response = "Message received\n";
    send(client_socket, response, strlen(response), 0);
  }
}
