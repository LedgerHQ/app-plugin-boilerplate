#include "boilerplate_plugin.h"

// Called once to init.
void handle_init_contract(void *parameters) {
    // Cast the msg to the type of structure we expect (here, ethPluginInitContract_t).
    ethPluginInitContract_t *msg = (ethPluginInitContract_t *) parameters;

    // Make sure we are running a compatible version.
    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_1) {
        // If not the case, return the `UNAVAILABLE` status.
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // TODO: this could be removed as this can be checked statically?
    if (msg->pluginContextLength < sizeof(boilerplate_parameters_t)) {
        PRINTF("Plugin parameters structure is bigger than allowed size\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    boilerplate_parameters_t *context = (boilerplate_parameters_t *) msg->pluginContext;

    // Initialize the context (to 0).
    memset(context, 0, sizeof(*context));

    // Look for the index of the selectorIndex passed in by `msg`.
    uint8_t i;
    for (i = 0; i < NUM_BOILERPLATE_SELECTORS; i++) {
        if (memcmp((uint8_t *) PIC(BOILERPLATE_SELECTORS[i]), msg->selector, SELECTOR_SIZE) == 0) {
            context->selectorIndex = i;
            break;
        }
    }

    // If `i == NUM_BOILERPLATE_SELECTOR` it means we haven't found the selector. Return an error.
    if (i == NUM_BOILERPLATE_SELECTORS) {
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
    }

    // Set `next_param` to be the first field we expect to parse.
    // EDIT THIS: Adapt the `cases`, and set the `next_param` to be the first parameter you expect
    // to parse.
    switch (context->selectorIndex) {
        case BOILERPLATE_DUMMY_1:
            context->next_param = TOKEN_SENT;
            break;
        case BOILERPLATE_DUMMY_2:
            context->next_param = TOKEN_RECEIVED;
        default:
            PRINTF("Missing selectorIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Return valid status.
    msg->result = ETH_PLUGIN_RESULT_OK;
}
