#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <syscall.h>
#include <oasislib.h>

int main() {
    sys_print("hello world!\n");

    return 0;
}
