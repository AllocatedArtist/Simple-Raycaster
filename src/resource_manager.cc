#include "resource_manager.h"

#include <algorithm>

ResourceManager::~ResourceManager() {

  //won't ever call and "unloading" an image will throw an exception... idk :/

  for (int i = 0; i < image_path_.size(); ++i) {
    UnloadImageColors(image_resources_.at(image_path_[i]).color_data_);
    UnloadImage(image_resources_.at(image_path_[i]).image_);
  }

  image_path_.clear();
  image_resources_.clear();
}

void ResourceManager::CreateImage(const std::string& path) {
  if (image_resources_.find(path) != image_resources_.cend()) {
    return;
  }
  if (!IsPathFile(path.c_str())) {
    return;
  }

  ImageResource resource;
  resource.image_ = LoadImage(path.c_str());
  resource.color_data_ = LoadImageColors(resource.image_);

  int tex_size = resource.image_.width;
  for (int x = 0; x < tex_size; ++x) {
    for (int y = 0; y < x; ++y) {
      std::swap(resource.color_data_[tex_size * y + x], resource.color_data_[tex_size * x + y]);
    }
  }

  image_path_.push_back(path);
  image_resources_.insert(std::make_pair(path, resource));
}

ResourceManager::ImageResource* ResourceManager::GetImageDataByPath(const std::string& path) {
  if (image_resources_.find(path) == image_resources_.cend()) {
    return nullptr;
  }
  return &image_resources_.at(path);
}

ResourceManager::ImageResource* ResourceManager::GetImageDataByIndex(int index) {
  if (index >= image_path_.size()) {
    return nullptr;
  }
  if (image_resources_.find(image_path_[index]) == image_resources_.cend()) {
    return nullptr;
  }
  return &image_resources_.at(image_path_[index]);
}