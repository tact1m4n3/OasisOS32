#pragma once

#include <types.h>

#define INFO(fmt, ...) { print("[info] "); print(fmt, ##__VA_ARGS__); }
#define WARN(fmt, ...) { print("[warn] "); print(fmt, ##__VA_ARGS__); }
#define ERROR(fmt, ...) { print("[error] "); print(fmt, ##__VA_ARGS__); }
#define PANIC(fmt, ...) { print("[panic] "); print(fmt, ##__VA_ARGS__); for (;;); }

void print(char* fmt, ...);
