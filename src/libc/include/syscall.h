#pragma once

#include <stdint.h>

#define SYS_print 0

extern uint32_t _syscall(int id, void* a0, void* a1, void* a2, void* a3, void* a4, void* a5);
