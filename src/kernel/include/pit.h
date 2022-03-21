#pragma once

#include <types.h>

void pit_init();
void pit_set_freq(uint16_t freq);
void pit_wait(uint32_t ticks);
