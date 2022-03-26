#include <pit.h>

#include <ports.h>
#include <cpu.h>
#include <process.h>

#define PIT_DATA 0x40
#define PIT_CMD 0x43

uint32_t pit_ticks;

static void pit_callback(regs_t* r) {
    pit_ticks++;
    yield();
}

void pit_init() {
    pit_set_freq(100);
    set_int_handler(32, &pit_callback);
}

void pit_set_freq(uint16_t freq) {
    freq = 1193180 / freq;
    outb(PIT_DATA, freq);
    outb(PIT_DATA, (freq >> 8) & 0xFF);
}

void pit_wait(uint32_t ticks) {
    uint32_t start_ticks = pit_ticks;
    while (pit_ticks != start_ticks + ticks);
}
