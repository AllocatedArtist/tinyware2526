#include <raylib.h>

#include <stdio.h>

#include "Captcha.h"
#include "PopupStack.h"
#include "Timer.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 2.0
#define CAPTCHA_SPAWNRATE 15.0
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
  int initialLoad;
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
  Globals.popupStack.headIdx = -1;

  if (Globals.initialLoad) {
    Globals.popupStack = PopupStackCreate();
    InitAudioDevice();

    Sounds.backgroundMusic = LoadMusicStream("resources/audio/background.mp3");
    Sounds.incorrect = LoadSound("resources/audio/Laugh.wav");
    Sounds.captchaSpawn = LoadSound("resources/audio/captchasound.mp3");
    Sounds.captchaDone = LoadSound("resources/audio/celebration.mp3");
    Sounds.vine = LoadSound("resources/audio/vineboom.pm3");

    Globals.texturesMap = TextureHashMapCreate();

    LoadAllPopupTextures(&Globals.texturesMap);
    LoadAllCaptchaTextures(&Globals.texturesMap);
  }

  Globals.completedCaptchas = 0;

  Globals.currentCaptcha = CaptchaDefault();

  Globals.popupSpawnTimer = TimerCreate(POPUP_SPAWNRATE, 1, PopupSpawnTimer);
  Globals.captchaSpawnTimer =
      TimerCreate(CAPTCHA_SPAWNRATE, 0, CaptchaSpawnTimer);

  TimerStart(&Globals.popupSpawnTimer);
  TimerStart(&Globals.captchaSpawnTimer);

  Globals.initialLoad = 0;
}

void GameOverScreen() {
  Vector2 currentPosOffset = {GetScreenWidth() * 0.5f,
                              GetScreenHeight() * 0.5f};

  const char *GAME_OVER = "Game Over";

  int fontSize = 48;
  size_t textLengthHalf = MeasureText(GAME_OVER, fontSize) * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  DrawText(GAME_OVER, currentPosOffset.x, currentPosOffset.y, fontSize, RED);

  const char *MSG = "YOUR PHONE EXPLODED FROM THE SHEER AMOUNT OF ADS";
  fontSize = 24;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureText(MSG, fontSize) * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 50;
  DrawText(MSG, currentPosOffset.x, currentPosOffset.y, fontSize, GRAY);

  const char *MSG2 = "CLICK SPACE TO PLAY AGAIN";
  fontSize = 24;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureText(MSG2, fontSize) * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 100;
  DrawText(MSG2, currentPosOffset.x, currentPosOffset.y, fontSize, DARKGRAY);

  if (IsKeyPressed(KEY_SPACE)) {
    InitGlobals();
  }
}

void UpdateDrawLoop() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  if (Globals.playerLives <= 0) {
    GameOverScreen();
    EndDrawing();
    return;
  }

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

  Globals.initialLoad = 1;
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
