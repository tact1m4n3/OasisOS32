#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <syscall.h>
#include <oasislib.h>

int main() {
    int pid = sys_fork();
    if (pid == 0) {
        sys_print("hello from child process\n");
        char* argv[2] = { "/initrd/hello", NULL };
        char* envp[1] = { NULL };
        if (sys_execve("/initrd/hello", argv, envp) == -1) {
            sys_print("failed to execute hello program\n");
        }
    } else {
        sys_print("hello from init process\n");
    }

    return 0;
}
