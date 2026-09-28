/*
 * macOS's libSystem has no explicit_bzero (a glibc/BSD extension), so this
 * defines it for APPLE builds.
 */
#ifndef FUZZING_MACOS_SHIM_H
#define FUZZING_MACOS_SHIM_H

#ifdef __APPLE__

#include <stddef.h>

/* Uses a volatile pointer so the store is not optimized away, unlike memset. */
static inline void explicit_bzero(void *s, size_t n) {
    volatile unsigned char *p = (volatile unsigned char *) s;
    while (n--) {
        *p++ = 0;
    }
}

#endif /* __APPLE__ */

#endif /* FUZZING_MACOS_SHIM_H */
