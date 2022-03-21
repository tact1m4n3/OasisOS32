#include <initrd.h>

#include <string.h>
#include <debug.h>
#include <mem.h>
#include <devfs.h>

static uint32_t initrd_device_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (offset > node->length) {
        WARN("initrd_read: nothing read... offset %x out of range\n", offset);
        return 0;
    }
    
    if (offset + size > node->length) {
        size = node->length - offset;
    }

    memcpy(buffer, (void*)((uint32_t)node->device + offset), size);

    return size;
}

static uint32_t initrd_device_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (offset > node->length) {
        WARN("initrd_write: nothing written... offset %x out of range\n", offset);
        return 0;
    }

    if (offset + size > node->length) {
        size = node->length - offset;
    }

    memcpy((void*)((uint32_t)node->device + offset), buffer, size);

    return size;
}

int initrd_init(uint32_t start_addr, uint32_t end_addr) {
    fs_node_t* node = (fs_node_t*)malloc(sizeof(fs_node_t));
    memset(node, 0, sizeof(fs_node_t));
    strcpy(node->name, "initrd");
    node->device = (void*)start_addr;
    node->uid = 0;
    node->gid = 0;
    node->mask = 770;
    node->flags = FS_BLOCKDEVICE;
    node->inode = 0;
    node->length = end_addr - start_addr;
    node->read = &initrd_device_read;
    node->write = &initrd_device_write;
    node->open = NULL;
    node->close = NULL;
    node->readdir = NULL;
    node->finddir = NULL;
    return devfs_register(node);
}
