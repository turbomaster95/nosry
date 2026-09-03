#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef NOSRY_GLUE_GLOBAL
    #include <vmglue.h>
#else
    #include "glue.h"
#endif

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

#define MAX_STRING_TABLE_SIZE 1024

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

typedef struct StringTable {
    char *strings[MAX_STRING_TABLE_SIZE];
    u32 count;
} StringTable;

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
    StringTable str_table;
} VM;

#pragma pack(push, 1)
typedef struct __attribute__((packed)) VMHeader {
    u32 magic;
    u32 version;
    u32 inst_count;
    u32 data_size;
} VMHeader;
#pragma pack(pop)

static inline int VM_register_str(VM *vm, const char *str) {
    if (!vm || !str) return -1;
    if (vm->str_table.count >= MAX_STRING_TABLE_SIZE) return -1;
    vm->str_table.strings[vm->str_table.count] = strdup(str);
    return vm->str_table.count++;
}

static inline const char* VM_get_string(VM* vm, u32 id) {
    if (id >= vm->str_table.count) return NULL;
    return vm->str_table.strings[id];
}

static inline void VM_clear_strings(VM* vm) {
    for (u32 i = 0; i < vm->str_table.count; i++) {
        vm->str_table.strings[i] = NULL;
    }
    vm->str_table.count = 0;
}

static inline void VM_reset(VM *vm, Memory *mem) {
    if (!vm || !mem) return;
    vm->PC = 0;
    vm->flags = 0;
    vm->SP = MAX_STACK_SIZE;
    vm->CSP = MAX_STACK_SIZE;
    vm->is_running = 1;
    vm->syscall_handler = NULL;

    StringTable new_str_tbl = {0};
    vm->str_table = new_str_tbl;

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
                    if (vm->regs[inst.src] == 0) { PRINTF("Fault: Div by 0\n"); vm->is_running = 0; break; }
                    vm->regs[inst.dest] /= vm->regs[inst.src];
                }
                break;

            case OP_DIVI:
                if (inst.dest < MAX_REGS) {
                    if (inst.imm == 0) { PRINTF("Fault: Div by 0\n"); vm->is_running = 0; break; }
                    vm->regs[inst.dest] /= inst.imm;
                }
                break;

            case OP_MOD:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    if (vm->regs[inst.src] == 0) { PRINTF("Fault: Mod by 0\n"); vm->is_running = 0; break; }
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
                    PRINTF("Fault: Call Stack Overflow\n");
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
                    PRINTF("Fault: Call Stack Underflow\n");
                    vm->is_running = 0;
                }
                break;

            case OP_LOAD:
                if (inst.dest < MAX_REGS && inst.src < MAX_REGS) {
                    u32 addr = vm->regs[inst.src] + (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                    	u32 val;
                    	memcpy(&val, &mem->ram[addr], sizeof(u32));
                        vm->regs[inst.dest] = val;
                    }
                }
                break;

            case OP_LOAD_PC:
                if (inst.dest < MAX_REGS) {
                    u32 addr = (vm->PC * sizeof(Inst)) + (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                    	u32 val;
                        memcpy(&val, &mem->ram[addr], sizeof(u32));
                        vm->regs[inst.dest] = val;
                    }
                }
                break;

            case OP_STORE:
                if (inst.src < MAX_REGS && inst.dest < MAX_REGS) {
                    u32 addr = vm->regs[inst.dest] + (u32)inst.imm;
                    if (addr + 3 < RAM_SIZE) {
                        memcpy(&mem->ram[addr], &vm->regs[inst.src], sizeof(u32));
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
                    u32 addr = (vm->PC * sizeof(Inst)) + (u32)inst.imm;
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
                                PRINTF("%s", VM_get_string(vm, vm->regs[0]));
                            }
                            break;
                        default:
                            PRINTF("Fault: Unhandled Syscall %d (No handler set)\n", inst.imm);
                            vm->is_running = 0;
                            break;
                    }
                }
                break;

            case OP_HALT:
                vm->is_running = 0;
                PRINTF("\n--- VM Halted ---\n");
                for (int i = 0; i < MAX_REGS; i++) {
                   PRINTF("R%-2d: 0x%08X (%u)\n", i, vm->regs[i], vm->regs[i]);
                }
                break;

            default:
                PRINTF("Fault: Invalid Opcode 0x%02X at PC=0x%04X\n", inst.opcode, vm->PC - 1);
                vm->is_running = 0;
                return -1;
        }
    }
    return 0;
}

static inline int VM_export_stream(VM *vm, FILESTRUCT *f, const Inst *program, size_t prog_len,
                                   const void *data_bytes, size_t data_size) {
    if (!f || !program) return -1;

    VMHeader header = {
        .magic = VM_MAGIC,
        .version = VM_VERSION,
        .inst_count = (u32)prog_len,
        .data_size = (u32)data_size
    };

    if (WRITFILE(&header, sizeof(VMHeader), 1, f) != 1) return -1;

    WRITFILE(&vm->str_table.count, sizeof(u32), 1, f);
    for (u32 i = 0; i < vm->str_table.count; i++) {
        u32 len = (u32)strlen(vm->str_table.strings[i]);
        WRITFILE(&len, sizeof(u32), 1, f);
        WRITFILE(vm->str_table.strings[i], sizeof(char), len, f);
    }

    if (WRITFILE(program, sizeof(Inst), prog_len, f) != prog_len) return -1;
    if (data_size > 0 && WRITFILE(data_bytes, 1, data_size, f) != data_size) return -1;

    return 0;
}

static inline int VM_import_stream(VM* vm, FILESTRUCT *f, Memory *mem, size_t *out_prog_len, u32 actual_data_vaddr) {
    if (!f || !mem) return -1;

    VMHeader header;
    if (READFILE(&header, sizeof(VMHeader), 1, f) != 1) return -1;
    if (header.magic != VM_MAGIC && header.magic != 0x4E4F5259) return -1; // 0x4E4F5259 (NORY) is a backup one for all nosry-bytecode to run universally!

    u32 str_count = 0;
    if (READFILE(&str_count, sizeof(u32), 1, f) == 1) {
        for (u32 i = 0; i < str_count; i++) {
            u32 len = 0;
            READFILE(&len, sizeof(u32), 1, f);
            char *s = GLUEMALLOC(len + 1);
            READFILE(s, sizeof(char), len, f);
            s[len] = '\0';
            VM_register_str(vm,s);
            GLUEFREE(s);
        }
    }

    if (READFILE(mem->rom, sizeof(Inst), header.inst_count, f) != header.inst_count) return -1;
    if (header.data_size > 0) {
        if (READFILE(mem->ram + actual_data_vaddr, 1, header.data_size, f) != header.data_size) return -1;
    }

    if (out_prog_len) *out_prog_len = header.inst_count;
    return 0;
}

static inline int VM_run_file(const char *filename, Memory *mem, VM *vm, u32 load_ram_vaddr) {
    FILESTRUCT *f = OPENFILE(filename, "rb");
    if (!f) return -1;

    size_t prog_len = 0;
    if (VM_import_stream(vm, f, mem, &prog_len, load_ram_vaddr) != 0) {
        CLOSFILE(f);
        return -1;
    }
    CLOSFILE(f);

    return VM_run(vm, mem->rom, prog_len, mem);
}

static inline void VM_printf(VM *vm, Memory *mem, int reg_base, int arg_count, const char *fmt) {
    if (!fmt) return;

    int arg_idx = 0;
    const char *p = fmt;

    while (*p) {
        if (*p != '%') {
            putchar(*p++);
            continue;
        }

        p++; // Skip '%'
        if (*p == '%') {
            putchar('%');
            p++;
            continue;
        }

        char spec[32];
        int len = 0;
        spec[len++] = '%';

        while (*p && !strchr("diuoxXcspfeEgGaA", *p) && len < 30) {
            spec[len++] = *p++;
        }

        if (*p) {
            char conversion = *p++;
            spec[len++] = conversion;
            spec[len] = '\0';

            if (arg_idx < arg_count && (reg_base + arg_idx) < MAX_REGS) {
                u32 raw_val = vm->regs[reg_base + arg_idx++];

                if (conversion == 's') {
                    const char *str = VM_get_string(vm, raw_val);
                    if (!str && raw_val < RAM_SIZE) {
                        str = (const char *)&mem->ram[raw_val];
                    }
                    PRINTF(spec, str ? str : "(null)");
                } else if (conversion == 'c') {
                    PRINTF(spec, (char)raw_val);
                } else {
                    PRINTF(spec, raw_val);
                }
            } else {
		PRINTF("%s", spec);
            }
        }
    }
}
#endif // VM_H
