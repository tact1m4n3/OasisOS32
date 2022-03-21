#include <ports.h>

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void outw(uint16_t port, uint16_t data) {
    asm volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t ind(uint16_t port) {
    uint32_t result;
    asm volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void outd(uint16_t port, uint32_t data) {
    asm volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}
