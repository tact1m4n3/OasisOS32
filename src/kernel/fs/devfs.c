#include <devfs.h>

#include <string.h>
#include <debug.h>
#include <mem.h>

#define MAX_DEVICES 32

fs_node_t** device_list;
uint32_t n_devices;

static dirent_t* devfs_readdir(fs_node_t* node, uint32_t idx) {
    if (idx == 0) {
        dirent_t* dirent = (dirent_t*)malloc(sizeof(dirent_t));
        memset(dirent, 0, sizeof(dirent_t));
        strcpy(dirent->name, ".");
        dirent->inode = 0;
        return dirent;
    }

    if (idx == 1) {
        dirent_t* dirent = (dirent_t*)malloc(sizeof(dirent_t));
        memset(dirent, 0, sizeof(dirent_t));
        strcpy(dirent->name, "..");
        dirent->inode = 0;
        return dirent;
    }

    idx -= 2;

    if (idx < n_devices) {
        fs_node_t* device = device_list[idx];
        dirent_t* dirent = (dirent_t*)malloc(sizeof(dirent_t));
        memset(dirent, 0, sizeof(dirent_t));
        strcpy(dirent->name, device->name);
        dirent->inode = idx;
        return dirent;
    }

    WARN("devfs_readdir: index %x out of range\n", idx);
    return NULL;
}

static fs_node_t* devfs_finddir(fs_node_t* node, char* name) {
    for (uint32_t i = 0; i < n_devices; i++) {
        fs_node_t* device = device_list[i];
        if (!strcmp(device->name, name)) {
            fs_node_t* device_clone = (fs_node_t*)malloc(sizeof(fs_node_t));
            memcpy(device_clone, device, sizeof(fs_node_t));
            return device_clone;
        }
    }

    WARN("devfs_finddir: file %s not found\n", name);
    return NULL;
}

int devfs_init() {
    device_list = (fs_node_t**)malloc(MAX_DEVICES * sizeof(fs_node_t));
    n_devices = 0;

    fs_node_t* root = (fs_node_t*)malloc(sizeof(fs_node_t));
    memset(root, 0, sizeof(fs_node_t));
    root->uid = 0;
    root->gid = 0;
    root->mask = 555;
    root->flags = FS_DIRECTORY;
    root->read = NULL;
    root->write = NULL;
    root->open = NULL;
    root->close = NULL;
    root->readdir = &devfs_readdir;
    root->finddir = &devfs_finddir;

    devfs_register(zero_device_create());
    devfs_register(null_device_create());
    
    return vfs_mount("/dev", root);
}

int devfs_register(fs_node_t* device) {
    if (n_devices >= MAX_DEVICES) {
        ERROR("devfs_register: maximum number of devices reached\n");
        return 0;
    }
    device_list[n_devices++] = device;
    return 1;
}
