#include <synthesizer/rtsp/handlers/play_handler.hpp>
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
    CSeq: 5
    Date: Tue, Sep 16 2025 09:05:24 GMT\r\n
    Range: npt=0.000-\r\n
    Session: ACB17A1B
    RTP-Info:
url=rtsp://192.168.239.129:8554/test.mkv/track1;seq=61212;rtptime=1390826539,url=rtsp://192.168.239.129:8554/test.mkv/track2;seq=5111;rtptime=3913030103\r\n
    \r\n
几个重点字段的构建
    range: 如果从头播放
           如果从指定地点 使用 av_frmae_seek
    session: 生成一个
重点:
    1) track 根据上层 提供，SDP 中的 track id（FFmpeg
解析媒体时得知stream索引）或你自己的媒体源描述（MediaSource 提供 track 信息）
    2)每个 track都有自己的 seq：16
位无符号整数通常从一个随机值开始.第一个将要发送的 RTP 包的 seq（初始 seq）
    3)首先，不同编码有不同的时间基；
    h264-》90khz  aac->sample_rate-based : like 48000khz
    其次，根据播放情况又分为
    首次 PLAY（从 0 开始）
    此时初始 rtptime 通常为 随机数。

    Seek 后 PLAY（Range > 0）
    计算：rtpTimestamp = (npt_seconds * RTP_clock_rate)
      也就是 （seek的时间* 时基）
*/