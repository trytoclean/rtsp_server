#include <filesystem>
#include <synthesizer/media/media_source_manager.hpp>
namespace synthesizer::media {

static std::string extractPath(const std::string &uri) {
  auto pos = uri.find("://");
  if (pos == std::string::npos) {
    return uri; // "/test.mp4"
  }
  auto slash = uri.find('/', pos + 3);
  if (slash == std::string::npos) {
    throw std::invalid_argument("invalid RTSP uri");
  }
  return uri.substr(slash);
}

MediaSourceManager::MediaSourceManager(std::string root)
    : root_(std::move(root)) {}

std::string MediaSourceManager::resolve(const std::string &uri) {
  auto it = mapCache_.find(uri);
  if (it != mapCache_.end()) {
    return it->second;
  }

  std::string pathPart = extractPath(uri);
  std::string real = root_ + pathPart;

  mapCache_[uri] = real;
  return real;
}

bool MediaSourceManager::exists(const std::string &uri) {
  auto real = resolve(uri);
  return std::filesystem::exists(real);
}

} // namespace synthesizer::media
