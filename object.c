#include "object.h"
#include "common.h"
#include "memory.h"
#include "value.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

#define ALLOCATE_OBJ(type, objectType)                                         \
  (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type) {
  Obj *object = (Obj *)reallocate(NULL, 0, size);
  object->type = type;
  object->next = vm.objects;
  vm.objects = object;
  return object;
}

ObjType objType(Value value) { return asObj(value)->type; }

bool isString(Value value) { return isObjType(value, OBJ_STRING); }

ObjString *asString(Value value) { return (ObjString *)asObj(value); }

char *asCString(Value value) { return ((ObjString *)asObj(value))->chars; }

static ObjString *allocateString(char *chars, int length) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  return string;
}

ObjString *takeString(char *chars, int length) {
  return allocateString(chars, length);
}

ObjString *copyString(const char *chars, int length) {
  char *heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length);
}

void printObject(Value value) {
  switch (objType(value)) {
  case OBJ_STRING:
    fprintf(stderr, "%s", asCString(value));
    break;
  }
}
