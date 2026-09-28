#include "plugin_utils.h"
#include "plugin.h"
#include "contracts.h"
#include "parse_kinds.h"

void handle_init_contract(ethPluginInitContract_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    const txContent_t *tx = msg->txContent;
    const selector_schema_t *schema;
    uint64_t data_size = msg->dataSize;
    size_t index;
    uint8_t family;

    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
        PRINTF("ERC8009 E:%u off=%u\n", (unsigned) U_VERSION, 0u);
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    if (msg->pluginContextLength < sizeof(context_t)) {
        PRINTF("ERC8009 E:%u off=%u\n", (unsigned) E_CONTEXT, 0u);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    memset(context, 0, sizeof(*context));
    for (uint8_t s = 0; s < MAX_LOOKUPS; s++) {
        context->lookups[s].source = LOOKUP_NONE;
    }

    if (msg->selector == NULL ||
        !find_selector(U4BE(msg->selector, 0), SELECTORS, SELECTOR_COUNT, &index)) {
        PRINTF("ERC8009 E:%u off=%u\n", (unsigned) U_SELECTOR, 0u);
        context->outcome = OUTCOME_UNAVAILABLE;
        context->reason = U_SELECTOR;
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }
    context->selector_index = (uint8_t) index;
    schema = &SCHEMAS[context->selector_index];

    if (tx == NULL || tx->destinationLength != ADDRESS_LENGTH ||
        !contract_family(tx->destination, &family)) {
        msg->result = erc8009_error(context, E_DEST, 0);
        return;
    }
    if (family != schema->family) {
        msg->result = erc8009_error(context, E_FAMILY, 0);
        return;
    }
    // Typed transactions carry the chain id already; legacy ones only in `v` (finalize).
    if (tx->chainID.length > 0 && !chain_id_ok(tx->chainID.value, tx->chainID.length)) {
        msg->result = erc8009_error(context, E_CHAIN, 0);
        return;
    }
    // dataSize includes the selector; the arguments are whole words.
    if (data_size < SELECTOR_SIZE || data_size > UINT32_MAX ||
        (data_size - SELECTOR_SIZE) % PARAMETER_LENGTH != 0) {
        msg->result = erc8009_error(context, E_DATASIZE, 0);
        return;
    }
    if (family == FAMILY_SR && !tx_value_is_zero(&tx->value)) {
        msg->result = erc8009_error(context, E_SAFE_VALUE, 0);
        return;
    }

    context->words_total = (uint32_t) ((data_size - SELECTOR_SIZE) / PARAMETER_LENGTH);
    parser_start(context);
    context->outcome = OUTCOME_PARSING;

    msg->result = ETH_PLUGIN_RESULT_OK;
}
