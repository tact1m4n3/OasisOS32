#pragma once

#include <types.h>
#include <cpu.h>
#include <mem.h>

#define PROC_READY 0
#define PROC_RUNNING 1
#define PROC_KILLED 2

typedef struct process {
    uint32_t pid;
    void* stack_ptr;
    uint32_t state;
    int exit_code;

    uint32_t brk;
    page_dir_t* pd;

    struct process* next;
} process_t;

process_t* new_process(void* entry);
void proc_brk(process_t* proc, uint32_t brk);
void yield();
void ready(process_t* proc);
void start_scheduler();
