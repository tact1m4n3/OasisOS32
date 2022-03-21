#pragma once

#include <types.h>

typedef struct regs {
    uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, _esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, esp, ss;
} regs_t;

extern void cli();
extern void sti();

void gdt_init();
void set_tss_stack(void* esp0);

void idt_init();
void set_int_handler(uint8_t id, void* handler);
