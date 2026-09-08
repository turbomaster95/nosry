#ifndef DISASM_H
#define DISASM_H

#include "vm.h"

// Function decls
void VM_disassemble_inst(const Inst *inst, u32 pc, char *buf, size_t buf_size);
void VM_disassemble_stream(const Inst *program, size_t count, FILESTRUCT *out);

#ifdef DISASM_IMPLEMENTATION

void VM_disassemble_inst(const Inst *inst, u32 pc, char *buf, size_t buf_size) {
    if (!inst || !buf || buf_size == 0) return;

    switch (inst->opcode) {
        case OP_NOP:
            snprintf(buf, buf_size, "NOP");
            break;
        case OP_MOV:
            snprintf(buf, buf_size, "MOV      R%u, %d", inst->dest, inst->imm);
            break;
        case OP_MOVR:
            snprintf(buf, buf_size, "MOVR     R%u, R%u", inst->dest, inst->src);
            break;
        case OP_ADD:
            snprintf(buf, buf_size, "ADD      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_ADDI:
            snprintf(buf, buf_size, "ADDI     R%u, %d", inst->dest, inst->imm);
            break;
        case OP_SUB:
            snprintf(buf, buf_size, "SUB      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_SUBI:
            snprintf(buf, buf_size, "SUBI     R%u, %d", inst->dest, inst->imm);
            break;
        case OP_MUL:
            snprintf(buf, buf_size, "MUL      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_MULI:
            snprintf(buf, buf_size, "MULI     R%u, %d", inst->dest, inst->imm);
            break;
        case OP_DIV:
            snprintf(buf, buf_size, "DIV      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_DIVI:
            snprintf(buf, buf_size, "DIVI     R%u, %d", inst->dest, inst->imm);
            break;
        case OP_MOD:
            snprintf(buf, buf_size, "MOD      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_AND:
            snprintf(buf, buf_size, "AND      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_OR:
            snprintf(buf, buf_size, "OR       R%u, R%u", inst->dest, inst->src);
            break;
        case OP_XOR:
            snprintf(buf, buf_size, "XOR      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_NOT:
            snprintf(buf, buf_size, "NOT      R%u", inst->dest);
            break;
        case OP_SHR:
            snprintf(buf, buf_size, "SHR      R%u, %d", inst->dest, inst->imm);
            break;
        case OP_SHL:
            snprintf(buf, buf_size, "SHL      R%u, %d", inst->dest, inst->imm);
            break;
        case OP_CMP:
            snprintf(buf, buf_size, "CMP      R%u, R%u", inst->dest, inst->src);
            break;
        case OP_CMPI:
            snprintf(buf, buf_size, "CMPI     R%u, %d", inst->dest, inst->imm);
            break;
        case OP_JMP:
            snprintf(buf, buf_size, "JMP      0x%04X", inst->imm);
            break;
        case OP_JMPO:
            snprintf(buf, buf_size, "JMPO     %+d (-> 0x%04X)", inst->imm, pc + 1 + inst->imm);
            break;
        case OP_JZ:
            snprintf(buf, buf_size, "JZ       0x%04X", inst->imm);
            break;
        case OP_JNZ:
            snprintf(buf, buf_size, "JNZ      0x%04X", inst->imm);
            break;
        case OP_JLT:
            snprintf(buf, buf_size, "JLT      0x%04X", inst->imm);
            break;
        case OP_JGT:
            snprintf(buf, buf_size, "JGT      0x%04X", inst->imm);
            break;
        case OP_PUSH:
            snprintf(buf, buf_size, "PUSH     R%u", inst->src);
            break;
        case OP_PUSHI:
            snprintf(buf, buf_size, "PUSHI    %d", inst->imm);
            break;
        case OP_POP:
            snprintf(buf, buf_size, "POP      R%u", inst->dest);
            break;
        case OP_CALL:
            snprintf(buf, buf_size, "CALL     0x%04X", inst->imm);
            break;
        case OP_CALLR:
            snprintf(buf, buf_size, "CALLR    %+d", inst->imm);
            break;
        case OP_RET:
            snprintf(buf, buf_size, "RET");
            break;
        case OP_LOAD:
            snprintf(buf, buf_size, "LOAD     R%u, [R%u + %d]", inst->dest, inst->src, inst->imm);
            break;
        case OP_LOAD_PC:
            snprintf(buf, buf_size, "LOAD_PC  R%u, [PC + %d]", inst->dest, inst->imm);
            break;
        case OP_STORE:
            snprintf(buf, buf_size, "STORE    [R%u + %d], R%u", inst->dest, inst->imm, inst->src);
            break;
        case OP_LOADB:
            snprintf(buf, buf_size, "LOADB    R%u, [R%u + %d]", inst->dest, inst->src, inst->imm);
            break;
        case OP_LOADB_PC:
            snprintf(buf, buf_size, "LOADB_PC R%u, [PC + %d]", inst->dest, inst->imm);
            break;
        case OP_STOREB:
            snprintf(buf, buf_size, "STOREB   [R%u + %d], R%u", inst->dest, inst->imm, inst->src);
            break;
        case OP_SYS:
            snprintf(buf, buf_size, "SYS      0x%X", inst->imm);
            break;
        case OP_HALT:
            snprintf(buf, buf_size, "HALT");
            break;
        default:
            snprintf(buf, buf_size, "UNKNOWN  0x%02X", inst->opcode);
            break;
    }
}

void VM_disassemble_stream(const Inst *program, size_t count, FILESTRUCT *out) {
    if (!program) return;

    char inst_str[128];
    for (size_t i = 0; i < count; i++) {
        VM_disassemble_inst(&program[i], (u32)i, inst_str, sizeof(inst_str));

        const u8 *raw = (const u8 *)&program[i];
        PRINTF("%04zu:  %02X %02X %02X %02X %02X %02X %02X %02X  | %s\n",
               i,
               raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7],
               inst_str);
    }
}

#endif // DISASM_IMPLEMENTATION

#endif // DISASM_H
