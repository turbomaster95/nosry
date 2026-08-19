#include "vm.h"

int main() {
  VM vm;
  Memory mem;
  
  VM_reset(&vm, &mem);
  
  struct Inst program[] = {
    { .opcode = OP_MOV, .dest = 0, .imm = 10 },
    { .opcode = OP_MOV, .dest = 1, .imm = 10 },
    { .opcode = OP_ADD, .dest = 0, .src = 1 },
    { .opcode = OP_HALT }
  };
  
  VM_run(&vm, program, COUNTOF(program), &mem);
}
