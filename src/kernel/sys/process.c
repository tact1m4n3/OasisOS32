#include <process.h>

#include <string.h>
#include <debug.h>

extern process_t* current_process;

uint32_t next_pid = 1;

regs_t* pagefault_callback(regs_t* r) {
    uint32_t fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    if (!(r->err_code & 0x4)) {
        ERROR("kernel page fault at %x\n", fault_addr);
    }

    process_t* proc = current_process;
    if ((uint32_t)proc->stack - PAGE_SIZE > fault_addr)

    return r;
}

process_t* new_process(void* entry) {
    if (next_pid == 1) {
        set_int_handler(14, &pagefault_callback);
    }

    process_t* proc = (process_t*)malloc(4096);
    memset(proc, 0, 4096);

    proc->pid = next_pid++;

    proc->pd = (page_dir_t*)malloc_page(sizeof(page_dir_t));
    memcpy(proc->pd, kernel_pd, sizeof(page_dir_t));
    map_page(proc->pd, 0xFFFFF000, alloc_frame(), PAGE_WRITE | PAGE_USER);

    proc->brk = (void*)USER_MEM_START;

    proc->stack = (void*)proc + 4096 - sizeof(regs_t);

    regs_t* stk = (regs_t*)proc->stack;
    stk->eip = (uint32_t)entry;
    stk->cs = 0x1B;
    stk->eflags = 0x206;
    stk->esp = 0xFFFFFFFF;
    stk->ss = stk->ds = stk->es = stk->fs = stk->gs = 0x23;

    return proc;
}

void proc_brk(process_t* proc, void* brk) {
    while (proc->brk < brk) {
        map_page(proc->pd, (uint32_t)proc->brk, alloc_frame(), PAGE_WRITE | PAGE_USER);
        proc->brk += PAGE_SIZE;
    }
}
