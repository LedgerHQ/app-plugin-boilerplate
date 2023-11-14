#include "origin_plugin.h"

// EDIT THIS: Adapt this function to your needs! Remember, the information for
// tokens are held in `msg->token1` and `msg->token2`. If those pointers are
// `NULL`, this means the ethereum app didn't find any info regarding the
// requested tokens!
void handle_provide_token(void *parameters) {
    ethPluginProvideInfo_t *msg = (ethPluginProvideInfo_t *) parameters;
    origin_parameters_t *context = (origin_parameters_t *) msg->pluginContext;
    PRINTF("OETH plugin provide token: 0x%p, 0x%p\n", msg->item1, msg->item2);
    if (!check_token_sent(context)) {
        if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
            sent_network_token(context);
        } else if (msg->item1 != NULL) {
            context->decimals_sent = msg->item1->token.decimals;
            strlcpy(context->ticker_sent,
                    (char *) msg->item1->token.ticker,
                    sizeof(context->ticker_sent));
            context->tokens_found |= TOKEN_SENT_FOUND;
        } else {
            // CAL did not find the token and token is not ETH.
            context->decimals_sent = DEFAULT_DECIMAL;
            strlcpy(context->ticker_sent, DEFAULT_TICKER, sizeof(context->ticker_sent));
            // // We will need an additional screen to display a warning message.
            msg->additionalScreens++;
        }
    }

    if (!check_token_received(context)) {
        if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
            received_network_token(context);
        } else if (msg->item2 != NULL) {
            context->decimals_received = msg->item2->token.decimals;
            strlcpy(context->ticker_received,
                    (char *) msg->item2->token.ticker,
                    sizeof(context->ticker_received));
            context->tokens_found |= TOKEN_RECEIVED_FOUND;
        } else if (context->selectorIndex == VAULT_REDEEM) {
            context->decimals_received = DEFAULT_DECIMAL;
            strlcpy(context->ticker_received, "UNITS", sizeof(context->ticker_received));
        } else {
            // CAL did not find the token and token is not ETH.
            context->decimals_received = DEFAULT_DECIMAL;
            strlcpy(context->ticker_received, DEFAULT_TICKER, sizeof(context->ticker_received));
            // // We will need an additional screen to display a warning message.
            msg->additionalScreens++;
        }
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}