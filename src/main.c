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
    Sound adPop;
    Sound captchaSpawn;
    Sound captchaDone;
    Sound vine;
} Sounds;

struct {
    Texture2D life;
    Texture2D usedLife;
    Texture2D progress;
    Texture2D doneProgress;
} LP;

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

void DrawLives() {
    for (int i = 0; i < 3 - Globals.playerLives; i++) {
        DrawTexture(LP.usedLife, GetScreenWidth() - (64 * (3 - i)), 0, WHITE);
    }
    for (int i = Globals.playerLives; i > 0; i--) {
        DrawTexture(LP.life, GetScreenWidth() - (64 * i), 0, WHITE);
    }
}

void DrawProgress() {
    for (int i = 0; i < 3 - Globals.completedCaptchas; i++) {
        DrawTexture(LP.progress, 64 * i, 0, WHITE);
    }
    for (int i = 0; i < Globals.completedCaptchas; i++) {
        DrawTexture(LP.doneProgress, 64 * i, 0, WHITE);
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
    Sounds.adPop = LoadSound("resources/audio/adpop.mp3");
    Sounds.captchaSpawn = LoadSound("resources/audio/captchasound.mp3");
    Sounds.captchaDone = LoadSound("resources/audio/celebration.mp3");
    Sounds.vine = LoadSound("resources/audio/vineboom.mp3");

    LP.life = LoadTexture("resources/textures/lives.png");
    LP.usedLife = LoadTexture("resources/textures/usedLife.png");
    LP.progress = LoadTexture("resources/textures/progress.png");
    LP.doneProgress = LoadTexture("resources/textures/progressDone.png");

    PlayMusicStream(Sounds.backgroundMusic);
    SetMusicVolume(Sounds.backgroundMusic, 1.0f);

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

  TimerUpdate(&Globals.popupSpawnTimer);
  TimerUpdate(&Globals.captchaSpawnTimer);

  UpdateMusicStream(Sounds.backgroundMusic);
  if (!PopupStackIsEmpty(Globals.popupStack)) {
    PopupStackDraw(Globals.popupStack);
    if (Globals.currentCaptcha.type == CAPTCHA_TYPE_NONE) {
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
    }
  }

  if (Globals.currentCaptcha.type != CAPTCHA_TYPE_NONE) {
    CaptchaDraw(&Globals.currentCaptcha);
    switch (CaptchaCheck(&Globals.currentCaptcha)) {
    case CAPTCHA_PRESSED_FAILURE:
      PlaySound(Sounds.incorrect);
      TimerStart(&Globals.captchaSpawnTimer);
      Globals.currentCaptcha = CaptchaDefault();
      LoseLife();
      break;
    case CAPTCHA_PRESSED_SUCCESSFULLY:
      PlaySound(Sounds.captchaDone);
      Globals.currentCaptcha = CaptchaDefault();
      if (Globals.playerLives > 0) {
        TimerStart(&Globals.captchaSpawnTimer);
      }
      break;
    default: // Idling
      break;
    }
  }

  DrawLives();
  DrawProgress();

  EndDrawing();
}

int main() {
  InitWindow(1000, 1000, "Window");

  Globals.initialLoad = 1;
  InitGlobals();

  printf("Started Game\n");

  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }

  return 0;
}
