#ifndef LEVEL_EDITOR_H_
#define LEVEL_EDITOR_H_

#include "scene.h"

#include <raylib/raylib.h>

#include <vector>

//quite hacky but works :P

class LevelEditor : public Scene {
public:
  LevelEditor(const std::string& name) : Scene(name) {}

  void Start() override;
  void Update() override;
  void End() override;
public:
  Vector2 GetPlayerPos() const;
  std::vector<uint32_t> GetLevelData() const;
  int GetMapWidth() const;
  int GetMapHeight() const;
private:
  void DrawGrid();
  void MouseToGrid();
private:
  float map_width_ = 10;
  float map_height_ = 10;

  Camera2D camera_; 

  int current_tile_x_;
  int current_tile_y_;

  //temp
  int wall_ = 0;
  std::vector<uint32_t> level_;
  Vector2 player_pos_ = { 0.0, 0.0 };
};

#endif