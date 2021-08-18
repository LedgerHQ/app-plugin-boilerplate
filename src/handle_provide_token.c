#include "boilerplate_plugin.h"

void handle_provide_token(void *parameters) {
    ethPluginProvideToken_t *msg = (ethPluginProvideToken_t *) parameters;
    boilerplate_parameters_t *context = (boilerplate_parameters_t *) msg->pluginContext;

    if (msg->token1) {
        context->decimals_sent = msg->token1->decimals;
        strlcpy(context->ticker_sent, (char *) msg->token1->ticker, sizeof(context->ticker_sent));

        // Keep track that we found the token.
        context->tokens_found |= TOKEN_SENT_FOUND;
    } else {
        context->decimals_sent = DEFAULT_DECIMAL;
        strlcpy(context->ticker_sent, DEFAULT_TICKER, sizeof(context->ticker_sent));

        // We will need an additional screen to display a warning message.
        // msg->additionalScreens++;
    }
    if (msg->token2) {
        context->decimals_received = msg->token2->decimals;
        strlcpy(context->ticker_received,
                (char *) msg->token2->ticker,
                sizeof(context->ticker_received));

        // Keep track that we found the token.
        context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else {
        context->decimals_received = DEFAULT_DECIMAL;
        strlcpy(context->ticker_received, DEFAULT_TICKER, sizeof(context->ticker_received));

        // We will need an additional screen to display a warning message.
        // msg->additionalScreens++;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}