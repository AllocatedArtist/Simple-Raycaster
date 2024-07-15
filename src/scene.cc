#include "scene.h"

#include <emscripten/emscripten.h>

#include "scene_manager.h"

Scene::Scene(const std::string& name) {
  if (!name.empty()) {
    name_ = name;
  } else {
    emscripten_throw_string("Can't create scene with empty name!");
  }
}

SceneManager* Scene::GetSceneManager() {
  return manager_;
}

const std::string& Scene::GetName() const {
  return name_;
}
