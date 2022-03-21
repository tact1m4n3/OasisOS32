#pragma once

#include <types.h>
#include <cpu.h>
#include <process.h>

void sched_start();
void sched_add(process_t* proc);
regs_t* schedule(regs_t* r);
