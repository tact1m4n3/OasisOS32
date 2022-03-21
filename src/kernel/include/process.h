#pragma once

#include <types.h>
#include <cpu.h>
#include <mem.h>

#define PROC_READY 0
#define PROC_RUNNING 1
#define PROC_KILLED 2

typedef struct process {
    uint32_t pid;
    uint32_t state;

    page_dir_t* pd;

    void* brk;
    void* stack;

    struct process* next;
} process_t;

process_t* new_process(void* entry);
void proc_brk(process_t* proc, void* brk);
