#include "plugin.h"

// EDIT THIS: You need to adapt / remove the static functions (set_send_ui, set_receive_ui ...) to
// match what you wish to display.

// Set UI for the "Send" screen.
static bool set_send_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Send", msg->titleLength);

    const uint8_t *token_amount;
    uint8_t token_amount_size;
    // char *ticker;
    uint8_t decimals;


    switch (context->selectorIndex) {

        case SWAP_EXACT_ETH_FOR_TOKENS:
             token_amount = msg->pluginSharedRO->txContent->value.value;
             token_amount_size = msg->pluginSharedRO->txContent->value.length;
             strlcpy(context->ticker_sent,"ETH",sizeof(context->ticker_sent));
             decimals = WEI_TO_ETHER;
             break;
        case SWAP_EXACT_TOKENS_FOR_ETH:
             strlcpy(context->ticker_sent, 
                     get_ticker_for_address(context->token_sent), 
                     sizeof(context->ticker_sent));
             printf_hex_array("TOKEN SENT: ", ADDRESS_LENGTH, context->token_received);
             token_amount = context->amount_sent;
             token_amount_size = msg->pluginSharedRO->txContent->value.length;
             decimals =  get_decimals_for_ticker(context->ticker_sent);
             break;
        default:
             PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
             msg->result = ETH_PLUGIN_RESULT_ERROR;
             return false;

    }

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(token_amount,
                        token_amount_size,
                        decimals,
                        context->ticker_sent,
                        msg->msg,
                        msg->msgLength);
}


// Set UI for "Receive" screen.
static bool set_receive_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Receive Min.", msg->titleLength);

    uint8_t decimals;
    uint8_t token_amount_size;



    switch (context->selectorIndex) {

        case SWAP_EXACT_ETH_FOR_TOKENS:
             strlcpy(context->ticker_received, 
                    get_ticker_for_address(context->token_received), 
                        sizeof(context->ticker_received));
             printf_hex_array("TOKEN RECEIVED: ", ADDRESS_LENGTH, context->token_received);
             decimals =  get_decimals_for_ticker(context->ticker_received);
             PRINTF("DECIMALS RECEIVED: %d\n", decimals);
             PRINTF("token_received... %s\n", context->ticker_received);
             break;
        case SWAP_EXACT_TOKENS_FOR_ETH: 
             strlcpy(context->amount_received,msg->pluginSharedRO->txContent->value.value,sizeof(context->amount_received));
             token_amount_size = msg->pluginSharedRO->txContent->value.length;
             strlcpy(context->ticker_sent,"ETH",sizeof(context->ticker_sent));
             decimals = WEI_TO_ETHER;
             break;
        default: 
             PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
             msg->result = ETH_PLUGIN_RESULT_ERROR;
             return false;

    }

    // If the token look up failed, use the default network ticker along with the default decimals.
    if (!context->token_received_found) {
        decimals = WEI_TO_ETHER;
        strlcpy(context->ticker_received, 
                msg->network_ticker, 
                sizeof(context->ticker_received));
    }

    return amountToString(context->amount_received,
                          sizeof(context->amount_received),
                          decimals,
                          context->ticker_received,
                          msg->msg,
                          msg->msgLength);
}

// Set UI for "Beneficiary" screen.
static bool set_beneficiary_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Beneficiary", msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        context->beneficiary,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);


    bool token_sent_found = context->token_sent_found;
    bool token_received_found = context->token_received_found;

    // EDIT THIS: Adapt the cases for the screens you'd like to display.
    switch (msg->screenIndex) {
        case 0:
            ret = set_send_ui(msg, context);
            break;
        case 1:
            ret = set_receive_ui(msg, context);
            break;
        case 2:
            ret = set_beneficiary_ui(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex\n");
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
