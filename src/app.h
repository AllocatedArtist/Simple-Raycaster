#ifndef APP_H_
#define APP_H_

#include <cstdint>

#include "scene_manager.h"

class App {
public:
  App(uint32_t width, uint32_t height, const char* title);  
  SceneManager* GetSceneManager();
  void Start();
private:
  static void Run(void* app);
  SceneManager scene_manager_;
};


#endif