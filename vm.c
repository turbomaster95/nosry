#include "vm.h"

static void custom_syscalls(VM *vm, Memory *mem, u32 sys_code) {
    switch (sys_code) {
        case 1: // PUTCHAR
            putchar((char)vm->regs[0]);
            break;

        case 2: // PUTS
            if (vm->regs[0] < RAM_SIZE) {
                printf("%s", (char *)&mem->ram[vm->regs[0]]);
            }
            break;

        case 3: // Custom SYS_WRITE_HEX (Prints R0 as Hexadecimal)
            printf("0x%08X\n", vm->regs[0]);
            break;

        default:
            printf("Fault: Unhandled Extended Syscall %u\n", sys_code);
            vm->is_running = 0;
            break;
    }
}

int main(int argc, char **argv) {
    VM vm;
    Memory mem;
    VM_reset(&vm, &mem);

    vm.syscall_handler = custom_syscalls;

    u32 preferred_data_vaddr = 0x10;

    if (argc > 1 && strcmp(argv[1], "-o") != 0) {
        if (VM_run_file(argv[1], &mem, &vm, preferred_data_vaddr) == 0) {
            return 0;
        }
        printf("Failed to load binary file: %s\n", argv[1]);
        return -1;
    }

    const char data_section[] = "Hello VM";

    Inst program[] = {
        INST_MOV(0, preferred_data_vaddr),  // R0 points to 0x10
        INST_LOAD(1, 0, 0),                 // Load 4 bytes from RAM[0x10] into R1
        INST_MOV(2, 42),
        INST_HALT()
    };

    if (argc > 2 && strcmp(argv[1], "-o") == 0) {
        FILE *f = fopen(argv[2], "wb");
        if (f) {
            VM_export_stream(f, program, COUNTOF(program), 
                               data_section, sizeof(data_section));
            fclose(f);
            printf("Successfully exported %s\n", argv[2]);
        }
        return 0;
    }

    memcpy(mem.ram + preferred_data_vaddr, data_section, sizeof(data_section));
    VM_run(&vm, program, COUNTOF(program), &mem);
    return 0;
}
