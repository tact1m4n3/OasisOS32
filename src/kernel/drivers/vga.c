#include <vga.h>

#include <string.h>
#include <ports.h>

#define VGA_ADDR_PORT 0x3D4
#define VGA_DATA_PORT 0x3D5

#define VGA_COLS 80
#define VGA_ROWS 24

#define VGA_COLOR 0x0F

uint16_t* vga_buffer;

static void vga_get_cursor(uint8_t* x, uint8_t* y) {
    uint16_t pos = 0;
    
    outb(VGA_ADDR_PORT, 0xF);
    pos |= inb(VGA_DATA_PORT);
    outb(VGA_ADDR_PORT, 0xE);
    pos |= inb(VGA_DATA_PORT) << 8;

    *x = pos % VGA_COLS;
    *y = pos / VGA_COLS;
}

static void vga_set_cursor(uint8_t x, uint8_t y) {
    uint16_t pos = x + y * VGA_COLS;

    outb(VGA_ADDR_PORT, 0xF);
    outb(VGA_DATA_PORT, pos & 0xFF);
    outb(VGA_ADDR_PORT, 0xE);
    outb(VGA_DATA_PORT, (pos >> 8) & 0xFF);
}

static void vga_scroll() {
    uint8_t y = 1;
    while (y <= VGA_ROWS) {
        memcpy(vga_buffer + (y - 1) * VGA_COLS, vga_buffer + y * VGA_COLS, 2 * VGA_COLS);
        y++;
    }
    memset(vga_buffer + VGA_ROWS * VGA_COLS, 0, 2 * VGA_COLS);
}

void vga_init() {
    vga_buffer = (uint16_t*)0xB8000;
    vga_clear();
}

void vga_putch(char c) {
    uint8_t x, y;
    vga_get_cursor(&x, &y);

    switch (c) {
        case '\n':
            x = 0;
            ++y;
            break;
        case '\t':
            x += 4;
            break;
        default:
            vga_buffer[x + y * VGA_COLS] = c | (VGA_COLOR << 8);
            ++x;
            break;
    }

    if (x >= VGA_COLS) {
        ++y;
        x = x % 80;
    }

    if (y > VGA_ROWS) {
        vga_scroll();
        y--;
    }

    vga_set_cursor(x, y);
}

void vga_clear() {
    vga_set_cursor(0, 0);
    memset(vga_buffer, 0, 2 * VGA_COLS * VGA_ROWS);
}
