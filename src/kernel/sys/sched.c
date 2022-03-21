#include <sched.h>

#include <string.h>
#include <debug.h>
#include <mem.h>

typedef struct process_queue {
    process_t* front;
    process_t* back;
} process_queue_t;

uint8_t is_sched;
uint8_t sched_running;
process_t* current_process;
process_queue_t* ready_queue;

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

void sched_init() {
    ready_queue = (process_queue_t*)malloc(sizeof(process_queue_t));
    memset(ready_queue, 0, sizeof(process_queue_t));

    is_sched = 1;
}

void sched_start() {
    sched_running = 1;
    
    for (;;);
}

void sched_ready(process_t* proc) {
    if (!is_sched) {
        WARN("sched_ready: scheduler not initialized\n");
        return;
    }
    proc->state = PROC_READY;
    queue_push(ready_queue, proc);
}

regs_t* schedule(regs_t* r) {
    if (!is_sched || !sched_running) {
        return r;
    }

    if (current_process) {
        current_process->stack = r;
        if (current_process->state != PROC_KILLED) {
            current_process->state = PROC_READY;
            queue_push(ready_queue, current_process);
        }
    }

    process_t* next_process = queue_pop(ready_queue);
    if (!next_process) {
        PANIC("no process running...");
    }

    next_process->state = PROC_RUNNING;
    current_process = next_process;

    switch_page_dir(next_process->pd);
    set_tss_stack((void*)next_process + 4096);

    return next_process->stack;
}
