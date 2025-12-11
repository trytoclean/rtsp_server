#include <sstream>
#include <string>
#include <synthesizer/rtsp/rtsp_parser.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>

namespace synthesizer::rtsp {

RtspResponse RtspResponseBuilder::buildSuccess(const RtspRequest &req) {
  RtspResponse res;

  res.code = 200;
  res.reason = "OK";
  res.version = req.version;
  res.setHeader("Cseq", std::to_string(req.cseq));
  res.setHeader("Date", get_time());
  res.setHeader("Server", "Synthesizer-RTSP/0.1");
  // Handler 自行补充它们自己的字段如:
  // Public, Session, Transport, Content-Type, Content-Length ...

  return res;
}

RtspResponse RtspResponseBuilder::buildError(const RtspRequest &req, int code,
                                             std::string reason) {
  RtspResponse res;

  res.code = code;
  res.reason = reason;
  res.version = req.version;

  res.headers["CSeq"] = std::to_string(req.cseq);
  res.headers["Date"] = get_time();
  res.headers["Server"] = "Synthesizer-RTSP/0.1";

  return res;
}

std::string RtspResponse::serialize() {
  std::ostringstream oss;
  oss << version << std::to_string(code) << reason << "\r\n";
  for (const auto &kv : headers) {
    oss << kv.first << ":" << kv.second << "\r\n";
  }
  oss << "\r\n";

  oss << body;

  return oss.str();
}
void RtspResponse::setHeader(const std::string &key, const std::string &value) {
  if (value.find('\r') != std::string::npos ||
      value.find('\n') != std::string::npos) {
    throw std::invalid_argument("Header value must not contain CR/LF");
  }
  headers[str_tolower(key)] = value;
}

void RtspResponse::appendHeader(const std::string &key,
                                const std::string &value) {

  if (headers.count(key) == 0) {
    headers[key] = value;
  } else {
    headers[key] += "," + value;
  }
}
} // namespace synthesizer::rtsp