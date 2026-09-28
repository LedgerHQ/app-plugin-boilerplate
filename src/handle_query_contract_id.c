#include "plugin.h"
#include "contracts.h"
#include "erc8009_format.h"

void handle_query_contract_id(ethQueryContractID_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    const char *version;

    if (context->outcome != OUTCOME_OK) {
        msg->result = erc8009_error(context, E_STATE, 0);
        return;
    }
    version = (SCHEMAS[context->selector_index].family == FAMILY_SR) ? "execute guarded Safe tx"
                                                                     : "execute guarded call";
    if (!erc8009_copy_string(msg->name, msg->nameLength, "ERC-8009") ||
        !erc8009_copy_string(msg->version, msg->versionLength, version)) {
        msg->result = erc8009_error(context, E_RENDER, 0);
        return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}
