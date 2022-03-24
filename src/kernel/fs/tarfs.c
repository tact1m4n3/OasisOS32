#include <tarfs.h>

#include <string.h>
#include <debug.h>
#include <mem.h>

typedef struct tar_header {
    char filename[100];
    char mode[8];
    char owner_id[8];
    char group_id[8];

    char size[12];
    char mtime[12];

    char checksum[8];
    char type[1];
    char link[100];

    char ustar[6];
    char version[2];

    char owner[32];
    char group[32];

    char dev_major[8];
    char dev_minor[8];

    char prefix[155];
} tar_header_t;

static uint32_t interpret_uid(tar_header_t* header) {
    return ((header->owner_id[0] - '0') << 18) |
           ((header->owner_id[1] - '0') << 15) |
           ((header->owner_id[2] - '0') << 12) |
           ((header->owner_id[3] - '0') <<  9) |
           ((header->owner_id[4] - '0') <<  6) |
           ((header->owner_id[5] - '0') <<  3) |
           ((header->owner_id[6] - '0') <<  0);
}

static uint32_t interpret_gid(tar_header_t* header) {
    return ((header->group_id[0] - '0') << 18) |
           ((header->group_id[1] - '0') << 15) |
           ((header->group_id[2] - '0') << 12) |
           ((header->group_id[3] - '0') <<  9) |
           ((header->group_id[4] - '0') <<  6) |
           ((header->group_id[5] - '0') <<  3) |
           ((header->group_id[6] - '0') <<  0);
}

static uint32_t interpret_mode(tar_header_t* header) {
    return ((header->mode[0] - '0') << 18) |
           ((header->mode[1] - '0') << 15) |
           ((header->mode[2] - '0') << 12) |
           ((header->mode[3] - '0') <<  9) |
           ((header->mode[4] - '0') <<  6) |
           ((header->mode[5] - '0') <<  3) |
           ((header->mode[6] - '0') <<  0);
}

static uint32_t interpret_size(tar_header_t* header) {
    return ((header->size[ 0] - '0') << 30) |
           ((header->size[ 1] - '0') << 27) |
           ((header->size[ 2] - '0') << 24) |
           ((header->size[ 3] - '0') << 21) |
           ((header->size[ 4] - '0') << 18) |
           ((header->size[ 5] - '0') << 15) |
           ((header->size[ 6] - '0') << 12) |
           ((header->size[ 7] - '0') <<  9) |
           ((header->size[ 8] - '0') <<  6) |
           ((header->size[ 9] - '0') <<  3) |
           ((header->size[10] - '0') <<  0);
}

static uint32_t round_size(uint32_t size) {
    return size + (512 - size % 512);
}

static uint32_t tarfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    fs_node_t* device = node->device;
    
    if (offset > node->length) {
        WARN("tarfs_read: nothing read... offset %x out of range\n", offset);
        return 0;
    }

    if (offset + size > node->length) {
        size = node->length - offset;
    }

    return vfs_read(device, node->inode + 512 + offset, size, buffer);
}

static uint32_t tarfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    return 0;
}

static dirent_t* tarfs_readdir(fs_node_t* node, uint32_t idx) {
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

    fs_node_t* device = node->device;

    tar_header_t* header = (tar_header_t*)malloc(sizeof(tar_header_t));
    
    uint32_t offset = 0;
    while (offset < device->length) {
        vfs_read(device, offset, sizeof(tar_header_t), (uint8_t*)header);

        if (memcmp(header->ustar, "ustar", 5)) {
            break;
        }

        if (idx == 0) {
            dirent_t* dirent = (dirent_t*)malloc(sizeof(dirent_t));
            memset(dirent, 0, sizeof(dirent_t));
            strcpy(dirent->name, header->filename);
            dirent->inode = offset;
            free(header);
            return dirent;
        }

        offset += 512;
        offset += round_size(interpret_size(header));

        idx--;
    }

    WARN("tarfs_readdir: index %x out of range\n", idx);
    free(header);
    return NULL;
}

static fs_node_t* tarfs_finddir(fs_node_t* node, char* name) {
    fs_node_t* device = node->device;

    tar_header_t* header = (tar_header_t*)malloc(sizeof(tar_header_t));
    
    uint32_t offset = 0;
    while (offset < device->length) {
        vfs_read(device, offset, sizeof(tar_header_t), (uint8_t*)header);

        if (memcmp(header->ustar, "ustar", 5)) {
            break;
        }

        if (!strcmp(header->filename, name)) {
            fs_node_t* file = (fs_node_t*)malloc(sizeof(fs_node_t));
            memset(file, 0, sizeof(fs_node_t));
            strcpy(file->name, name);
            file->device = device;
            file->uid = interpret_uid(header);
            file->gid = interpret_gid(header);
            file->mask = interpret_mode(header);
            file->flags = FS_FILE;
            file->inode = offset;
            file->length = interpret_size(header);
            file->read = &tarfs_read;
            file->write = &tarfs_write;
            file->open = NULL;
            file->close = NULL;
            file->readdir = NULL;
            file->finddir = NULL;
            free(header);
            return file;
        }

        offset += 512;
        offset += round_size(interpret_size(header));
    }

    WARN("tarfs_finddir: file %s not found\n", name);
    free(header);
    return NULL;
}

int tarfs_mount(char* mount_path, char* device_path) {
    fs_node_t* device = vfs_lookup(device_path);
    if (!device) {
        WARN("tarfs_init: device %s not found\n", device_path);
        return 0;
    }

    fs_node_t* root = (fs_node_t*)malloc(sizeof(fs_node_t));
    memset(root, 0, sizeof(fs_node_t));
    root->device = device;
    root->uid = 0;
    root->gid = 0;
    root->mask = 555;
    root->flags = FS_DIRECTORY;
    root->read = NULL;
    root->write = NULL;
    root->open = NULL;
    root->close = NULL;
    root->readdir = &tarfs_readdir;
    root->finddir = &tarfs_finddir;

    return vfs_mount(mount_path, root);
}
