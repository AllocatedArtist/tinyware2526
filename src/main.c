#include <raylib.h>

#include <stdio.h>

#include "Captcha.h"
#include "PopupStack.h"

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

  Captcha currentCaptcha;
} Globals;

void UpdatePopupSpawnTimer() {
  Globals.popupSpawnTimer += GetFrameTime();
  if (Globals.popupSpawnTimer >= POPUP_SPAWNRATE) {
    Globals.popupSpawnTimer = 0.0f;
    SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
  }
}

void LoseLife() {
  --Globals.playerLives;
  if (Globals.playerLives <= 0) {
    // Lose Game
  }
}

void InitGlobals() {
  Globals.playerLives = PLAYER_LIVES;
  Globals.popupStack = PopupStackCreate();

  InitAudioDevice();

  Globals.soundLib.incorrect = LoadSound("resources/audio/Laugh.wav");
  Globals.soundLib.captchaDone = LoadSound("resources/audio/Laugh.wav");
  Globals.soundLib.vine = LoadSound("resources/audio/Laugh.wav");

  Globals.texturesMap = TextureHashMapCreate();

  Globals.currentCaptcha = CaptchaDefault();

  LoadAllPopupTextures(&Globals.texturesMap);
  LoadAllCaptchaTextures(&Globals.texturesMap);

  SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  UpdatePopupSpawnTimer();
  if (!PopupStackIsEmpty(Globals.popupStack) &&
      Globals.currentCaptcha.type == CAPTCHA_TYPE_NONE) {
    PopupStackDraw(Globals.popupStack);
    switch (PopupStackReadInput(Globals.popupStack)) {
    case POPUP_PRESSED_FAILURE:
      PlaySound(Globals.soundLib.incorrect);
      LoseLife();
      break;
    case POPUP_PRESSED_SUCCESSFULLY:
      PopupStackPop(&Globals.popupStack);
      break;
    default: // Idling
      break;
    }
  } else if (Globals.currentCaptcha.type != CAPTCHA_TYPE_NONE) {
    CaptchaDraw(&Globals.currentCaptcha);
    switch (CaptchaCheck(&Globals.currentCaptcha)) {
    case CAPTCHA_PRESSED_FAILURE:
      CaptchaCreateRandom(&Globals.texturesMap, &Globals.currentCaptcha);
      LoseLife();
      break;
    case CAPTCHA_PRESSED_SUCCESSFULLY:
      CaptchaCreateRandom(&Globals.texturesMap, &Globals.currentCaptcha);
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
