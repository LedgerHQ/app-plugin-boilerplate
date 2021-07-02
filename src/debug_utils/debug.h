#pragma once
#include <stdint.h>
void debug_write(const char *buf);

int semihosted_printf(const char *format, ...);
void print_bytes(const uint8_t *bytes, uint16_t len);