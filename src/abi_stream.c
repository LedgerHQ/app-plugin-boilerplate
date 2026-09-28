#include "abi_stream.h"
#include "plugin_utils.h"

#define ADDRESS_PAD_LEN (ABI_WORD_SIZE - ADDRESS_LENGTH)

bool abi_is_address(const uint8_t *word) {
    return allzeroes(word, ADDRESS_PAD_LEN) != 0;
}

bool abi_read_uint8(const uint8_t *word, uint8_t *value) {
    if (!allzeroes(word, ABI_WORD_SIZE - 1)) {
        return false;
    }
    *value = word[ABI_WORD_SIZE - 1];
    return true;
}

bool abi_is_bool(const uint8_t *word) {
    uint8_t value;
    return abi_read_uint8(word, &value) && value <= 1;
}

bool abi_read_u32(const uint8_t *word, uint32_t *value) {
    return U4BE_from_parameter(word, value);
}

bool abi_equals_u32(const uint8_t *word, uint32_t value) {
    uint32_t read;
    return abi_read_u32(word, &read) && read == value;
}

bool abi_is_zero(const uint8_t *word) {
    return allzeroes(word, ABI_WORD_SIZE) != 0;
}

bool abi_is_negative(const uint8_t *word) {
    return (word[0] & 0x80) != 0;
}

bool abi_array_fits(uint32_t count, uint32_t element_words, uint32_t remaining_words) {
    if (element_words == 0) {
        return true;
    }
    // count * element_words <= remaining_words, without multiplying.
    return count <= remaining_words / element_words;
}

uint32_t abi_bytes_words(uint32_t len) {
    return len / ABI_WORD_SIZE + ((len % ABI_WORD_SIZE) != 0 ? 1 : 0);
}

bool abi_bytes_padding_ok(const uint8_t *word, uint32_t len) {
    uint32_t used = len % ABI_WORD_SIZE;
    if (used == 0) {
        return true;
    }
    return allzeroes(word + used, ABI_WORD_SIZE - used) != 0;
}

bool abi_symbol_ok(const uint8_t *word, uint32_t len) {
    if (len == 0 || len > ABI_WORD_SIZE) {
        return false;
    }
    for (uint32_t i = 0; i < len; i++) {
        if (word[i] < 0x20 || word[i] > 0x7e) {
            return false;
        }
    }
    return allzeroes(word + len, ABI_WORD_SIZE - len) != 0;
}
