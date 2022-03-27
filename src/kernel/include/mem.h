#pragma once

#include <types.h>

#define K 1024
#define M (1024 * K)

#define PAGE_SIZE (4 * K)

#define KERNEL_MEM_START 0x0
#define KERNEL_MEM_SIZE (4 * M)
#define HEAP_MEM_START 0x400000
#define HEAP_MEM_SIZE (4 * M)
#define INITRD_MEM_START 0x800000
#define INITRD_MEM_SIZE (2 * M)

#define IS_MEMORY (mem_left != 0)
#define NO_MEMORY (mem_left == 0)

#define MEMORY_SIZE (16 * M)

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

typedef struct page_table {
    uint32_t pages[1024];
} page_table_t;

typedef struct page_dir {
    uint32_t tables[1024];
    
    page_table_t* ref_tables[1024];
} page_dir_t;

extern uint32_t mem_left;
extern page_dir_t* kernel_pd;

void phys_init();
uint32_t alloc_frame();
void free_frame(uint32_t frame);

void virt_init();
uint32_t virt2phys(page_dir_t* pd, uint32_t addr);
void switch_page_dir(page_dir_t* pd);
int is_page(page_dir_t* pd, uint32_t addr);
int map_page(page_dir_t* pd, uint32_t addr, uint32_t frame, uint16_t flags);

void heap_init();
void* malloc(uint32_t size);
void free(void* ptr);
void* malloc_page(uint32_t size);
void free_page(void* ptr);
