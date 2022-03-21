#pragma once

#include <types.h>
#include <vfs.h>

fs_node_t* zero_device_create();
fs_node_t* null_device_create();

int devfs_init();
int devfs_register(fs_node_t* device);
