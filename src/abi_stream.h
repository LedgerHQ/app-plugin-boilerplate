#pragma once

#include <stdbool.h>
#include <stdint.h>

// Every `word` points to 32 bytes.

#define ABI_WORD_SIZE 32

bool abi_is_address(const uint8_t *word);

bool abi_read_uint8(const uint8_t *word, uint8_t *value);

bool abi_is_bool(const uint8_t *word);

bool abi_read_u32(const uint8_t *word, uint32_t *value);

bool abi_equals_u32(const uint8_t *word, uint32_t value);

bool abi_is_zero(const uint8_t *word);

bool abi_is_negative(const uint8_t *word);

// count * element_words <= remaining_words, without overflowing via multiplication.
bool abi_array_fits(uint32_t count, uint32_t element_words, uint32_t remaining_words);

uint32_t abi_bytes_words(uint32_t len);

bool abi_bytes_padding_ok(const uint8_t *word, uint32_t len);

bool abi_symbol_ok(const uint8_t *word, uint32_t len);
