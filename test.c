#include <stdio.h>
#include "vm.h"
#include "verify.h"

int main(void) {
    Inst invalid_program[] = {
        INST_MOV(0, 100),

        { .opcode = OP_MOV, .dest = 255, .src = 0, .imm = 42 },

        INST_DIVI(1, 0),

        INST_JMP(9999),

        { .opcode = 0xFE, .dest = 0, .src = 0, .imm = 0 },

        INST_HALT()
    };

    printf("=== Testing Static Verifier Guardrails ===\n");
    
    VerifierReport report = VM_verify(invalid_program, COUNTOF(invalid_program));

    if (!report.is_valid) {
        printf("[VERIFIER BLOCKED EXECUTION]\n");
        printf("Failed at Instruction Index (PC) : %zu\n", report.invalid_pc);
        printf("Reason                           : %s\n", report.reason);
    } else {
        printf("[FAIL] Invalid program bypassed verification unexpectedly!\n");
    }

    return 0;
}
