#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

#include <emscripten/emscripten.h>

#include <iostream>
#include <array>

constexpr uint32_t MAP_WIDTH = 10;
constexpr uint32_t MAP_HEIGHT = 10;

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 400;

int map[MAP_WIDTH][MAP_HEIGHT] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 2, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 2, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 2, 1, 2, 1, 0, 1,
  1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  1, 0, 0, 2, 0, 0, 0, 0, 2, 1,
  1, 0, 0, 0, 0, 2, 2, 0, 2, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

std::array<std::array<Color, SCREEN_WIDTH>, SCREEN_HEIGHT> pixel_buffer = { 0 };
std::array<uint8_t, SCREEN_WIDTH * SCREEN_HEIGHT> pixel_buffer_tex = { 0 };

Vector2 player_pos = { 1, 2 };
Vector2 player_dir = { 0, -1 };
Vector2 plane = { -0.66, 0 };

Image wall_texture;
Color* wall_colors;

Texture pixel_texture;

float current_time = 0.0;
float previous_time = 0.0;

void DrawGridLines(void) {
  for (int x = 0; x < MAP_WIDTH; ++x) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
      if (map[y][x] == 0) {
        DrawRectangle(x * 10, y * 10, 10, 10, WHITE);
        DrawRectangleLines(x * 10 , y * 10, 10, 10, BLACK);
      } else {
        DrawRectangle(x * 10 , y * 10, 10, 10, BLACK);
      }
    }
  }
}

void Raycast() {
  float screen_width = GetScreenWidth();
 
  for (int x = 0; x < screen_width; ++x) {
    float camera_x = 2 * x / (float)screen_width - 1;
    
    Vector2 raydir;
    raydir.x = player_dir.x + plane.x * camera_x;
    raydir.y = player_dir.y + plane.y * camera_x;

    int map_x = static_cast<int>(player_pos.x);
    int map_y = static_cast<int>(player_pos.y);
 
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
      side_distance.x = (player_pos.x - map_x) * delta_dist.x;
    } else {
      step_x = 1;
      side_distance.x = (map_x + 1.0 - player_pos.x) * delta_dist.x;
    }

    if (raydir.y < 0.0) {
      step_y = -1;
      side_distance.y = (player_pos.y - map_y) * delta_dist.y;
    } else {
      step_y = 1;
      side_distance.y = (map_y + 1.0 - player_pos.y) * delta_dist.y;
    }

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

      if (map[map_y][map_x] > 0) {
        hit = true;
      }
    }

    if (!y_side) {
      perp_wall_distance = side_distance.x - delta_dist.x;
    } else {
      perp_wall_distance = side_distance.y - delta_dist.y;
    }
  
    int screen_height = GetScreenHeight();
    if (perp_wall_distance <= 0.0) perp_wall_distance = 1.0;
    int line_height = static_cast<int>(screen_height / std::abs(perp_wall_distance)); 
 
    int color_index = map[map_y][map_x];
    Color color;

    switch (color_index) {
      case 1:
        color = BLUE;
        break;
      case 2:
        color = RED;
        break;
    }

    color = y_side ? ColorBrightness(color, 0.1) : color;

    int line_start = (screen_height - line_height) * 0.5;
    int line_end = (line_height + screen_height) * 0.5;

    if (line_start < 0) line_start = 0;
    if (line_end >= screen_height) line_end = screen_height - 1;

    int texture_index = map[map_y][map_x] - 1;

    float wall_x = 0;

    if (!y_side) {
      wall_x = player_pos.y + perp_wall_distance * raydir.y;
    } else {
      wall_x = player_pos.x + perp_wall_distance * raydir.x;
    }

    wall_x -= floorf(wall_x);

    float tex_width = 128;
    float tex_height = 128;

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
      Color color = wall_colors[static_cast<int>(tex_width) * tex_x + tex_y];
      if (y_side) color = ColorBrightness(color, 0.1);
      pixel_buffer[y][x] = color;
    }
  }
}

void UpdateTextureBuffer() {
  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; x += 4) {
      Color color = pixel_buffer[y][x];
      pixel_buffer_tex[SCREEN_HEIGHT * y + x] = color.r;
      pixel_buffer_tex[SCREEN_HEIGHT * y + x + 1] = color.g;
      pixel_buffer_tex[SCREEN_HEIGHT * y + x + 2] = color.b;
      pixel_buffer_tex[SCREEN_HEIGHT * y + x + 3] = color.a;
    }
  }

  pixel_texture.width = SCREEN_WIDTH;
  pixel_texture.height = SCREEN_HEIGHT;
  pixel_texture.mipmaps = 1;
  pixel_texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
  UpdateTexture(pixel_texture, (void*)pixel_buffer.data());
}

void Loop(void) {
  BeginDrawing(); 
  ClearBackground(BLACK);

  if (IsKeyDown(KEY_LEFT)) {
    player_dir = Vector2Rotate(player_dir, 2.0 * DEG2RAD);
    plane = Vector2Rotate(plane, 2.0 * DEG2RAD);
  }
  if (IsKeyDown(KEY_RIGHT)) {
    player_dir = Vector2Rotate(player_dir, -2.0 * DEG2RAD);
    plane = Vector2Rotate(plane, -2.0 * DEG2RAD);
  }
  if (IsKeyDown(KEY_UP)) {
    Vector2 new_pos = Vector2Add(player_pos, Vector2Scale(player_dir, 2.0 * GetFrameTime()));
    int px = (int)new_pos.x; 
    int py = (int)new_pos.y;
    if (map[py][px] == 0) player_pos = new_pos;
  }
  if (IsKeyDown(KEY_DOWN)) {
    Vector2 new_pos = Vector2Subtract(player_pos, Vector2Scale(player_dir, 2.0 * GetFrameTime()));
    int px = (int)new_pos.x;
    int py = (int)new_pos.y;
    if (map[py][px] == 0) player_pos = new_pos;
  }

  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
      Color color = pixel_buffer[y][x];
      pixel_buffer[y][x] = BLACK;
    }
  }

  UpdateTextureBuffer();
  Raycast(); 
  UpdateTextureBuffer();

  DrawTexture(pixel_texture, 0, 0, WHITE);

  DrawFPS(0, 0);
  
  EndDrawing();
}

int main(void) {
  InitWindow(800, 400, "My horror game :)");

  wall_texture = LoadImage("assets/Brick/Brick_06-128x128.png"); 
  wall_colors = LoadImageColors(wall_texture);

  int tex_size = wall_texture.width;
  for (int x = 0; x < tex_size; ++x) {
    for (int y = 0; y < x; ++y) {
      std::swap(wall_colors[tex_size * y + x], wall_colors[tex_size * x + y]);
    }
  }

  pixel_texture.id = rlLoadTexture(nullptr, SCREEN_WIDTH, SCREEN_HEIGHT, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1); 

  emscripten_set_main_loop(Loop, 0, 1);

  return 0;
}