#include "scene_manager.h"

#include <raylib/raylib.h>

void SceneManager::AddScene(std::unique_ptr<Scene>&& new_scene) {
  if (new_scene == nullptr) {
    TraceLog(LOG_FATAL, "Unable to create new scene!");
    return;
  }   
  if (SceneExists(new_scene->GetName())) {
    TraceLog(LOG_FATAL, "Scene already exists! [%s]", new_scene->GetName().c_str());
  }

  std::string scene_name = new_scene->GetName();
  scenes_.emplace(std::make_pair(scene_name, std::move(new_scene)));
  scenes_.at(scene_name)->manager_ = this;
}

bool SceneManager::SceneExists(const std::string& name) const {
  return scenes_.find(name) != scenes_.cend();
}

void SceneManager::SwitchScene(const std::string& new_scene) {
  if (SceneExists(new_scene)) {
    scene_to_load_ = new_scene;
  } else {
    TraceLog(LOG_WARNING, "Scene does not exist (can't load): %s", new_scene.c_str());
  }
}

std::unique_ptr<Scene>* SceneManager::GetCurrentScene() {
  if (current_scene_.empty()) {
    return nullptr;
  }
  return &scenes_.at(current_scene_);
}

void SceneManager::LoadScene() {
  if (scene_to_load_.empty()) {
    return;
  }

  if (!current_scene_.empty()) {
    scenes_.at(current_scene_)->End();
  }

  current_scene_ = scene_to_load_;
  scenes_.at(current_scene_)->Start();

  scene_to_load_.clear();
}

