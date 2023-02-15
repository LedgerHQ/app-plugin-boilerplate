#include "pstake_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    msg->numScreens = 1;  // for all selectors we have only 1 screen

    msg->result = ETH_PLUGIN_RESULT_OK;
}
