#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);
  size_t instruction = vm.ip - vm.chunk->code - 1;
  int line = vm.chunk->lines[instruction];
  fprintf(stderr, "[line %d] in script.\n", line);
  resetStack();
}

void initVM() {
  resetStack();
  vm.objects = NULL;
}

void freeVM() {
  freeObjects();
}

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop += 1;
}

Value pop() {
  vm.stackTop -= 1;
  return *vm.stackTop;
}

static Value peek(int distance) { return vm.stackTop[-1 - distance]; }

static bool isFalsey(Value value) {
  return isNil(value) || (isBool(value) && !asBool(value));
}

static void concatenate() {
#define IGNORE_RETURN(expr)                                                    \
  if (expr) {                                                                  \
  }
  ObjString *b = asString(pop());
  ObjString *a = asString(pop());
  int length = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);
  IGNORE_RETURN(memcpy(chars, a->chars, a->length));
  IGNORE_RETURN(memcpy(chars + a->length, b->chars, b->length));
  ObjString *result = takeString(chars, length);
  push(objVal((Obj *)result));
#undef IGNORE_RETURN
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!isNumber(peek(0)) || !isNumber(peek(1))) {                            \
      runtimeError("Operands must be numbers.");                               \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = asNumber(pop());                                                \
    double a = asNumber(pop());                                                \
    push(valueType(a op b));                                                   \
  } while (false)

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    fprintf(stderr, "\t\t");
    for (Value *slot = vm.stack; slot < vm.stackTop; slot += 1) {
      fprintf(stderr, "[ ");
      printValue(*slot);
      fprintf(stderr, " ]");
    }
    fprintf(stderr, "\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTNAT: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_NIL:
      push(nilVal());
      break;
    case OP_TRUE:
      push(boolVal(true));
      break;
    case OP_FALSE:
      push(boolVal(false));
      break;
    case OP_EQUAL: {
      Value b = pop();
      Value a = pop();
      push(boolVal(valuesEqual(a, b)));
      break;
    }
    case OP_GREATER:
      BINARY_OP(boolVal, >);
      break;
    case OP_LESS:
      BINARY_OP(boolVal, <);
      break;
    case OP_ADD: {
      if (isString(peek(0)) && isString(peek(1))) {
        concatenate();
      } else if (isNumber(peek(0)) && isNumber(peek(1))) {
        double b = asNumber(pop());
        double a = asNumber(pop());
        push(numberVal(a + b));
      } else {
        runtimeError("Operands must be two numbers or two strings.");
      }
      break;
    }
    case OP_SUBTRACT:
      BINARY_OP(numberVal, -);
      break;
    case OP_MULTIPLY:
      BINARY_OP(numberVal, *);
      break;
    case OP_DIVIDE:
      BINARY_OP(numberVal, /);
      break;
    case OP_NOT:
      push(boolVal(isFalsey(pop())));
      break;
    case OP_NEGATE:
      if (!isNumber(peek(0))) {
        runtimeError("Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(numberVal(-asNumber(pop())));
      break;
    case OP_RETURN: {
      printValue(pop());
      fprintf(stderr, "\n");
      return INTERPRET_OK;
    }
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(const char *source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }
  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;
  InterpretResult result = run();
  freeChunk(&chunk);
  return result;
};
