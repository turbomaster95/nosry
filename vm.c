#include "vm.h"

#define INST_NOP() \
    ((Inst){ .opcode = OP_NOP })
    
#define INST_MOV(r_dst, immv) \
    ((Inst){ .opcode = OP_MOV, .dest = (r_dst), .imm = (immv) })

#define INST_ADD(r_dst, r_src) \
    ((Inst){ .opcode = OP_ADD, .dest = (r_dst), .src = (r_src) })
    
#define INST_ADDI(r_dst, immv) \
    ((Inst){ .opcode = OP_ADDI, .dest = (r_dst), .imm = (immv) })

#define INST_SUB(r_dst, r_src) \
    ((Inst){ .opcode = OP_SUB, .dest = (r_dst), .src = (r_src) })

#define INST_SUBI(r_dst, immv) \
    ((Inst){ .opcode = OP_SUBI, .dest = (r_dst), .imm = (immv) })

#define INST_AND(r_dst, r_src) \
    ((Inst){ .opcode = OP_AND, .dest = (r_dst), .src = (r_src) })

#define INST_SHR(r_dst, offset) \
    ((Inst){ .opcode = OP_SHR, .dest = (r_dst), .offset = (offset) })

#define INST_SHL(r_dst, offset) \
    ((Inst){ .opcode = OP_SHL, .dest = (r_dst), .offset = (offset) })

#define INST_PUSH(r_src) \
    ((Inst){ .opcode = OP_PUSH, .src = (r_src) })

#define INST_POP(r_dst) \
    ((Inst){ .opcode = OP_POP, .dest = (r_dst) })

#define INST_CALL(imm_addr) \
    ((Inst){ .opcode = OP_CALL, .imm = (imm_addr) })

#define INST_CALLR(off_addr) \
    ((Inst){ .opcode = OP_CALLR, .offset = (off_addr) })

#define INST_RET() \
    ((Inst){ .opcode = OP_RET })
     
#define INST_HALT() \
    ((Inst){ .opcode = OP_HALT })

int main() {
  VM vm;
  Memory mem;
  
  VM_reset(&vm, &mem);

  enum {
    L_MAIN = 0,
    L_SUBROUTINE = L_MAIN + 6,
    L_END = L_SUBROUTINE + 2 
  };

  struct Inst program[] = {
    [L_MAIN]       = INST_MOV(0, 10),
                     INST_MOV(1, 10),
                     INST_ADD(0, 1),
                     INST_CALL(L_SUBROUTINE),
                     INST_MOV(2, 19),
                     INST_HALT(),

    [L_SUBROUTINE] = INST_MOV(3, 42),
                     INST_RET()
  };

  
  VM_run(&vm, program, COUNTOF(program), &mem);
}
