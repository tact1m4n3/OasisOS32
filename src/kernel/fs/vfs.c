#include <vfs.h>

#include <string.h>
#include <debug.h>
#include <mem.h>

#define MAX_MOUNTS 16

typedef struct mount {
    char* path;
    fs_node_t* root;
} mount_t;

mount_t** mount_list;
uint32_t n_mounts;

static uint32_t parse_path(char* path) {
    uint32_t depth = 0;
    for (char* p = path; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\0';
            depth++;
        }
    }
    return depth;
}

static mount_t* find_mount(char* path, char** out_path) {
    for (uint32_t i = 0; i < n_mounts; i++) {
        mount_t* mount = mount_list[i];
        uint32_t path_len = strlen(mount->path);
        if (!memcmp(path, mount->path, path_len) && mount->root) {
            if (out_path) {
                *out_path = path + path_len;
            }
            return mount;
        }
    }
    return NULL;
}

void vfs_init() {
    mount_list = (mount_t**)malloc(MAX_MOUNTS * sizeof(mount_t*));
    n_mounts = 0;
}

uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->read) {
        return node->read(node, offset, size, buffer);
    } else {
        return 0;
    }
}

uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->write) {
        return node->write(node, offset, size, buffer);
    } else {
        return 0;
    }
}

void vfs_open(fs_node_t* node, uint32_t flags) {
    if (node->open) {
        node->open(node, flags);
    }
}

void vfs_close(fs_node_t* node) {
    if (node->close) {
        node->close(node);
    }
}

dirent_t* vfs_readdir(fs_node_t* node, uint32_t idx) {
    if ((node->flags & FS_DIRECTORY) && node->readdir) {
        return node->readdir(node, idx);
    } else {
        return NULL;
    }
}

fs_node_t* vfs_finddir(fs_node_t* node, char* name) {
    if ((node->flags & FS_DIRECTORY) && node->finddir) {
        return node->finddir(node, name);
    } else {
        return NULL;
    }
}

int vfs_mount(char* path, fs_node_t* root) {
    if (path[0] != '/') {
        ERROR("vfs_mount: mount path has to be absolute\n");
        return 0;
    }

    mount_t* mount = find_mount(path, NULL);
    if (mount) {
        if (mount->root) {
            WARN("vfs_mount: something already mounted on %s\n", path);
        }
        mount->root = root;
        return 1;
    }

    if (n_mounts >= MAX_MOUNTS) {
        ERROR("vfs_mount: maximum number of mounts reached\n")
        return 0;
    }

    mount = (mount_t*)malloc(sizeof(mount_t));
    mount->path = strdup(path);
    mount->root = root;

    mount_list[n_mounts++] = mount;

    return 1;
}

fs_node_t* vfs_lookup(char* path) {
    if (path[0] != '/') {
        ERROR("vfs_lookup: lookup path has to be absolute\n");
        return NULL;
    }
    
    char* local_path;
    mount_t* mount = find_mount(path, &local_path);
    if (!mount) {
        WARN("vfs_lookup: mount point for %s not found\n", path);
        return NULL;
    }

    char* path_clone = strdup(local_path);
    uint32_t path_depth = parse_path(path_clone);

    fs_node_t* root_clone = (fs_node_t*)malloc(sizeof(fs_node_t));
    memcpy(root_clone, mount->root, sizeof(fs_node_t));

    fs_node_t* current = root_clone;
    char* name = &path_clone[1];
    uint32_t i = 0;
    while (i < path_depth) {
        fs_node_t* next = vfs_finddir(current, name);
        free(current);

        if (!next) {
            WARN("vfs_lookup: file %s not found\n", path);
            free(path_clone);
            return NULL;
        }
        
        i++;
        current = next;
        name += strlen(name) + 1;
    }

    free(path_clone);
    
    return current;
}
