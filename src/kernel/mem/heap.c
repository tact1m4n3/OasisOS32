#include <mem.h>

#include <debug.h>

typedef struct memory_chunk {
    uint8_t free;
    uint32_t size;
    
    struct memory_chunk* next;
} memory_chunk_t;

uint8_t is_heap = 0;
memory_chunk_t* first_chunk;

static memory_chunk_t* first_free_chunk(uint32_t size) {
    memory_chunk_t* chunk = first_chunk;
    while (chunk) {
        if (chunk->free && chunk->size >= size) {
            return chunk;
        }
        chunk = chunk->next;
    }
    return NULL;
}

static void group_free_chunks() {
    memory_chunk_t* last_chunk = first_chunk;
    memory_chunk_t* current_chunk = first_chunk->next;

    while (current_chunk) {
        if (last_chunk->free && current_chunk->free) {
            last_chunk->next = current_chunk->next;
            last_chunk->size += current_chunk->size + sizeof(memory_chunk_t);
        } else {
            last_chunk = current_chunk;
        }
        current_chunk = current_chunk->next;
    }
}

void heap_init() {
    first_chunk = (memory_chunk_t*)HEAP_MEM_START;
    first_chunk->next = NULL;
    first_chunk->free = 1;
    first_chunk->size = HEAP_MEM_SIZE - sizeof(memory_chunk_t);

    is_heap = 1;
}

void* malloc(uint32_t size) {
    memory_chunk_t* free_chunk = first_free_chunk(size);
    if (!free_chunk) {
        ERROR("malloc: no free chunk of size %x found\n", size);
        return NULL;
    }

    if (free_chunk->size > size + sizeof(memory_chunk_t)) {
        memory_chunk_t* new_chunk = (void*)free_chunk + sizeof(memory_chunk_t) + size;
        new_chunk->next = free_chunk->next;
        new_chunk->free = 1;
        new_chunk->size = free_chunk->size - sizeof(memory_chunk_t) - size;

        free_chunk->next = new_chunk;
        free_chunk->size = size;
    }

    free_chunk->free = 0;

    return (void*)free_chunk + sizeof(memory_chunk_t);
}

void free(void* ptr) {
    if (!ptr) {
        WARN("free: cannot free null pointer\n");
        return;
    }

    memory_chunk_t* chunk = (memory_chunk_t*)(ptr - sizeof(memory_chunk_t));
    chunk->free = 1;

    group_free_chunks();
}

void* malloc_page(uint32_t size) {
    void* ptr = malloc(size + sizeof(void*) + (PAGE_SIZE - 1));
    if (!ptr) {
        return NULL;
    }

    void* aligned_ptr = (void*)(((uint32_t)ptr + sizeof(void*) + PAGE_SIZE) & ~(PAGE_SIZE - 1));
    *((void**)aligned_ptr - 1) = ptr;

    return aligned_ptr;
}

void free_page(void* ptr) {
    if (!ptr) {
        return;
    }
    free(*((void**)ptr - 1));
}
