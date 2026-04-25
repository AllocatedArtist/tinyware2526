#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_DEFAULT_CAPACITY 5
#define PLAYER_LIVES 3

struct {
  int playerLives;
} Globals;

typedef struct {
  const char *image;
  int number;
} Popup;

typedef struct {
  Popup *data;
  int headIdx;
  int size;
  int capacity;
} PopupStack;

PopupStack PopupStackCreate() {
  PopupStack stack;
  stack.data = malloc(sizeof(PopupStack) * STACK_DEFAULT_CAPACITY);
  stack.headIdx = -1;
  stack.size = 0;
  stack.capacity = STACK_DEFAULT_CAPACITY;
}

void PopupStackDelete(PopupStack *popupStack) { free(popupStack->data); }

void InitGlobals() { Globals.playerLives = PLAYER_LIVES; }

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  EndDrawing();
}

int main() {
  InitWindow(1600, 1480, "Window");
  printf("Started Game");
  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }
  return 0;
}
