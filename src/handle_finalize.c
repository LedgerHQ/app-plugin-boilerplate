#include "plugin.h"
#include "contracts.h"
#include "parse_kinds.h"
#include "screens.h"

// Exactly one of chainID (typed tx) / v (legacy tx) carries the chain id.
static bool network_ok(const txContent_t *tx) {
    if (tx == NULL || (tx->chainID.length > 0) == (tx->vLength > 0)) {
        return false;
    }
    if (tx->chainID.length > 0) {
        return chain_id_ok(tx->chainID.value, tx->chainID.length);
    }
    return chain_id_ok(tx->v, tx->vLength);
}

static bool approval_covered(const context_t *context,
                             const approval_t *approval,
                             const uint8_t *signer) {
    for (uint8_t g = 0; g < context->guard_count; g++) {
        const guard_t *guard = &context->guards[g];
        if (memcmp(guard->target, signer, ADDRESS_LENGTH) == 0 &&
            memcmp(guard->token, approval->token, ADDRESS_LENGTH) == 0) {
            return true;
        }
    }
    return false;
}

static void request_lookup(context_t *context, uint8_t source) {
    const uint8_t *token = lookup_token(context, source);

    for (uint8_t s = 0; s < MAX_LOOKUPS; s++) {
        uint8_t current = context->lookups[s].source;
        if (current == LOOKUP_NONE) {
            context->lookups[s].source = source;
            return;
        }
        if (memcmp(lookup_token(context, current), token, ADDRESS_LENGTH) == 0) {
            return;
        }
    }
}

void handle_finalize(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    const selector_schema_t *schema;
    const uint8_t *signer = msg->address;
    const uint8_t *owner;
    uint8_t meta_guard;
    uint8_t screens;

    msg->tokenLookup1 = NULL;
    msg->tokenLookup2 = NULL;
    if (context->outcome != OUTCOME_PARSING) {
        msg->result = erc8009_error(context, E_STATE, 0);
        return;
    }
    if (!parser_done(context)) {
        msg->result = erc8009_error(context, E_TRUNC, 0);
        return;
    }
    if (context->words_seen != context->words_total) {
        msg->result = erc8009_error(context, E_TRAILING, 0);
        return;
    }
    if (!network_ok(msg->txContent)) {
        msg->result = erc8009_error(context, E_CHAIN, 0);
        return;
    }
    if (context->reason != R_OK) {
        context->outcome = OUTCOME_FALLBACK;
        msg->result = ETH_PLUGIN_RESULT_FALLBACK;
        return;
    }
    // The signer is the `From` address; a Safe call guards the Safe instead.
    if (signer == NULL) {
        msg->result = erc8009_error(context, E_STATE, 0);
        return;
    }
    schema = &SCHEMAS[context->selector_index];
    owner = (schema->family == FAMILY_SR) ? context->safe : signer;
    for (uint8_t g = 0; g < context->guard_count; g++) {
        guard_t *guard = &context->guards[g];
        if (memcmp(guard->target, owner, ADDRESS_LENGTH) != 0) {
            guard->flags |= GUARD_FLAG_OTHER;
        }
    }
    context->has_send = schema->family != FAMILY_SR && !tx_value_is_zero(&msg->txContent->value);

    if (schema->family == FAMILY_AR || schema->family == FAMILY_PR) {
        for (uint8_t a = 0; a < context->approval_count; a++) {
            approval_t *approval = &context->approvals[a];
            approval->shown = schema->diffs || !approval_covered(context, approval, signer);
        }
    }

    for (uint8_t g = 0; g < context->guard_count; g++) {
        if (token_source(context, context->guards[g].token, false, g, &meta_guard) ==
            TOKEN_UNKNOWN) {
            request_lookup(context, g);
        }
    }
    for (uint8_t a = 0; a < context->approval_count; a++) {
        if (context->approvals[a].shown &&
            token_source(context, context->approvals[a].token, true, 0, &meta_guard) ==
                TOKEN_UNKNOWN) {
            request_lookup(context, LOOKUP_APPROVAL_BASE + a);
        }
    }

    screens = resolve_screen(context, NULL, NULL);
    if (screens == 0) {
        msg->result = erc8009_error(context, E_STATE, 0);
        return;
    }
    if (context->lookups[0].source != LOOKUP_NONE) {
        msg->tokenLookup1 = lookup_token(context, context->lookups[0].source);
    }
    if (context->lookups[1].source != LOOKUP_NONE) {
        msg->tokenLookup2 = lookup_token(context, context->lookups[1].source);
    }
    context->num_screens = screens;
    context->outcome = OUTCOME_OK;
    msg->uiType = ETH_UI_TYPE_GENERIC;
    msg->numScreens = screens;
    msg->result = ETH_PLUGIN_RESULT_OK;
}
