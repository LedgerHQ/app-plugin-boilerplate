#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Every function returns false when the result does not fit the output buffer
// (reported as E_RENDER); the buffer is then left empty.

typedef enum amount_sign_e {
    AMOUNT_UNSIGNED = 0,
    AMOUNT_SIGNED,
    AMOUNT_MAGNITUDE,
} amount_sign_t;

bool int256_magnitude(const uint8_t *value, uint8_t *magnitude);

bool erc8009_format_amount(const uint8_t *amount,
                           uint8_t amount_len,
                           uint8_t sign,
                           uint8_t decimals,
                           const char *ticker,
                           char *out,
                           size_t out_len);

bool erc8009_format_address(const uint8_t *address, char *out, size_t out_len);

bool erc8009_format_title(char *out,
                          size_t out_len,
                          const char *label,
                          uint8_t index,
                          uint8_t total);

bool erc8009_copy_string(char *out, size_t out_len, const char *src);
