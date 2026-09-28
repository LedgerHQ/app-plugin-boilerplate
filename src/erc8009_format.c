#include "erc8009_format.h"
#include "contracts.h"

static bool append(char *out, size_t out_len, size_t *used, const char *src) {
    size_t len = strlen(src);

    if (*used >= out_len || len >= out_len - *used) {
        return false;
    }
    memcpy(out + *used, src, len);
    *used += len;
    out[*used] = '\0';
    return true;
}

static bool append_number(char *out, size_t out_len, size_t *used, uint8_t value) {
    char digits[4];  // "255" + NUL

    if (!u64_to_string(value, digits, sizeof(digits))) {
        return false;
    }
    return append(out, out_len, used, digits);
}

bool int256_magnitude(const uint8_t *value, uint8_t *magnitude) {
    uint16_t carry = 1;

    if ((value[0] & 0x80) == 0) {
        memcpy(magnitude, value, INT256_LENGTH);
        return false;
    }
    for (int i = INT256_LENGTH - 1; i >= 0; i--) {
        uint16_t sum = (uint16_t) ((uint8_t) ~value[i]) + carry;
        magnitude[i] = (uint8_t) sum;
        carry = sum >> 8;
    }
    return true;
}

bool erc8009_format_amount(const uint8_t *amount,
                           uint8_t amount_len,
                           uint8_t sign,
                           uint8_t decimals,
                           const char *ticker,
                           char *out,
                           size_t out_len) {
    uint8_t magnitude[INT256_LENGTH];
    const uint8_t *digits = amount;
    size_t used = 0;

    if (out_len == 0) {
        return false;
    }
    out[0] = '\0';
    if (sign != AMOUNT_UNSIGNED) {
        bool negative;

        if (amount_len != INT256_LENGTH) {
            return false;
        }
        negative = int256_magnitude(amount, magnitude);
        digits = magnitude;
        if (sign == AMOUNT_SIGNED && (negative || !allzeroes(amount, INT256_LENGTH))) {
            if (!append(out, out_len, &used, negative ? "-" : "+")) {
                return false;
            }
        }
    }
    if (!amountToString(digits, amount_len, decimals, ticker, out + used, out_len - used)) {
        out[0] = '\0';
        return false;
    }
    return true;
}

bool erc8009_format_address(const uint8_t *address, char *out, size_t out_len) {
    // "0x" + 40 hex digits + NUL
    if (out_len < 2 + 2 * ADDRESS_LENGTH + 1) {
        if (out_len > 0) {
            out[0] = '\0';
        }
        return false;
    }
    if (!getEthDisplayableAddress(address, out, out_len, CHAIN_ID)) {
        out[0] = '\0';
        return false;
    }
    return true;
}

bool erc8009_format_title(char *out,
                          size_t out_len,
                          const char *label,
                          uint8_t index,
                          uint8_t total) {
    size_t used = 0;
    bool ok;

    if (out_len == 0) {
        return false;
    }
    ok = append(out, out_len, &used, label);
    if (ok && index != 0) {
        ok = append(out, out_len, &used, " ") && append_number(out, out_len, &used, index);
        if (ok && total != 0) {
            ok = append(out, out_len, &used, "/") && append_number(out, out_len, &used, total);
        }
    }
    if (!ok) {
        out[0] = '\0';
    }
    return ok;
}

bool erc8009_copy_string(char *out, size_t out_len, const char *src) {
    size_t used = 0;

    if (out_len == 0) {
        return false;
    }
    if (!append(out, out_len, &used, src)) {
        out[0] = '\0';
        return false;
    }
    return true;
}
