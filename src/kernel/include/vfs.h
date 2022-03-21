#pragma once

#include <types.h>

#define FS_FILE 0x1
#define FS_DIRECTORY 0x2
#define FS_CHARDEVICE 0x4
#define FS_BLOCKDEVICE 0x8

struct fs_node;
struct dirent;

typedef struct fs_node {
    char name[256];
    void* device;
    uint32_t uid;
    uint32_t gid;
    uint32_t mask;
    uint32_t flags;
    uint32_t inode;
    uint32_t length;

    uint32_t (*read)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
    uint32_t (*write)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
    void (*open)(struct fs_node*, uint32_t);
    void (*close)(struct fs_node*);
    struct dirent* (*readdir)(struct fs_node*, uint32_t);
    struct fs_node* (*finddir)(struct fs_node*, char*);
} fs_node_t;

typedef struct dirent {
    char name[256];
    uint32_t inode;
} dirent_t;

void vfs_init();
uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
void vfs_open(fs_node_t* node, uint32_t flags);
void vfs_close(fs_node_t* node);
dirent_t* vfs_readdir(fs_node_t* node, uint32_t idx);
fs_node_t* vfs_finddir(fs_node_t* node, char* name);
int vfs_mount(char* path, fs_node_t* root);
fs_node_t* vfs_lookup(char* path);
