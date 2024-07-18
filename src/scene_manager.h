#ifndef SCENE_MANAGER_H_
#define SCENE_MANAGER_H_

#include <memory>
#include <unordered_map>
#include <type_traits>

#include "scene.h"

class SceneManager {
public:
  SceneManager() = default;

  template <typename T>
  void AddScene(const std::string& name) {
    static_assert(std::is_base_of<Scene, T>(), "New scene needs to derive from base class scene!");
    AddScene(std::make_unique<T>(name));
  }

  void AddScene(std::unique_ptr<Scene>&& new_scene);

  bool SceneExists(const std::string& name) const;

  void SwitchScene(const std::string& new_scene);
  std::unique_ptr<Scene>* GetCurrentScene();

  template <typename T>
  T* GetScene(const std::string& name) {
    static_assert(std::is_base_of<Scene, T>(), "Type must derive from scene!");
    return dynamic_cast<T*>(GetScene(name)->get());
  }

  std::unique_ptr<Scene>* GetScene(const std::string& name);

  //Call in Update Loop
  void LoadScene();
private:
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
  std::string current_scene_;
  std::string scene_to_load_;
};

#endif