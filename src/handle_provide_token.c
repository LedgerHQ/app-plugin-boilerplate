#include "plugin.h"

// The information for tokens are held in `msg->token1` and `msg->token2`.
// If those pointers are `NULL`, this means the ethereum app didn't
// find any info regarding the requested tokens!
void handle_provide_token(ethPluginProvideInfo_t *msg) {
  context_t *context = (context_t *)msg->pluginContext;

  PRINTF("Origin plugin provide token: 0x%p, 0x%p\n", msg->item1, msg->item2);
  if (!check_token_sent(context)) {
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_sent)) {
      sent_network_token(context);
    } else if (msg->item1 != NULL) {
      context->decimals_sent = msg->item1->token.decimals;
      strlcpy(context->ticker_sent, (char *)msg->item1->token.ticker,
              sizeof(context->ticker_sent));
      context->tokens_found |= TOKEN_SENT_FOUND;
    } else {
      // The Ethereum App found the information for the requested token!
      // Store its decimals.
      context->decimals_sent = DEFAULT_DECIMAL;
      // Store its ticker.
      strlcpy(context->ticker_sent, DEFAULT_TICKER,
              sizeof(context->ticker_sent));
      // // We will need an additional screen to display a warning message.
      msg->additionalScreens++;
    }
  }

  if (!check_token_received(context)) {
    if (ADDRESS_IS_NETWORK_TOKEN(context->contract_address_received)) {
      received_network_token(context);
    } else if (msg->item2 != NULL) {
      context->decimals_received = msg->item2->token.decimals;
      strlcpy(context->ticker_received, (char *)msg->item2->token.ticker,
              sizeof(context->ticker_received));
      context->tokens_found |= TOKEN_RECEIVED_FOUND;
    } else if (context->selectorIndex == VAULT_REDEEM) {
      context->decimals_received = DEFAULT_DECIMAL;
      strlcpy(context->ticker_received, "UNITS",
              sizeof(context->ticker_received));
    } else {
      // The Ethereum App did not manage to find the info for the requested
      // token.
      context->decimals_received = DEFAULT_DECIMAL;
      strlcpy(context->ticker_received, DEFAULT_TICKER,
              sizeof(context->ticker_received));
      // If we wanted to add a screen, say a warning screen for example, we
      // could instruct the ethereum app to add an additional screen by setting
      // `msg->additionalScreens` here, just like so:
      msg->additionalScreens++;
    }
  }

  msg->result = ETH_PLUGIN_RESULT_OK;
}
