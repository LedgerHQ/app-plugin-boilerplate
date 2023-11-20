#include "plugin.h"

size_t strlcat(char *dst, const char *src, size_t size) {
    size_t srclen; /* Length of source string */
    size_t dstlen; /* Length of destination string */

    dstlen = strlen(dst);
    size -= dstlen + 1;

    if (!size) return (dstlen); /* No room, return immediately... */

    srclen = strlen(src);

    if (srclen > size) srclen = size;

    memcpy(dst + dstlen, src, srclen);
    dst[dstlen + srclen] = '\0';

    return (dstlen + srclen);
}

size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t srclen; /* Length of source string */

    size--;

    srclen = strlen(src);

    if (srclen > size) srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}
