#include "plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // EDIT THIS: Set the total number of screen you will need.
    msg->numScreens = 2;
    // EDIT THIS: Handle this case like you wish to (i.e. maybe no additional screen needed?).
    // If the beneficiary is NOT the sender, we will need an additional screen to display it.
    if (memcmp(msg->address, context->beneficiary, ADDRESS_LENGTH) != 0) {
        msg->numScreens += 1;
    }

    // EDIT THIS: set `tokenLookup1` (and maybe `tokenLookup2`) to point to
    // token addresses you will info for (such as decimals, ticker...).
    if(context->token_sent != NULL) {
        msg->tokenLookup1 = context->token_sent;
        printf_hex_array("setting token sent to: ",
                        ADDRESS_LENGTH,
                        context->token_sent);
    } else {
        msg->tokenLookup1 = NULL;
    }

    if(context->token_received != NULL) {
        msg->tokenLookup2 = context->token_received;
        printf_hex_array("setting token recieved to: ",
                        ADDRESS_LENGTH,
                        context->token_received);
    } else {
        msg->tokenLookup2 = NULL;
    }



    msg->result = ETH_PLUGIN_RESULT_OK;
}
