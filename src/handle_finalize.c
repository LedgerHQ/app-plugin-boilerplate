#include "plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    switch (context->selectorIndex) {
        case CLAIM_REGULAR_ACCOUNT:
        case CLAIM_MULTI_SIGNATURE_ACCOUNT:
            msg->numScreens = 3;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
