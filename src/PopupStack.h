#ifndef POPUPSTACK_H_
#define POPUPSTACK_H_

#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

#include "TextureHashMap.h"

#define STACK_DEFAULT_CAPACITY 5

#define POPUP_WAITING 2
#define POPUP_PRESSED_SUCCESSFULLY 1
#define POPUP_PRESSED_FAILURE 0

typedef struct {
  int number;
  Texture2D imageTexture;
} Popup;

typedef struct {
  Popup *data;
  int headIdx;
  int capacity;
} PopupStack;

Popup PopupDefault() { return (Popup){.number = -1, .imageTexture = {}}; }

PopupStack PopupStackCreate() {
  PopupStack stack;
  stack.data = malloc(sizeof(Popup) * STACK_DEFAULT_CAPACITY);
  stack.headIdx = -1;
  stack.capacity = STACK_DEFAULT_CAPACITY;
  return stack;
}

void PopupStackPush(PopupStack *stack, Texture2D texture, int number) {
  if (stack->headIdx + 1 >= stack->capacity) {
    stack->capacity *= 2;
    stack->data = realloc(stack->data, sizeof(Popup) * stack->capacity);
  }
  stack->data[++stack->headIdx].number = number;
  stack->data[stack->headIdx].imageTexture = texture;
}

int PopupStackIsEmpty(PopupStack stack) { return stack.headIdx < 0; }

void PopupStackPop(PopupStack *stack) {
  assert(stack->headIdx >= 0 && "Invalid head index. Cannot pop stack.");

  Popup *popup = &stack->data[stack->headIdx--];
  popup->number = -1;
  popup->imageTexture = (Texture2D){};
}

Popup PopupStackPeek(PopupStack stack) {
  assert(stack.headIdx >= 0 && "Invalid head index. Cannot pop stack.");
  return stack.data[stack.headIdx];
}

void PopupStackDelete(PopupStack *popupStack) {
  while (!PopupStackIsEmpty(*popupStack)) {
    PopupStackPop(popupStack);
  }

  if (popupStack->data != NULL) {
    free(popupStack->data);
  }

  popupStack->data = NULL;

  popupStack->headIdx = -1;
  popupStack->capacity = 0;
}

int PopupStackReadInput(PopupStack popupStack) {
  Popup currentPopup = PopupStackPeek(popupStack);

  if (IsKeyPressed(KEY_ZERO + currentPopup.number)) {
    return POPUP_PRESSED_SUCCESSFULLY;
  }

  if (GetKeyPressed() == 0) {
    return POPUP_WAITING;
  }

  return POPUP_PRESSED_FAILURE;
}

void PopupStackDraw(PopupStack popupStack) {
  Vector2 startPos;
  startPos.x = GetScreenWidth() * 0.5f;
  startPos.y = GetScreenHeight() * 0.5f;

  Vector2 currentPosOffset = startPos;

  Vector2 scale = {300.0f, 500.0f};
  Vector2 imageOffset;
  imageOffset.x = scale.x * 0.1f;
  imageOffset.y = scale.y * 0.1f;

  for (int i = 0; i < popupStack.headIdx; ++i) {
    Popup popup = popupStack.data[i];

    DrawRectangleLines(currentPosOffset.x - scale.x * 0.5f,
                       currentPosOffset.y - scale.y * 0.5f, scale.x, scale.y,
                       GREEN);

    float imgWidth = popup.imageTexture.width;
    float imgHeight = popup.imageTexture.height;

    DrawTexturePro(
        popup.imageTexture,
        (Rectangle){
            .x = 0.0f, .y = 0.0f, .width = imgWidth, .height = imgHeight},
        (Rectangle){.x = currentPosOffset.x,
                    .y = currentPosOffset.y,
                    .width = scale.x,
                    .height = scale.y},
        (Vector2){scale.x * 0.5f, scale.y * 0.5f}, 0.0f, GRAY);

    const char *text = TextFormat("%d", popup.number);
    DrawText(text, currentPosOffset.x, currentPosOffset.y, 24, RED);

    currentPosOffset.x += imageOffset.x;
    currentPosOffset.y -= imageOffset.y;
  }

  Popup currentPopup = PopupStackPeek(popupStack);

  float imgWidth = currentPopup.imageTexture.width;
  float imgHeight = currentPopup.imageTexture.height;

  DrawRectangleLines(currentPosOffset.x - scale.x * 0.5f,
                     currentPosOffset.y - scale.y * 0.5f, scale.x, scale.y,
                     GREEN);

  DrawTexturePro(
      currentPopup.imageTexture,
      (Rectangle){.x = 0.0f, .y = 0.0f, .width = imgWidth, .height = imgHeight},
      (Rectangle){.x = currentPosOffset.x,
                  .y = currentPosOffset.y,
                  .width = scale.x,
                  .height = scale.y},
      (Vector2){scale.x * 0.5f, scale.y * 0.5f}, 0.0f, WHITE);

  const char *text = TextFormat("%d", currentPopup.number);
  DrawText(text, currentPosOffset.x, currentPosOffset.y, 24, RED);
}

void LoadAllPopupTextures(TextureHashMap *hashMap) {
  *hashMap = TextureHashMapCreate();
  hashMap->popupPaths = LoadDirectoryFiles("resources/textures/popups");

  assert(hashMap->popupPaths.count > 0 && "Need at least one popup texture!");

  for (int i = 0; i < hashMap->popupPaths.count; ++i) {
    const char *filePath = hashMap->popupPaths.paths[i];
    TextureHashMapGet(hashMap, filePath);
  }
}

void SpawnRandomPopup(TextureHashMap *hashMap, PopupStack *popupStack) {
  int index = GetRandomValue(0, hashMap->popupPaths.count - 1);
  Texture2D texture =
      TextureHashMapGet(hashMap, hashMap->popupPaths.paths[index]);
  PopupStackPush(popupStack, texture, GetRandomValue(0, 9));
}

#endif
