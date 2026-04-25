#include <raylib.h>

#include <stdio.h>

#include "PopupStack.h"
#include "Captcha.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 1.5

typedef struct {
    Sound incorrect;
    Sound captchaDone;
    Sound vine;
} Sounds;

struct {
  int playerLives;
  int completedCaptchas;
  PopupStack popupStack;
  TextureHashMap texturesMap;
  float popupSpawnTimer;
  Sounds soundLib;
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

  InitAudioDevice();

  Globals.soundLib.incorrect = LoadSound("resources/audio/laugh.wav");
  Globals.soundLib.captchaDone = LoadSound("resources/audio/laugh.wav");
  Globals.soundLib.vine = LoadSound("resources/audio/laugh.wav");

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
      PlaySound(Globals.soundLib.incorrect);
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
  InitWindow(1000, 1000, "Window");

  InitGlobals();

  printf("Started Game\n");

  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }

  return 0;
}
