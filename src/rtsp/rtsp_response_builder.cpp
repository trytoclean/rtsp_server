#include "synthesizer/rtsp/rtsp_parser.h"
#include <synthesizer/rtsp/rtsp_response_builder.h>

namespace synthesizer::rtsp {

RtspResponse RtspResponseBuilder::buildSuccess(const RtspRequest &req) {
  RtspResponse res;

  res.code = 200;
  res.reason = "OK";
  res.version = req.version;

  res.headers["CSeq"] = std::to_string(req.cseq);
  res.headers["Date"] = get_time();
  res.headers["Server"] = "Synthesizer-RTSP/0.1";
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

} // namespace synthesizer::rtsp