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
  }

  void Start() override {
    info_.dir_ = player_dir_;
    info_.pos_ = player_pos_;
    info_.plane_ = plane_;
    info_.map_width_ = MAP_WIDTH;
    info_.map_height_ = MAP_HEIGHT;

    info_.map_.reserve(MAP_WIDTH * MAP_HEIGHT);
    for (int x = 0; x < MAP_WIDTH; ++x) {
      for (int y = 0; y < MAP_HEIGHT; ++y) {
        info_.map_.push_back(map[x][y]);
      }
    }
 
    info_.resource_manager_ = &resource_manager_;
  }

  void End() override {}

  void Update() override {
    ClearBackground(BLACK);
    if (IsKeyPressed(KEY_Q)) {
      GetSceneManager()->SwitchScene("blue");
    }

    if (IsKeyDown(KEY_LEFT)) {
      player_dir_ = Vector2Rotate(player_dir_, 2.0 * DEG2RAD);
      plane_ = Vector2Rotate(plane_, 2.0 * DEG2RAD);
    }
    if (IsKeyDown(KEY_RIGHT)) {
      player_dir_ = Vector2Rotate(player_dir_, -2.0 * DEG2RAD);
      plane_ = Vector2Rotate(plane_, -2.0 * DEG2RAD);
    }
    if (IsKeyDown(KEY_UP)) {
      Vector2 new_pos = Vector2Add(player_pos_, Vector2Scale(player_dir_, 2.0 * GetFrameTime()));
      int px = (int)new_pos.x; 
      int py = (int)new_pos.y;
      if (map[py][px] == 0) player_pos_ = new_pos;
    }
    if (IsKeyDown(KEY_DOWN)) {
      Vector2 new_pos = Vector2Subtract(player_pos_, Vector2Scale(player_dir_, 2.0 * GetFrameTime()));
      int px = (int)new_pos.x;
      int py = (int)new_pos.y;
      if (map[py][px] == 0) player_pos_ = new_pos;
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

class BlueScreen : public Scene {
public:
  BlueScreen(const std::string& name) : Scene(name) {}
  void Start() override {}
  void End() override {}
  void Update() override {
    ClearBackground(BLUE);
    if (IsKeyPressed(KEY_E)) {
      GetSceneManager()->SwitchScene("red");
    }
  }
};

int main(void) { 
  App app(SCREEN_WIDTH, SCREEN_HEIGHT, "Horror Game :P");

  app.GetSceneManager()->AddScene<RedScreen>("red");
  app.GetSceneManager()->AddScene<BlueScreen>("blue");

  app.GetSceneManager()->SwitchScene("red");
  app.Start();

  return 0;
}