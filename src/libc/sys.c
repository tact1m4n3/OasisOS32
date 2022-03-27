#include <syscall.h>
#include <oasislib.h>

int sys_fork() {
    return (int)_syscall(SYS_FORK, 0, 0, 0, 0, 0, 0);
}

void sys_exit(int status) {
    _syscall(SYS_EXIT, (void*)status, 0, 0, 0, 0, 0);
}

int sys_execve(char* path, char** argv, char** envp) {
    _syscall(SYS_EXEC, path, argv, envp, 0, 0, 0);
}

int sys_getpid() {
    return (int)_syscall(SYS_GETPID, 0, 0, 0, 0, 0, 0);
}

int sys_brk(void* addr) {
    return (int)_syscall(SYS_BRK, addr, 0, 0, 0, 0, 0);
}

void* sys_sbrk(uint32_t inc) {
    return (void*)_syscall(SYS_SBRK, (void*)inc, 0, 0, 0, 0, 0);
}

void sys_print(char* str) {
    _syscall(SYS_PRINT, str, 0, 0, 0, 0, 0);
}
