/*******************************************************************************
 *   Plugin Expand
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


// Xmacro helpers to define the enum and map
// Do not modify !
#define TO_ENUM(selector_name, selector_id)  selector_name,
#define TO_VALUE(selector_name, selector_id) selector_id,


// All possible selectors of your plugin.
// EDIT THIS: Enter your selectors here, in the format X(NAME, value)
// A Xmacro below will create for you:
//     - an enum named selector_t with every NAME
//     - a map named SELECTORS associating each NAME with it's value
#define SELECTORS_LIST(X)                           \
    X(SWAP_EXACT_ETH_FOR_TOKENS, 0x7ff36ab5)         \
    X(SWAP_EXACT_TOKENS_FOR_ETH, 0x18cbafe5)          \
    X(SWAP_EXACT_TOKENS_FOR_TOKENS, 0x38ed1739)        \
    X(APPROVE, 0x095ea7b3)

// This enum will be automatically expanded to hold all selector names.
// The value SELECTOR_COUNT can be used to get the number of defined selectors
// Do not modify !
typedef enum selector_e {
    SELECTORS_LIST(TO_ENUM) SELECTOR_COUNT,
} selector_t;

// This array will be automatically expanded to map all selector_t names with the correct value.
// Do not modify !
extern const uint32_t SELECTORS[SELECTOR_COUNT];

// Enumeration used to parse the smart contract data.
typedef enum {
    AMOUNT_SENT,
    MIN_AMOUNT_RECEIVED,
    TOKEN_SENT,
    TOKEN_RECEIVED,
    BENEFICIARY,
    PATH_OFFSET,
    PATH_LENGTH,
    UNEXPECTED_PARAMETER,
} parameter;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
// will need to adapt this struct to your plugin.
typedef struct context_s {
    // For display.
    uint8_t amount_sent[INT256_LENGTH];
    uint8_t amount_received[INT256_LENGTH];
    uint8_t beneficiary[ADDRESS_LENGTH];
    uint8_t token_sent[ADDRESS_LENGTH];
    uint8_t token_received[ADDRESS_LENGTH];
    // uint8_t decimals_sent;
    // uint8_t decimals_recieved;
    bool token_sent_found;
    bool token_received_found;
    uint8_t skip;
    char ticker_sent[MAX_TICKER_LEN];
    char ticker_received[MAX_TICKER_LEN];



    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint16_t offset;     // Offset at which the array or struct starts.
    bool go_to_offset;   // If set, will force the parsing to iterate through parameters until
                         // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;



// #define TOKEN_SENT_FOUND     1
// #define TOKEN_RECEIVED_FOUND 1 << 1

#define DEFAULT_TICKER ""

#define ETH_TICKER "ETH"
#define WETH_TICKER "WETH"

#define DAI_TICKER "DAI"
#define DAI_DECIMALS WEI_TO_ETHER

#define USDC_TICKER "USDC"
#define USDC_DECIMALS 6

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


#define ADDRESS_IS_NETWORK_TOKEN(_addr)                  \
    !memcmp(_addr, NULL_ETH_ADDRESS, ADDRESS_LENGTH) 
#define ADDRESS_IS_WETH(_addr)   (!memcmp(_addr, WETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_STETH(_addr)  (!memcmp(_addr, STETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_RETH(_addr)   (!memcmp(_addr, RETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_FRXETH(_addr) (!memcmp(_addr, FRXETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_OUSD(_addr)   (!memcmp(_addr, OUSD_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_DAI(_addr)    (!memcmp(_addr, DAI_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDC(_addr)   (!memcmp(_addr, USDC_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDT(_addr)   (!memcmp(_addr, USDT_ADDRESS, ADDRESS_LENGTH))




// Check if the context structure will fit in the RAM section ETH will prepare for us
// Do not remove!
ASSERT_SIZEOF_PLUGIN_CONTEXT(context_t);


static inline const char *get_ticker_for_address(uint8_t address[ADDRESS_LENGTH]) {
    if(ADDRESS_IS_NETWORK_TOKEN(address)) {
        return ETH_TICKER;
    } else if (ADDRESS_IS_WETH(address)) {
        return WETH_TICKER;
    } else if (ADDRESS_IS_DAI(address)) {
        return DAI_TICKER; 
    } else if (ADDRESS_IS_USDC(address)) {
        return USDC_TICKER;
    }
}

static inline void printf_hex_array(const char *title __attribute__((unused)),
                                    size_t len __attribute__((unused)),
                                    const uint8_t *data __attribute__((unused))) {
        PRINTF(title);
        for (size_t i = 0; i < len; ++i){
            PRINTF("%02x", data[i]);
        }
        PRINTF("\n");
}
                    
static inline const uint8_t get_decimals_for_ticker(const char *ticker __attribute__((unused))) {
    if(strcmp(ticker, USDC_TICKER)== 0) {
        return USDC_DECIMALS;
    } else if(strcmp(ticker, WETH_TICKER) == 0) {
        return WEI_TO_ETHER;
    } else if(strcmp(ticker, DAI_TICKER) == 0) {
        return WEI_TO_ETHER;
    } else {
        return WETH_TICKER;
    }
}