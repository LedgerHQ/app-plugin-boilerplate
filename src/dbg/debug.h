#pragma once
#include <stdint.h>

// Printf that uses speculos semi-hosting features.
void semihosted_printf(const char *format, ...);