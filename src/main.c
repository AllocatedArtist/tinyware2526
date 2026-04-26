#include <raylib.h>

#include <stdio.h>

#include "Captcha.h"
#include "PopupStack.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 1.5

struct {
    Music backgroundMusic;
    Sound incorrect;
    Sound adPop;
    Sound captchaSpawn;
    Sound captchaDone;
    Sound vine;
} Sounds;

struct {
  int playerLives;
  int completedCaptchas;
  PopupStack popupStack;
  TextureHashMap texturesMap;
  float popupSpawnTimer;
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

  Sounds.backgroundMusic = LoadMusicStream("resources/audio/background.mp3");
  Sounds.incorrect = LoadSound("resources/audio/Laugh.wav");
  Sounds.adPop = LoadSound("resources/audio/adpop.mp3");
  Sounds.captchaSpawn = LoadSound("resources/audio/captchasound.mp3");
  Sounds.captchaDone = LoadSound("resources/audio/celebration.mp3");
  Sounds.vine = LoadSound("resources/audio/vineboom.mp3");

  PlayMusicStream(Sounds.backgroundMusic);
  SetMusicVolume(Sounds.backgroundMusic, 1.0f);

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
  UpdateMusicStream(Sounds.backgroundMusic);
  if (!PopupStackIsEmpty(Globals.popupStack) &&
      Globals.currentCaptcha.type == CAPTCHA_TYPE_NONE) {
    PopupStackDraw(Globals.popupStack);
    switch (PopupStackReadInput(Globals.popupStack)) {
    case POPUP_PRESSED_FAILURE:
      PlaySound(Sounds.incorrect);
      LoseLife();
      break;
    case POPUP_PRESSED_SUCCESSFULLY:
      PlaySound(Sounds.adPop);
      PopupStackPop(&Globals.popupStack);
      break;
    default: // Idling
      break;
    }
  } else if (Globals.currentCaptcha.type != CAPTCHA_TYPE_NONE) {
    CaptchaDraw(&Globals.currentCaptcha);
    switch (CaptchaCheck(&Globals.currentCaptcha)) {
    case CAPTCHA_PRESSED_FAILURE:
      PlaySound(Sounds.incorrect);
      CaptchaCreateRandom(&Globals.texturesMap, &Globals.currentCaptcha);
      LoseLife();
      break;
    case CAPTCHA_PRESSED_SUCCESSFULLY:
      PlaySound(Sounds.captchaDone);
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
