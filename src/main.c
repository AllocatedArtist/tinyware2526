#include <raylib.h>

#include <math.h>
#include <stdio.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#include "Captcha.h"
#include "PopupStack.h"
#include "Timer.h"

#define PLAYER_LIVES 3
#define POPUP_SPAWNRATE 2.0
#define CAPTCHA_SPAWNRATE 15.0
#define MAX_CAPTCHAS_COMPLETED 5
#define MAX_ADS 10

#define FONT_SIZE_BASE 128
#define FONT_SIZE_TITLE 96
#define FONT_SIZE_MESSAGE 48
#define FONT_SIZE_SMALL 36

struct {
  Music backgroundMusic;
  Sound incorrect;
  Sound adPop;
  Sound captchaSpawn;
  Sound captchaDone;
  Sound vine;
  Sound tacobell;
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
  Font mainFont;

  Model cat;
  Camera3D camera;
  Color backgroundColor;
} Globals;

void PopupSpawnTimer() {
  SpawnRandomPopup(&Globals.texturesMap, &Globals.popupStack);
}

void CaptchaSpawnTimer() {
  CaptchaCreateRandom(&Globals.texturesMap, &Globals.currentCaptcha);
  PlaySound(Sounds.vine);
}

void PlayIncorrectSfx() {
  if (GetRandomValue(0, 1)) {
    PlaySound(Sounds.incorrect);
  } else {
    PlaySound(Sounds.tacobell);
  }
}

void LoseGame() {
  Globals.playerLives = 0;
  TimerEnd(&Globals.popupSpawnTimer);
  TimerEnd(&Globals.captchaSpawnTimer);
}

void LoseLife() {
  --Globals.playerLives;
  if (Globals.playerLives <= 0) {
    LoseGame();
  }
}

void DrawLives() {
  for (int i = 0; i < PLAYER_LIVES - Globals.playerLives; i++) {
    DrawTexture(LP.usedLife, GetScreenWidth() - (64 * (PLAYER_LIVES - i)), 0,
                WHITE);
  }
  for (int i = Globals.playerLives; i > 0; i--) {
    DrawTexture(LP.life, GetScreenWidth() - (64 * i), 0, WHITE);
  }
}

void DrawProgress() {
  for (int i = 0; i < Globals.completedCaptchas; i++) {
    DrawTexture(LP.doneProgress, 64 * i, 0, WHITE);
  }
  for (int i = Globals.completedCaptchas; i < MAX_CAPTCHAS_COMPLETED; i++) {
    DrawTexture(LP.progress, 64 * i, 0, WHITE);
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
    Sounds.tacobell = LoadSound("resources/audio/tacobell.mp3");

    Globals.mainFont =
        LoadFontEx("resources/fonts/Changa_One/ChangaOne-Regular.ttf",
                   FONT_SIZE_BASE, 0, 0);

    LP.life = LoadTexture("resources/textures/lives.png");
    LP.usedLife = LoadTexture("resources/textures/usedLife.png");
    LP.progress = LoadTexture("resources/textures/progress.png");
    LP.doneProgress = LoadTexture("resources/textures/progressDone.png");

    PlayMusicStream(Sounds.backgroundMusic);
    SetMusicVolume(Sounds.backgroundMusic, 1.0f);

    Globals.texturesMap = TextureHashMapCreate();

    LoadAllPopupTextures(&Globals.texturesMap);
    LoadAllCaptchaTextures(&Globals.texturesMap);

    Globals.cat = LoadModel("resources/models/cat.glb");
  }

  Globals.camera = (Camera3D){0};
  Globals.camera.position.x = 10;
  Globals.camera.position.y = 10;
  Globals.camera.position.z = 10;
  Globals.camera.target.x = 0;
  Globals.camera.target.y = 0;
  Globals.camera.target.z = 0;
  Globals.camera.up.x = 0;
  Globals.camera.up.y = 1;
  Globals.camera.up.z = 0;
  Globals.camera.fovy = 45.0f;
  Globals.camera.projection = CAMERA_PERSPECTIVE;

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

  int fontSize = FONT_SIZE_TITLE;
  size_t textLengthHalf =
      MeasureTextEx(Globals.mainFont, GAME_OVER, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  DrawTextEx(Globals.mainFont, GAME_OVER,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             RED);

  const char *MSG = "YOUR PHONE EXPLODED FROM THE SHEER AMOUNT OF ADS";
  fontSize = FONT_SIZE_MESSAGE;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureTextEx(Globals.mainFont, MSG, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 50 + fontSize;
  DrawTextEx(Globals.mainFont, MSG,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             GRAY);

  const char *MSG2 = "CLICK SPACE TO PLAY AGAIN";
  fontSize = FONT_SIZE_MESSAGE;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureTextEx(Globals.mainFont, MSG2, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 100 + fontSize;
  DrawTextEx(Globals.mainFont, MSG2,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             DARKGRAY);

  if (IsKeyPressed(KEY_SPACE)) {
    InitGlobals();
  }
}

void WinScreen() {
  Vector2 currentPosOffset = {GetScreenWidth() * 0.5f,
                              GetScreenHeight() * 0.5f};

  const char *GAME_OVER = "ADBLOCKER RESTORED! YOU WIN!";

  int fontSize = FONT_SIZE_TITLE;
  size_t textLengthHalf =
      MeasureTextEx(Globals.mainFont, GAME_OVER, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  DrawTextEx(Globals.mainFont, GAME_OVER,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             RED);

  const char *MSG =
      "PLEASE DON'T TRUST PEOPLE ON REDDIT WHEN THEY SAY 100\% GUARANTEED";
  fontSize = FONT_SIZE_MESSAGE;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureTextEx(Globals.mainFont, MSG, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 50 + fontSize;
  DrawTextEx(Globals.mainFont, MSG,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             GRAY);

  const char *MSG2 = "CLICK SPACE TO PLAY AGAIN";
  fontSize = FONT_SIZE_MESSAGE;

  currentPosOffset =
      (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};

  textLengthHalf = MeasureTextEx(Globals.mainFont, MSG2, fontSize, 0).x * 0.5f;
  currentPosOffset.x -= textLengthHalf;
  currentPosOffset.y += 100 + fontSize;
  DrawTextEx(Globals.mainFont, MSG2,
             (Vector2){currentPosOffset.x, currentPosOffset.y}, fontSize, 0,
             DARKGRAY);

  if (IsKeyPressed(KEY_SPACE)) {
    InitGlobals();
  }
}

void UpdateDrawLoop() {
  BeginDrawing();
  Globals.backgroundColor = ColorLerp(YELLOW, DARKGREEN, sinf(GetTime()));
  ClearBackground(ColorBrightness(Globals.backgroundColor, 0.5));

  UpdateCamera(&Globals.camera, CAMERA_ORBITAL);
  BeginMode3D(Globals.camera);
  DrawModel(Globals.cat, (Vector3){0, 0, 0}, 10.0f, WHITE);
  DrawGrid(10, 1.0f);
  EndMode3D();

  if (Globals.playerLives <= 0) {
    GameOverScreen();
    EndDrawing();
    return;
  } else if (Globals.completedCaptchas >= MAX_CAPTCHAS_COMPLETED) {
    WinScreen();
    EndDrawing();
    return;
  } else {
    if (Globals.popupStack.headIdx + 1 >= MAX_ADS) {
      PlayIncorrectSfx();
      LoseGame();
    }
  }

  TimerUpdate(&Globals.popupSpawnTimer);
  TimerUpdate(&Globals.captchaSpawnTimer);

  UpdateMusicStream(Sounds.backgroundMusic);
  if (!PopupStackIsEmpty(Globals.popupStack)) {
    PopupStackDraw(Globals.popupStack, Globals.mainFont);
    if (Globals.currentCaptcha.type == CAPTCHA_TYPE_NONE) {
      switch (PopupStackReadInput(Globals.popupStack)) {
      case POPUP_PRESSED_FAILURE:
        PlayIncorrectSfx();
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
    CaptchaDraw(&Globals.currentCaptcha, Globals.mainFont);
    switch (CaptchaCheck(&Globals.currentCaptcha)) {
    case CAPTCHA_PRESSED_FAILURE:
      PlayIncorrectSfx();
      TimerStart(&Globals.captchaSpawnTimer);
      Globals.currentCaptcha = CaptchaDefault();
      LoseLife();
      break;
    case CAPTCHA_PRESSED_SUCCESSFULLY:
      PlaySound(Sounds.captchaDone);
      Globals.currentCaptcha = CaptchaDefault();
      ++Globals.completedCaptchas;
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
  InitWindow(1920, 1080, "To Captcha Virus");
  SetTargetFPS(60);

  Globals.initialLoad = 1;
  InitGlobals();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(UpdateDrawLoop, 0, 1);
#else
  while (!WindowShouldClose()) {
    UpdateDrawLoop();
  }
#endif

  return 0;
}
