#include "plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    msg->numScreens = 2;
    // If the beneficiary is NOT the sender, we will need an additional screen to display it.
    if (memcmp(msg->address, context->beneficiary, ADDRESS_LENGTH) != 0) {
        msg->numScreens += 1;
    }



    // if(context->selectorIndex == APPROVE) {
    //     msg->numScreens = 2;
    // }

    // token addresses you will info for (such as decimals, ticker...).
    if(context->token_sent != NULL) {
        msg->tokenLookup1 = context->token_sent;
        context->token_sent_found = true;
        printf_hex_array("setting token sent to: ",
                        ADDRESS_LENGTH,
                        context->token_sent);
    } else {
        msg->tokenLookup1 = NULL;
        context->token_sent_found = false;
    }

    if(context->token_received != NULL) {
        msg->tokenLookup2 = context->token_received;
        context->token_received_found = true;
        printf_hex_array("setting token recieved to: ",
                        ADDRESS_LENGTH,
                        context->token_received);
    } else {
        msg->tokenLookup2 = NULL;
        context->token_received_found = false;

    }



    msg->result = ETH_PLUGIN_RESULT_OK;
}
