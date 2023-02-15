#include "pstake_plugin.h"

// Set UI for the "Stake" screen.
static void set_deposit_amount_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Amount", msg->titleLength);

    const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
    uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    amountToString(eth_amount, eth_amount_size, WEI_TO_ETHER, "BNB", msg->msg, msg->msgLength);
}

// Set UI for "Unstake" screen.
static void set_stkbnb_send_amount_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Amount", msg->titleLength);

    amountToString(context->amount,
                   sizeof(context->amount),
                   WEI_TO_ETHER,
                   "stkBNB",
                   msg->msg,
                   msg->msgLength);
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case DEPOSIT:
            switch (msg->screenIndex) {
                case 0:
                    set_deposit_amount_ui(msg);
                    break;
                // Keep this
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;
        case CLAIM_ALL:
            switch (msg->screenIndex) {
                case 0:
                    strlcpy(msg->msg, "Get unstaked BNB", msg->msgLength);
                    break;
                // Keep this
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            return;
        case STKBNB_SEND:
            switch (msg->screenIndex) {
                case 0:
                    set_stkbnb_send_amount_ui(msg, context);
                    break;
                // Keep this
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;
        // Keep this
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}
