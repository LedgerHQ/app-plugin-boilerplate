#include "plugin.h"

cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size) {
    UNUSED(hash);
    UNUSED(size);
    return CX_OK;
}

cx_err_t cx_hash_no_throw(cx_hash_t *hash,
                          uint32_t mode,
                          const uint8_t *in,
                          size_t len,
                          uint8_t *out,
                          size_t out_len) {
    UNUSED(hash);
    UNUSED(mode);
    UNUSED(in);
    UNUSED(len);
    UNUSED(out);
    UNUSED(out_len);
    return CX_OK;
}

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
