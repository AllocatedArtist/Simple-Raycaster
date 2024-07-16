#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

#include <emscripten/emscripten.h>

#include <iostream>
#include <array>

#include "app.h"
#include "scene.h"
#include "raycaster.h"
#include "resource_manager.h"
#include "level_editor.h"

constexpr uint32_t MAP_WIDTH = 10;
constexpr uint32_t MAP_HEIGHT = 10;

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 400;

uint32_t map[MAP_WIDTH][MAP_HEIGHT] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 2, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 2, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
  1, 1, 1, 1, 2, 0, 2, 1, 0, 1,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 0, 0, 2, 0, 0, 0, 0, 2, 1,
  1, 0, 0, 0, 0, 2, 2, 0, 2, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

class RedScreen : public Scene {
public:
  RedScreen(const std::string& name) : Scene(name) {
    caster_.InitializeBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    resource_manager_.CreateImage("assets/Brick/Brick_02-128x128.png");
    resource_manager_.CreateImage("assets/Brick/Brick_07-128x128.png");

    info_.map_.resize(MAP_WIDTH * MAP_HEIGHT);
  }

  void Start() override {
    info_.dir_ = player_dir_;
    info_.pos_ = player_pos_;
    info_.plane_ = plane_;

    std::unique_ptr<Scene>* editor = GetSceneManager()->GetScene("editor");

    if (editor == nullptr) {
      info_.map_width_ = MAP_WIDTH;
      info_.map_height_ = MAP_HEIGHT;

      for (int x = 0; x < MAP_WIDTH; ++x) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
          info_.map_[MAP_HEIGHT * y + x] = map[y][x];
        }
      }
    } else {
      LevelEditor* editor_scene = reinterpret_cast<LevelEditor*>(editor->get());
      if (editor_scene != nullptr) {
        player_pos_ = editor_scene->GetPlayerPos();
        info_.map_width_ = editor_scene->GetMapWidth();
        info_.map_height_ = editor_scene->GetMapHeight();
        info_.map_.resize(info_.map_width_ * info_.map_height_);
        info_.map_ = editor_scene->GetLevelData();
      } 
    }


 
    info_.resource_manager_ = &resource_manager_;
  }

  void End() override {}

  void Update() override {
    ClearBackground(BLACK);
    if (IsKeyPressed(KEY_Q)) {
      GetSceneManager()->SwitchScene("editor");
    }

    if (IsKeyDown(KEY_LEFT)) {
      player_dir_ = Vector2Rotate(player_dir_, 2.0 * DEG2RAD);
      plane_ = Vector2Rotate(plane_, 2.0 * DEG2RAD);
    }
    if (IsKeyDown(KEY_RIGHT)) {
      player_dir_ = Vector2Rotate(player_dir_, -2.0 * DEG2RAD);
      plane_ = Vector2Rotate(plane_, -2.0 * DEG2RAD);
    }

    std::vector<uint32_t> level_copy = info_.map_;
    int width = info_.map_width_;

    auto collides = [level_copy, width](int x, int y) {
      if (level_copy[x + width * y] > 0) {
        return true;
      }
      return false;
    };

    if (IsKeyDown(KEY_UP)) {
      Vector2 new_pos = Vector2Add(player_pos_, Vector2Scale(player_dir_, 2.0 * GetFrameTime()));
      int px = (int)new_pos.x; 
      int py = (int)new_pos.y;
      if (!collides(px, py)) player_pos_ = new_pos;
    }
    if (IsKeyDown(KEY_DOWN)) {
      Vector2 new_pos = Vector2Subtract(player_pos_, Vector2Scale(player_dir_, 2.0 * GetFrameTime()));
      int px = (int)new_pos.x;
      int py = (int)new_pos.y;
      if (!collides(px, py)) player_pos_ = new_pos;
    }

    info_.dir_ = player_dir_;
    info_.pos_ = player_pos_;
    info_.plane_ = plane_;

    caster_.ClearBuffer(BLACK);
    caster_.Render(info_);
    caster_.UpdateBuffer();

    caster_.Draw();
  }

  Vector2 player_pos_ = { 2, 3 };
  Vector2 player_dir_ = { 0, -1 };
  Vector2 plane_ = { -0.66, 0 };


  Raycaster caster_; 
  Raycaster::Info info_;
  ResourceManager resource_manager_;
};

int main(void) { 
  App app(SCREEN_WIDTH, SCREEN_HEIGHT, "Horror Game :P");

  app.GetSceneManager()->AddScene<RedScreen>("game");
  app.GetSceneManager()->AddScene<LevelEditor>("editor");

  app.GetSceneManager()->SwitchScene("editor");
  app.Start();

  return 0;
}