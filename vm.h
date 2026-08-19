#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// https://stackoverflow.com/a/57537491
#include <assert.h>

#define is_same_type(a, b)    __builtin_types_compatible_p(a, b)
#define is_same_typeof(a, b)  is_same_type(typeof(a), typeof(b))
#define decay(a)              (&*(a))
#define is_array(arr)         (!is_same_typeof(arr, decay(arr)))
#define Static_assert_array(arr) static_assert(is_array(arr), "COUNTOF requires a true array, not a pointer")

#define COUNTOF(arr)                                                 \
({                                                                      \
        Static_assert_array(arr);                                       \
        sizeof(arr) / sizeof((arr)[0]);                                 \
})

#define MAX_REGS 5
#define MAX_STACK_SIZE 128

// Opcodes
enum VMOpcodes {
  OP_NOP = 0x00,
  OP_MOV,
  OP_ADD,
  OP_ADDI,
  OP_SUB,
  OP_SUBI,
  OP_AND,
  OP_SHR,
  OP_SHL,
  OP_PUSH,
  OP_POP,
  OP_CALL,
  OP_CALLR, // relative
  OP_RET,
  OP_HALT = 0xFF
};

// Unsigned Integers
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed Integers
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct __attribute__((packed)) Inst {
  u8 opcode;
  struct {
        u8 dest : 4;
        u8 src  : 4;
  };
  i16 offset;
  i32 imm;
} Inst;

#define MEM_SIZE 64

typedef struct Memory {
  Inst mem[MEM_SIZE];
} Memory;

typedef struct VM {
  u32 regs[MAX_REGS];
  u32 stack[MAX_STACK_SIZE];
  u32 call_stack[MAX_STACK_SIZE];
  u32 PC;
  u32 SP;
  u32 CSP;
  Inst current_inst;
  int is_running;  
} VM;

static inline void VM_reset(VM *vm, Memory *rom) {
  if (!vm) return;
  if (!rom) return;

  vm->PC = 0x0;
  vm->SP = MAX_STACK_SIZE;
  vm->CSP = MAX_STACK_SIZE;
  vm->is_running = 1;
  for (int i = 0; i < MAX_REGS; i++) vm->regs[i] = 0;
  for (int i = 0; i < MAX_STACK_SIZE; i++) vm->stack[i] = 0;
  for (int i = 0; i < MAX_STACK_SIZE; i++) vm->call_stack[i] = 0;
  vm->current_inst = (Inst){0};

  for (int i = 0; i < MEM_SIZE; i++) rom->mem[i] = (Inst){0};
}

static inline int VM_run(VM *vm, const Inst *program, size_t progsize, Memory *rom) {
  if (!vm || !program || !progsize) return -1;
  if (!rom) return -1;
  if (progsize > MEM_SIZE) return -1;
  
  memcpy(rom->mem, program, sizeof(Inst) * progsize);

  while (vm->is_running && vm->PC < MEM_SIZE) {
    vm->current_inst = (Inst)rom->mem[vm->PC];
    
    switch (vm->current_inst.opcode) {
      case OP_NOP:
        vm->PC++;
        break;

      case OP_MOV:
        if (vm->current_inst.dest < MAX_REGS)
          vm->regs[vm->current_inst.dest] = vm->current_inst.imm;
        vm->PC++;
        break;

      case OP_ADD:
        if (vm->current_inst.dest < MAX_REGS &&
            vm->current_inst.src < MAX_REGS)
          vm->regs[vm->current_inst.dest] =
              vm->regs[vm->current_inst.dest] +
                vm->regs[vm->current_inst.src];
        
        vm->PC++;
        break;

      case OP_ADDI:
        if (vm->current_inst.dest < MAX_REGS)
          vm->regs[vm->current_inst.dest] =
              vm->regs[vm->current_inst.dest] +
                vm->current_inst.imm;
        
        vm->PC++;
        break;

      case OP_SUB:
        if (vm->current_inst.dest < MAX_REGS &&
            vm->current_inst.src < MAX_REGS)
          vm->regs[vm->current_inst.dest] =
              vm->regs[vm->current_inst.dest] -
                vm->regs[vm->current_inst.src];
        vm->PC++;
        break;

     case OP_SUBI:
        if (vm->current_inst.dest < MAX_REGS)
          vm->regs[vm->current_inst.dest] =
              vm->regs[vm->current_inst.dest] - vm->current_inst.imm;
        vm->PC++;
        break;

      case OP_AND:
        if (vm->current_inst.dest < MAX_REGS &&
            vm->current_inst.src < MAX_REGS)
          vm->regs[vm->current_inst.dest] =
              vm->regs[vm->current_inst.dest] &
                vm->regs[vm->current_inst.src];
        vm->PC++;
        break;

      case OP_SHR:
        if (vm->current_inst.dest < MAX_REGS && vm->current_inst.offset)
         vm->regs[vm->current_inst.dest] =
             vm->regs[vm->current_inst.dest] >>= (vm->current_inst.offset & 0x1F);
        vm->PC++;
        break;

      case OP_SHL:
        if (vm->current_inst.dest < MAX_REGS && vm->current_inst.offset)
           vm->regs[vm->current_inst.dest] =
             vm->regs[vm->current_inst.dest] <<= (vm->current_inst.offset & 0x1F);

        vm->PC++;
        break;

      case OP_PUSH:
        if (vm->current_inst.src < MAX_REGS) {
          if (vm->SP > 0) {
            vm->SP--;
            vm->stack[vm->SP] = vm->regs[vm->current_inst.src];
          } else {
            printf("Error: Stack Overflow\n");
            vm->is_running = 0;
            return -1;
          }
        }
        vm->PC++;
        break;
        
      case OP_POP:
        if (vm->current_inst.dest < MAX_REGS) {
          if (vm->SP < MAX_STACK_SIZE) {
            vm->regs[vm->current_inst.dest] = vm->stack[vm->SP];
            vm->SP++;
          } else {
            printf("Error: Stack Underflow\n");
            vm->is_running = 0;
            return -1;
          }
        }
        vm->PC++;
        break;

      case OP_CALL:
        if (vm->CSP > 0) {
          vm->CSP--;
          vm->call_stack[vm->CSP] = vm->PC + 1;
          vm->PC = (u32)vm->current_inst.imm;
        } else {
          printf("Error: Call Stack Overflow\n");
          vm->is_running = 0;
          return -1;
        }
        break;

      case OP_CALLR:
        if (vm->CSP > 0) {
          u32 next_pc = vm->PC + 1;
          vm->CSP--;
          vm->call_stack[vm->CSP] = next_pc;
          vm->PC = (u32)((i32)next_pc + vm->current_inst.offset);
        } else {
          printf("Error: Call Stack Overflow\n");
          vm->is_running = 0;
          return -1;
        }
        break;

      case OP_RET:
        if (vm->CSP < MAX_STACK_SIZE) {
          vm->PC = vm->call_stack[vm->CSP];
          vm->CSP++;
        } else {
          printf("Error: Call Stack Underflow\n");
          vm->is_running = 0;
          return -1;
        }
        break;
        
      case OP_HALT:
        vm->PC++;
        vm->is_running = 0;
        printf("Halted.\n");
        for (int i = 0; i < MAX_REGS; i++) {
          printf("R%d: %u\n", i, vm->regs[i]);
        }
        printf("Stack (SP = %u):\n", vm->SP);
        for (int i = vm->SP; i < MAX_STACK_SIZE; i++) {
          printf("  [%d]: %u\n", i, vm->stack[i]);
        }
        printf("Call Stack (CSP = %u):\n", vm->CSP);
        for (int i = vm->CSP; i < MAX_STACK_SIZE; i++) {
          printf("  [%d]: %u\n", i, vm->call_stack[i]);
        }
        break;
        
      default:
            vm->is_running = 0;
            printf("Unknown Opcode: %d\n", vm->current_inst.opcode);
            break;
    }
  }
  
  return 0; // add logic later
}

#endif
