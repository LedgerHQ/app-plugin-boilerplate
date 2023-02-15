#include "pstake_plugin.h"

static void handle_stkbnb_send(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case RECIPIENT:  // recipient
            copy_address(context->recipient, msg->parameter, sizeof(context->recipient));
            context->next_param = AMOUNT;
            break;
        case AMOUNT:  // amount
            copy_parameter(context->amount, msg->parameter, sizeof(context->amount));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        case UNEXPECTED_PARAMETER:  // skip rest of the params as we don't need to parse them
            // do nothing
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case DEPOSIT:
            // Do nothing
            break;
        case CLAIM_ALL:
            // Do nothing
            break;
        case STKBNB_SEND:
            handle_stkbnb_send(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}