#include <syscall.h>

#include <string.h>
#include <vga.h>
#include <cpu.h>
#include <process.h>
#include <vfs.h>

extern process_t* current_process;

static int sys_brk(void* addr) {
    proc_brk(current_process, (uint32_t)addr);
    return 0;
}

static void* sys_sbrk(uint32_t inc) {
    uint32_t brk = current_process->brk;
    if (!inc) {
        return (void*)brk;
    }
    brk += inc;
    proc_brk(current_process, brk);
    return (void*)brk;
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
            break;
        case 8: /* exit */
            break;
        case 9: /* exec */
            break;
        case 10: /* wait */
            break;
        case 11: /* getpid */
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
