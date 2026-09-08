#include <stdio.h>
#include <fcntl.h>
#include "vm.h"

#define DISASM_IMPLEMENTATION
#include "disasm.h"

int main(void) {
    Memory mem;
    VM vm;
    VM_reset(&vm, &mem);

    int str_id = VM_register_str(&vm, "Hello, World!\n");

    Inst program[] = {
        INST_MOV(0, str_id),
        INST_SYS(2),
        INST_HALT()
    };

    VM_run(&vm, program, COUNTOF(program), &mem);

    const char *out_filename = "program.vmo";
    FILE *f = fopen(out_filename, "wb");
    if (f) {
        VM_export_stream(&vm, f, program, COUNTOF(program), NULL, 0);
        fclose(f);
        printf("Exported payload binary to %s\n\n", out_filename);
    }

    FILE *f_in = fopen(out_filename, "rb");
    if (f_in) {
        VM read_vm;
        Memory read_mem;
        VM_reset(&read_vm, &read_mem);

        size_t read_prog_len = 0;
        if (VM_import_stream(&read_vm, f_in, &read_mem, &read_prog_len, 0) == 0) {
            printf("=== Disassembly of %s (%zu instructions) ===\n", out_filename, read_prog_len);
            VM_disassemble_stream(read_mem.rom, read_prog_len, stdout);
        } else {
            printf("Failed to import %s for disassembly.\n", out_filename);
        }
        fclose(f_in);
    }

    return 0;
}
