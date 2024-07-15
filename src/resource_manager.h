#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <raylib/raylib.h>

#include <unordered_map>
#include <vector>
#include <string>

class ResourceManager {
public:
  struct ImageResource {
    Image image_;
    Color* color_data_;
  };

  ResourceManager() = default;
  ~ResourceManager();

  ResourceManager(const ResourceManager&& manager) = delete;

  void CreateImage(const std::string& path);
  void DeleteImage(const std::string& path);

  ImageResource* GetImageDataByPath(const std::string& path);
  ImageResource* GetImageDataByIndex(int index);
private:
  std::vector<std::string> image_path_;
  std::unordered_map<std::string, ImageResource> image_resources_;
};


#endif