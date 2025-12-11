#include <filesystem>
#include <iostream>

#include <synthesizer/media/media_source_manager.hpp>
#include <synthesizer/media/source/file_source.hpp>

namespace fs = std::filesystem;

namespace synthesizer::media {

MediaSourceManager::MediaSourceManager(std::string mediaRootDir)
    : mediaRootDir_(std::move(mediaRootDir)) {}

// 扫描本地目录并注册
void MediaSourceManager::scanAndRegisterLocalFiles() {
  if (!fs::exists(mediaRootDir_)) {
    std::cerr << "[MediaSourceManager] mediaRootDir not exists: "
              << mediaRootDir_ << "\n";
    return;
  }

  for (auto &entry : fs::directory_iterator(mediaRootDir_)) {
    if (!entry.is_regular_file())
      continue;

    auto path = entry.path().string();
    auto filename = entry.path().filename().string();

    MediaSourceConfig cfg;
    cfg.id = filename; // MVP：id 使用文件名
    cfg.filePath = path;

    registerSource(cfg);
  }
}

bool MediaSourceManager::registerSource(const MediaSourceConfig &config) {
  auto it = registryById_.find(config.id);
  if (it != registryById_.end())
    return false;

  registryById_[config.id] = config;
  return true;
}

bool MediaSourceManager::unregisterSource(const std::string &id) {
  auto it = registryById_.find(id);
  if (it == registryById_.end())
    return false;

  registryById_.erase(it);
  activeSources_.erase(id);
  return true;
}

// MVP：解析 URL 最简单写法：rtsp://host:port/filename
std::optional<MediaSourceConfig>
MediaSourceManager::resolveSourceConfig(const std::string &url) {
  // 找最后一个 "/"
  auto pos = url.rfind('/');
  if (pos == std::string::npos)
    return std::nullopt;

  std::string file = url.substr(pos + 1);

  auto it = registryById_.find(file);
  if (it == registryById_.end())
    return std::nullopt;

  return it->second;
}

bool MediaSourceManager::hasSource(const std::string &url) {
  return resolveSourceConfig(url).has_value();
}

std::shared_ptr<IMediaSource>
MediaSourceManager::acquireSource(const std::string &url) {
  auto cfgOpt = resolveSourceConfig(url);
  if (!cfgOpt.has_value())
    return nullptr;

  const std::string &id = cfgOpt->id;

  // 若已存在活跃实例则直接复用
  auto it = activeSources_.find(id);
  if (it != activeSources_.end())
    return it->second;

  // 创建实例（MVP 使用 FileSource）
  std::shared_ptr<IMediaSource> src = std::make_shared<FileSource>(*cfgOpt);
  if (!src->open()) {
    std::cerr << "[MediaSourceManager] acquireSource open failed: " << id
              << "\n";
    return nullptr;
  }

  activeSources_[id] = src;
  return src;
}

void MediaSourceManager::releaseSource(const std::string &url) {
  auto cfgOpt = resolveSourceConfig(url);
  if (!cfgOpt.has_value())
    return;

  const std::string &id = cfgOpt->id;

  auto it = activeSources_.find(id);
  if (it != activeSources_.end()) {
    it->second->close();
    activeSources_.erase(it);
  }
}
} // namespace synthesizer::media