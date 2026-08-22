#include "vm.h"
#include <fcntl.h>

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
    FILE* f = fopen("program.vmo", "wb");
    if (f) {
        VM_export_stream(&vm, f, program, COUNTOF(program), NULL, 0);
        fclose(f);
    }

    return 0;
}
