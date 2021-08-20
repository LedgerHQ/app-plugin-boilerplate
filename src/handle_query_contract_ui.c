#include "boilerplate_plugin.h"

// EDIT THIS: You need to adapt / remove the static functions (set_send_ui, set_receive_ui ...) to
// match what you wish to display.

// Set UI for the "Send" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static void set_send_ui(ethQueryContractUI_t *msg, context_t *context) {
    switch (context->selectorIndex) {
        case SWAP_EXACT_ETH_FOR_TOKENS:
            strlcpy(msg->title, "Send", msg->titleLength);
            break;
        case BOILERPLATE_DUMMY_2:
            strlcpy(msg->title, "Send Max", msg->titleLength);
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Converts the uint256 number located in `context->amount_sent` to its string representation,
    // it copies this to `msg->msg`.
    amountToString(context->amount_sent,
                   sizeof(context->amount_sent),
                   context->decimals_sent,
                   context->ticker_sent,
                   msg->msg,
                   msg->msgLength);
}

// Set UI for "Receive" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static void set_receive_ui(ethQueryContractUI_t *msg, context_t *context) {
    switch (context->selectorIndex) {
        case SWAP_EXACT_ETH_FOR_TOKENS:
            strlcpy(msg->title, "Receive Min", msg->titleLength);
            break;
        case BOILERPLATE_DUMMY_2:
            strlcpy(msg->title, "Receive", msg->titleLength);
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    amountToString(context->amount_received,
                   sizeof(context->amount_received),
                   context->decimals_received,
                   context->ticker_received,
                   msg->msg,
                   msg->msgLength);
}

// Set UI for "Beneficiary" screen.
// EDIT THIS: Adapt / remove this function to your needs.
static void set_beneficiary_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Beneficiary", msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // Initialize an empty chainConfig because it's needed by `getEthAddressStringFromBinary`.
    chain_config_t chainConfig = {0};

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    getEthAddressStringFromBinary(
        (uint8_t *) context->beneficiary,
        (uint8_t *) msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        &chainConfig);
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    screens_t screen = SEND_SCREEN;  // TODO
    // EDIT THIS: Adapt the cases for the screens you'd like to display.
    switch (screen) {
        case SEND_SCREEN:
            set_send_ui(msg, context);
            break;
        case RECEIVE_SCREEN:
            set_receive_ui(msg, context);
            break;
        case BENEFICIARY_SCREEN:
            set_beneficiary_ui(msg, context);
            break;
        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
