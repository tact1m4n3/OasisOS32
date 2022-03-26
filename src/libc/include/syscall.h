#pragma once

#include <stdint.h>

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_LSEEK 4
#define SYS_STAT 5
#define SYS_FSTAT 6
#define SYS_FORK 7
#define SYS_EXIT 8
#define SYS_EXEC 9
#define SYS_WAIT 10
#define SYS_GETPID 11
#define SYS_SLEEP 12
#define SYS_BRK 13
#define SYS_SBRK 14
#define SYS_PRINT 20

extern uint32_t _syscall(int num, void* a0, void* a1, void* a2, void* a3, void* a4, void* a5);
