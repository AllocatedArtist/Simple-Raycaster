#ifndef LEVEL_INFO_H_
#define LEVEL_INFO_H_

#include <vector>
#include <string>

struct LevelInfo {
  std::vector<uint32_t> map_data_;

  std::vector<std::string> texture_paths_;

  int map_width_, map_height_;

  float starting_pos_x_, starting_pos_y_;

  float dir_x_, dir_y_;

  float plane_x_, plane_y_;

  int floor_index_, ceiling_index_;
};

LevelInfo LoadLevelInfo(const std::string& filepath);

#endif