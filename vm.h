#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "glue.h"

#define INST_NOP()                    ((Inst){ .opcode = OP_NOP })
#define INST_MOV(r_dst, immv)         ((Inst){ .opcode = OP_MOV, .dest = (r_dst), .imm = (immv) })
#define INST_MOVR(r_dst, r_src)       ((Inst){ .opcode = OP_MOVR, .dest = (r_dst), .src = (r_src) })
#define INST_ADD(r_dst, r_src)        ((Inst){ .opcode = OP_ADD, .dest = (r_dst), .src = (r_src) })
#define INST_ADDI(r_dst, immv)        ((Inst){ .opcode = OP_ADDI, .dest = (r_dst), .imm = (immv) })
#define INST_SUB(r_dst, r_src)        ((Inst){ .opcode = OP_SUB, .dest = (r_dst), .src = (r_src) })
#define INST_SUBI(r_dst, immv)        ((Inst){ .opcode = OP_SUBI, .dest = (r_dst), .imm = (immv) })
#define INST_MUL(r_dst, r_src)        ((Inst){ .opcode = OP_MUL, .dest = (r_dst), .src = (r_src) })
#define INST_MULI(r_dst, immv)        ((Inst){ .opcode = OP_MULI, .dest = (r_dst), .imm = (immv) })
#define INST_DIV(r_dst, r_src)        ((Inst){ .opcode = OP_DIV, .dest = (r_dst), .src = (r_src) })
#define INST_DIVI(r_dst, immv)        ((Inst){ .opcode = OP_DIVI, .dest = (r_dst), .imm = (immv) })
#define INST_MOD(r_dst, r_src)        ((Inst){ .opcode = OP_MOD, .dest = (r_dst), .src = (r_src) })
#define INST_AND(r_dst, r_src)        ((Inst){ .opcode = OP_AND, .dest = (r_dst), .src = (r_src) })
#define INST_OR(r_dst, r_src)         ((Inst){ .opcode = OP_OR, .dest = (r_dst), .src = (r_src) })
#define INST_XOR(r_dst, r_src)        ((Inst){ .opcode = OP_XOR, .dest = (r_dst), .src = (r_src) })
#define INST_NOT(r_dst)               ((Inst){ .opcode = OP_NOT, .dest = (r_dst) })
#define INST_SHR(r_dst, off)          ((Inst){ .opcode = OP_SHR, .dest = (r_dst), .imm = (off) })
#define INST_SHL(r_dst, off)          ((Inst){ .opcode = OP_SHL, .dest = (r_dst), .imm = (off) })
#define INST_CMP(r_dst, r_src)        ((Inst){ .opcode = OP_CMP, .dest = (r_dst), .src = (r_src) })
#define INST_CMPI(r_dst, immv)        ((Inst){ .opcode = OP_CMPI, .dest = (r_dst), .imm = (immv) })
#define INST_JMP(imm_addr)            ((Inst){ .opcode = OP_JMP, .imm = (imm_addr) })
#define INST_JMPO(off_addr)           ((Inst){ .opcode = OP_JMPO, .imm = (off_addr) })
#define INST_JZ(imm_addr)             ((Inst){ .opcode = OP_JZ, .imm = (imm_addr) })
#define INST_JNZ(imm_addr)            ((Inst){ .opcode = OP_JNZ, .imm = (imm_addr) })
#define INST_JLT(imm_addr)            ((Inst){ .opcode = OP_JLT, .imm = (imm_addr) })
#define INST_JGT(imm_addr)            ((Inst){ .opcode = OP_JGT, .imm = (imm_addr) })
#define INST_PUSH(r_src)              ((Inst){ .opcode = OP_PUSH, .src = (r_src) })
#define INST_PUSHI(immv)              ((Inst){ .opcode = OP_PUSHI, .imm = (immv) })
#define INST_POP(r_dst)               ((Inst){ .opcode = OP_POP, .dest = (r_dst) })
#define INST_CALL(imm_addr)           ((Inst){ .opcode = OP_CALL, .imm = (imm_addr) })
#define INST_CALLR(off_addr)          ((Inst){ .opcode = OP_CALLR, .imm = (off_addr) })
#define INST_RET()                    ((Inst){ .opcode = OP_RET })
#define INST_LOAD(r_dst, r_src, off)  ((Inst){ .opcode = OP_LOAD, .dest = (r_dst), .src = (r_src), .imm = (off) })
#define INST_LOAD_PC(r_dst, off)      ((Inst){ .opcode = OP_LOAD_PC, .dest = (r_dst), .imm = (off) })
#define INST_STORE(r_src, r_dst, off) ((Inst){ .opcode = OP_STORE, .src = (r_src), .dest = (r_dst), .imm = (off) })
#define INST_LOADB(r_dst, r_src, off) ((Inst){ .opcode = OP_LOADB, .dest = (r_dst), .src = (r_src), .imm = (off) })
#define INST_LOADB_PC(r_dst, off)     ((Inst){ .opcode = OP_LOADB_PC, .dest = (r_dst), .imm = (off) })
#define INST_STOREB(r_src, r_dst, off)((Inst){ .opcode = OP_STOREB, .src = (r_src), .dest = (r_dst), .imm = (off) })
#define INST_SYS(sys_code)            ((Inst){ .opcode = OP_SYS, .imm = (sys_code) })
#define INST_HALT()                   ((Inst){ .opcode = OP_HALT })

#define COUNTOF(arr) (sizeof(arr) / sizeof((arr)[0]))

enum VMOpcodes {
    OP_NOP = 0x00,
    OP_MOV,       // Reg = Imm
    OP_MOVR,      // Reg = Reg
    OP_ADD,       // Reg += Reg
    OP_ADDI,      // Reg += Imm
    OP_SUB,       // Reg -= Reg
    OP_SUBI,      // Reg -= Imm
    OP_MUL,       // Reg *= Reg
    OP_MULI,      // Reg *= Imm
    OP_DIV,       // Reg /= Reg
    OP_DIVI,      // Reg /= Imm
    OP_MOD,       // Reg %= Reg
    OP_AND,       // Reg &= Reg
    OP_OR,        // Reg |= Reg
    OP_XOR,       // Reg ^= Reg
    OP_NOT,       // Reg = ~Reg
    OP_SHR,       // Reg >>= Imm
    OP_SHL,       // Reg <<= Imm
    OP_CMP,       // Compare Reg, Reg
    OP_CMPI,      // Compare Reg, Imm
    OP_JMP,       // Absolute Jump
    OP_JMPO,      // PC-Relative Jump (PC + imm)
    OP_JZ,        // Jump if Zero
    OP_JNZ,       // Jump if Not Zero
    OP_JLT,       // Jump if Less Than
    OP_JGT,       // Jump if Greater Than
    OP_PUSH,      // Stack Push Reg
    OP_PUSHI,     // Stack Push Imm
    OP_POP,       // Stack Pop Reg
    OP_CALL,      // Absolute Call
    OP_CALLR,     // Relative Call
    OP_RET,       // Return
    OP_LOAD,      // Reg = RAM[Reg + Imm]
    OP_LOAD_PC,   // Reg = RAM[PC_offset + Imm] (Position Independent Load)
    OP_STORE,     // RAM[Reg + Imm] = Reg
    OP_LOADB,     // Reg = RAM[Reg + Imm] (1 byte)
    OP_LOADB_PC,  // Reg = RAM[PC_offset + Imm] (1 byte, PIE)
    OP_STOREB,    // RAM[Reg + Imm] = Reg (1 byte)
    OP_SYS,       // System Call
    OP_HALT = 0xFF
};

typedef struct __attribute__((packed)) Inst {
    u8  opcode;
    u8  dest;
    u8  src;
    u8  reserved;
    i32 imm;
} Inst;

typedef struct Memory {
    Inst rom[ROM_SIZE];
    u8   ram[RAM_SIZE];
} Memory;

struct VM;
typedef void (*VMSyscallHandler)(struct VM *vm, Memory *mem, u32 sys_code);

typedef struct VM {
    u32 regs[MAX_REGS];
    u32 flags;
    u32 stack[MAX_STACK_SIZE];
    u32 call_stack[MAX_STACK_SIZE];
    u32 PC;
    u32 SP;
    u32 CSP;
    Inst current_inst;
    int is_running;
    VMSyscallHandler syscall_handler;
} VM;

#pragma pack(push, 1)
typedef struct __attribute__((packed)) VMHeader {
    u32 magic;
    u32 version;
    u32 inst_count;
    u32 data_size;
} VMHeader;
#pragma pack(pop)

static inline void VM_reset(VM *vm, Memory *mem) {
    if (!vm || !mem) return;
    vm->PC = 0;
    vm->flags = 0;
    vm->SP = MAX_STACK_SIZE;
    vm->CSP = MAX_STACK_SIZE;
    vm->is_running = 1;
    vm->syscall_handler = NULL;
    memset(vm->regs, 0, sizeof(vm->regs));
    memset(vm->stack, 0, sizeof(vm->stack));
    memset(vm->call_stack, 0, sizeof(vm->call_stack));
    vm->current_inst = (Inst){0};
    memset(mem->rom, 0, sizeof(mem->rom));
    memset(mem->ram, 0, sizeof(mem->ram));
}

static inline void VM_update_flags(VM *vm, i64 result) {
    vm->flags = 0;
    if ((u32)result == 0) vm->flags |= FLAG_ZERO;
    if (result < 0)       vm->flags |= FLAG_NEGATIVE;
}

static inline int VM_run(VM *vm, const Inst *program, size_t progsize, Memory *mem) {
    if (!vm || !program || !progsize || !mem) return -1;
    if (progsize > ROM_SIZE) return -1;

    memcpy(mem->rom, program, sizeof(Inst) * progsize);

    while (vm->is_running && vm->PC < ROM_SIZE) {
        Inst inst = mem->rom[vm->PC++];
        vm->current_inst = inst;

        switch (inst.opcode) {
            case OP_NOP:
                break;

            case OP_MOV:
                if (inst.dest < MAX_REGS) vm->regs[inst.dest] = inst.imm;
                break;

            case OP_MOVR:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS)
                    vm->regs[inst.dest] = vm->regs[inst.src];
                break;

            case OP_ADD:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    vm->regs[inst.dest] += vm->regs[inst.src];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_ADDI:
                if (inst.dest < MAX_REGS) {
                    vm->regs[inst.dest] += inst.imm;
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_SUB:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    vm->regs[inst.dest] -= vm->regs[inst.src];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_SUBI:
                if (inst.dest < MAX_REGS) {
                    vm->regs[inst.dest] -= inst.imm;
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_MUL:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS)
                    vm->regs[inst.dest] *= vm->regs[inst.src];
                break;

            case OP_MULI:
                if (inst.dest < MAX_REGS) vm->regs[inst.dest] *= inst.imm;
                break;

            case OP_DIV:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    if (vm->regs[inst.src] == 0) { printf("Fault: Div by 0\n"); vm->is_running = 0; break; }
                    vm->regs[inst.dest] /= vm->regs[inst.src];
                }
                break;

            case OP_DIVI:
                if (inst.dest < MAX_REGS) {
                    if (inst.imm == 0) { printf("Fault: Div by 0\n"); vm->is_running = 0; break; }
                    vm->regs[inst.dest] /= inst.imm;
                }
                break;

            case OP_MOD:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    if (vm->regs[inst.src] == 0) { printf("Fault: Mod by 0\n"); vm->is_running = 0; break; }
                    vm->regs[inst.dest] %= vm->regs[inst.src];
                }
                break;

            case OP_AND:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    vm->regs[inst.dest] &= vm->regs[inst.src];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_OR:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    vm->regs[inst.dest] |= vm->regs[inst.src];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_XOR:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    vm->regs[inst.dest] ^= vm->regs[inst.src];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_NOT:
                if (inst.dest < MAX_REGS) {
                    vm->regs[inst.dest] = ~vm->regs[inst.dest];
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_SHR:
                if (inst.dest < MAX_REGS) {
                    vm->regs[inst.dest] >>= inst.imm;
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_SHL:
                if (inst.dest < MAX_REGS) {
                    vm->regs[inst.dest] <<= inst.imm;
                    VM_update_flags(vm, (i32)vm->regs[inst.dest]);
                }
                break;

            case OP_CMP:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    i64 diff = (i64)vm->regs[inst.dest] - (i64)vm->regs[inst.src];
                    VM_update_flags(vm, diff);
                }
                break;

            case OP_CMPI:
                if (inst.dest < MAX_REGS) {
                    i64 diff = (i64)vm->regs[inst.dest] - (i64)inst.imm;
                    VM_update_flags(vm, diff);
                }
                break;

            case OP_JMP:
                vm->PC = inst.imm;
                break;

            case OP_JMPO:
                vm->PC += inst.imm;
                break;

            case OP_JZ:
                if (vm->flags & FLAG_ZERO) vm->PC = inst.imm;
                break;

            case OP_JNZ:
                if (!(vm->flags & FLAG_ZERO)) vm->PC = inst.imm;
                break;

            case OP_JLT:
                if (vm->flags & FLAG_NEGATIVE) vm->PC = inst.imm;
                break;

            case OP_JGT:
                if (!(vm->flags & (FLAG_ZERO | FLAG_NEGATIVE))) vm->PC = inst.imm;
                break;

            case OP_PUSH:
                if (inst.src < MAX_REGS && vm->SP > 0)
                    vm->stack[--vm->SP] = vm->regs[inst.src];
                break;

            case OP_PUSHI:
                if (vm->SP > 0)
                    vm->stack[--vm->SP] = inst.imm;
                break;

            case OP_POP:
                if (inst.dest < MAX_REGS && vm->SP < MAX_STACK_SIZE)
                    vm->regs[inst.dest] = vm->stack[vm->SP++];
                break;

            case OP_CALL:
                if (vm->CSP > 0) {
                    vm->call_stack[--vm->CSP] = vm->PC;
                    vm->PC = inst.imm;
                } else {
                    printf("Fault: Call Stack Overflow\n");
                    vm->is_running = 0;
                }
                break;

            case OP_CALLR:
                if (vm->CSP > 0) {
                    vm->call_stack[--vm->CSP] = vm->PC;
                    vm->PC += inst.imm;
                }
                break;

            case OP_RET:
                if (vm->CSP < MAX_STACK_SIZE) {
                    vm->PC = vm->call_stack[vm->CSP++];
                } else {
                    printf("Fault: Call Stack Underflow\n");
                    vm->is_running = 0;
                }
                break;

            case OP_LOAD:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    u32 addr = vm->regs[inst.src] + (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                        vm->regs[inst.dest] = *(u32 *)&mem->ram[addr];
                    }
                }
                break;

            case OP_LOAD_PC:
                if (inst.dest < MAX_REGS) {
                    u32 addr = (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                        vm->regs[inst.dest] = *(u32 *)&mem->ram[addr];
                    }
                }
                break;

            case OP_STORE:
                if (inst.src < MAX_REGS && inst.dest < MAX_REGS) {
                    u32 addr = vm->regs[inst.dest] + (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                        *(u32 *)&mem->ram[addr] = vm->regs[inst.src];
                    }
                }
                break;

            case OP_LOADB:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    u32 addr = vm->regs[inst.src] + (u32)inst.imm;
                    if (addr < RAM_SIZE) {
                        vm->regs[inst.dest] = mem->ram[addr];
                    }
                }
                break;

            case OP_LOADB_PC:
                if (inst.dest < MAX_REGS) {
                    u32 addr = (u32)inst.imm;
                    if (addr < RAM_SIZE) {
                        vm->regs[inst.dest] = mem->ram[addr];
                    }
                }
                break;

            case OP_STOREB:
                if (inst.src < MAX_REGS && inst.dest < MAX_REGS) {
                    u32 addr = vm->regs[inst.dest] + (u32)inst.imm;
                    if (addr < RAM_SIZE) {
                        mem->ram[addr] = (u8)vm->regs[inst.src];
                    }
                }
                break;

            case OP_SYS:
                if (vm->syscall_handler) {
                    vm->syscall_handler(vm, mem, (u32)inst.imm);
                } else {
                    switch (inst.imm) {
                        case 1: putchar((char)vm->regs[0]); break;
                        case 2:
                            if (vm->regs[0] < RAM_SIZE) {
                                printf("%s", (char *)&mem->ram[vm->regs[0]]);
                            }
                            break;
                        default:
                            printf("Fault: Unhandled Syscall %d (No handler set)\n", inst.imm);
                            vm->is_running = 0;
                            break;
                    }
                }
                break;

            case OP_HALT:
                vm->is_running = 0;
                printf("\n--- VM Halted ---\n");
                for (int i = 0; i < MAX_REGS; i++) {
                    printf("R%-2d: 0x%08X (%u)\n", i, vm->regs[i], vm->regs[i]);
                }
                break;

            default:
                printf("Fault: Invalid Opcode 0x%02X at PC=0x%04X\n", inst.opcode, vm->PC - 1);
                vm->is_running = 0;
                return -1;
        }
    }
    return 0;
}

static inline int VM_export_stream(FILE *f, const Inst *program, size_t prog_len,
                                  const void *data_bytes, size_t data_size) {
    if (!f || !program) return -1;

    VMHeader header = {
        .magic = VM_MAGIC,
        .version = VM_VERSION,
        .inst_count = (u32)prog_len,
        .data_size = (u32)data_size
    };

    if (fwrite(&header, sizeof(VMHeader), 1, f) != 1) return -1;
    if (fwrite(program, sizeof(Inst), prog_len, f) != prog_len) return -1;
    if (data_size > 0 && fwrite(data_bytes, 1, data_size, f) != data_size) return -1;

    return 0;
}

static inline int VM_import_stream(FILE *f, Memory *mem, size_t *out_prog_len, u32 actual_data_vaddr) {
    if (!f || !mem) return -1;

    VMHeader header;
    if (fread(&header, sizeof(VMHeader), 1, f) != 1) return -1;
    if (header.magic != VM_MAGIC) return -1;

    if (fread(mem->rom, sizeof(Inst), header.inst_count, f) != header.inst_count) return -1;

    if (header.data_size > 0) {
        if (fread(mem->ram + actual_data_vaddr, 1, header.data_size, f) != header.data_size) return -1;
    }

    if (out_prog_len) *out_prog_len = header.inst_count;
    return 0;
}

static inline int VM_run_file(const char *filename, Memory *mem, VM *vm, u32 load_ram_vaddr) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    size_t prog_len = 0;
    if (VM_import_stream(f, mem, &prog_len, load_ram_vaddr) != 0) {
        fclose(f);
        return -1;
    }
    fclose(f);

    return VM_run(vm, mem->rom, prog_len, mem);
}

#endif // VM_H
