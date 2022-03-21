#include <mem.h>

#include <string.h>
#include <debug.h>

#define SET_BIT(i) frame_bitmap[i / 8] = frame_bitmap[i / 8] | (1 << (i % 8))
#define CLEAR_BIT(i) frame_bitmap[i / 8] = frame_bitmap[i / 8] & (~(1 << (i % 8)))
#define IS_SET(i) ((frame_bitmap[i / 8] >> (i % 8)) & 0x1)

uint8_t* frame_bitmap;
uint32_t n_frames;

static uint32_t first_free_frame() {
    for (uint32_t i = 0; i < n_frames; i++) {
        if (!IS_SET(i)) {
            return i;
        }
    }
    return -1;
}

void phys_init() {
    frame_bitmap = (uint8_t*)0x7BFF;
    n_frames = MEMORY_SIZE / PAGE_SIZE;
    memset(frame_bitmap, 0, 8 * n_frames + 1);
}

uint32_t alloc_frame() {
    uint32_t idx = first_free_frame();
    if (idx == -1) {
        ERROR("alloc_frame: no free frames left\n");
        return -1;
    }
    SET_BIT(idx);
    return idx * PAGE_SIZE;
}

void free_frame(uint32_t frame) {
    uint32_t idx = frame / PAGE_SIZE;
    if (idx >= n_frames) {
        WARN("free_frame: frame %x not valid\n", frame);
        return;
    }
    CLEAR_BIT(idx);
}
