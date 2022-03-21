#include <sched.h>

#include <debug.h>
#include <mem.h>

uint8_t is_sched;
process_t* current_process;
struct {
    process_t* first;
    process_t* last;
} process_list;

void sched_start() {
    is_sched = 1;
    
    for (;;);
}

void sched_add(process_t* proc) {
    proc->state = PROC_READY;

    if (!process_list.first) {
        process_list.first = proc;
        process_list.last = proc;
        return;
    }

    process_list.last->next = proc;
    process_list.last = proc;
}

regs_t* schedule(regs_t* r) {
    if (!is_sched) {
        return r;
    }

    if (current_process) {
        current_process->state = PROC_READY;
        current_process->stack = r;
    }

    process_t* next_process;
    if (!current_process) {
        next_process = process_list.first;
    } else if (!current_process->next) {
        next_process = process_list.first;
    } else {
        next_process = current_process->next;
    }

    if (!next_process) {
        PANIC("no process running...");
    }

    next_process->state = PROC_RUNNING;
    current_process = next_process;

    switch_page_dir(next_process->pd);
    set_tss_stack((void*)next_process + 4096);

    return next_process->stack;
}
