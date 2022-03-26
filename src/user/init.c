#include <stdint.h>
#include <stddef.h>
#include <syscall.h>

int sys_brk(void* addr) {
    return (int)_syscall(SYS_BRK, addr, 0, 0, 0, 0, 0);
}

void* sys_sbrk(uint32_t inc) {
    return (void*)_syscall(SYS_SBRK, (void*)inc, 0, 0, 0, 0, 0);
}

void sys_print(char* str) {
    _syscall(SYS_PRINT, str, 0, 0, 0, 0, 0);
}

int main() {
    sys_print("hello from init process\n");

    return 0;
}
