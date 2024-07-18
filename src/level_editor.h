#ifndef LEVEL_EDITOR_H_
#define LEVEL_EDITOR_H_

#include <raylib/raylib.h>

#include <vector>

#include "scene.h"
#include "level_info.h"

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
 
  LevelInfo GetCurrentLevelInfo(); 
private:
  void DrawGrid();
  void MouseToGrid();

  void CheckForFiles();
  void AddTexture(const char* path);

  void ExportLevelData();

  void LoadLevelData(const char* path);
private:
  float map_width_ = 10;
  float map_height_ = 10;


  float pan_speed_ = 20.0;
  float scroll_speed_ = 2.0;
  int grid_size_ = 10;

  Camera2D camera_; 

  int current_tile_x_;
  int current_tile_y_;

  //temp
  int wall_ = 0;

  std::vector<uint32_t> level_;
  std::vector<Texture> textures_data_;
  std::vector<std::string> textures_; 

  float plane_scale_ = 1.0;
  Vector2 player_dir_ = { 0.0, -1.0 };
  Vector2 player_plane_ = { -1.0, 0.0 };
  Vector2 player_pos_ = { 0.0, 0.0 };
};

#endif