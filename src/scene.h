#ifndef SCENE_H_
#define SCENE_H_

#include <string>

class Scene {
public:
  Scene(const std::string& name); 
  virtual ~Scene() = default;

  const std::string& GetName() const;

  class SceneManager* GetSceneManager();

  virtual void Start() = 0;
  virtual void Update() = 0;
  virtual void End() = 0;
private:
  std::string name_;
  friend class SceneManager;
  class SceneManager* manager_;
};


#endif