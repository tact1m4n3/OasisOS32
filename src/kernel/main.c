#include <types.h>
#include <mboot.h>
#include <string.h>
#include <ports.h>
#include <vga.h>
#include <debug.h>
#include <cpu.h>
#include <pit.h>
#include <mem.h>
#include <vfs.h>
#include <devfs.h>
#include <initrd.h>
#include <tarfs.h>
#include <process.h>
#include <elf.h>
#include <syscall.h>

static void parse_mboot(uint32_t mboot_magic, mboot_info_t* mboot_info, uint32_t* initrd_start, uint32_t* initrd_end) {
    if (mboot_magic != MBOOT_MAGIC) {
        PANIC("unknown bootloader\n");
    }

    if (mboot_info->mods_count > 0) {
        mboot_module_t* mods = (mboot_module_t*)mboot_info->mods_addr;

        uint32_t mod_start = mods[0].mod_start;
        uint32_t mod_end = mods[0].mod_end;
        uint32_t mod_size = mod_end - mod_start;

        if (mod_size > INITRD_MEM_SIZE) {
            PANIC("initrd to large\n");
        }
        
        memcpy((void*)INITRD_MEM_START, (void*)mod_start, mod_size);

        *initrd_start = INITRD_MEM_START;
        *initrd_end = INITRD_MEM_START + mod_size;
    } else {
        PANIC("no initrd found\n");
    }
}

void kernel_main(uint32_t mboot_magic, mboot_info_t* mboot_info) {
    vga_init();

    uint32_t initrd_start, initrd_end;
    parse_mboot(mboot_magic, mboot_info, &initrd_start, &initrd_end);

    INFO("kernel loaded successfully\n");

    gdt_init();
    INFO("initialized gdt\n");
    idt_init();
    INFO("initialized idt\n");

    phys_init();
    virt_init();
    heap_init();
    INFO("initialized memory\n");

    pit_init();
    INFO("initialized pit\n");

    _sti();
    INFO("interrupts enabled\n");

    vfs_init();
    devfs_init();
    INFO("initialized vfs\n");

    initrd_init(initrd_start, initrd_end);
    INFO("initialized devices\n");

    syscall_init();
    INFO("initialized syscalls\n");

    tarfs_mount("/initrd", "/dev/initrd");
    INFO("mounted initrd\n");

    process_t* init_process = new_process();
    elf_load(init_process, "/initrd/init");
    ready(init_process);

    INFO("starting scheduler\n");
    start_scheduler();

    PANIC("reached end of kernel...\n");
    for (;;);
}
