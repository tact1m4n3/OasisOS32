#include <process.h>

#include <string.h>
#include <debug.h>

typedef struct swtch_stack {
    uint32_t ebp;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp2;
    uint32_t ret;
    regs_t r;
} swtch_stack_t;

typedef struct process_queue {
    process_t* front;
    process_t* back;
} process_queue_t;

extern void int_return();
extern void switch_stack(void* old_ptr, void* new_ptr);

uint32_t next_pid = 1;
process_t* scheduler;
process_t* current_process;
process_queue_t ready_queue;
process_queue_t retire_queue;

static void queue_push(process_queue_t* queue, process_t* proc) {
    if (!queue->front) {
        queue->front = queue->back = proc;
    } else {
        queue->back->next = proc;
        queue->back = proc;
    }
}

static process_t* queue_pop(process_queue_t* queue) {
    if (!queue->front) {
        return NULL;
    } else {
        process_t* proc = queue->front;
        queue->front = proc->next;
        proc->next = NULL;
        if (queue->front == NULL) {
            queue->front = queue->back = NULL;
        }
        return proc;
    }
}

static void scheduler_loop() {
    while (1) {
        process_t* next;
        while (!(next = queue_pop(&ready_queue)));

        current_process = next;
        switch_page_dir(next->pd);
        set_tss_stack((void*)next + 4096);
        switch_stack(&scheduler->stack_ptr, &next->stack_ptr);

        if (current_process->state != PROC_KILLED) {
            current_process->state = PROC_READY;
            queue_push(&ready_queue, current_process);
        } else {
            INFO("process %x exited with code %x\n", current_process->pid, current_process->exit_code);
            queue_push(&retire_queue, current_process);
        }
        current_process = NULL;
    }
}

process_t* new_process(process_t* parent) {
    process_t* proc = (process_t*)malloc(4096);
    memset(proc, 0, 4096);
    proc->pid = next_pid++;
    proc->stack_ptr = (void*)proc + 4096;
    proc->brk = 0xA0000000;
    proc->pd = (page_dir_t*)malloc_page(sizeof(page_dir_t));
    memcpy(proc->pd, kernel_pd, sizeof(page_dir_t));
    
    for (uint32_t j = 0; j < PROC_STACK_SIZE; j += PAGE_SIZE) {
        if (NO_MEMORY) {
            ERROR("new_process: no memory left\n");
            return NULL;
        }
        map_page(proc->pd, 0xFFFFF000 - j, alloc_frame(), PAGE_WRITE | PAGE_USER);
    }

    proc_init_stack(proc, (void*)0xA0000000, (void*)0xFFFFFFFF);

    return proc;
}

void* proc_init_stack(process_t* proc, void* entry, void* stack) {
    proc->stack_ptr = (void*)proc + 4096 - sizeof(swtch_stack_t);
    swtch_stack_t* stk = (swtch_stack_t*)proc->stack_ptr;
    stk->ebp = (uint32_t)&stk->ebp2;
    stk->ret = (uint32_t)&int_return;
    stk->r.eip = (uint32_t)entry;
    stk->r.cs = 0x1B;
    stk->r.eflags = 0x206;
    stk->r.esp = (uint32_t)stack;
    stk->r.ds = stk->r.es = stk->r.fs = stk->r.gs = stk->r.ss = 0x23;
    return (void*)&stk->r;
}

void yield() {
    if (current_process) {
        switch_stack(&current_process->stack_ptr, &scheduler->stack_ptr);
    }
}

void ready(process_t* proc) {
    proc->state = PROC_READY;
    queue_push(&ready_queue, proc);
}

void start_scheduler() {
    scheduler = (process_t*)malloc(4096);
    memset(scheduler, 0, 4096);
    scheduler->pid = -1;
    scheduler->stack_ptr = (void*)scheduler + 4096 - sizeof(swtch_stack_t);
    scheduler->pd = kernel_pd;

    swtch_stack_t* stk = (swtch_stack_t*)scheduler->stack_ptr;
    stk->ebp = (uint32_t)&stk->ebp2;
    stk->ret = (uint32_t)&scheduler_loop;

    uint32_t stack_ptr;
    switch_stack(&stack_ptr, &scheduler->stack_ptr);
}
