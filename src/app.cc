#include "app.h"

#include <emscripten/emscripten.h>
#include <raylib/raylib.h>

#include <iostream>

App::App(uint32_t width, uint32_t height, const char* title) {
  InitWindow(width, height, title);  
}

void App::Start() {
  emscripten_set_main_loop_arg(App::Run, this, 0, 1);
}

SceneManager* App::GetSceneManager() {
  return &scene_manager_;
}

void App::Run(void* app) {
  App* app_ptr = reinterpret_cast<App*>(app);

  SceneManager* manager = app_ptr->GetSceneManager();

  //hopefully this never happens :)
  if (manager == nullptr) return;

  manager->LoadScene();

  BeginDrawing();

  std::unique_ptr<Scene>* current_scene = manager->GetCurrentScene();
  if (current_scene != nullptr) {
    current_scene->get()->Update();
  } else {
    ClearBackground(BLACK);
    int scr_width = GetScreenWidth();
    int scr_height = GetScreenHeight();

    int pos_x = (scr_width - MeasureText("NO SCENE!", 24)) * 0.5;

    DrawText("NO SCENE!", pos_x, scr_height / 2, 24, WHITE);
  }

  EndDrawing();
}