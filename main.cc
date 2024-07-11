
#include <raylib/raylib.h>

#include <emscripten/emscripten.h>
#include <iostream>


void Loop(void) {
  BeginDrawing(); 
  ClearBackground(RAYWHITE);

  DrawFPS(0, 0);

  EndDrawing();
}

int main(void) {

  InitWindow(800, 400, "My horror game :)");
  std::cout << "Let the horror begin!" << std::endl;
  emscripten_set_main_loop(Loop, 0, 1);
   
  


  return 0;
}
