#ifndef POPUPSTACK_H_
#define POPUPSTACK_H_

#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

#define STACK_DEFAULT_CAPACITY 5

typedef struct {
  const char *imagePath;
  int number;
  Texture2D imageTexture;
} Popup;

typedef struct {
  Popup *data;
  int headIdx;
  int capacity;
} PopupStack;

Popup PopupDefault() {
  return (Popup){.imagePath = NULL, .number = -1, .imageTexture = {}};
}

PopupStack PopupStackCreate() {
  PopupStack stack;
  stack.data = malloc(sizeof(Popup) * STACK_DEFAULT_CAPACITY);
  stack.headIdx = -1;
  stack.capacity = STACK_DEFAULT_CAPACITY;
  return stack;
}

void PopupStackPush(PopupStack *stack, const char *imagePath, int number) {
  if (stack->headIdx + 1 >= stack->capacity) {
    stack->capacity *= 2;
    stack->data = realloc(stack->data, sizeof(Popup) * stack->capacity);
  }
  stack->data[++stack->headIdx].imagePath = imagePath;
  stack->data[stack->headIdx].number = number;
  stack->data[stack->headIdx].imageTexture = LoadTexture(imagePath);
}

int PopupStackIsEmpty(PopupStack stack) { return stack.headIdx < 0; }

Popup PopupStackPop(PopupStack *stack) {
  assert(stack->headIdx + 1 > 0 && "Cannot pop empty stack!");
  assert(stack->headIdx >= 0 && "Invalid head index. Cannot pop stack.");

  Popup popup = stack->data[stack->headIdx--];
  popup.imagePath = NULL;
  popup.number = -1;
  UnloadTexture(popup.imageTexture);
  popup.imageTexture = (Texture2D){};

  return popup;
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

#endif
