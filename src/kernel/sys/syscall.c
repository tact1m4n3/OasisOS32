#include <syscall.h>

#include <string.h>
#include <vga.h>
#include <debug.h>
#include <cpu.h>
#include <mem.h>
#include <vfs.h>
#include <process.h>
#include <elf.h>

extern process_t* current_process;

static int sys_fork(regs_t* r) {
    process_t* clone = new_process();

    regs_t* clone_regs = (regs_t*)proc_init_stack(clone, NULL, NULL);
    memcpy(clone_regs, r, sizeof(regs_t));
    clone_regs->eax = 0;

    uint8_t* buffer = (uint8_t*)malloc(PAGE_SIZE);
    
    uint32_t i = 0xA0000000;
    while (is_page(current_process->pd, i)) {
        if (NO_MEMORY) {
            ERROR("sys_fork: no memory left\n");
            return -1;
        }
        map_page(clone->pd, i, alloc_frame(), PAGE_WRITE | PAGE_USER);

        memcpy(buffer, (void*)i, PAGE_SIZE);
        switch_page_dir(clone->pd);
        memcpy((void*)i, buffer, PAGE_SIZE);
        switch_page_dir(current_process->pd);

        i += PAGE_SIZE;
    }
    clone->brk = i;

    i = 0xFFFFF000;
    while (is_page(current_process->pd, i)) {
        if (NO_MEMORY) {
            ERROR("sys_fork: no memory left\n");
            return -1;
        }
        map_page(clone->pd, i, alloc_frame(), PAGE_WRITE | PAGE_USER);

        memcpy(buffer, (void*)i, PAGE_SIZE);
        switch_page_dir(clone->pd);
        memcpy((void*)i, buffer, PAGE_SIZE);
        switch_page_dir(current_process->pd);

        i -= PAGE_SIZE;
    }

    ready(clone);

    return clone->pid;
}

static void sys_exit(int status) {
    current_process->exit_code = status;
    current_process->state = PROC_KILLED;
    yield();
}

static int sys_execve(char* path, char** argv, char** envp) {
    uint32_t argc = 0;
    uint32_t envc = 0;
    while (argv[argc]) { argc++; }
    if (envp) { while (envp[envc]) { envc++; } }

    char* argv_clone[argc + 1];
    for (int i = 0; i < argc; i++) {
        argv_clone[i] = strdup(argv[i]);
    }
    argv_clone[argc] = NULL;

    char* envp_clone[envc + 1];
    if (envc && envp) {
        for (int i = 0; i < envc; i++) {
            envp_clone[i] = strdup(envp[i]);
        }
    }
    envp_clone[envc] = NULL;
    
    if (!elf_load(current_process, path, argc, argv, envp)) {
        ERROR("sys_execve: failed to load executable\n");
        for (uint32_t i = 0; i < argc; i++) { free(argv_clone[i]); }
        for (uint32_t i = 0; i < envc; i++) { free(envp_clone[i]); }
        return -1;
    }

    for (uint32_t i = 0; i < argc; i++) { free(argv_clone[i]); }
    for (uint32_t i = 0; i < envc; i++) { free(envp_clone[i]); }
    
    yield();
    return -1;
}

static int sys_getpid() {
    return (int)current_process->pid;
}

static int sys_brk(void* addr) {
    while (current_process->brk < (uint32_t)addr) {
        if (NO_MEMORY) {
            ERROR("sys_brk: no memory left\n");
            return -1;
        }
        map_page(current_process->pd, current_process->brk, alloc_frame(), PAGE_WRITE | PAGE_USER);
        current_process->brk += PAGE_SIZE;
    }
    return 0;
}

static void* sys_sbrk(uint32_t inc) {
    if (inc == 0) { return (void*)-1; }
    uint32_t new_brk = current_process->brk + inc;
    while (current_process->brk < new_brk) {
        if (NO_MEMORY) {
            ERROR("sys_sbrk: no memory left\n");
            return (void*)-1;
        }
        map_page(current_process->pd, current_process->brk, alloc_frame(), PAGE_WRITE | PAGE_USER);
        current_process->brk += PAGE_SIZE;
    }
    return (void*)new_brk;
}

static void sys_print(char* str) {
    while (*str != '\0') {
        vga_putch(*str++);
    }
}

static void syscall_callback(regs_t* r) {
    uint32_t id = r->eax;
    switch (id) {
        case 0: /* read */
            break;
        case 1: /* write */
            break;
        case 2: /* open */
            break;
        case 3: /* close */
            break;
        case 4: /* lseek */
            break;
        case 5: /* stat */
            break;
        case 6: /* fstat */
            break;
        case 7: /* fork */
            r->eax = sys_fork(r);
            break;
        case 8: /* exit */
            sys_exit((int)r->ebx);
            break;
        case 9: /* exec */
            r->eax = sys_execve((char*)r->ebx, (char**)r->ecx, (char**)r->edx);
            break;
        case 10: /* wait */
            break;
        case 11: /* getpid */
            r->eax = sys_getpid();
            break;
        case 12: /* sleep */
            break;
        case 13: /* brk */
            r->eax = sys_brk((void*)r->ebx);
            break;
        case 14: /* sbrk */
            r->eax = (uint32_t)sys_sbrk(r->ebx);
            break;
        case 20:
            sys_print((char*)r->ebx);
            break;
        default:
            break;
    }
}

void syscall_init() {
    set_int_handler(128, &syscall_callback);
}
