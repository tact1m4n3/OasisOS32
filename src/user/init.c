#include <stdint.h>
#include <stddef.h>
#include <syscall.h>

int main() {
    _syscall(SYS_print, "hello from init process\n", 0, 0, 0, 0, 0);

    return 0;
}
