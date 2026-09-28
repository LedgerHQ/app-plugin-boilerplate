#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "plugin.h"

#define CHAIN_ID 11155111ULL

#define NATIVE_TICKER   "ETH"
#define NATIVE_DECIMALS 18

typedef struct head_slot_s {
    uint8_t kind;
#ifdef ERC8009_SCHEMA_NAMES
    const char *name;
#endif
} head_slot_t;

typedef struct selector_schema_s {
    uint8_t family;
    uint8_t diffs;
    uint8_t head_len;
    head_slot_t slots[MAX_HEAD_SLOTS];
#ifdef ERC8009_SCHEMA_NAMES
    const char *function;
#endif
} selector_schema_t;

extern const selector_schema_t SCHEMAS[SELECTOR_COUNT];

// FAMILY_BP is the canonical BalanceProxy expected in every ADDR_PROXY slot.
extern const uint8_t CONTRACT_ADDRESSES[FAMILY_COUNT][ADDRESS_LENGTH];

bool contract_family(const uint8_t *address, uint8_t *family);

bool param_is_dynamic(uint8_t kind);

bool schema_is_meta(const selector_schema_t *schema);

bool chain_id_ok(const uint8_t *value, uint8_t length);

// Only the 32 bytes of the txInt256_t field are read, even if length claims more.
bool tx_value_is_zero(const txInt256_t *value);
