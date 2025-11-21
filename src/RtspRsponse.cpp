
#include <RtspResponse.h>

std::string RtspResponse::serialize() const {
  std::string response;

  // Status line
  response += "RTSP/1.0 " + std::to_string(code) + " " + reason + "\r\n";

  // Headers
  for (const auto &header : headers) {
    response += header.first + ": " + header.second + "\r\n";
  }

  // End of headers
  response += "\r\n";

  // Body
  response += body;

  return response;
}

RtspResponse RtspResponseBuilder::ok(const RtspRequest &req) {
  RtspResponse res;
  res.code = 200;
  res.reason = "OK";
  res.headers["CSeq"] = req.headers.at("CSeq");
  return res;
}

RtspResponse RtspResponseBuilder::error(const RtspRequest &req, int code,
                                        std::string reason) {
  RtspResponse res;
  res.code = code;
  res.reason = reason;
  res.headers["CSeq"] = req.headers.at("CSeq");
  return res;
}