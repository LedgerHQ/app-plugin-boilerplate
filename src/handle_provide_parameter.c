#include "boilerplate_plugin.h"

// Copies the whole parameter (32 bytes long) from `src` to `dst`.
// Useful for numbers, data...
static copy_parameter(char *dst, size_t dst_len, char *src) {
    size_t len = MIN(dst_len, PARAMETER_LENGTH);
    memcpy(dst, src, len);
}

// Copies a 20 byte address (located in a 32 bytes parameter) `from `src` to `dst`.
// Useful for token addresses, user addresses.
static copy_address(char *dst, size_t dst_len, char *src) {
    size_t offset = PARAMETER_LENGTH - ADDRESS_LENGTH;
    memcpy(dst, &src[offset], ADDRESS_LENGTH);
}

static void handle_dummy_one(ethPluginProvideParameter_t *msg, boilerplate_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:  // fromToken
            copy_address(context->contract_address_sent,
                         sizeof(context->contract_address_sent),
                         msg->parameter);
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:  // toToken
            copy_address(context->contract_address_received,
                         sizeof(context->contract_address_received),
                         msg->parameter);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:  // fromAmount
            copy_parameter(context->amount_sent, sizeof(context->amount_sent), msg->parameter);
            context->next_param = AMOUNT_RECEIVED;
            break;
        case AMOUNT_RECEIVED:  // toAmount
            copy_parameter(context->amount_received,
                           sizeof(context->amount_received),
                           msg->parameter);
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:  // beneficiary
            copy_address(context->beneficiary, sizeof(context->beneficiary), msg->parameter);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    boilerplate_parameters_t *context = (boilerplate_parameters_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    // EDIT THIS: adapt the cases and the names of the functions.
    switch (context->selectorIndex) {
        case BOILERPLATE_DUMMY_1:
            handle_dummy_one(msg, context);
            break;
        default:
            PRINTF("Selector Index %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}