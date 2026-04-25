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

  PopupStackPush(&Globals.popupStack, "resources/textures/test1.png", 6);
  PopupStackPush(&Globals.popupStack, "resources/textures/test2.png", 4);
  PopupStackPush(&Globals.popupStack, "resources/textures/test3.png", 2);
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  if (!PopupStackIsEmpty(Globals.popupStack)) {
    switch (PopupStackReadInput(Globals.popupStack)) {
    case POPUP_PRESSED_FAILURE:
      printf("Wrong number!\n");
      break;
    case POPUP_PRESSED_SUCCESSFULLY:
      printf("Right number!\n");
      break;
    default: // Idling
      break;
    }
    PopupStackDraw(Globals.popupStack);
  }

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
