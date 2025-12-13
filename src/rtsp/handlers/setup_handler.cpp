#include <cstdlib>
#include <cstring>
#include <synthesizer/rtsp/handlers/setup_handler.hpp>

namespace synthesizer::rtsp {

RtspResponse SetupHandler::handle(const RtspRequest &req) {
  RtspResponse resp = RtspResponseBuilder::buildSuccess(req);

  // 1. 解析 Transport 请求头
  std::string transport = req.transport;
  // MVP：简单解析 client_port=xxxx-yyyy
  int pos = transport.find("client_port=");
  int c1 = 0, c2 = 0;
  if (pos != std::string::npos) {
    pos += std::strlen("client_port=");
    int dash = transport.find("-", pos);
    c1 = std::stoi(transport.substr(pos, dash - pos));
    int end = transport.find(";", dash);
    c2 = std::stoi(transport.substr(dash + 1, end - dash - 1));
  }

  // 2. server port
  int serverRtpPort = 6970;
  int serverRtcpPort = 6971;

  // 3. 服务端 IP（先写死）
  std::string serverIp = "192.168.239.129";

  // 4. 构建 Transport 响应
  std::string transportResp = "RTP/AVP;unicast;"
                              "client_port=" +
                              std::to_string(c1) + "-" + std::to_string(c2) +
                              ";server_port=" + std::to_string(serverRtpPort) +
                              "-" + std::to_string(serverRtcpPort) +
                              ";source=" + serverIp;

  resp.setHeader("Transport", transportResp);

  // 5. 生成 Session ID
  std::string sessionId = generateSessionId();
  resp.setHeader("Session", sessionId + ";timeout=60");

  return resp;
}

} // namespace synthesizer::rtsp

/*reply
Real Time Streaming Protocol
    Response: RTSP/1.0 200 OK\r\n
    CSeq: 3
    Date: Tue, Sep 16 2025 09:05:24 GMT\r\n
    Transport:
RTP/AVP;unicast;destination=192.168.239.1;source=192.168.239.129;client_port=5276-5277;server_port=6970-6971
    Session: ACB17A1B;timeout=65
    \r\n


可能采取的方案:
struct SessionInfo {
    std::string id;
    time_t last_active;
    int timeout;
    TransportContext transport;
};

struct ClientTransport {
    bool unicast;
    int client_rtp_port;
    int client_rtcp_port;
    std::string lowertransport = "UDP";
};

关于timeout字段：
当客户端断连或者没有响应。设置timeout字段，服务器会在这个结束后自动断开连接，并且，通知客户端heartbeat
：发送OPTIONS 或者GET_PARAMETER 请求，证明活性。 如果 N
秒内没有任何请求，服务器会：停止推流,回收 RTP/RTCP socket,删除 Session 记录.
值设置在60 客户端一般会在30s的时候发送请求
*/