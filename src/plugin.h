/*******************************************************************************
 *   Origin Plugin
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

#include "eth_internals.h"
#include "eth_plugin_interface.h"

// All possible selectors of your plugin.
// A Xmacro below will create for you:
//     - an enum named selector_t with every NAME
//     - a map named SELECTORS associating each NAME with it's value
#define SELECTORS_LIST(X)                                          \
    /* ### VAULT */                                                \
    /* mint(address,uint256,uint256) */                            \
    X(VAULT_MINT, 0x156e29f6)                                      \
    /* redeem(uint256,uint256) */                                  \
    X(VAULT_REDEEM, 0x7cbc2373)                                    \
    /* ### CURVE */                                                \
    /* exchange(int128,int128,uint256,uint256) */                  \
    X(CURVE_POOL_EXCHANGE, 0x3df02124)                             \
    /* exchange_underlying(int128,int128,uint256,uint256) */       \
    X(CURVE_POOL_EXCHANGE_UNDERLYING, 0xa6417ed6)                  \
    /* exchange_multiple(address[9],uint256[3],uint256,uint256) */ \
    X(CURVE_ROUTER_EXCHANGE_MULTIPLE, 0x353ca424)                  \
    /* ### UNISWAP */                                              \
    /* exactInput(ExactInputParams params) */                      \
    X(UNISWAP_V3_ROUTER_EXACT_INPUT, 0xc04b8d59)                   \
    /* exactInputSingle(ExactInputSingleParams params) */          \
    X(UNISWAP_ROUTER_EXACT_INPUT_SINGLE, 0x414bf389)               \
    /* ### ZAPPER */                                               \
    /* deposit() */                                                \
    X(ZAPPER_DEPOSIT_ETH, 0xd0e30db0)                              \
    /* depositSFRXETH(uint256,uint256) */                          \
    X(ZAPPER_DEPOSIT_SFRXETH, 0xd443e97d)                          \
    /* ### FLIPPER */                                              \
    /* buyOusdWithUsdt(uint256) */                                 \
    X(FLIPPER_BUY_OUSD_WITH_USDT, 0x35aa0b96)                      \
    /* sellOusdForUsdt(uint256) */                                 \
    X(FLIPPER_SELL_OUSD_FOR_USDT, 0xcb939053)                      \
    /* buyOusdWithDai(uint256) */                                  \
    X(FLIPPER_BUY_OUSD_WITH_DAI, 0x5981c746)                       \
    /* sellOusdForDai(uint256) */                                  \
    X(FLIPPER_SELL_OUSD_FOR_DAI, 0x8a095a0f)                       \
    /* buyOusdWithUsdc(uint256) */                                 \
    X(FLIPPER_BUY_OUSD_WITH_USDC, 0xbfc11ffd)                      \
    /* sellOusdForUsdc(uint256) */                                 \
    X(FLIPPER_SELL_OUSD_FOR_USDC, 0xc6b68169)                      \
    /* ### 4626 VAULT */                                           \
    /* deposit(uint256,address) */                                 \
    X(WRAP, 0x6e553f65)                                            \
    /* redeem(uint256,address,address) */                          \
    X(UNWRAP, 0xba087652)

// Xmacro helpers to define the enum and map
// Do not modify !
#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,

// This enum will be automatically expanded to hold all selector names.
// The value SELECTOR_COUNT can be used to get the number of defined selectors
// Do not modify !
typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

// This array will be automatically expanded to map all selector_t names with
// the correct value. Do not modify !
extern const uint32_t SELECTORS[SELECTOR_COUNT];

// Enumeration used to parse the smart contract data.
typedef enum {
    TOKEN_SENT,
    TOKEN_RECEIVED,
    TOKEN_RECEIVED_REST,
    AMOUNT_SENT,
    MIN_AMOUNT_RECEIVED,
    BENEFICIARY,
    PATH_LENGTH,
    UNEXPECTED_PARAMETER,
    NONE,
} parameter;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
// parse. You will need to adapt this struct to your plugin.
typedef struct context_s {
    // For display.

    uint8_t amount_sent[INT256_LENGTH];
    uint8_t min_amount_received[INT256_LENGTH];
    uint8_t contract_address_sent[ADDRESS_LENGTH];
    uint8_t contract_address_received[ADDRESS_LENGTH];
    uint8_t beneficiary[ADDRESS_LENGTH];
    char ticker_sent[MAX_TICKER_LEN];
    char ticker_received[MAX_TICKER_LEN];

    uint8_t tokens_found;
    uint8_t decimals_sent;
    uint8_t decimals_received;
    uint8_t skip;

    bool valid;
    uint8_t amount_length;

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint8_t counter;
    uint16_t offset;  // Offset at which the array or struct starts.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

typedef enum {
    SEND_SCREEN,
    RECEIVE_SCREEN,
    WARN_SCREEN,
    BENEFICIARY_SCREEN,
    ERROR,
} screens_t;

#define TOKEN_SENT_FOUND     1
#define TOKEN_RECEIVED_FOUND 1 << 1

// Number of decimals used when the token wasn't found in the CAL.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the CAL.
#define DEFAULT_TICKER ""

#define ETH_TICKER "ETH"

#define OETH_TICKER   "OETH"
#define OETH_DECIMALS WEI_TO_ETHER

#define WETH_TICKER   "WETH"
#define WETH_DECIMALS WEI_TO_ETHER

#define STETH_TICKER   "STETH"
#define STETH_DECIMALS WEI_TO_ETHER

#define RETH_TICKER   "RETH"
#define RETH_DECIMALS WEI_TO_ETHER

#define FRXETH_TICKER   "FRXETH"
#define FRXETH_DECIMALS WEI_TO_ETHER

#define OUSD_TICKER   "OUSD"
#define OUSD_DECIMALS WEI_TO_ETHER

#define DAI_TICKER   "DAI"
#define DAI_DECIMALS WEI_TO_ETHER

#define USDC_TICKER   "USDC"
#define USDC_DECIMALS 6

#define USDT_TICKER   "USDT"
#define USDT_DECIMALS 6

#define WOETH_TICKER "WOETH"
#define WOUSD_TICKER "WOUSD"

#define SFRXETH_TICKER "sfrxETH"

extern const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t WETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t FRXETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t RETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t STETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t OETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t OUSD_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t DAI_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t USDC_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t USDT_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t CURVE_POOL_ETH_ADDRESS[ADDRESS_LENGTH];

extern const uint8_t OETH_VAULT_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t CURVE_OETH_POOL_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t CURVE_OUSD_POOL_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t WOETH_ADDRESS[ADDRESS_LENGTH];

#define ADDRESS_IS_NETWORK_TOKEN(_addr)                  \
    (!memcmp(_addr, NULL_ETH_ADDRESS, ADDRESS_LENGTH) || \
     !memcmp(_addr, CURVE_POOL_ETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_OETH(_addr)   (!memcmp(_addr, OETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_WETH(_addr)   (!memcmp(_addr, WETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_STETH(_addr)  (!memcmp(_addr, STETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_RETH(_addr)   (!memcmp(_addr, RETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_FRXETH(_addr) (!memcmp(_addr, FRXETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_OUSD(_addr)   (!memcmp(_addr, OUSD_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_DAI(_addr)    (!memcmp(_addr, DAI_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDC(_addr)   (!memcmp(_addr, USDC_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDT(_addr)   (!memcmp(_addr, USDT_ADDRESS, ADDRESS_LENGTH))

// Check if the context structure will fit in the RAM section ETH will prepare
// for us Do not remove!
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);

void handle_provide_parameter(ethPluginProvideParameter_t *msg);
void handle_query_contract_ui(ethQueryContractUI_t *msg);
void handle_init_contract(ethPluginInitContract_t *msg);
void handle_finalize(ethPluginFinalize_t *msg);
void handle_provide_token(ethPluginProvideInfo_t *msg);
void handle_query_contract_id(ethQueryContractID_t *msg);

static inline bool check_token_sent(context_t *context) {
    if (ADDRESS_IS_OETH(context->contract_address_sent)) {
        context->decimals_sent = OETH_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
        context->decimals_sent = DEFAULT_DECIMAL;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_WETH(context->contract_address_sent)) {
        context->decimals_sent = WETH_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_RETH(context->contract_address_sent)) {
        context->decimals_sent = RETH_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_STETH(context->contract_address_sent)) {
        context->decimals_sent = STETH_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_FRXETH(context->contract_address_sent)) {
        context->decimals_sent = FRXETH_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_OUSD(context->contract_address_sent)) {
        context->decimals_sent = OUSD_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_DAI(context->contract_address_sent)) {
        context->decimals_sent = DAI_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_USDC(context->contract_address_sent)) {
        context->decimals_sent = USDC_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else if (ADDRESS_IS_USDT(context->contract_address_sent)) {
        context->decimals_sent = USDT_DECIMALS;
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else {
        return false;
    }
    return true;
}

static inline bool check_token_received(context_t *context) {
    if (ADDRESS_IS_OETH(context->contract_address_received)) {
        context->decimals_received = OETH_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
        context->decimals_sent = DEFAULT_DECIMAL;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_WETH(context->contract_address_received)) {
        context->decimals_received = WETH_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_RETH(context->contract_address_received)) {
        context->decimals_received = RETH_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_STETH(context->contract_address_received)) {
        context->decimals_received = STETH_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_FRXETH(context->contract_address_received)) {
        context->decimals_received = FRXETH_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_OUSD(context->contract_address_received)) {
        context->decimals_received = OUSD_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_DAI(context->contract_address_received)) {
        context->decimals_received = DAI_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_USDC(context->contract_address_received)) {
        context->decimals_received = USDC_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (ADDRESS_IS_USDT(context->contract_address_received)) {
        context->decimals_received = USDT_DECIMALS;
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else {
        return false;
    }
    return true;
}

static inline void sent_network_token(context_t *context) {
    context->decimals_sent = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_SENT_FOUND;
}

static inline void received_network_token(context_t *context) {
    context->decimals_received = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_RECEIVED_FOUND;
}

static inline void printf_hex_array(const char *title __attribute__((unused)),
                                    size_t len __attribute__((unused)),
                                    const uint8_t *data __attribute__((unused))) {
    PRINTF(title);
    for (size_t i = 0; i < len; ++i) {
        PRINTF("%02x", data[i]);
    };
    PRINTF("\n");
}

static inline const char *get_ticker_for_address(uint8_t address[ADDRESS_LENGTH]) {
    if (ADDRESS_IS_NETWORK_TOKEN(address)) {
        return ETH_TICKER;
    } else if (ADDRESS_IS_OETH(address)) {
        return OETH_TICKER;
    } else if (ADDRESS_IS_WETH(address)) {
        return WETH_TICKER;
    } else if (ADDRESS_IS_FRXETH(address)) {
        return FRXETH_TICKER;
    } else if (ADDRESS_IS_RETH(address)) {
        return RETH_TICKER;
    } else if (ADDRESS_IS_STETH(address)) {
        return STETH_TICKER;
    } else if (ADDRESS_IS_OUSD(address)) {
        return OUSD_TICKER;
    } else if (ADDRESS_IS_DAI(address)) {
        return DAI_TICKER;
    } else if (ADDRESS_IS_USDC(address)) {
        return USDC_TICKER;
    } else if (ADDRESS_IS_USDT(address)) {
        return USDT_TICKER;
    } else {
        return DEFAULT_TICKER;
    }
}