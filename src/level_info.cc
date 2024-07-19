#include "level_info.h"

#include <nlohmann/json.hpp>

#include <raylib/raylib.h>

#include <fstream>

LevelInfo LoadLevelInfo(const std::string& filepath) {
  LevelInfo info;

  if (!IsFileExtension(filepath.c_str(), ".json")) {
    TraceLog(LOG_WARNING, "Can only load json files!");
    return info;
  }

  std::ifstream file(filepath);
  nlohmann::json data;
  file >> data;

  for (auto& element : data.items()) {
    std::string key = element.key();

    //I'm SORRY

    if (key == "map_data") {
      data.at("map_data").get_to(info.map_data_);
    } else if (key == "texture_paths") {
      data.at("texture_paths").get_to(info.texture_paths_);
    } else if (key == "map_height") {
      data.at("map_height").get_to(info.map_height_);
    } else if (key == "map_width") {
      data.at("map_width").get_to(info.map_width_);
    } else if (key == "starting_pos_x") {
      data.at("starting_pos_x").get_to(info.starting_pos_x_);
    } else if (key == "starting_pos_y") {
      data.at("starting_pos_y").get_to(info.starting_pos_y_);
    } else if (key == "dir_x") {
      data.at("dir_x").get_to(info.dir_x_);
    } else if (key == "dir_y") {
      data.at("dir_y").get_to(info.dir_y_);
    } else if (key == "plane_x") {
      data.at("plane_x").get_to(info.plane_x_);
    } else if (key == "plane_y") {
      data.at("plane_y").get_to(info.plane_y_);
    } else if (key == "floor_index") {
      data.at("floor_index").get_to(info.floor_index_);
    } else if (key == "ceiling_index") {
      data.at("ceiling_index").get_to(info.ceiling_index_);
    }
  }
 
  return info;
}