#include <devfs.h>

#include <string.h>
#include <mem.h>

static uint32_t null_device_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    return 0;
}

static uint32_t null_device_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    return size;
}

static uint32_t zero_device_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    memset(buffer, 0, size);
    return size;
}

static uint32_t zero_device_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    return 0;
}

fs_node_t* null_device_create() {
    fs_node_t* node = (fs_node_t*)malloc(sizeof(fs_node_t));
    memset(node, 0, sizeof(fs_node_t));
    strcpy(node->name, "null");
    node->device = NULL;
    node->uid = 0;
    node->gid = 0;
    node->mask = 666;
    node->flags = FS_CHARDEVICE;
    node->inode = 0;
    node->length = 0;
    node->read = &null_device_read;
    node->write = &null_device_write;
    node->open = NULL;
    node->close = NULL;
    node->readdir = NULL;
    node->finddir = NULL;
    return node;
}

fs_node_t* zero_device_create() {
    fs_node_t* node = (fs_node_t*)malloc(sizeof(fs_node_t));
    memset(node, 0, sizeof(fs_node_t));
    strcpy(node->name, "zero");
    node->device = NULL;
    node->uid = 0;
    node->gid = 0;
    node->mask = 666;
    node->flags = FS_CHARDEVICE;
    node->inode = 0;
    node->length = 0;
    node->read = &zero_device_read;
    node->write = &zero_device_write;
    node->open = NULL;
    node->close = NULL;
    node->readdir = NULL;
    node->finddir = NULL;
    return node;
}
