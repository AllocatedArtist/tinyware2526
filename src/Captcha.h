#ifndef CAPTCHA_H_
#define CAPTCHA_H_

#include <raylib.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "TextureHashMap.h"

#define MAX_ANSWER_DIGITS 3

#define CAPTCHA_WAITING 2
#define CAPTCHA_PRESSED_SUCCESSFULLY 1
#define CAPTCHA_PRESSED_FAILURE 0

typedef enum {
  CAPTCHA_TYPE_NONE,
  CAPTCHA_TYPE_CAR,
  CAPTCHA_TYPE_MATH
} CaptchaType;

typedef enum { MATH_OP_ADD, MATH_OP_SUB } MathOp;

typedef struct {
  MathOp operationType;
  int firstNum;
  int secondNum;
} MathProblem;

typedef struct {
  CaptchaType type;

  int answer[MAX_ANSWER_DIGITS];

  int currentDigitIdx;

  // Number of digits to complete answer
  int answerLength;

  union {
    Texture2D imageTexture;
    MathProblem mathProblem;
  } data;
} Captcha;

Captcha CaptchaDefault() {
  return (Captcha){.type = CAPTCHA_TYPE_NONE,
                   .answer = {0},
                   .currentDigitIdx = 0,
                   .answerLength = 0,
                   .data = {}};
}

int CaptchaCheck(Captcha *currentCaptcha) {
  assert(currentCaptcha != NULL && "Captcha shouldn't be NULL");
  assert(currentCaptcha->answerLength > 0 && "Invalid answer length");

  int currentKey = GetKeyPressed();
  if (!(currentKey >= KEY_ZERO && currentKey <= KEY_NINE) || currentKey == 0) {
    return CAPTCHA_WAITING;
  }

  int keyVal =
      KEY_ZERO + currentCaptcha->answer[currentCaptcha->currentDigitIdx];

  if (currentKey == keyVal) {
    ++currentCaptcha->currentDigitIdx;
    if (currentCaptcha->currentDigitIdx >= currentCaptcha->answerLength) {
      return CAPTCHA_PRESSED_SUCCESSFULLY;
    }
    return CAPTCHA_WAITING;
  }

  return CAPTCHA_PRESSED_FAILURE;
}

void LoadAllCaptchaTextures(TextureHashMap *hashMap) {
  hashMap->captchaPaths = LoadDirectoryFiles("resources/textures/captchas");

  assert(hashMap->captchaPaths.count > 0 &&
         "Need at least one captcha texture!");

  for (int i = 0; i < hashMap->captchaPaths.count; ++i) {
    const char *filePath = hashMap->captchaPaths.paths[i];
    TextureHashMapGet(hashMap, filePath);
  }
}

void CaptchaCreateCar(TextureHashMap *hashMap, Captcha *currentCaptcha) {
  *currentCaptcha = CaptchaDefault();
  int randomCaptchaNum = GetRandomValue(1, 3);
  const char *captchaPath = TextFormat(
      "resources/textures/captchas/carcaptcha%d.png", randomCaptchaNum);
  currentCaptcha->data.imageTexture = TextureHashMapGet(hashMap, captchaPath);
  if (randomCaptchaNum == 1) {
    currentCaptcha->answer[0] = 1;
    currentCaptcha->answer[1] = 5;
  } else if (randomCaptchaNum == 2) {
    currentCaptcha->answer[0] = 2;
    currentCaptcha->answer[1] = 0;
  } else {
    currentCaptcha->answer[0] = 1;
    currentCaptcha->answer[1] = 0;
  }

  currentCaptcha->answerLength = 2;
  currentCaptcha->type = CAPTCHA_TYPE_CAR;
}

void CaptchaCreateMath(Captcha *currentCaptcha) {
  *currentCaptcha = CaptchaDefault();

  int firstVal = GetRandomValue(1, 400);
  int secondVal = GetRandomValue(0, firstVal);
  int answer = 0;

  currentCaptcha->data.mathProblem.firstNum = firstVal;
  currentCaptcha->data.mathProblem.secondNum = secondVal;

  if (GetRandomValue(0, 1)) {
    currentCaptcha->data.mathProblem.operationType = MATH_OP_ADD;
    answer = firstVal + secondVal;
  } else {
    currentCaptcha->data.mathProblem.operationType = MATH_OP_SUB;
    answer = firstVal - secondVal;
  }

  int counter = answer;
  while (counter != 0) {
    counter /= 10;
    ++currentCaptcha->answerLength;
  }

  if (answer == 0) {
    currentCaptcha->answerLength = 1;
    currentCaptcha->answer[0] = 0;
  } else {
    counter = answer;
    int currentIdx = currentCaptcha->answerLength - 1;
    while (counter != 0) {
      assert(currentIdx >= 0 && "Negative index!");
      currentCaptcha->answer[currentIdx--] = counter % 10;
      counter /= 10;
    }
  }

  currentCaptcha->type = CAPTCHA_TYPE_MATH;
}

void CaptchaDrawMath(Captcha *currentCaptcha) {
  MathProblem mathProblem = currentCaptcha->data.mathProblem;

  Vector2 centerPos = {};
  centerPos.x = GetScreenWidth() / 2.f;
  centerPos.y = GetScreenHeight() / 2.f;

  Vector2 rectSize = {400, 300};

  Vector2 renderPos = centerPos;
  renderPos.x -= rectSize.x * 0.5f;
  renderPos.y -= rectSize.y * 0.5f;

  DrawRectangleV(renderPos, rectSize, DARKGREEN);

  Vector2 textPos = centerPos;

  int fontSize = 24;

  if (mathProblem.operationType == MATH_OP_ADD) {
    const char *text =
        TextFormat("%d + %d = ?", mathProblem.firstNum, mathProblem.secondNum);

    size_t textLengthHalf = MeasureText(text, fontSize) * 0.5f;
    textPos.x -= textLengthHalf;

    DrawText(text, textPos.x, textPos.y, fontSize, WHITE);
  } else {
    const char *text =
        TextFormat("%d - %d = ?", mathProblem.firstNum, mathProblem.secondNum);

    size_t textLengthHalf = MeasureText(text, fontSize) * 0.5f;
    textPos.x -= textLengthHalf;

    DrawText(text, textPos.x, textPos.y, fontSize, WHITE);
  }
}

void CaptchaDrawCar(Captcha *currentCaptcha) {
  Texture2D carTexture = currentCaptcha->data.imageTexture;
  float imgWidth = carTexture.width;
  float imgHeight = carTexture.height;

  Vector2 scale = {400.0f, 400.0f};

  Vector2 currentPosOffset = {
      GetScreenWidth() * 0.5f,
      GetScreenHeight() * 0.5f,
  };

  DrawTexturePro(
      carTexture,
      (Rectangle){.x = 0.0f, .y = 0.0f, .width = imgWidth, .height = imgHeight},
      (Rectangle){.x = currentPosOffset.x,
                  .y = currentPosOffset.y,
                  .width = scale.x,
                  .height = scale.y},
      (Vector2){scale.x * 0.5f, scale.y * 0.5f}, 0.0f, WHITE);
}

void CaptchaCreateRandom(TextureHashMap *hashMap, Captcha *currentCaptcha) {
  if (GetRandomValue(0, 1)) {
    CaptchaCreateMath(currentCaptcha);
  } else {
    CaptchaCreateCar(hashMap, currentCaptcha);
  }
}

void CaptchaDraw(Captcha *currentCaptcha) {
  assert(currentCaptcha != NULL && "Captcha is NULL");
  if (currentCaptcha->type == CAPTCHA_TYPE_NONE)
    return;
  if (currentCaptcha->type == CAPTCHA_TYPE_MATH) {
    CaptchaDrawMath(currentCaptcha);
    return;
  }
  CaptchaDrawCar(currentCaptcha);
}

#endif
