#include <stdbool.h>
#include "origin_plugin.h"

// EDIT THIS: You need to adapt / remove the static functions (set_send_ui, set_receive_ui ...) to
// match what you wish to display.

// Set UI for the "Send" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static bool set_send_ui(ethQueryContractUI_t *msg, origin_parameters_t *context) {
    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, msg->network_ticker, sizeof(context->ticker_sent));
    } else if (ADDRESS_IS_OETH(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, OETH_TICKER, sizeof(context->ticker_sent));
    } else if (ADDRESS_IS_OUSD(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, OUSD_TICKER, sizeof(context->ticker_sent));
    } else if (ADDRESS_IS_DAI(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, DAI_TICKER, sizeof(context->ticker_sent));
    } else if (ADDRESS_IS_USDC(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, USDC_TICKER, sizeof(context->ticker_sent));
    } else if (ADDRESS_IS_USDT(context->contract_address_sent)) {
        strlcpy(context->ticker_sent, USDT_TICKER, sizeof(context->ticker_sent));
    }

    context->amount_length = INT256_LENGTH;

    strlcpy(msg->title, "Send", msg->titleLength);

    switch (context->selectorIndex) {
        case ZAPPER_DEPOSIT_ETH:
            strlcpy(msg->title, "Deposit", msg->titleLength);
            memcpy(context->amount_sent,
                   &msg->pluginSharedRO->txContent->value.value,
                   msg->pluginSharedRO->txContent->value.length);
            context->amount_length = msg->pluginSharedRO->txContent->value.length;
            break;
        case ZAPPER_DEPOSIT_SFRXETH:
            strlcpy(msg->title, "Deposit", msg->titleLength);
            strlcpy(context->ticker_sent, SFRXETH_TICKER, sizeof(context->ticker_sent));
            break;
        case VAULT_MINT:
            strlcpy(msg->title, "Deposit", msg->titleLength);
            break;
        case VAULT_REDEEM:
            strlcpy(msg->title, "Redeem", msg->titleLength);
            if (memcmp(OETH_VAULT_ADDRESS,
                       msg->pluginSharedRO->txContent->destination,
                       ADDRESS_LENGTH) == 0) {
                strlcpy(context->ticker_sent, OETH_TICKER, sizeof(context->ticker_sent));
            } else {
                strlcpy(context->ticker_sent, OUSD_TICKER, sizeof(context->ticker_sent));
            }
            break;
        case CURVE_POOL_EXCHANGE:
            break;
        case CURVE_POOL_EXCHANGE_UNDERLYING:
            break;
        case CURVE_ROUTER_EXCHANGE_MULTIPLE:
        case UNISWAP_ROUTER_EXACT_INPUT:
        case UNISWAP_ROUTER_EXACT_INPUT_SINGLE:
            break;
        case FLIPPER_BUY_OUSD_WITH_USDT:
            strlcpy(context->ticker_sent, USDT_TICKER, sizeof(context->ticker_sent));
            context->decimals_sent = DEFAULT_DECIMAL;
            break;
        case FLIPPER_BUY_OUSD_WITH_DAI:
            strlcpy(context->ticker_sent, DAI_TICKER, sizeof(context->ticker_sent));
            context->decimals_sent = DAI_DECIMALS;
            break;
        case FLIPPER_BUY_OUSD_WITH_USDC:
            strlcpy(context->ticker_sent, USDC_TICKER, sizeof(context->ticker_sent));
            context->decimals_sent = DEFAULT_DECIMAL;
            break;
        case FLIPPER_SELL_OUSD_FOR_USDT:
        case FLIPPER_SELL_OUSD_FOR_DAI:
        case FLIPPER_SELL_OUSD_FOR_USDC:
            strlcpy(context->ticker_sent, OUSD_TICKER, sizeof(context->ticker_sent));
            context->decimals_sent = OUSD_DECIMALS;
            break;
        case WRAP:
            strlcpy(msg->title, "Wrap", msg->titleLength);
            if (memcmp(WOETH_ADDRESS,
                       msg->pluginSharedRO->txContent->destination,
                       ADDRESS_LENGTH) == 0) {
                strlcpy(context->ticker_sent, OETH_TICKER, sizeof(context->ticker_sent));
            } else {
                strlcpy(context->ticker_sent, OUSD_TICKER, sizeof(context->ticker_sent));
            }
            break;
        case UNWRAP:
            strlcpy(msg->title, "Unwrap", msg->titleLength);
            if (memcmp(WOETH_ADDRESS,
                       msg->pluginSharedRO->txContent->destination,
                       ADDRESS_LENGTH) == 0) {
                strlcpy(context->ticker_sent, WOETH_TICKER, sizeof(context->ticker_sent));
            } else {
                strlcpy(context->ticker_sent, WOUSD_TICKER, sizeof(context->ticker_sent));
            }
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return false;
    }

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(context->amount_sent,
                          context->amount_length,
                          context->decimals_sent,
                          context->ticker_sent,
                          msg->msg,
                          msg->msgLength);
}

// Set UI for "Receive" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static bool set_receive_ui(ethQueryContractUI_t *msg, origin_parameters_t *context) {
    // set network ticker (ETH, BNB, etc) if needed
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
        strlcpy(context->ticker_received, msg->network_ticker, sizeof(context->ticker_received));
    } else if (ADDRESS_IS_OETH(context->contract_address_received)) {
        strlcpy(context->ticker_received, OETH_TICKER, sizeof(context->ticker_received));
    } else if (ADDRESS_IS_OUSD(context->contract_address_received)) {
        strlcpy(context->ticker_received, OUSD_TICKER, sizeof(context->ticker_received));
    } else if (ADDRESS_IS_DAI(context->contract_address_received)) {
        strlcpy(context->ticker_received, DAI_TICKER, sizeof(context->ticker_received));
    } else if (ADDRESS_IS_USDC(context->contract_address_received)) {
        strlcpy(context->ticker_received, USDC_TICKER, sizeof(context->ticker_received));
    } else if (ADDRESS_IS_USDT(context->contract_address_received)) {
        strlcpy(context->ticker_received, USDT_TICKER, sizeof(context->ticker_received));
    }

    context->amount_length = INT256_LENGTH;

    strlcpy(msg->title, "Receive Min", msg->titleLength);

    switch (context->selectorIndex) {
        case ZAPPER_DEPOSIT_ETH:
            memcpy(context->min_amount_received,
                   &msg->pluginSharedRO->txContent->value.value,
                   msg->pluginSharedRO->txContent->value.length);
            context->amount_length = msg->pluginSharedRO->txContent->value.length;
            strlcpy(context->ticker_received, OETH_TICKER, sizeof(context->ticker_received));
            break;
        case ZAPPER_DEPOSIT_SFRXETH:
        case VAULT_MINT:
            if (memcmp(OETH_VAULT_ADDRESS,
                       msg->pluginSharedRO->txContent->destination,
                       ADDRESS_LENGTH) == 0) {
                strlcpy(context->ticker_received, OETH_TICKER, sizeof(context->ticker_received));
            } else {
                strlcpy(context->ticker_received, OUSD_TICKER, sizeof(context->ticker_received));
            }
            break;
        case VAULT_REDEEM:
            if (memcmp(OETH_VAULT_ADDRESS,
                       msg->pluginSharedRO->txContent->destination,
                       ADDRESS_LENGTH) == 0) {
                strlcpy(context->ticker_received, "LST MIX", sizeof(context->ticker_received));
            } else {
                strlcpy(context->ticker_received, "USD MIX", sizeof(context->ticker_received));
            }
            break;
            break;
        case CURVE_POOL_EXCHANGE:
            break;
        case CURVE_POOL_EXCHANGE_UNDERLYING:
            break;
        case CURVE_ROUTER_EXCHANGE_MULTIPLE:
        case UNISWAP_ROUTER_EXACT_INPUT:
        case UNISWAP_ROUTER_EXACT_INPUT_SINGLE:
            break;
        case FLIPPER_BUY_OUSD_WITH_USDT:
        case FLIPPER_BUY_OUSD_WITH_DAI:
        case FLIPPER_BUY_OUSD_WITH_USDC:
            memcpy(context->min_amount_received,
                   context->amount_sent,
                   sizeof(context->amount_sent));
            strlcpy(context->ticker_received, OUSD_TICKER, sizeof(context->ticker_received));
            context->decimals_received = OUSD_DECIMALS;
            break;
        case FLIPPER_SELL_OUSD_FOR_USDT:
            memcpy(context->min_amount_received,
                   context->amount_sent,
                   sizeof(context->amount_sent));
            strlcpy(context->ticker_received, USDT_TICKER, sizeof(context->ticker_received));
            context->decimals_received = DEFAULT_DECIMAL;
            break;
        case FLIPPER_SELL_OUSD_FOR_DAI:
            memcpy(context->min_amount_received,
                   context->amount_sent,
                   sizeof(context->amount_sent));
            strlcpy(context->ticker_received, DAI_TICKER, sizeof(context->ticker_received));
            context->decimals_received = DAI_DECIMALS;
            break;
        case FLIPPER_SELL_OUSD_FOR_USDC:
            memcpy(context->min_amount_received,
                   context->amount_sent,
                   sizeof(context->amount_sent));
            strlcpy(context->ticker_received, USDC_TICKER, sizeof(context->ticker_received));
            context->decimals_received = DEFAULT_DECIMAL;
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return false;
    }

    // Convert to string.
    return amountToString(context->min_amount_received,
                          context->amount_length,
                          context->decimals_received,
                          context->ticker_received,
                          msg->msg,
                          msg->msgLength);
}

// Set UI for "Warning" screen.
static bool set_warning_ui(ethQueryContractUI_t *msg,
                           const origin_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
    return true;
}

// Set UI for "Beneficiary" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static bool set_beneficiary_ui(ethQueryContractUI_t *msg, origin_parameters_t *context) {
    strlcpy(msg->title, "Beneficiary", msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary((uint8_t *) context->beneficiary,
                                          msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
                                          msg->pluginSharedRW->sha3,
                                          chainid);
}

// Helper function that returns the enum corresponding to the screen that should
// be displayed.
static screens_t get_screen(const ethQueryContractUI_t *msg, const origin_parameters_t *context) {
    uint8_t index = msg->screenIndex;

    bool token_sent_found = context->tokens_found & TOKEN_SENT_FOUND;
    bool token_received_found = context->tokens_found & TOKEN_RECEIVED_FOUND;

    bool both_tokens_found = token_received_found && token_sent_found;
    bool both_tokens_not_found = !token_received_found && !token_sent_found;

    bool wrap = context->selectorIndex == WRAP || context->selectorIndex == UNWRAP;

    // EDIT THIS: Adapt the cases for the screens you'd like to display.
    switch (index) {
        case 0:
            if (both_tokens_found) {
                return SEND_SCREEN;
            } else if (both_tokens_not_found) {
                return WARN_SCREEN;
            } else if (token_sent_found) {
                return SEND_SCREEN;
            } else if (token_received_found) {
                return WARN_SCREEN;
            }
            break;
        case 1:
            if (wrap) {
                return BENEFICIARY_SCREEN;
            } else if (both_tokens_found) {
                return RECEIVE_SCREEN;
            } else if (both_tokens_not_found) {
                return SEND_SCREEN;
            } else if (token_sent_found) {
                return WARN_SCREEN;
            } else if (token_received_found) {
                return SEND_SCREEN;
            }
            break;
        case 2:
            if (both_tokens_found) {
                return BENEFICIARY_SCREEN;
            } else if (both_tokens_not_found) {
                return WARN_SCREEN;
            } else {
                return RECEIVE_SCREEN;
            }
            break;
        case 3:
            if (both_tokens_found) {
                return ERROR;
            } else if (both_tokens_not_found) {
                return RECEIVE_SCREEN;
            } else {
                return BENEFICIARY_SCREEN;
            }
            break;
        case 4:
            if (both_tokens_found) {
                return ERROR;
            } else if (both_tokens_not_found) {
                return BENEFICIARY_SCREEN;
            }
            break;
        default:
            return ERROR;
            break;
    }
    return ERROR;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    origin_parameters_t *context = (origin_parameters_t *) msg->pluginContext;
    bool ret = false;
    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);
    msg->result = ETH_PLUGIN_RESULT_OK;
    screens_t screen = get_screen(msg, context);
    switch (screen) {
        case SEND_SCREEN:
            ret = set_send_ui(msg, context);
            break;
        case RECEIVE_SCREEN:
            ret = set_receive_ui(msg, context);
            break;
        case WARN_SCREEN:
            ret = set_warning_ui(msg, context);
            break;
        case BENEFICIARY_SCREEN:
            ret = set_beneficiary_ui(msg, context);
            break;

        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
