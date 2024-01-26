#include "plugin.h"

static void handle_swap_exact_eth_for_tokens(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case MIN_AMOUNT_RECEIVED:  // amountOutMin
            copy_parameter(context->amount_received,
                           msg->parameter,
                           sizeof(context->amount_received));
            context->next_param = PATH_OFFSET;
            break;
        case PATH_OFFSET:  // path
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:  // to
            copy_address(context->beneficiary, msg->parameter, sizeof(context->beneficiary));
            context->next_param = PATH_LENGTH;
            context->go_to_offset = true;
            break;
        case PATH_LENGTH:
            context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH * 2;
            context->go_to_offset = true;
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:  // path[1] -> contract address of token received
            copy_address(context->token_received, msg->parameter, sizeof(context->token_received));
            printf_hex_array("TOKEN RECEIVED: ", ADDRESS_LENGTH, context->token_received);
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_swap_exact_tokens_for_eth (ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    PRINTF("amount_sent %d amount_reci: %d", AMOUNT_SENT, MIN_AMOUNT_RECEIVED);
    switch (context->next_param) {
        case AMOUNT_SENT:
            copy_parameter(context->amount_sent,
                           msg->parameter,
                           sizeof(context->amount_sent));
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            copy_parameter(context->amount_received, 
                            msg->parameter,
                            sizeof(context->amount_received));
            context->next_param = PATH_OFFSET;
            break;
        case PATH_OFFSET:  // path
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:  // to
            copy_address(context->beneficiary, msg->parameter, sizeof(context->beneficiary));
            context->next_param = PATH_LENGTH;
            context->go_to_offset = true;
            break;
        case PATH_LENGTH:
            context->offset = msg->parameterOffset - SELECTOR_SIZE + PARAMETER_LENGTH;
            context->go_to_offset = true;
            context->next_param = TOKEN_SENT;
            break;
        case TOKEN_SENT:
            copy_address(context->token_sent, msg->parameter, sizeof(context->token_sent));
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:  // path[1] -> contract address of token received
            copy_address(context->token_received, msg->parameter, sizeof(context->token_received));
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("expand plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    if (context->skip) {
        context->skip--;
        return;
    }

    // EDIT THIS: adapt the cases and the names of the functions.
    switch (context->selectorIndex) {
        case SWAP_EXACT_ETH_FOR_TOKENS:
            handle_swap_exact_eth_for_tokens(msg, context);
            break;
        case SWAP_EXACT_TOKENS_FOR_ETH:
            handle_swap_exact_tokens_for_eth(msg, context);
            PRINTF("insideC",msg, "completed");
            break;
        case BOILERPLATE_DUMMY_2:
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
