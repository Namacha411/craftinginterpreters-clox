#include <stdint.h>
#include <stdio.h>
#include "debug.h"
#include "chunk.h"
#include "value.h"

static int simpleInstruction(const char* name, int offset) {
  fprintf(stderr, "%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char* name, Chunk* chunk, int offset) {
  uint8_t constant = chunk->code[offset + 1];
  fprintf(stderr, "%-16s %4d '",  name, constant);
  printValue(chunk->constants.values[constant]);
  fprintf(stderr, "'\n");
  return offset + 2;
}

void disassembleChunk(Chunk* chunk, const char* name) {
  fprintf(stderr, "== %s ==\n", name);
  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

int disassembleInstruction(Chunk* chunk, int offset) {
  fprintf(stderr, "%04d ", offset);
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    fprintf(stderr, "   | ");
  } else {
    fprintf(stderr, "%4d ", chunk->lines[offset]);
  }
  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN:
      return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTNAT:
      return constantInstruction("OP_CONSTNAT", chunk, offset);
    case OP_ADD:
      return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
      return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
      return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
      return simpleInstruction("OP_DIVIDE", offset);
    case OP_NEGATE:
      return simpleInstruction("OP_NEGATE", offset);
    default:
      fprintf(stderr, "Unknown opcode %d\n", instruction);
      return offset + 1;
  }
}
