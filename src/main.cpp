#include <net/SocketServer.h>

const int PORT = 12345;
int main() {
  SocketServer server(PORT);
  if (server.init()) {
    server.start();
  } else {
    return 1;
  }
  return 0;
}
