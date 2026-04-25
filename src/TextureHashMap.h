#ifndef TEXTURE_HASHMAP_H_
#define TEXTURE_HASHMAP_H_

#include <raylib.h>

#include <assert.h>
#include <stdlib.h>

#define TEXTURE_HASHMAP_DEFAULT_CAPACITY 20

typedef struct TextureHashNode {
  const char *filePath;
  Texture2D texture;
  struct TextureHashNode *next;
} TextureHashNode;

typedef struct {
  TextureHashNode *data;
  FilePathList popupPaths;
  FilePathList captchaPaths;
} TextureHashMap;

TextureHashMap TextureHashMapCreate() {
  TextureHashMap hashMap;

  hashMap.data =
      malloc(sizeof(TextureHashNode) * TEXTURE_HASHMAP_DEFAULT_CAPACITY);
  for (int i = 0; i < TEXTURE_HASHMAP_DEFAULT_CAPACITY; ++i) {
    hashMap.data[i].filePath = NULL;
    hashMap.data[i].texture = (Texture2D){};
    hashMap.data[i].next = NULL;
  }

  return hashMap;
}

unsigned long Djb2Hash(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  return hash;
}

// Returns loaded texture corresponding to file path
// and inserts + loads new texture if it doesn't already exist.
Texture2D TextureHashMapGet(TextureHashMap *hashMap, const char *filePath) {
  assert(filePath != NULL && "Invalid file path comparison");
  size_t index = Djb2Hash(filePath) % TEXTURE_HASHMAP_DEFAULT_CAPACITY;
  if (hashMap->data[index].filePath == NULL) {
    hashMap->data[index].filePath = filePath;
    hashMap->data[index].texture = LoadTexture(filePath);
    return hashMap->data[index].texture;
  }

  TextureHashNode *current = &hashMap->data[index];
  while (current != NULL) {
    assert(current->filePath != NULL && "Invalid file path comparison");
    if (TextIsEqual(current->filePath, filePath)) {
      return current->texture;
    }
    if (current->next == NULL)
      break;
    current = current->next;
  }

  current->next = malloc(sizeof(TextureHashNode));
  current->next->filePath = filePath;
  current->next->texture = LoadTexture(filePath);
  current->next->next = NULL;

  return current->next->texture;
}

#endif
