#include "plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // Number of screens needed
    msg->numScreens = 2;

    bool sender_is_beneficiary = memcmp(msg->address, context->beneficiary, ADDRESS_LENGTH) == 0;

    if (sender_is_beneficiary) {
        // No need to show beneficary screen when signer is same as `beneficiary`
        bool wrap_tx = (context->selectorIndex == WRAP || context->selectorIndex == UNWRAP);
        bool uniswap_tx = (context->selectorIndex == UNISWAP_V3_ROUTER_EXACT_INPUT ||
                           context->selectorIndex == UNISWAP_ROUTER_EXACT_INPUT_SINGLE);

        if (wrap_tx) {
            msg->numScreens -= 1;
        } else if (uniswap_tx) {
            msg->numScreens += 1;
        }
    }
    if (!ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
        // Address is not network token (0xeee...) so we will need to look up the
        // token in the CAL.
        printf_hex_array("Setting address sent to: ",
                         ADDRESS_LENGTH,
                         context->contract_address_sent);
        msg->tokenLookup1 = context->contract_address_sent;
    } else {
        sent_network_token(context);
        msg->tokenLookup1 = NULL;
    }
    if (!ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
        // Address is not network token (0xeee...) so we will need to look up the
        // token in the CAL.
        printf_hex_array("Setting address received to: ",
                         ADDRESS_LENGTH,
                         context->contract_address_received);
        msg->tokenLookup2 = context->contract_address_received;
    } else {
        received_network_token(context);
        msg->tokenLookup2 = NULL;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
