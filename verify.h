#ifndef VERIFIER_H
#define VERIFIER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "vm.h"

typedef struct VerifierReport {
    bool is_valid;
    size_t invalid_pc;
    const char *reason;
} VerifierReport;

static inline VerifierReport VM_verify(const Inst *program, size_t prog_len) {
    if (!program || prog_len == 0) {
        return (VerifierReport){ false, 0, "Empty program stream or NULL pointer" };
    }

    if (prog_len > ROM_SIZE) {
        return (VerifierReport){ false, 0, "Program length exceeds maximum ROM size" };
    }

    bool has_halt = false;

    for (size_t i = 0; i < prog_len; i++) {
        Inst inst = program[i];

        switch (inst.opcode) {
            case OP_NOP:
            case OP_HALT:
            case OP_RET:
                break;

            case OP_MOV: case OP_ADDI: case OP_SUBI: case OP_MULI:
            case OP_DIVI: case OP_NOT: case OP_SHR: case OP_SHL:
            case OP_CMPI: case OP_POP:
                if (inst.dest >= MAX_REGS) {
                    return (VerifierReport){ false, i, "Destination register index out of bounds" };
                }
                break;

            case OP_LOAD_PC: case OP_LOADB_PC:
                if (inst.dest >= MAX_REGS) {
                    return (VerifierReport){ false, i, "Destination register index out of bounds" };
                }
                if (inst.imm < 0 || (size_t)inst.imm >= RAM_SIZE) {
                    return (VerifierReport){ false, i, "PC-relative load target out of RAM bounds" };
                }
                break;

            case OP_PUSH:
                if (inst.src >= MAX_REGS) {
                    return (VerifierReport){ false, i, "Source register index out of bounds" };
                }
                break;

            case OP_MOVR: case OP_ADD: case OP_SUB: case OP_MUL:
            case OP_DIV:  case OP_MOD: case OP_AND: case OP_OR:
            case OP_XOR:  case OP_CMP: case OP_LOAD: case OP_STORE:
            case OP_LOADB: case OP_STOREB:
                if (inst.dest >= MAX_REGS || inst.src >= MAX_REGS) {
                    return (VerifierReport){ false, i, "Register index (dest or src) out of bounds" };
                }
                break;

            case OP_JMP: case OP_JZ: case OP_JNZ:
            case OP_JLT: case OP_JGT: case OP_CALL:
                if (inst.imm < 0 || (size_t)inst.imm >= prog_len) {
                    return (VerifierReport){ false, i, "Absolute jump/call target out of ROM bounds" };
                }
                break;

            case OP_JMPO: case OP_CALLR: {
                i64 target = (i64)i + 1 + inst.imm;
                if (target < 0 || (size_t)target >= prog_len) {
                    return (VerifierReport){ false, i, "Relative jump/call target out of ROM bounds" };
                }
                break;
            }

            case OP_SYS:
                if (inst.imm <= 0) {
                    return (VerifierReport){ false, i, "Invalid or unhandled Syscall code" };
                }
                break;

            default:
                return (VerifierReport){ false, i, "Unknown/Illegal Opcode detected" };
        }

        if (inst.opcode == OP_DIVI && inst.imm == 0) {
            return (VerifierReport){ false, i, "Immediate division by zero detected" };
        }

        if (inst.opcode == OP_HALT) {
            has_halt = true;
        }
    }

    if (!has_halt) {
        return (VerifierReport){ false, prog_len - 1, "Missing OP_HALT instruction (Non-terminating risk)" };
    }

    return (VerifierReport){ true, 0, "PASS" };
}

#endif // VERIFIER_H
