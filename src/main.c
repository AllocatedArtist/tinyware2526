#include <raylib.h>

#include <stdio.h>

#include "Captcha.h"
#include "PopupStack.h"
#include "Timer.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 2.0
#define CAPTCHA_SPAWNRATE 1
#define MAX_CAPTCHAS_COMPLETED 10

struct {
  Music backgroundMusic;
  Sound incorrect;
  Sound captchaSpawn;
  Sound captchaDone;
  Sound vine;
} Sounds;

struct {
  int playerLives;
  int completedCaptchas;
  PopupStack popupStack;
  TextureHashMap texturesMap;
  Timer popupSpawnTimer;
  Timer captchaSpawnTimer;
  Captcha currentCaptcha;
} Globals;

void PopupSpawnTimer() {
  SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
}

void CaptchaSpawnTimer() {
  CaptchaCreateRandom(&Globals.texturesMap, &Globals.currentCaptcha);
}

void LoseLife() {
  --Globals.playerLives;
  if (Globals.playerLives <= 0) {
    // Lose Game
    TimerEnd(&Globals.popupSpawnTimer);
    TimerEnd(&Globals.captchaSpawnTimer);
  }
}

void InitGlobals() {
  Globals.playerLives = PLAYER_LIVES;
  Globals.popupStack = PopupStackCreate();
  Globals.completedCaptchas = 0;

  InitAudioDevice();

  Sounds.backgroundMusic = LoadMusicStream("resources/audio/background.mp3");
  Sounds.incorrect = LoadSound("resources/audio/Laugh.wav");
  Sounds.captchaSpawn = LoadSound("resources/audio/captchasound.mp3");
  Sounds.captchaDone = LoadSound("resources/audio/celebration.mp3");
  Sounds.vine = LoadSound("resources/audio/vineboom.pm3");

  Globals.texturesMap = TextureHashMapCreate();

  Globals.currentCaptcha = CaptchaDefault();

  LoadAllPopupTextures(&Globals.texturesMap);
  LoadAllCaptchaTextures(&Globals.texturesMap);

  Globals.popupSpawnTimer = TimerCreate(POPUP_SPAWNRATE, 1, PopupSpawnTimer);
  Globals.captchaSpawnTimer =
      TimerCreate(CAPTCHA_SPAWNRATE, 0, CaptchaSpawnTimer);

  TimerStart(&Globals.popupSpawnTimer);
  TimerStart(&Globals.captchaSpawnTimer);
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  TimerUpdate(&Globals.captchaSpawnTimer);
  TimerUpdate(&Globals.popupSpawnTimer);

  if (!PopupStackIsEmpty(Globals.popupStack)) {
    PopupStackDraw(Globals.popupStack);
    if (Globals.currentCaptcha.type == CAPTCHA_TYPE_NONE) {
      switch (PopupStackReadInput(Globals.popupStack)) {
      case POPUP_PRESSED_FAILURE:
        PlaySound(Sounds.incorrect);
        LoseLife();
        break;
      case POPUP_PRESSED_SUCCESSFULLY:
        PopupStackPop(&Globals.popupStack);
        break;
      default: // Idling
        break;
      }
    }
  }

  if (Globals.currentCaptcha.type != CAPTCHA_TYPE_NONE) {
    CaptchaDraw(&Globals.currentCaptcha);
    switch (CaptchaCheck(&Globals.currentCaptcha)) {
    case CAPTCHA_PRESSED_FAILURE:
      Globals.currentCaptcha = CaptchaDefault();
      TimerStart(&Globals.captchaSpawnTimer);
      LoseLife();
      break;
    case CAPTCHA_PRESSED_SUCCESSFULLY:
      Globals.currentCaptcha = CaptchaDefault();
      ++Globals.completedCaptchas;
      if (Globals.completedCaptchas < MAX_CAPTCHAS_COMPLETED)
        TimerStart(&Globals.captchaSpawnTimer);
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

  PlayMusicStream(Sounds.backgroundMusic);
  SetMusicVolume(Sounds.backgroundMusic, 1.0f);

  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }

  UnloadMusicStream(Sounds.backgroundMusic);
  CloseAudioDevice();

  return 0;
}
