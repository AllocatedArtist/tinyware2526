#include <raylib.h>

#include <stdio.h>

#include "PopupStack.h"

#define PLAYER_LIVES 3

struct {
  int playerLives;
  PopupStack popupStack;
  Popup currentPopup;
} Globals;

void InitGlobals() {
  Globals.playerLives = PLAYER_LIVES;
  Globals.popupStack = PopupStackCreate();
  Globals.currentPopup = PopupDefault();
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  DrawTextureRec(
      Globals.currentPopup.imageTexture,
      (Rectangle){.x = 0.0f,
                  .y = 0.0f,
                  .width = Globals.currentPopup.imageTexture.width,
                  .height = Globals.currentPopup.imageTexture.height},
      (Vector2){.x = 0.0f, .y = 0.0f}, WHITE);

  EndDrawing();
}

int main() {
  InitWindow(1600, 1480, "Window");

  PopupStack popups = PopupStackCreate();
  PopupStackPush(&popups, "resources/textures/test1.jpg", 69);
  PopupStackPush(&popups, "resources/textures/test2.jpg", 420);
  PopupStackPush(&popups, "resources/textures/test3.jpg", 21);

  printf("Started Game\n");

  Globals.currentPopup = PopupStackPop(&popups);

  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }

  // This will never be called in the web build
  PopupStackDelete(&popups);

  return 0;
}
