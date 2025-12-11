#pragma once
#include "Imedia_source.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
struct MediaSourceConfig {
  std::string id;         // 逻辑ID，比如 "test.mkv"
  std::string urlPattern; // 请求中的匹配模式，例如 "/test.mkv"
  std::string filePath;   // 本地实际路径，例如 "../input/test.mkv"
  std::string mediaType;  // "file" / "live" / "network"，MVP 先只用 "file"
                          // 后续扩展：codecHint, tags, durationHint 等
};
namespace synthesizer::media {

class MediaSourceManager {
public:
  explicit MediaSourceManager(std::string mediaRootDir = "../input");

  // 启动与注册
  void scanAndRegisterLocalFiles();
  bool registerSource(const MediaSourceConfig &config);
  bool unregisterSource(const std::string &id);

  // 查询与匹配
  std::optional<MediaSourceConfig> resolveSourceConfig(const std::string &url);
  bool hasSource(const std::string &url);

  // 获取具体的媒体源实例
  std::shared_ptr<IMediaSource> acquireSource(const std::string &url);
  void releaseSource(const std::string &url);

  // 调试、管理：
  std::vector<MediaSourceConfig> listAllSources() const;

private:
  std::unordered_map<std::string, MediaSourceConfig> registryById_;
  std::unordered_map<std::string, std::shared_ptr<IMediaSource>> activeSources_;
  std::string mediaRootDir_;
  // mutable std::mutex mutex_; // v2 再加
};

} // namespace synthesizer::media
