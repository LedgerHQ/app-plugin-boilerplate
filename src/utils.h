#pragma once

#include "eth_internals.h"
#include <stddef.h>

/// Utility function to compute the public key given a `bip32Path`.
void get_public_key(uint8_t bip32PathLength,
                    uint32_t *bip32Path,
                    cx_sha3_t *sha3Context,
                    chain_config_t *chain_config,
                    uint8_t *out,
                    uint8_t outLength);