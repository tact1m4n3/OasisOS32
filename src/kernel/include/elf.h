#pragma once

#include <types.h>
#include <process.h>

int elf_load(process_t* proc, char* path, int argc, char** argv, char** envp);
