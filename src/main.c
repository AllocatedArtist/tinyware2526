#include <raylib.h>

#include <stdio.h>

#include "PopupStack.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 1.5

struct {
  int playerLives;
  PopupStack popupStack;
  TextureHashMap texturesMap;
  float popupSpawnTimer;
} Globals;

void UpdatePopupSpawnTimer() {
  Globals.popupSpawnTimer += GetFrameTime();
  if (Globals.popupSpawnTimer >= POPUP_SPAWNRATE) {
    Globals.popupSpawnTimer = 0.0f;
    SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
  }
}

void InitGlobals() {
  Globals.playerLives = PLAYER_LIVES;
  Globals.popupStack = PopupStackCreate();

  LoadAllPopupTextures(&Globals.texturesMap);

  SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  UpdatePopupSpawnTimer();
  if (!PopupStackIsEmpty(Globals.popupStack)) {
    PopupStackDraw(Globals.popupStack);
    switch (PopupStackReadInput(Globals.popupStack)) {
    case POPUP_PRESSED_FAILURE:
      printf("Wrong number!\n");
      break;
    case POPUP_PRESSED_SUCCESSFULLY:
      PopupStackPop(&Globals.popupStack);
      break;
    default: // Idling
      break;
    }
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

  return 0;
}
