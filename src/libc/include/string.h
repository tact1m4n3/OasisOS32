#pragma once

#include <stdint.h>

void* memcpy(void* dst, void* src, uint32_t n);
void* memset(void* dst, uint8_t val, uint32_t n);
int memcmp(void* buf1, void* buf2, uint32_t n);
uint32_t strlen(char* str);
char* strcpy(char* dst, char* src);
int strcmp(char* str1, char* str2);
