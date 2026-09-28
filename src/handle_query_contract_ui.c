#include "plugin.h"
#include "screens.h"

// One pair per call; the pair list is the one counted in finalize (resolve_screen).
void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    uint8_t reason = E_STATE;

    if (context->outcome == OUTCOME_OK && msg->screenIndex < context->num_screens) {
        if (msg->title == NULL || msg->msg == NULL || msg->titleLength == 0 ||
            msg->msgLength == 0) {
            reason = E_RENDER;
        } else {
            memset(msg->title, 0, msg->titleLength);
            memset(msg->msg, 0, msg->msgLength);
            resolve_screen(context, msg, &reason);
        }
    }

    if (reason != R_OK) {
        msg->result = erc8009_error(context, reason, msg->screenIndex);
        return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}
