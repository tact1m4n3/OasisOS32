#include <syscall.h>

#include <string.h>
#include <vga.h>
#include <cpu.h>

static void sys_print(char* s) {
    while (*s != '\0') {
        vga_putch(*s++);
    }
}

static regs_t* syscall_callback(regs_t* r) {
    uint32_t id = r->eax;
    switch (id) {
        case 0:
            sys_print((char*)r->ebx);
            return r;
        default:
            return r;
    }
}

void syscall_init() {
    set_int_handler(128, &syscall_callback);
}
