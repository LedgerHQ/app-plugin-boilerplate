#include "plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(ethQueryContractID_t *msg) {
    context_t *context = NULL;

    if ((!msg) || (!msg->pluginContext) || (!msg->name) || (!msg->version)) return;

    context = (context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, APPNAME, msg->nameLength);

    // EDIT THIS: Adapt the cases by modifying the strings you pass to `strlcpy`.
    if (context->selectorIndex == SWAP_EXACT_ETH_FOR_TOKENS) {
        strlcpy(msg->version, "Swap", msg->versionLength);
        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Selector index: %d not supported\n", context->selectorIndex);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}
