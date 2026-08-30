#ifndef VERIFIER_H
#define VERIFIER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vm.h"

#ifndef VM_VERIFY_MAX_STEPS
#define VM_VERIFY_MAX_STEPS 100000u
#endif

#ifndef VM_VERIFY_MAX_CALL_DEPTH
#define VM_VERIFY_MAX_CALL_DEPTH 64u
#endif

#ifndef VM_SYSCALL_MAX
#define VM_SYSCALL_MAX 64
#endif

#ifndef VM_VERIFY_ALLOW_REGISTER_MEMORY
#define VM_VERIFY_ALLOW_REGISTER_MEMORY 1
#endif

#ifndef VM_VERIFY_SYSCALL_VALID
#define VM_VERIFY_SYSCALL_VALID(id) ((id) >= 0 && (id) < VM_SYSCALL_MAX)
#endif

typedef enum VmProgramKind {
    VM_PROGRAM_ENTRY = 0,
    VM_PROGRAM_FUNCTION = 1
} VmProgramKind;

typedef enum VmVerifyError {
    VM_VERIFY_OK = 0,
    VM_VERIFY_NULL_PROGRAM,
    VM_VERIFY_EMPTY_PROGRAM,
    VM_VERIFY_PROGRAM_TOO_LARGE,
    VM_VERIFY_BAD_OPCODE,
    VM_VERIFY_BAD_DEST_REGISTER,
    VM_VERIFY_BAD_SRC_REGISTER,
    VM_VERIFY_BAD_ABSOLUTE_TARGET,
    VM_VERIFY_BAD_RELATIVE_TARGET,
    VM_VERIFY_BACKWARD_JUMP,
    VM_VERIFY_BACKWARD_CALL,
    VM_VERIFY_BAD_SYSCALL,
    VM_VERIFY_DIVIDE_BY_ZERO,
    VM_VERIFY_MEMORY_ACCESS_UNSUPPORTED,
    VM_VERIFY_NO_HALT,
    VM_VERIFY_INVALID_RETURN,
    VM_VERIFY_CALL_DEPTH_UNPROVEN
} VmVerifyError;

typedef struct VerifierReport {
    bool valid;
    bool is_valid;
    size_t invalid_pc;
    VmVerifyError error;
    const char *reason;
} VerifierReport;

static inline VerifierReport vm_verify_error(size_t pc, VmVerifyError error, const char *reason) {
    return (VerifierReport){
        .valid = false,
        .is_valid = false,
        .invalid_pc = pc,
        .error = error,
        .reason = reason
    };
}

static inline VerifierReport vm_verify_success(void) {
    return (VerifierReport){
        .valid = true,
        .is_valid = true,
        .invalid_pc = 0,
        .error = VM_VERIFY_OK,
        .reason = "PASS"
    };
}

static inline bool vm_verify_valid_reg(uint8_t reg) {
    return reg < MAX_REGS;
}

static inline bool vm_verify_is_call(uint8_t opcode) {
    return opcode == OP_CALL || opcode == OP_CALLR;
}

static inline bool vm_verify_is_unconditional_jump(uint8_t opcode) {
    return opcode == OP_JMP || opcode == OP_JMPO;
}

static inline VerifierReport VM_verify_kind(const Inst *program, size_t prog_len, VmProgramKind kind) {
    if (program == NULL) {
        return vm_verify_error(
            0,
            VM_VERIFY_NULL_PROGRAM,
            "Program stream is NULL"
        );
    }

    if (prog_len == 0) {
        return vm_verify_error(
            0,
            VM_VERIFY_EMPTY_PROGRAM,
            "Program stream is empty"
        );
    }

    if (prog_len > ROM_SIZE) {
        return vm_verify_error(
            0,
            VM_VERIFY_PROGRAM_TOO_LARGE,
            "Program length exceeds maximum ROM size"
        );
    }

    bool has_halt = false;
    bool has_ret = false;
    size_t call_count = 0;

    for (size_t pc = 0; pc < prog_len; pc++) {
        const Inst inst = program[pc];

        switch (inst.opcode) {
            case OP_NOP:
                break;

            case OP_HALT:
                has_halt = true;
                break;

            case OP_RET:
                has_ret = true;
                break;

            case OP_MOV:
            case OP_ADDI:
            case OP_SUBI:
            case OP_MULI:
            case OP_DIVI:
            case OP_NOT:
            case OP_SHR:
            case OP_SHL:
            case OP_CMPI:
            case OP_POP:
                if (!vm_verify_valid_reg(inst.dest)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_DEST_REGISTER,
                        "Destination register index out of bounds"
                    );
                }
                break;

            case OP_LOAD_PC:
            case OP_LOADB_PC:
                if (!vm_verify_valid_reg(inst.dest)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_DEST_REGISTER,
                        "Destination register index out of bounds"
                    );
                }

                if (inst.imm < 0 || (uint64_t)inst.imm >= (uint64_t)RAM_SIZE) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_RELATIVE_TARGET,
                        "PC-relative load target is outside RAM"
                    );
                }
                break;

            case OP_PUSH:
                if (!vm_verify_valid_reg(inst.src)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_SRC_REGISTER,
                        "Source register index out of bounds"
                    );
                }
                break;

            case OP_MOVR:
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
            case OP_MOD:
            case OP_AND:
            case OP_OR:
            case OP_XOR:
            case OP_CMP:
            case OP_LOAD:
            case OP_STORE:
            case OP_LOADB:
            case OP_STOREB:
                if (!vm_verify_valid_reg(inst.dest)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_DEST_REGISTER,
                        "Destination register index out of bounds"
                    );
                }

                if (!vm_verify_valid_reg(inst.src)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_SRC_REGISTER,
                        "Source register index out of bounds"
                    );
                }

#if !VM_VERIFY_ALLOW_REGISTER_MEMORY
                if (inst.opcode == OP_LOAD ||
                    inst.opcode == OP_STORE ||
                    inst.opcode == OP_LOADB ||
                    inst.opcode == OP_STOREB) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_MEMORY_ACCESS_UNSUPPORTED,
                        "Register-derived memory access requires runtime bounds checks"
                    );
                }
#endif
                break;

            case OP_JMP:
            case OP_JZ:
            case OP_JNZ:
            case OP_JLT:
            case OP_JGT:
            case OP_CALL:
                if (inst.imm < 0 ||
                    (uint64_t)inst.imm >= (uint64_t)prog_len) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_ABSOLUTE_TARGET,
                        "Absolute branch or call target is outside ROM"
                    );
                }

                if ((size_t)inst.imm <= pc) {
                    if (inst.opcode == OP_CALL) {
                        return vm_verify_error(
                            pc,
                            VM_VERIFY_BACKWARD_CALL,
                            "Backward or recursive call is not allowed"
                        );
                    }

                    if (inst.opcode == OP_JMP) {
                        return vm_verify_error(
                            pc,
                            VM_VERIFY_BACKWARD_JUMP,
                            "Backward unconditional jump is not allowed"
                        );
                    }
                }
                break;

            case OP_JMPO:
            case OP_CALLR: {
                int64_t target = (int64_t)pc + 1 + (int64_t)inst.imm;

                if (target < 0 || target >= (int64_t)prog_len) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_RELATIVE_TARGET,
                        "Relative branch or call target is outside ROM"
                    );
                }

                if (target <= (int64_t)pc) {
                    if (inst.opcode == OP_CALLR) {
                        return vm_verify_error(
                            pc,
                            VM_VERIFY_BACKWARD_CALL,
                            "Backward or recursive call is not allowed"
                        );
                    }

                    if (inst.opcode == OP_JMPO) {
                        return vm_verify_error(
                            pc,
                            VM_VERIFY_BACKWARD_JUMP,
                            "Backward unconditional jump is not allowed"
                        );
                    }
                }
                break;
            }

            case OP_SYS:
                if (!VM_VERIFY_SYSCALL_VALID(inst.imm)) {
                    return vm_verify_error(
                        pc,
                        VM_VERIFY_BAD_SYSCALL,
                        "Unknown, invalid, or unavailable syscall"
                    );
                }
                break;

            default:
                return vm_verify_error(
                    pc,
                    VM_VERIFY_BAD_OPCODE,
                    "Unknown or illegal opcode"
                );
        }

        if (inst.opcode == OP_DIVI && inst.imm == 0) {
            return vm_verify_error(
                pc,
                VM_VERIFY_DIVIDE_BY_ZERO,
                "Immediate division by zero"
            );
        }

        if (vm_verify_is_call(inst.opcode)) {
            call_count++;

            if (call_count > VM_VERIFY_MAX_CALL_DEPTH) {
                return vm_verify_error(
                    pc,
                    VM_VERIFY_CALL_DEPTH_UNPROVEN,
                    "Maximum call depth cannot be proven"
                );
            }
        }
    }

    if (kind == VM_PROGRAM_ENTRY && !has_halt) {
        return vm_verify_error(
            prog_len - 1,
            VM_VERIFY_NO_HALT,
            "Entry program has no OP_HALT instruction"
        );
    }

    if (kind == VM_PROGRAM_FUNCTION && !has_ret) {
        return vm_verify_error(
            prog_len - 1,
            VM_VERIFY_INVALID_RETURN,
            "Function program has no OP_RET instruction"
        );
    }

    return vm_verify_success();
}

static inline VerifierReport VM_verify(const Inst *program, size_t prog_len) {
    return VM_verify_kind(program, prog_len, VM_PROGRAM_ENTRY);
}

#endif
