#include "boilerplate_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    boilerplate_parameters_t *context = (boilerplate_parameters_t *) msg->pluginContext;

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    // msg->name will be the upper line displayed on the screen.
    // msg->version will be the lower line displayed on the screen.
    // EDIT THIS: Adapt the cases by modifying the strings you pass to `strlcpy`.
    switch (context->selectorIndex) {
        case BOILERPLATE_DUMMY_1:
            strlcpy(msg->version, "Send", msg->versionLength);
            break;
        case BOILERPLATE_DUMMY_2:
            strlcpy(msg->version, "Receive", msg->versionLength);
            break;
        default:
            PRINTF("Selector Index :%d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}