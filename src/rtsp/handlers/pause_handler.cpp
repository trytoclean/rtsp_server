#include <synthesizer/rtsp/handlers/options_handler.hpp>
#include <synthesizer/rtsp/rtsp_response_builder.hpp>

namespace synthesizer::rtsp {
RtspResponse handle(const RtspRequest &req) {
  RtspResponse res = RtspResponseBuilder::buildSuccess(req);
  res.headers["Public"] = "OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, TEARDOWN";
  res.headers["Content-Type"] = "application/sdp";
  res.headers["Content-Length"] = std::to_string(res.body.size());
  return res;
}
} // namespace synthesizer::rtsp

/* reply
Real Time Streaming Protocol
    Response: RTSP/1.0 200 OK\r\n
    CSeq: 6
    Date: Tue, Sep 16 2025 09:05:32 GMT\r\n
    Session: ACB17A1B
    \r\n
*/
/*
1 验证session是否合法，
  验证 Session 是否存在
  验证 Session 状态是否允许暂停（通常 PLAYING 状态才能暂停）
2.停止调度器
停止 RTP 包发送   <-
停止 Frame Scheduler 的调度（如基于 timebase 的发送循环）
停止 RTCP SR 定时器
3. 保存npt
不要摧毁会话
*/