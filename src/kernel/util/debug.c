#include <debug.h>

#include <string.h>
#include <vga.h>

static uint32_t hex2str(char* buf, uint32_t val) {
    char syms[] = "0123456789ABCDEF";
    uint32_t i = 8;
    while (i--) {
        int offset = val % 16;
        buf[i] = syms[offset];
        val /= 16;
    }
    return 8;
}

static uint32_t vsprintf(char* buf, char* fmt, va_list args) {
    uint32_t i = 0, j = 0;
    uint32_t len = strlen(fmt);
    while (i < len) {
        if (fmt[i] != '%') {
            buf[j++] = fmt[i];
            i++;
            continue;
        }

        char* s;
        switch (fmt[++i]) {
            case 'c':
                buf[j++] = (char)va_arg(args, int);
                break;
            case 's':
                s = (char*)va_arg(args, char*);
                while (*s)
                    buf[j++] = *s++;
                break;
            case 'x':
                j += hex2str(buf + j, (uint32_t)va_arg(args, int));
                break;
            case '%':
                buf[j++] = '%';
                break;
            default:
                buf[j++] = fmt[i];
                break;
        }

        i++;
    }
    return j;
}

void print(char* fmt, ...) {
    char buf[128];
    memset(buf, 0, 128);

    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    char* s = buf;
    while (*s) {
        vga_putch(*s++);
    }
}
