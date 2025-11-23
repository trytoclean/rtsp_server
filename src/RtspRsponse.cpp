
#include <RtspResponse.h>
#include <ctime>
RtspResponse RtspResponseBuilder::ok(const RtspRequest &req) {
  RtspResponse res;
  res.code = 200;
  res.reason = "OK";

  auto it = req.headers.find("cseq");
  if (it != req.headers.end()) {
    res.headers["CSeq"] = it->second; // 回给客户端时保持标准大小写
  } else {
    // 没 CSeq 按协议也行：可以不回或回 400（看你策略）
    // 这里先不回 CSeq 也不会崩
  }

  return res;
}

RtspResponse RtspResponseBuilder::error(const RtspRequest &req, int code,
                                        std::string reason) {
  RtspResponse res;
  res.code = code;
  res.reason = reason;

  auto it = req.headers.find("cseq");
  if (it != req.headers.end()) {
    res.headers["CSeq"] = it->second;
  }

  return res;
}

static std::string current_gmt_date() {
  // RFC1123 格式: Mon, 20 Nov 1995 19:12:08 GMT
  std::time_t t = std::time(nullptr);
  std::tm gm{};
  gmtime_r(&t, &gm);

  char buf[128];
  std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gm);
  return buf;
}

std::string RtspResponse::serialize() const {
  std::string response;

  // 1) Status line
  response += "RTSP/1.0 " + std::to_string(code) + " " + reason + "\r\n";

  // 2) Server Header（如无手动设置，自动补）
  if (headers.find("Server") == headers.end()) {
    response += "Server: MyRTSPServer/0.1\r\n";
  }

  // 3) Date Header（如无手动设置，自动补）
  if (headers.find("Date") == headers.end()) {
    response += "Date: " + current_gmt_date() + "\r\n";
  }

  // 4) CSeq Header —— 建议强制要求外部设置，如果不存在则不补
  for (const auto &kv : headers) {
    // Server 和 Date 已经处理过了，不重复输出
    if (kv.first == "Server" || kv.first == "Date")
      continue;

    response += kv.first + ": " + kv.second + "\r\n";
  }

  // 5) 自动处理 Content-Length
  if (!body.empty()) {
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
  }

  // 6) Headers 结束
  response += "\r\n";

  // 7) Body（如果没有则为空）
  response += body;

  return response;
}
