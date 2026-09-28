#include "plugin.h"
#include "parse_kinds.h"

// `parameter_size` is the size of the last APDU piece copied, not the word size: it is ignored.
void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    uint64_t expected_offset =
        SELECTOR_SIZE + (uint64_t) PARAMETER_LENGTH * (uint64_t) context->words_seen;
    uint8_t reason;

    if (context->outcome != OUTCOME_PARSING) {
        reason = E_STATE;
    } else if (msg->parameterOffset != expected_offset ||
               context->words_seen >= context->words_total) {
        reason = E_SEQ;
    } else {
        reason = parser_feed(context, msg->parameter);
    }

    if (reason != R_OK) {
        msg->result = erc8009_error(context, reason, msg->parameterOffset);
        return;
    }
    // A FALLBACK condition is only recorded here: it is returned by finalize.
    msg->result = ETH_PLUGIN_RESULT_OK;
}
