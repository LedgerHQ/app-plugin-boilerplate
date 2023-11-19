#pragma once

#include <string.h>

#include "eth_internals.h"
#include "eth_plugin_interface.h"

// Number of selectors defined in this plugin. Should match the enum
// `selector_t`. EDIT THIS: Put in the number of selectors your plugin is going
// to support.
#define NUM_SELECTORS    17
#define PARAMETER_LENGTH 32

// Name of the plugin.
// EDIT THIS: Replace with your plugin name.
#define PLUGIN_NAME "Origin DeFi"

#define TOKEN_SENT_FOUND     1
#define TOKEN_RECEIVED_FOUND 1 << 1

// Number of decimals used when the token wasn't found in the CAL.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the CAL.
#define DEFAULT_TICKER ""

#define OETH_TICKER   "OETH"
#define OETH_DECIMALS WEI_TO_ETHER

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

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
// EDIT THIS: Change the naming (`selector_t`), and add your selector names.
typedef enum {
    ZAPPER_DEPOSIT_ETH,
    ZAPPER_DEPOSIT_SFRXETH,
    VAULT_MINT,
    VAULT_REDEEM,
    CURVE_POOL_EXCHANGE,
    CURVE_POOL_EXCHANGE_UNDERLYING,
    CURVE_ROUTER_EXCHANGE_MULTIPLE,
    UNISWAP_ROUTER_EXACT_INPUT,
    UNISWAP_ROUTER_EXACT_INPUT_SINGLE,
    FLIPPER_BUY_OUSD_WITH_USDT,
    FLIPPER_SELL_OUSD_FOR_USDT,
    FLIPPER_BUY_OUSD_WITH_DAI,
    FLIPPER_SELL_OUSD_FOR_DAI,
    FLIPPER_BUY_OUSD_WITH_USDC,
    FLIPPER_SELL_OUSD_FOR_USDC,
    WRAP,
    UNWRAP
} selector_t;

typedef enum {
    SEND_SCREEN,
    RECEIVE_SCREEN,
    WARN_SCREEN,
    BENEFICIARY_SCREEN,
    ERROR,
} screens_t;

extern const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t STETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t OETH_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t OUSD_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t DAI_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t USDC_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t USDT_ADDRESS[ADDRESS_LENGTH];

extern const uint8_t OETH_VAULT_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t CURVE_OETH_POOL_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t CURVE_OUSD_POOL_ADDRESS[ADDRESS_LENGTH];
extern const uint8_t WOETH_ADDRESS[ADDRESS_LENGTH];

#define ADDRESS_IS_NETWORK_TOKEN(_addr) (!memcmp(_addr, NULL_ETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_OETH(_addr)          (!memcmp(_addr, OETH_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_OUSD(_addr)          (!memcmp(_addr, OUSD_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_DAI(_addr)           (!memcmp(_addr, DAI_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDC(_addr)          (!memcmp(_addr, USDC_ADDRESS, ADDRESS_LENGTH))
#define ADDRESS_IS_USDT(_addr)          (!memcmp(_addr, USDT_ADDRESS, ADDRESS_LENGTH))
// #define ADDRESS_IS_FRXETH(_addr) (!memcmp(_addr, FRXETH_ADDRESS,
//  ADDRESS_LENGTH))

// Enumeration used to parse the smart contract data.
// EDIT THIS: Adapt the parameter names here.
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

// EDIT THIS: Rename `BOILERPLATE` to be the same as the one initialized in
// `main.c`.
extern const uint32_t ORIGIN_SELECTORS[NUM_SELECTORS];

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
// EDIT THIS: This struct is used by your plugin to save the parameters you
// parse. You will need to adapt this struct to your plugin.
typedef struct origin_parameters_t {
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
    // uint16_t checkpoint;
    uint16_t offset;
    // bool go_to_offset;   // If set, will force the parsing to iterate through
    // parameters until
    // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} origin_parameters_t;

// Piece of code that will check that the above structure is not bigger than 5
// * 32. Do not remove this check.
_Static_assert(sizeof(origin_parameters_t) <= 5 * 32, "Structure of parameters too big.");

static inline bool check_token_sent(origin_parameters_t *context) {
    if (ADDRESS_IS_OETH(context->contract_address_sent)) {
        context->decimals_sent = OETH_DECIMALS;
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

static inline bool check_token_received(origin_parameters_t *context) {
    if (ADDRESS_IS_OETH(context->contract_address_received)) {
        context->decimals_received = OETH_DECIMALS;
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

static inline void sent_network_token(origin_parameters_t *context) {
    context->decimals_sent = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_SENT_FOUND;
}

static inline void received_network_token(origin_parameters_t *context) {
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