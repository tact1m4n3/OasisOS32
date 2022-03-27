#pragma once

#include <types.h>
#include <cpu.h>
#include <mem.h>

#define PROC_READY 0x1
#define PROC_RUNNING 0x2
#define PROC_KILLED 0x3
#define PROC_RETIRED 0x4

#define PROC_STACK_SIZE (12 * K)

typedef struct process {
    uint32_t pid;
    void* stack_ptr;
    uint32_t state;
    int exit_code;

    uint32_t brk;
    page_dir_t* pd;

    struct process* next;
} process_t;

process_t* new_process();
void* proc_init_stack(process_t* proc, void* entry, void* stack);
void yield();
void ready(process_t* proc);
void start_scheduler();
