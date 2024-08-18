#pragma once

#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
  struct Obj* next;
};

struct ObjString {
  Obj obj;
  int length;
  char *chars;
};

ObjType objType(Value value);

bool isString(Value value);

ObjString *asString(Value value);

char *asCString(Value value);

ObjString *takeString(char *chars, int length);

ObjString *copyString(const char *chars, int length);

void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
  return isObj(value) && asObj(value)->type == type;
}
