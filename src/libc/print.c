#include <print.h>

void print(char* str) {
    asm volatile("int $0x80" : : "a"(0), "b"(str));
}
