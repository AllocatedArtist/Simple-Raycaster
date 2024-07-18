#include "level_editor.h"

#include <emscripten/emscripten.h>

#include <raylib/raymath.h>
#include <raygui/raygui.h>

#include <nlohmann/json.hpp>

#include "scene_manager.h"

void LevelEditor::Start() {
  current_tile_x_ = 0;
  current_tile_y_ = 0;

  camera_.offset = { 0.0, 0.0 };
  camera_.target = { (float)grid_size_, (float)grid_size_ };
  camera_.zoom = 1.0;
  camera_.rotation = 0.0;
  
  level_.resize((int)map_width_ * (int)map_height_); 
}

void LevelEditor::LoadLevelData(const char* path) {
  LevelInfo info = LoadLevelInfo(path);
  level_ = info.map_data_;
  map_height_ = info.map_height_;
  map_width_ = info.map_width_;
  player_pos_  = { info.starting_pos_x_ * grid_size_, info.starting_pos_y_ * grid_size_ };
  player_dir_ = { info.dir_x_, info.dir_y_ };
  player_plane_ = { info.plane_x_, info.plane_y_ };

  for (const std::string& path : info.texture_paths_) {
    AddTexture(path.c_str());
  }
}

void DrawMouseRect(
  Vector2 current_tile, 
  Vector2 mouse_tile, 
  float grid_size, 
  const std::vector<Texture2D>& textures,
  int current_index
) {
  if (!Vector2Equals(current_tile, mouse_tile)) {
    return;
  }

  if (textures.size() > 0 && current_index > 0) {
    float width = textures[current_index - 1].width;
    float height = textures[current_index - 1].height;
    Rectangle source = { 0, 0, width, height };
    Rectangle dest = { mouse_tile.x, mouse_tile.y, grid_size, grid_size };
    DrawTexturePro(textures[current_index - 1], source, dest, { 0.0, 0.0 }, 0.0, WHITE);
  } else {
    DrawRectangleV(mouse_tile, { grid_size, grid_size }, PURPLE);
  }
}

void LevelEditor::DrawGrid() {
  for (int x = 0; x < map_width_; ++x) {
    for (int y = 0; y < map_height_; ++y) {
      DrawMouseRect(
        { 
          (float)x * grid_size_, 
          (float)y * grid_size_ 
        }, 
        { 
          (float)current_tile_x_, 
          (float)current_tile_y_ 
        },
        grid_size_,
        textures_data_,
        wall_
      );

      int wall = level_[x + (int)map_width_ * y];
      if (wall > 0) {
        Color col = wall == 1 ? RED : BLUE;
        float width = textures_data_[wall - 1].width;
        float height = textures_data_[wall - 1].height;
        float grid_size = (float)grid_size_;
        Rectangle source = { 0, 0, width, height };
        Rectangle dest = { x * grid_size, y * grid_size, grid_size, grid_size };
        DrawTexturePro(textures_data_[wall - 1], source, dest, { 0.0, 0.0 }, 0.0, WHITE);
      } else {
        DrawRectangleLines(x * grid_size_, y * grid_size_, grid_size_, grid_size_, BLACK);
      }
    }
  }
}


void LevelEditor::MouseToGrid() {
  Vector2 mouse_pos = GetMousePosition();

  DrawCircleV(mouse_pos, 5.0, PURPLE);

  for (int x = 0; x < map_width_; ++x) {
    for (int y = 0; y < map_height_; ++y) {
      Vector2 grid_pos = { (float)x * grid_size_, (float)y * grid_size_ };
      grid_pos = GetWorldToScreen2D(grid_pos, camera_);

      Vector2 size = { grid_size_ * camera_.zoom, grid_size_ * camera_.zoom };

      if (CheckCollisionPointRec(mouse_pos, { grid_pos.x, grid_pos.y, size.x, size.y })) {
        current_tile_x_ = x * grid_size_;
        current_tile_y_ = y * grid_size_;
      }
    }
  }

  Vector2 whole_grid_origin =  GetWorldToScreen2D({ 0.0, 0.0 }, camera_);
  Vector2 size = { map_width_ * grid_size_ * camera_.zoom, map_height_ * grid_size_ * camera_.zoom };
  if (!CheckCollisionPointRec(mouse_pos, { whole_grid_origin.x, whole_grid_origin.y, size.x, size.y })) {
    current_tile_x_ = -INT_MAX;
    current_tile_y_ = -INT_MAX;
  }
}

void LevelEditor::CheckForFiles() {
  if (IsFileDropped()) {
    FilePathList files = LoadDroppedFiles();
    for (int i = 0; i < files.count; ++i) {
      if (IsFileExtension(files.paths[i], ".json")) {
        LoadLevelData(files.paths[i]);
      } else {
        AddTexture(files.paths[i]);
      }
    }
    UnloadDroppedFiles(files);
  }
}


void LevelEditor::AddTexture(const char* path) {
  auto loaded = std::find(textures_.cbegin(), textures_.cend(), path);
  if (loaded != textures_.cend()) {
    return;
  }

  textures_data_.emplace_back(LoadTexture(path));
  textures_.push_back(path);
}

void LevelEditor::Update() {
  ClearBackground(GRAY);

  if (IsKeyPressed(KEY_Q)) {
    GetSceneManager()->SwitchScene("game");
  } 

  Vector2 mouse_delta = GetMouseDelta();
  if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    camera_.offset = Vector2Add(camera_.offset, Vector2Scale(mouse_delta, GetFrameTime() * pan_speed_));
  }

  float mouse_wheel = GetMouseWheelMove();
  if (mouse_wheel != 0.0) {
    camera_.zoom += mouse_wheel * GetFrameTime() * scroll_speed_;
    camera_.zoom = Clamp(camera_.zoom, 0.5, 5.0);
  }

  MouseToGrid();

  if (IsKeyPressed(KEY_RIGHT)) {
    wall_ += 1;
  }
  if (IsKeyPressed(KEY_LEFT)) {
    wall_ -= 1;
  }

  wall_ = Clamp(wall_, 0, textures_.size());

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    int x = current_tile_x_ / grid_size_;
    int y = current_tile_y_ / grid_size_;
    int pos = x + (int)map_width_ * y;
    if (pos >= 0 && pos < (int)map_width_ * (int)map_height_) {
      level_[pos] = wall_;
    }
  }
  if (IsKeyPressed(KEY_E)) {
    player_pos_ = GetScreenToWorld2D(GetMousePosition(), camera_);
  }

  if (IsKeyPressed(KEY_S)) {
    ExportLevelData();
  }

  if (IsKeyDown(KEY_R)) {
    Vector2 target = GetScreenToWorld2D(GetMousePosition(), camera_);
    Vector2 dir = Vector2Normalize(Vector2Subtract(target, player_pos_));
    player_dir_ = dir;
    player_plane_ = { dir.y, -dir.x };
  }

  CheckForFiles();

  BeginMode2D(camera_);

  DrawGrid();
  DrawCircleV(player_pos_, 5.0, DARKGREEN);
  DrawLineV(player_pos_, Vector2Add(player_pos_, Vector2Scale(player_dir_, 15.0)), YELLOW);

  EndMode2D();

  //clearing the level everytime the slider is adjusted is not great... fix?
 
  if (GuiSliderBar({ 70.0, 10.0, 50.0, 10.0 }, "Map width:", TextFormat("%.1f", map_width_), &map_width_, 10.0, 200.0)) {
    map_width_ = roundf(map_width_);
    level_.clear();
    level_.resize((int)map_width_ * (int)map_height_);
  }
  if (GuiSliderBar({ 70.0, 20.0, 50.0, 10.0 }, "Map height:", TextFormat("%.1f", map_height_), &map_height_, 10.0, 200.0)) {
    map_height_ = roundf(map_height_);
    level_.clear();
    level_.resize((int)map_width_ * (int)map_height_);
  } 
 
  DrawText(TextFormat("Wall index: %d", wall_), 10, 50, 8, RED);  
}

void LevelEditor::End() {

}

Vector2 LevelEditor::GetPlayerPos() const {
  return { player_pos_.x / grid_size_, player_pos_.y / grid_size_ };
}

std::vector<uint32_t> LevelEditor::GetLevelData() const {
  return level_;
}

int LevelEditor::GetMapWidth() const {
  return (int)map_width_;
}

int LevelEditor::GetMapHeight() const {
  return (int)map_height_;
}

/*
snippet from 
  https://stackoverflow.com/questions/3665115/how-to-create-a-file-in-memory-for-user-to-download-but-not-through-server
*/

EM_JS(void, _js_save, (const char* filename, const char* text), {
  let file_name = UTF8ToString(filename);
  let text_string = UTF8ToString(text);

  var element = document.createElement('a');
  element.setAttribute('href', 'data:text/plain;charset=utf-8,' + text_string);
  element.setAttribute('download', file_name);

  element.style.display = 'none';
  document.body.appendChild(element);

  element.click();

  document.body.removeChild(element);
});

EM_JS(void, alert_error, (const char* msg), {
  alert(UTF8ToString(msg));
});


/*
  if the user attempts to save after having already pressed Q ("game mode"),
  and one of the images used is not found in the preloaded asset directory,
  the user can say bye-bye to all their work because of an exception thrown :) 

  to fix this, dropped files should be checked beforehand and only loaded in if they are in the
  asset directory. i should add this, and it would be easy too, but i won't because I need
  to sleep.  
*/

void LevelEditor::ExportLevelData() {

  LevelInfo info = GetCurrentLevelInfo();

  std::vector<std::string> texture_paths_revised;
  FilePathList asset_directory = LoadDirectoryFilesEx("assets", NULL, true);

  for (const std::string& path : info.texture_paths_) {
    const char* path_revised = GetFileName(path.c_str());
    bool found = false;
    for (int i = 0; i < asset_directory.count; ++i) {
      char* asset_path = asset_directory.paths[i];
      if (TextIsEqual(GetFileName(asset_path), path_revised)) {
        texture_paths_revised.push_back(asset_path);
        found = true;
      }
    }
    if (!found) {
      const char* msg = TextFormat("Unable to find asset [%s] in preloaded assets folder!", path_revised);
      alert_error(msg);
      break;
    }
  }

  UnloadDirectoryFiles(asset_directory);

  nlohmann::json data = {
    { "map_data", info.map_data_ },
    { "map_height", info.map_height_ },
    { "map_width", info.map_width_ },
    { "starting_pos_x", info.starting_pos_x_ },
    { "starting_pos_y", info.starting_pos_y_ },
    { "dir_x", info.dir_x_ },
    { "dir_y", info.dir_y_ },
    { "plane_x", info.plane_x_ },
    { "plane_y", info.plane_y_ },
    { "texture_paths", texture_paths_revised }
  };

  _js_save("level.json", data.dump(4).c_str());
}

LevelInfo LevelEditor::GetCurrentLevelInfo() {
  LevelInfo info;
  info.map_data_ = level_;
  info.map_height_ = (int)map_height_;
  info.map_width_ = (int)map_width_;
  info.starting_pos_x_ = player_pos_.x / grid_size_;
  info.starting_pos_y_ = player_pos_.y / grid_size_;
  info.dir_x_ = player_dir_.x;
  info.dir_y_ = player_dir_.y;
  info.plane_x_ = player_plane_.x;
  info.plane_y_ = player_plane_.y;
  info.texture_paths_ = textures_;
  return info;
}
