/*******************************************************************************
 *   ERC-8009 plugin
 *   (c) 2023 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#pragma once

#include <string.h>
#include "eth_plugin_interface.h"

#define SELECTORS_LIST(X)                        \
    X(PROXY_CALL, 0xce9506f9)                    \
    X(PROXY_CALL_DIFFS, 0x890e3add)              \
    X(APPROVE_PROXY_CALL, 0xe5d77b3c)            \
    X(APPROVE_PROXY_CALL_DIFFS, 0xc6707270)      \
    X(APPROVE_PROXY_CALL_META, 0x2f5a71c1)       \
    X(APPROVE_PROXY_CALL_DIFFS_META, 0xb3a7f90a) \
    X(PERMIT_PROXY_CALL, 0x029a2053)             \
    X(PERMIT_PROXY_CALL_DIFFS, 0xbf10e28e)       \
    X(PERMIT_PROXY_CALL_META, 0x366ffd79)        \
    X(PERMIT_PROXY_CALL_DIFFS_META, 0xb44921d7)  \
    X(SAFE_PROXY_CALL, 0x510818c0)               \
    X(SAFE_PROXY_CALL_DIFFS, 0x7ce6cc45)         \
    X(SAFE_PROXY_CALL_META, 0xe48215b2)          \
    X(SAFE_PROXY_CALL_DIFFS_META, 0x4674b941)

#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,

typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

extern const uint32_t SELECTORS[SELECTOR_COUNT];

#define MAX_GUARDS     8
#define MAX_APPROVALS  2
#define MAX_SYMBOL_LEN 16
#define MAX_DECIMALS   77
#define MAX_HEAD_SLOTS 8
#define MAX_LOOKUPS    2

typedef enum contract_kind_e {
    FAMILY_BP = 0,
    FAMILY_AR,
    FAMILY_PR,
    FAMILY_SR,
    FAMILY_COUNT,
} contract_kind_t;

// Kinds from P_META on are dynamic (offset word pointing to a tail).
typedef enum param_kind_e {
    P_ADDR_PROXY = 0,
    P_ADDR_TARGET,
    P_ADDR_SAFE,
    P_META,
    P_GUARDS,
    P_APPROVALS,
    P_PERMITS,
    P_BYTES,
    P_WITHDRAWALS,
    P_SAFE_TX,
} param_kind_t;

typedef enum erc8009_reason_e {
    R_OK = 0,
    U_VERSION = 1,
    U_SELECTOR = 2,
    E_CONTEXT = 3,
    E_DEST = 4,
    E_FAMILY = 5,
    E_CHAIN = 6,
    E_DATASIZE = 7,
    E_SAFE_VALUE = 8,
    E_SEQ = 9,
    E_PROXY = 10,
    E_OFFSET = 11,
    E_PAD_ADDR = 12,
    E_PAD_UINT = 13,
    E_BOOL = 14,
    E_TRUNC = 15,
    E_TRAILING = 16,
    E_META_LEN = 17,
    E_META_HEAD = 18,
    E_SYMBOL = 19,
    E_DECIMALS = 20,
    E_NATIVE_META = 21,
    E_APPROVALS_MAX = 22,
    E_APPROVAL_NEG = 23,
    E_PERMITS_LEN = 24,
    E_SAFE_OP = 25,
    E_BYTES_PAD = 26,
    E_RENDER = 27,
    E_STATE = 28,
    F_EMPTY = 29,
    F_TOO_MANY = 30,
} erc8009_reason_t;

typedef enum erc8009_outcome_e {
    OUTCOME_PARSING = 0,
    OUTCOME_OK,
    OUTCOME_ERROR,
    OUTCOME_FALLBACK,
    OUTCOME_UNAVAILABLE,
} erc8009_outcome_t;

#define GUARD_FLAG_OTHER 0x01

#define LOOKUP_NONE          0xFF
#define LOOKUP_APPROVAL_BASE MAX_GUARDS

typedef struct guard_s {
    uint8_t amount[INT256_LENGTH];
    uint8_t target[ADDRESS_LENGTH];
    uint8_t token[ADDRESS_LENGTH];
    char symbol[MAX_SYMBOL_LEN + 1];
    uint8_t decimals;
    uint8_t flags;
} guard_t;

typedef struct approval_s {
    uint8_t amount[INT256_LENGTH];
    uint8_t token[ADDRESS_LENGTH];
    uint8_t shown;
} approval_t;

typedef struct lookup_s {
    char ticker[MAX_SYMBOL_LEN + 1];
    uint8_t decimals;
    uint8_t source;
    uint8_t accepted;
} lookup_t;

// Shared global memory with Ethereum app. No pointers, no size_t; enum-typed fields are
// stored as uint8_t.
typedef struct context_s {
    uint32_t offsets[MAX_HEAD_SLOTS];
    uint32_t words_total;
    uint32_t words_seen;
    uint32_t count;
    uint32_t item;
    uint32_t bytes_left;
    uint32_t meta_len;
    uint32_t safe_data_words;
    uint32_t safe_sig_offset;

    guard_t guards[MAX_GUARDS];
    approval_t approvals[MAX_APPROVALS];
    lookup_t lookups[MAX_LOOKUPS];
    uint8_t safe[ADDRESS_LENGTH];

    uint8_t selector_index;
    uint8_t outcome;
    uint8_t reason;
    uint8_t stage;
    uint8_t slot;
    uint8_t sub;
    uint8_t field;
    uint8_t bytes_rem;
    uint8_t sym_len;
    uint8_t guard_count;
    uint8_t approval_count;
    uint8_t num_screens;
    uint8_t has_send;
} context_t;

ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);

eth_plugin_result_t erc8009_error(context_t *context, uint8_t reason, uint32_t offset);
