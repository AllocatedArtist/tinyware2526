#ifndef CAPTCHA_H_
#define CAPTCHA_H_

#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

#include "TextureHashMap.h"

#define MAX_ANSWER_DIGITS 3

#define CAPTCHA_WAITING 2
#define CAPTCHA_PRESSED_SUCCESSFULLY 1
#define CAPTCHA_PRESSED_FAILURE 0

typedef struct {
  const char *imagePath;
  int type;

  int answer[MAX_ANSWER_DIGITS];

  int currentDigitIdx;

  // Number of digits to complete answer
  int answerLength;

  Texture2D imageTexture;
} Captcha;

Captcha CaptchaDefault() {
  return (Captcha){.imagePath = NULL,
                   .type = -1,
                   .answer = {0},
                   .currentDigitIdx = 0,
                   .answerLength = 0,
                   .imageTexture = {}};
}

int CaptchaCheck(Captcha *currentCaptcha) {
  assert(currentCaptcha != NULL && "Captcha shouldn't be NULL");

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
  *hashMap = TextureHashMapCreate();

  hashMap->captchaPaths = LoadDirectoryFiles("resources/textures/captcha");

  assert(hashMap->captchaPaths.count > 0 &&
         "Need at least one captcha texture!");

  for (int i = 0; i < hashMap->captchaPaths.count; ++i) {
    const char *filePath = hashMap->captchaPaths.paths[i];
    TextureHashMapGet(hashMap, filePath);
  }
}

void CaptchaSpawn(Captcha *curr) {}

void CaptchaDespawn(Captcha *curr) {}

#endif
