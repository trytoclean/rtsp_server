#include <RtspHandlers.h>

RtspResponse OptionsHandler::handle(const RtspRequest &req) {
  auto resp = RtspResponseBuilder::ok(req);
  resp.headers["Public"] = "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE";
  return resp;
}