#include "plugin.h"

// EDIT THIS: Adapt this function to your needs! Remember, the information for tokens are held in
// `msg->token1` and `msg->token2`. If those pointers are `NULL`, this means the ethereum app didn't
// find any info regarding the requested tokens!
void handle_provide_token(ethPluginProvideInfo_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    printf_hex_array("msg->item1: ",
                        ADDRESS_LENGTH,
                        msg->item1);
    if (msg->item1) {
        // The Ethereum App found the information for the requested token!
        // Store its decimals.
        // context->decimals_sent = msg->item1->token.decimals;
        // // Store its ticker.
        // strlcpy(context->ticker_sent, (char *) msg->item1->token.ticker, sizeof(context->ticker_sent));

        // Keep track that we found the token.
        context->token_sent_found = true;
        PRINTF("Setting token lookup1 true");
        // strlcpy(context->ticker_sent, 
        //         get_ticker_for_address(context->token_received), 
        //         sizeof(context->ticker_sent));
        // strlcpy(context->decimals_sent, 
        //         get_decimals_for_ticker(context->ticker_sent),
        //         sizeof(context->decimals_sent));
        // printf_hex_array("TOKEN SENT: ", ADDRESS_LENGTH, context->token_sent);

    }
    else {
        // The Ethereum App did not manage to find the info for the requested token.
        context->token_sent_found = false;
        PRINTF("Setting token lookup1 false");

        // If we wanted to add a screen, say a warning screen for example, we could instruct the
        // ethereum app to add an additional screen by setting `msg->additionalScreens` here, just
        // like so:
        // msg->additionalScreens = 1;
    }

    if (msg->item2) {
        // context->decimals_recieved = msg->item2->token.decimals;
        // strlcpy(context->ticker_received, (char *) msg->item2->token.ticker, sizeof(context->ticker_received));

        context->token_received_found = true;
        PRINTF("Setting token lookup2 true");
        // strlcpy(context->ticker_received, 
        //         get_ticker_for_address(context->token_received), 
        //         sizeof(context->ticker_received));
        // printf_hex_array("TOKEN RECEIVED: ", ADDRESS_LENGTH, context->token_received);
    } else {
        context->token_received_found = false;
        PRINTF("Setting token lookup2 false");
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}
