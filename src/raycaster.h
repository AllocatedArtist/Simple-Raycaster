#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <cstdint>

#include <vector>

class Raycaster {
public:
  Raycaster() = default;
  ~Raycaster();

  struct Info {
    Vector2 pos_;
    Vector2 dir_;
    Vector2 plane_;

    int ceiling_index_ = 0, floor_index_ = 0;

    std::vector<uint32_t> map_;
    uint32_t map_width_;
    uint32_t map_height_;

    class ResourceManager* resource_manager_;
  };

  void InitializeBuffer(uint32_t width, uint32_t height);

  void Render(Info info);

  void ClearBuffer(Color color);
  void UpdateBuffer();

  void Draw();

  static void FillLevelData(Info& raycaster_info, struct LevelInfo level_info, struct ResourceManager* manager);
private:
  std::vector<Color> pixel_buffer_;
  Texture render_texture_;

  uint32_t width_, height_;
};


#endif