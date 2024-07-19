#include "raycaster.h"

#include <raylib/rlgl.h>

#include <numeric>

#include "resource_manager.h"
#include "level_info.h"

Raycaster::~Raycaster() {
  rlUnloadTexture(render_texture_.id);
}


void Raycaster::InitializeBuffer(uint32_t width, uint32_t height) {
  if (width == 0 || height == 0) {
    TraceLog(LOG_FATAL, "Pixel buffer dimensions cannot be 0!");
  }
  pixel_buffer_.reserve(width * height);

  for (int i = 0; i < width * height; ++i) {
    pixel_buffer_.push_back(BLACK);
  }

  width_ = width;
  height_ = height;

  render_texture_.id = rlLoadTexture(nullptr, width_, height_, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1); 
}

void Raycaster::Render(Info info) {
  float screen_width = width_;

  for (int y = 0; y < height_; ++y) {
    Vector2 raydir1 = Vector2Subtract(info.dir_, info.plane_);
    Vector2 raydir0 = Vector2Add(info.dir_, info.plane_);

    int dist_from_center = y - height_ * 0.5;
    float camera_y = height_ * 0.5;

    float row_distance = camera_y / dist_from_center;

    Vector2 dir_step = Vector2Subtract(raydir1, raydir0);
    Vector2 floor_step = Vector2Scale(Vector2Scale(dir_step, row_distance), 1 / screen_width);

    Vector2 floor_pos = { 
      info.pos_.x + row_distance * raydir0.x, 
      info.pos_.y + row_distance * raydir0.y 
    };

    for (int x = 0; x < width_; ++x) {
      int cell_x = (int)floor_pos.x;
      int cell_y = (int)floor_pos.y;

      ResourceManager::ImageResource* res = info.resource_manager_->GetImageDataByIndex(info.floor_index_);
      if (res == nullptr) break;

      int tex_width = res->image_.width;
      int tex_height = res->image_.height;
      
      int texture_x = (int)(tex_width * (floor_pos.x - cell_x)) & (tex_width - 1);
      int texture_y = (int)(tex_height * (floor_pos.y - cell_y)) & (tex_height - 1);     

      floor_pos = Vector2Add(floor_pos, floor_step);

      Color floor_texture = res->color_data_[tex_width * texture_y + texture_x];
      pixel_buffer_[x + width_ * y] = floor_texture;

      res = info.resource_manager_->GetImageDataByIndex(info.ceiling_index_);
      if (res == nullptr) break;

      Color ceiling_texture = res->color_data_[tex_width * texture_y + texture_x];

      int ceiling_y = height_ - y - 1;
      pixel_buffer_[x + width_ * ceiling_y] = ceiling_texture;
    }
  }
 
  for (int x = 0; x < screen_width; ++x) {
    float camera_x = 2 * x / (float)screen_width - 1;
    
    Vector2 raydir;
    raydir.x = info.dir_.x + info.plane_.x * -camera_x;
    raydir.y = info.dir_.y + info.plane_.y * -camera_x;

    int map_x = static_cast<int>(info.pos_.x);
    int map_y = static_cast<int>(info.pos_.y);
 
    Vector2 delta_dist;
    float infinity = std::numeric_limits<float>().max();

    delta_dist.x = raydir.x == 0 ? infinity : std::abs(1 / raydir.x);
    delta_dist.y = raydir.y == 0 ? infinity : std::abs(1 / raydir.y);

    Vector2 side_distance;
    float perp_wall_distance = 0.0;

    int step_x = 0;
    int step_y = 0;

    bool hit = false;
    bool y_side = false;

    if (raydir.x < 0.0) {
      step_x = -1;
      side_distance.x = (info.pos_.x - map_x) * delta_dist.x;
    } else {
      step_x = 1;
      side_distance.x = (map_x + 1.0 - info.pos_.x) * delta_dist.x;
    }

    if (raydir.y < 0.0) {
      step_y = -1;
      side_distance.y = (info.pos_.y - map_y) * delta_dist.y;
    } else {
      step_y = 1;
      side_distance.y = (map_y + 1.0 - info.pos_.y) * delta_dist.y;
    }

    uint32_t map_wall = info.map_[info.map_height_ * map_y + map_x];

    while (!hit) {
      if (side_distance.x < side_distance.y) {
        side_distance.x += delta_dist.x;
        map_x += step_x;
        y_side = false;
      } else {
        side_distance.y += delta_dist.y;
        map_y += step_y;
        y_side = true;
      }      

      map_wall = info.map_[map_x + info.map_width_ * map_y];

      if (map_wall > 0) {
        hit = true;
      }
    }

    if (!y_side) {
      perp_wall_distance = side_distance.x - delta_dist.x;
    } else {
      perp_wall_distance = side_distance.y - delta_dist.y;
    }
  
    int screen_height = height_;
    if (perp_wall_distance <= 0.0) perp_wall_distance = 1.0;
    int line_height = static_cast<int>(screen_height / std::abs(perp_wall_distance)); 
  
    int line_start = (screen_height - line_height) * 0.5;
    int line_end = (line_height + screen_height) * 0.5;

    if (line_start < 0) line_start = 0;
    if (line_end >= screen_height) line_end = screen_height - 1;
 
    int texture_index = map_wall - 1;
    
    //hacky and dirty... fix
    ResourceManager::ImageResource* image_res = info.resource_manager_->GetImageDataByIndex(texture_index);
    if (image_res == nullptr) {
      for (int y = line_start; y < line_end; ++y) {
        Color color = PINK;
        pixel_buffer_[x + width_ * y] = color;
      }
      return;
    } 

    float wall_x = 0;

    if (!y_side) {
      wall_x = info.pos_.y + perp_wall_distance * raydir.y;
    } else {
      wall_x = info.pos_.x + perp_wall_distance * raydir.x;
    }

    wall_x -= floorf(wall_x);

    float tex_width = image_res->image_.width;
    float tex_height = image_res->image_.height;

    int tex_x = static_cast<int>(wall_x * tex_width);

    if (!y_side && raydir.x > 0) {
      tex_x = tex_width - tex_x - 1;
    } else if (y_side && raydir.y < 0) {
      tex_x = tex_width - tex_x - 1;
    }

    float step = tex_height / line_height;

    float tex_pos = (line_start - screen_height / 2 + line_height / 2) * step;


    for (int y = line_start; y < line_end; ++y) {
      int tex_y = static_cast<int>(tex_pos) & (static_cast<int>(tex_height) - 1);
      tex_pos += step;

      Color color = image_res->color_data_[static_cast<int>(tex_width) * tex_x + tex_y];
      if (y_side) color = ColorBrightness(color, 0.1);

      pixel_buffer_[x + width_ * y] = color;
    }
  }
}

void Raycaster::ClearBuffer(Color color) {
  for (int x = 0; x < width_; ++x) {
    for (int y = 0; y < height_; ++y) {
      pixel_buffer_[x + width_ * y] = color;
    }
  }
  UpdateBuffer();
}

void Raycaster::UpdateBuffer() {
  render_texture_.width = width_;
  render_texture_.height = height_;
  render_texture_.mipmaps = 1;
  render_texture_.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  UpdateTexture(render_texture_, (void*)pixel_buffer_.data());
}

void Raycaster::Draw() {
  float screen_width = GetScreenWidth();
  float screen_height = GetScreenHeight();
  DrawTexturePro(
    render_texture_, 
    { 0, 0, (float)width_, (float)height_ }, 
    { 0, 0, screen_width, screen_height }, 
    { 0, 0 }, 
    0.0, 
    WHITE
  );
}

void Raycaster::FillLevelData(
  Raycaster::Info& raycaster_info, 
  struct LevelInfo level_info,
  struct ResourceManager* manager
) {
  if (manager == nullptr) {
    TraceLog(LOG_ERROR, "RESOURCE MANAGER IS NULL");
    return;
  }
  raycaster_info.resource_manager_ = manager;
  raycaster_info.map_ = level_info.map_data_;
  raycaster_info.map_width_ = level_info.map_width_;
  raycaster_info.map_height_ = level_info.map_height_;
  raycaster_info.pos_ = { level_info.starting_pos_x_, level_info.starting_pos_y_ };
  raycaster_info.dir_ = { level_info.dir_x_, level_info.dir_y_ };
  raycaster_info.plane_ = { level_info.plane_x_, level_info.plane_y_ };
  raycaster_info.floor_index_ = level_info.floor_index_;
  raycaster_info.ceiling_index_ = level_info.ceiling_index_;
  for (const std::string& path : level_info.texture_paths_) {
    raycaster_info.resource_manager_->CreateImage(path);
  }
}
