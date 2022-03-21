#include <mem.h>

#include <string.h>
#include <debug.h>
#include <cpu.h>

#define CLEAR_FLAGS(x) (x & ~0xFFF)
#define GET_FLAG(x, y) (x & y)

#define PD_OFFSET(x) (x >> 22)
#define PT_OFFSET(x) ((x >> 12) & 0x3FF)

#define PDE (pd->tables[PD_OFFSET(addr)])
#define PDE_REF (pd->ref_tables[PD_OFFSET(addr)])
#define PTE (PDE_REF->pages[PT_OFFSET(addr)])

extern void* end;
extern uint8_t is_heap;

uint8_t is_virt;
uint32_t tmp_mem;
page_dir_t* kernel_pd;

static void* initial_malloc(uint32_t size, int align) {
    if (align && (tmp_mem & ~0xFFFFF000)) {
        tmp_mem &= 0xFFFFF000;
        tmp_mem += PAGE_SIZE;
    }
    
    uint32_t addr = tmp_mem;
    tmp_mem += size;
    
    return (void*)addr;
}

static void enable_paging() {
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

static int is_page(page_dir_t* pd, uint32_t addr) {
    if (pd && GET_FLAG(PDE, PAGE_PRESENT) && GET_FLAG(PTE, PAGE_PRESENT)) {
        return 1;
    } else {
        return 0;
    }
}

void virt_init() {
    tmp_mem = (uint32_t)&end;

    kernel_pd = (page_dir_t*)initial_malloc(sizeof(page_dir_t), 1);
    memset(kernel_pd, 0, sizeof(page_dir_t));

    uint32_t i = KERNEL_MEM_START;
    while (i < KERNEL_MEM_START + KERNEL_MEM_SIZE) {
        map_page(kernel_pd, i, alloc_frame(), PAGE_WRITE | PAGE_USER);
        i += PAGE_SIZE;
    }
    
    i = HEAP_MEM_START;
    while (i < HEAP_MEM_START + HEAP_MEM_SIZE) {
        map_page(kernel_pd, i, alloc_frame(), PAGE_WRITE | PAGE_USER);
        i += PAGE_SIZE;
    }

    i = INITRD_MEM_START;
    while (i < INITRD_MEM_START + INITRD_MEM_SIZE) {
        map_page(kernel_pd, i, alloc_frame(), PAGE_WRITE | PAGE_USER);
        i += PAGE_SIZE;
    }

    switch_page_dir(kernel_pd);
    enable_paging();

    is_virt = 1;
}

uint32_t virt2phys(page_dir_t* pd, uint32_t addr) {
    if (!is_virt) {
        return addr;
    }

    if (!is_page(pd, addr)) {
        WARN("virt2phys: address %x not mapped\n", addr);
        return 0;
    }
    
    return CLEAR_FLAGS(PTE);
}

void switch_page_dir(page_dir_t* pd) {
    asm volatile("mov %0, %%cr3" : : "r"(virt2phys(kernel_pd, (uint32_t)pd)));
}

int map_page(page_dir_t* pd, uint32_t addr, uint32_t frame, uint16_t flags) {
    if (frame == -1) {
        WARN("map_page: invalid frame\n");
        return 0;
    }

    if (!GET_FLAG(PDE, PAGE_PRESENT)) {
        page_table_t* pt;
        if (is_heap) {
            pt = (page_table_t*)malloc_page(sizeof(page_table_t));
        } else {
            pt = (page_table_t*)initial_malloc(sizeof(page_table_t), 1);
        }

        if (!pt) {
            ERROR("map_page: failed to allocate page table\n");
            return 0;
        }
        
        memset(pt, 0, sizeof(page_table_t));

        PDE_REF = pt;

        PDE = virt2phys(kernel_pd, (uint32_t)pt);
        PDE |= PAGE_PRESENT;
        PDE |= flags;
    }
    
    PTE = frame;
    PTE |= PAGE_PRESENT;
    PTE |= flags;

    return 1;
}
