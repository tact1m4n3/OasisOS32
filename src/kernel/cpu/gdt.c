#include <cpu.h>

typedef struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

typedef struct tss {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap;
} __attribute__((packed)) tss_t;

extern void load_gdt(void* ptr);

gdt_entry_t gdt[6];
gdt_ptr_t gdt_ptr;
tss_t tss;

static void set_gdt_entry(uint8_t id, uint32_t base, uint32_t limit, uint8_t type, uint8_t gran) {
    gdt[id].limit_low = (limit & 0xFFFF);
    gdt[id].base_low = (base & 0xFFFF);
    gdt[id].base_mid = (base >> 16) & 0xFF;
    gdt[id].access = type;
    gdt[id].granularity = (limit >> 16) & 0x0F;
    gdt[id].granularity |= (gran & 0xF0);
    gdt[id].base_high = (base >> 24) & 0xFF;
}

static void construct_tss(uint8_t id, uint32_t ss0, uint32_t esp0)
{
    uint32_t tss_base = (uint32_t)&tss;
    uint32_t tss_limit = tss_base + sizeof(tss_t);
    
    set_gdt_entry(id, tss_base, tss_limit, 0xE9, 0x00);
    
    tss.esp0 = esp0;
    tss.ss0 = ss0;

    tss.cs = 0x0B;
    tss.ds = tss.es = tss.fs = tss.gs = tss.ss = 0x13;

    tss.iomap = sizeof(tss_t);
}

void gdt_init() {
    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    construct_tss(5, 0x10, 0);

    gdt_ptr.limit = 6 * sizeof(gdt_entry_t) - 1;
    gdt_ptr.base = (uint32_t)&gdt;

    load_gdt(&gdt_ptr);
}

void set_tss_stack(void* esp0) {
    tss.esp0 = (uint32_t)esp0;
}
