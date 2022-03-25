#pragma once

#include <stdint.h>

#define SYS_PRINT 0

extern uint32_t _syscall(int num, void* a0, void* a1, void* a2, void* a3, void* a4, void* a5);

void print(char* str);
