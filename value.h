#pragma once

#include "common.h"

#include <stdint.h>

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
  } as;
} Value;

typedef struct {
  int capacity;
  uint8_t count;
  Value *values;
} ValueArray;

bool valuesEqual(Value a, Value b);

Value boolVal(bool value);
Value nilVal();
Value numberVal(double value);

bool asBool(Value value);
double asNumber(Value value);

bool isBool(Value value);
bool isNil(Value value);
bool isNumber(Value value);

void initValueArray(ValueArray *array);

void writeValueArray(ValueArray *array, Value value);

void freeValueArray(ValueArray *array);

void printValue(Value value);
