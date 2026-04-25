#include <raylib.h>
#include <stdio.h>

int main() {
  InitWindow(1600, 1480, "Window");
  printf("Started Game");
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    EndDrawing();
  }
  return 0;
}
