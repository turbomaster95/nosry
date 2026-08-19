#include <stdio.h>
#include <string.h>
#include "vm.h"
#include "verify.h"

// System Calls Hook Callback
static void custom_syscalls(VM *vm, Memory *mem, u32 sys_code) {
    switch (sys_code) {
        case 1: // PUTCHAR: Print char stored in R0
            putchar((char)vm->regs[0]);
            break;

        case 2: // PUTS: Print string located at RAM[R0]
            if (vm->regs[0] < RAM_SIZE) {
                printf("%s\n", (char *)&mem->ram[vm->regs[0]]);
            }
            break;

        case 3: // SYS_WRITE_HEX: Print R0 as 32-bit hex
            printf("0x%08X\n", vm->regs[0]);
            break;

        default:
            printf("Fault: Unhandled System Call %u\n", sys_code);
            vm->is_running = 0;
            break;
    }
}

int main(int argc, char **argv) {
    VM vm;
    Memory mem;
    VM_reset(&vm, &mem);

    vm.syscall_handler = custom_syscalls;
    u32 preferred_data_vaddr = 0x20;

    // 1. Binary Input Execution Path
    if (argc > 1 && strcmp(argv[1], "-o") != 0) {
        printf("=== Executing Binary File: %s ===\n", argv[1]);
        if (VM_run_file(argv[1], &mem, &vm, preferred_data_vaddr) == 0) {
            return 0;
        }
        printf("Failed to load binary stream: %s\n", argv[1]);
        return -1;
    }

    const char hello_msg[] = "Hello from Virtual Machine Payload!";

    /* Routine:
     * 1. Output hello_msg via sys_puts (Syscall 2)
     * 2. Execute loop (Sum 1..10 in R1 using OP_SUBI and OP_JNZ)
     * 3. Store result in RAM, reload into R0, and print in hex via sys_write_hex (Syscall 3)
     */
    Inst program[] = {
        // --- Phase 1: Output Message ---
        INST_MOV(0, preferred_data_vaddr),  // R0 = 0x20
        INST_SYS(2),                        // sys_puts(R0)

        // --- Phase 2: Compute Sum ---
        INST_MOV(1, 0),                     // R1 = Accumulator
        INST_MOV(2, 10),                    // R2 = Loop Counter

        // Loop Target PC: 4
        INST_ADD(1, 2),                     // R1 += R2
        INST_SUBI(2, 1),                    // R2 -= 1
        INST_CMPI(2, 0),                    // Compare R2 against 0
        INST_JNZ(4),                        // Jump to PC 4 if R2 != 0

        // --- Phase 3: Store and Output Hex ---
        INST_MOV(3, 0x00),                  // R3 = Address 0x00
        INST_STORE(1, 3, 0),                // RAM[R3] = R1 (55 / 0x37)
        INST_LOAD(0, 3, 0),                 // R0 = RAM[R3]
        INST_SYS(3),                        // sys_write_hex(R0) -> Outputs 0x00000037

        INST_HALT()
    };

    // 2. Binary Export Target Path (-o <file>)
    if (argc > 2 && strcmp(argv[1], "-o") == 0) {
        FILE *f = fopen(argv[2], "wb");
        if (f) {
            VM_export_stream(f, program, COUNTOF(program), 
                               hello_msg, sizeof(hello_msg));
            fclose(f);
            printf("Exported payload binary to %s\n", argv[2]);
        }
        return 0;
    }

    // 3. Bytecode Verification Pass
    printf("=== Pass 1: Static Bytecode Verification ===\n");
    VerifierReport report = VM_verify(program, COUNTOF(program));
    if (!report.is_valid) {
        printf("[VERIFIER ERROR] PC %zu: %s\n", report.invalid_pc, report.reason);
        return -1;
    }
    printf("[VERIFIER SUCCESS] Bytecode stream validated.\n\n");

    // Copy string into payload RAM virtual address
    memcpy(mem.ram + preferred_data_vaddr, hello_msg, sizeof(hello_msg));

    vm.syscall_handler = custom_syscalls; // Re-bind handler
    VM_run(&vm, program, COUNTOF(program), &mem);

    // Dump Register State
    printf("--- Final Register State ---\n");
    for (int i = 0; i < MAX_REGS; i++) {
        printf("R%-2d: 0x%08X (%u)\n", i, vm.regs[i], vm.regs[i]);
    }

    return 0;
}
