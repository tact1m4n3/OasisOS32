#pragma once

#include <stdint.h>

int sys_fork();
void sys_exit(int status);
int sys_execve(char* path, char** argv, char** envp);
int sys_getpid();
int sys_brk(void* addr);
void* sys_sbrk(uint32_t inc);
void sys_print(char* str);
