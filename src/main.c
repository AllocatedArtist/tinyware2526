#include <raylib.h>

#include <stdio.h>

#include "PopupStack.h"

#define PLAYER_LIVES 3

struct {
  int playerLives;
  PopupStack popupStack;
} Globals;

void InitGlobals() {
  Globals.playerLives = PLAYER_LIVES;
  Globals.popupStack = PopupStackCreate();

  PopupStackPush(&Globals.popupStack, "resources/textures/test1.png", 69);
  PopupStackPush(&Globals.popupStack, "resources/textures/test2.png", 420);
  PopupStackPush(&Globals.popupStack, "resources/textures/test3.png", 21);
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  Popup currentPopup = PopupStackPeek(Globals.popupStack);

  DrawTextureRec(currentPopup.imageTexture,
                 (Rectangle){.x = 0.0f,
                             .y = 0.0f,
                             .width = currentPopup.imageTexture.width,
                             .height = currentPopup.imageTexture.height},
                 (Vector2){.x = 100.0f, .y = 100.0f}, WHITE);

  EndDrawing();
}

int main() {
  InitWindow(1600, 1480, "Window");

  InitGlobals();

  printf("Started Game\n");

  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }

  // This will never be called in the web build
  PopupStackDelete(&Globals.popupStack);

  return 0;
}
