#include "plugin.h"
#include "screens.h"

// Printable ASCII of 1..MAX_SYMBOL_LEN characters, NUL-terminated within the buffer.
static bool ticker_ok(const char *ticker) {
    size_t len = strnlen(ticker, MAX_TICKER_LEN);

    if (len == 0 || len > MAX_SYMBOL_LEN) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        uint8_t c = (uint8_t) ticker[i];
        if (c < 0x20 || c > 0x7e) {
            return false;
        }
    }
    return true;
}

static void accept_token(context_t *context, uint8_t slot, const extraInfo_t *item) {
    lookup_t *lookup = &context->lookups[slot];

    if (item == NULL || lookup->source == LOOKUP_NONE) {
        return;
    }
    if (memcmp(item->token.address, lookup_token(context, lookup->source), ADDRESS_LENGTH) != 0 ||
        !ticker_ok(item->token.ticker) || item->token.decimals > MAX_DECIMALS) {
        return;
    }
    memcpy(lookup->ticker, item->token.ticker, strlen(item->token.ticker) + 1);
    lookup->decimals = item->token.decimals;
    lookup->accepted = 1;
}

void handle_provide_token(ethPluginProvideInfo_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->additionalScreens = 0;
    // A frozen decision is returned unchanged: a lookup never turns it into OK.
    if (context->outcome == OUTCOME_ERROR) {
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }
    if (context->outcome == OUTCOME_FALLBACK) {
        msg->result = ETH_PLUGIN_RESULT_FALLBACK;
        return;
    }
    if (context->outcome != OUTCOME_OK) {
        msg->result = erc8009_error(context, E_STATE, 0);
        return;
    }
    accept_token(context, 0, msg->item1);
    accept_token(context, 1, msg->item2);
    msg->result = ETH_PLUGIN_RESULT_OK;
}
