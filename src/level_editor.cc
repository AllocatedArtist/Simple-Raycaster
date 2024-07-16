#include "level_editor.h"

#include <raylib/raymath.h>
#include <raygui/raygui.h>

#include "scene_manager.h"

void LevelEditor::Start() {
  current_tile_x_ = 0;
  current_tile_y_ = 0;

  camera_.offset = { 0.0, 0.0 };
  camera_.target = { 10.0, 10.0 };
  camera_.zoom = 1.0;
  
  level_.resize((int)map_width_ * (int)map_height_);
}

void LevelEditor::DrawGrid() {
  for (int x = 0; x < map_width_; ++x) {
    for (int y = 0; y < map_height_; ++y) {
      if (x * 10 == current_tile_x_ && y * 10 == current_tile_y_) {
        DrawRectangle(x * 10, y * 10, 10, 10, PURPLE);
      }

      int wall = level_[x + (int)map_width_ * y];
      if (wall > 0) {
        Color col = wall == 1 ? RED : BLUE;
        DrawRectangle(x * 10, y * 10, 10, 10, col);
      } else {
        DrawRectangleLines(x * 10, y * 10, 10, 10, BLACK);
      }
    }
  }
}


void LevelEditor::MouseToGrid() {
  Vector2 mouse_pos = GetMousePosition();

  DrawCircleV(mouse_pos, 5.0, PURPLE);

  for (int x = 0; x < map_width_; ++x) {
    for (int y = 0; y < map_height_; ++y) {
      Vector2 grid_pos = { (float)x * 10, (float)y * 10 };
      grid_pos = GetWorldToScreen2D(grid_pos, camera_);

      Vector2 size = { 10 * camera_.zoom, 10 * camera_.zoom };

      if (CheckCollisionPointRec(mouse_pos, { grid_pos.x, grid_pos.y, size.x, size.y })) {
        current_tile_x_ = x * 10.0;
        current_tile_y_ = y * 10.0;
      }
    }
  }
}

void LevelEditor::Update() {
  ClearBackground(RAYWHITE);

  if (IsKeyPressed(KEY_Q)) {
    GetSceneManager()->SwitchScene("game");
  } 

  Vector2 mouse_delta = GetMouseDelta();
  if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    float pan_speed = 20.0;
    camera_.offset = Vector2Add(camera_.offset, Vector2Scale(mouse_delta, GetFrameTime() * pan_speed));
  }

  float mouse_wheel = GetMouseWheelMove();
  if (mouse_wheel != 0.0) {
    float scroll_speed = 2.0;
    camera_.zoom += mouse_wheel * GetFrameTime() * scroll_speed;
    camera_.zoom = Clamp(camera_.zoom, 0.5, 5.0);
  }

  MouseToGrid();

  if (IsKeyPressed(KEY_RIGHT)) {
    wall_ += 1;
  }
  if (IsKeyPressed(KEY_LEFT)) {
    wall_ -= 1;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    int x = current_tile_x_ * 0.1;
    int y = current_tile_y_ * 0.1;
    int pos = x + (int)map_width_ * y;
    if (pos >= 0 && pos < (int)map_width_ * (int)map_height_) {
      level_[pos] = wall_;
    }
  }
  if (IsKeyPressed(KEY_E)) {
    player_pos_ = GetScreenToWorld2D(GetMousePosition(), camera_);
  }

  wall_ = Clamp(wall_, 0, 2);

  BeginMode2D(camera_);

  DrawGrid();
  DrawCircleV(player_pos_, 5.0, DARKGREEN);

  EndMode2D();

  //clearing the level everytime the slider is adjusted is not great... fix?
 
  if (GuiSliderBar({ 70.0, 10.0, 50.0, 10.0 }, "Map width:", TextFormat("%.1f", map_width_), &map_width_, 10.0, 50.0)) {
    map_width_ = roundf(map_width_);
    level_.clear();
    level_.resize((int)map_width_ * (int)map_height_);
  }
  if (GuiSliderBar({ 70.0, 20.0, 50.0, 10.0 }, "Map height:", TextFormat("%.1f", map_height_), &map_height_, 10.0, 50.0)) {
    map_height_ = roundf(map_height_);
    level_.clear();
    level_.resize((int)map_width_ * (int)map_height_);
  }
 
  DrawText(TextFormat("Wall index: %d", wall_), 10, 50, 8, RED);  
}

void LevelEditor::End() {

}

Vector2 LevelEditor::GetPlayerPos() const {
  return { player_pos_.x * 0.1f, player_pos_.y * 0.1f };
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

