#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "printf.h"

// Writes the string buf using QEMU's semi-hosting features. Only usable with speculos, the device emulator!
void debug_write(const char *buf) {
    asm volatile(
        "movs r0, #0x04\n"
        "movs r1, %0\n"
        "svc      0xab\n" ::"r"(buf)
        : "r0", "r1");
}

// Special printf function using the `debug_write` function.
int semihosted_printf(const char *format, ...) {
    char buf[128+1];

    va_list args;
    va_start(args, format);

    int ret = vsnprintf(buf, sizeof(buf) - 1, format, args);

    va_end(args);

    debug_write("semi-hosting: ");
    if (ret > 0) {
      buf[ret] = 0;
      debug_write(buf);
    }

    return ret;
}

static const char G_HEX[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

// %.*H doesn't work with semi-hosted printf, so here's a utility function to print bytes in hex format.
void print_bytes(const uint8_t *bytes, uint16_t len) {
    unsigned char nibble1, nibble2;
    char str[] = {0, 0, 0};

    debug_write("bytes: ");
    for (uint16_t count = 0; count < len; count++) {
        nibble1 = (bytes[count] >> 4) & 0xF;
        nibble2 = bytes[count] & 0xF;
        str[0] = G_HEX[nibble1];
        str[1] = G_HEX[nibble2];
        debug_write(str);
        debug_write(" ");
    }
    debug_write("\n");
}