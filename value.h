#pragma once

#include "common.h"
#include <stdint.h>

typedef double Value;

typedef struct {
  int capacity;
  uint8_t count;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *array);

void writeValueArray(ValueArray *array, Value value);

void freeValueArray(ValueArray *array);

void printValue(Value value);
