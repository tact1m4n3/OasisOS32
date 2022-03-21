#include <pic.h>

#include <ports.h>

#define PIC_MASTER_CMD_PORT 0x20
#define PIC_MASTER_DATA_PORT 0x21
#define PIC_SLAVE_CMD_PORT 0xA0
#define PIC_SLAVE_DATA_PORT 0xA1

void pic_remap() {
    outb(PIC_MASTER_CMD_PORT, 0x11);
    outb(PIC_SLAVE_CMD_PORT, 0x11);

    outb(PIC_MASTER_DATA_PORT, 0x20);
    outb(PIC_SLAVE_DATA_PORT, 0x20 + 8);

    outb(PIC_MASTER_DATA_PORT, 0x04);
    outb(PIC_SLAVE_DATA_PORT, 0x02);

    outb(PIC_MASTER_DATA_PORT, 0x01);
    outb(PIC_SLAVE_DATA_PORT, 0x01);
    
    outb(PIC_MASTER_DATA_PORT, 0x0);
    outb(PIC_SLAVE_DATA_PORT, 0x0);
}

void pic_ack(uint32_t int_no) {
    if (int_no >= 40) {
        outb(PIC_SLAVE_CMD_PORT, 0x20);
    }
    outb(PIC_MASTER_CMD_PORT, 0x20);
}
