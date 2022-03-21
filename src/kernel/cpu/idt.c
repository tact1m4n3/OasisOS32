#include <cpu.h>

#include <string.h>
#include <debug.h>
#include <pic.h>

typedef struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

extern void load_idt(void* ptr);

#include "int.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;
void* int_handlers[256];

static void set_idt_entry(uint8_t id, uint32_t base, uint16_t selector, uint16_t flags) {
    idt[id].base_low = base & 0xFFFF;
    idt[id].selector = selector;
    idt[id].reserved = 0;
    idt[id].flags = flags | 0x60;
    idt[id].base_high = (base >> 16) & 0xFFFF;
}

void idt_init() {
    idt_ptr.limit = 256 * sizeof(idt_entry_t) - 1;
    idt_ptr.base = (uint32_t)&idt;

    set_idt_entry(0, (uint32_t)&_isr0, 0x08, 0x8E);
    set_idt_entry(1, (uint32_t)&_isr1, 0x08, 0x8E);
    set_idt_entry(2, (uint32_t)&_isr2, 0x08, 0x8E);
    set_idt_entry(3, (uint32_t)&_isr3, 0x08, 0x8E);
    set_idt_entry(4, (uint32_t)&_isr4, 0x08, 0x8E);
    set_idt_entry(5, (uint32_t)&_isr5, 0x08, 0x8E);
    set_idt_entry(6, (uint32_t)&_isr6, 0x08, 0x8E);
    set_idt_entry(7, (uint32_t)&_isr7, 0x08, 0x8E);
    set_idt_entry(8, (uint32_t)&_isr8, 0x08, 0x8E);
    set_idt_entry(9, (uint32_t)&_isr9, 0x08, 0x8E);
    set_idt_entry(10, (uint32_t)&_isr10, 0x08, 0x8E);
    set_idt_entry(11, (uint32_t)&_isr11, 0x08, 0x8E);
    set_idt_entry(12, (uint32_t)&_isr12, 0x08, 0x8E);
    set_idt_entry(13, (uint32_t)&_isr13, 0x08, 0x8E);
    set_idt_entry(14, (uint32_t)&_isr14, 0x08, 0x8E);
    set_idt_entry(15, (uint32_t)&_isr15, 0x08, 0x8E);
    set_idt_entry(16, (uint32_t)&_isr16, 0x08, 0x8E);
    set_idt_entry(17, (uint32_t)&_isr17, 0x08, 0x8E);
    set_idt_entry(18, (uint32_t)&_isr18, 0x08, 0x8E);
    set_idt_entry(19, (uint32_t)&_isr19, 0x08, 0x8E);
    set_idt_entry(20, (uint32_t)&_isr20, 0x08, 0x8E);
    set_idt_entry(21, (uint32_t)&_isr21, 0x08, 0x8E);
    set_idt_entry(22, (uint32_t)&_isr22, 0x08, 0x8E);
    set_idt_entry(23, (uint32_t)&_isr23, 0x08, 0x8E);
    set_idt_entry(24, (uint32_t)&_isr24, 0x08, 0x8E);
    set_idt_entry(25, (uint32_t)&_isr25, 0x08, 0x8E);
    set_idt_entry(26, (uint32_t)&_isr26, 0x08, 0x8E);
    set_idt_entry(27, (uint32_t)&_isr27, 0x08, 0x8E);
    set_idt_entry(28, (uint32_t)&_isr28, 0x08, 0x8E);
    set_idt_entry(29, (uint32_t)&_isr29, 0x08, 0x8E);
    set_idt_entry(30, (uint32_t)&_isr30, 0x08, 0x8E);
    set_idt_entry(31, (uint32_t)&_isr31, 0x08, 0x8E);
    set_idt_entry(128, (uint32_t)&_isr128, 0x08, 0x8E);
    
    set_idt_entry(32 + 0, (uint32_t)&_irq0, 0x08, 0x8E);
    set_idt_entry(32 + 1, (uint32_t)&_irq1, 0x08, 0x8E);
    set_idt_entry(32 + 2, (uint32_t)&_irq2, 0x08, 0x8E);
    set_idt_entry(32 + 3, (uint32_t)&_irq3, 0x08, 0x8E);
    set_idt_entry(32 + 4, (uint32_t)&_irq4, 0x08, 0x8E);
    set_idt_entry(32 + 5, (uint32_t)&_irq5, 0x08, 0x8E);
    set_idt_entry(32 + 6, (uint32_t)&_irq6, 0x08, 0x8E);
    set_idt_entry(32 + 7, (uint32_t)&_irq7, 0x08, 0x8E);
    set_idt_entry(32 + 8, (uint32_t)&_irq8, 0x08, 0x8E);
    set_idt_entry(32 + 9, (uint32_t)&_irq9, 0x08, 0x8E);
    set_idt_entry(32 + 10, (uint32_t)&_irq10, 0x08, 0x8E);
    set_idt_entry(32 + 11, (uint32_t)&_irq11, 0x08, 0x8E);
    set_idt_entry(32 + 12, (uint32_t)&_irq12, 0x08, 0x8E);
    set_idt_entry(32 + 13, (uint32_t)&_irq13, 0x08, 0x8E);
    set_idt_entry(32 + 14, (uint32_t)&_irq14, 0x08, 0x8E);
    set_idt_entry(32 + 15, (uint32_t)&_irq15, 0x08, 0x8E);

    pic_remap();

    load_idt(&idt_ptr);
}

void set_int_handler(uint8_t id, void* handler) {
    int_handlers[id] = handler;
}

regs_t* int_handler(regs_t* r) {
    regs_t* (*handler)(regs_t* r);
    handler = int_handlers[r->int_no];
    if (handler) {
        r = handler(r);
    } else if (r->int_no < 32) {
        PANIC("unhandled exception %x\n", r->int_no);
    } else {
        WARN("unhandled interrupt %x\n", r->int_no);
    }

    pic_ack(r->int_no);

    return r;
}
