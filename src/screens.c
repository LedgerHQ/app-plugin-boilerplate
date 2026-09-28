#include "screens.h"
#include "contracts.h"
#include "erc8009_format.h"

#define UNKNOWN_TICKER "???"

typedef struct token_info_s {
    const char *ticker;
    uint8_t decimals;
} token_info_t;

typedef struct walk_s {
    const ethQueryContractUI_t *ui;  // NULL: count only
    uint8_t count;
    bool hit;
    bool ok;
} walk_t;

const uint8_t *lookup_token(const context_t *context, uint8_t source) {
    if (source >= LOOKUP_APPROVAL_BASE) {
        return context->approvals[source - LOOKUP_APPROVAL_BASE].token;
    }
    return context->guards[source].token;
}

uint8_t token_source(const context_t *context,
                     const uint8_t *token,
                     bool is_pull,
                     uint8_t guard,
                     uint8_t *meta_guard) {
    if (allzeroes(token, ADDRESS_LENGTH)) {
        return TOKEN_NATIVE;
    }
    if (!schema_is_meta(&SCHEMAS[context->selector_index])) {
        return TOKEN_UNKNOWN;
    }
    if (!is_pull) {
        *meta_guard = guard;
        return TOKEN_META;
    }
    // A pull token is known through the first guard on the same token.
    for (uint8_t g = 0; g < context->guard_count; g++) {
        if (memcmp(context->guards[g].token, token, ADDRESS_LENGTH) == 0) {
            *meta_guard = g;
            return TOKEN_META;
        }
    }
    return TOKEN_UNKNOWN;
}

static void token_info(const context_t *context,
                       const uint8_t *token,
                       bool is_pull,
                       uint8_t guard,
                       token_info_t *info) {
    uint8_t meta_guard = 0;
    uint8_t source = token_source(context, token, is_pull, guard, &meta_guard);

    info->ticker = UNKNOWN_TICKER;
    info->decimals = 0;
    if (source == TOKEN_NATIVE) {
        info->ticker = NATIVE_TICKER;
        info->decimals = NATIVE_DECIMALS;
    } else if (source == TOKEN_META) {
        info->ticker = context->guards[meta_guard].symbol;
        info->decimals = context->guards[meta_guard].decimals;
    } else {
        for (uint8_t s = 0; s < MAX_LOOKUPS; s++) {
            const lookup_t *lookup = &context->lookups[s];
            if (lookup->source != LOOKUP_NONE && lookup->accepted &&
                memcmp(lookup_token(context, lookup->source), token, ADDRESS_LENGTH) == 0) {
                info->ticker = lookup->ticker;
                info->decimals = lookup->decimals;
                return;
            }
        }
    }
}

static bool next_pair(walk_t *walk) {
    bool hit = walk->ui != NULL && walk->ui->screenIndex == walk->count;

    walk->count++;
    walk->hit = walk->hit || hit;
    return hit;
}

static bool render_amount(const walk_t *walk,
                          const char *label,
                          uint8_t number,
                          uint8_t total,
                          const uint8_t *amount,
                          uint8_t amount_len,
                          uint8_t sign,
                          const token_info_t *info) {
    const ethQueryContractUI_t *ui = walk->ui;

    return erc8009_format_title(ui->title, ui->titleLength, label, number, total) &&
           erc8009_format_amount(amount,
                                 amount_len,
                                 sign,
                                 info->decimals,
                                 info->ticker,
                                 ui->msg,
                                 ui->msgLength);
}

static bool render_address(const walk_t *walk,
                           const char *label,
                           uint8_t number,
                           uint8_t total,
                           const uint8_t *address) {
    const ethQueryContractUI_t *ui = walk->ui;

    return erc8009_format_title(ui->title, ui->titleLength, label, number, total) &&
           erc8009_format_address(address, ui->msg, ui->msgLength);
}

uint8_t resolve_screen(const context_t *context, const ethQueryContractUI_t *ui, uint8_t *reason) {
    const selector_schema_t *schema = &SCHEMAS[context->selector_index];
    walk_t walk = {.ui = ui, .count = 0, .hit = false, .ok = false};
    uint8_t guards = context->guard_count;
    uint8_t pulls = 0;
    uint8_t pull = 0;
    token_info_t info;

    if (context->has_send && next_pair(&walk)) {
        info.ticker = NATIVE_TICKER;
        info.decimals = NATIVE_DECIMALS;
        walk.ok = ui->txContent != NULL && render_amount(&walk,
                                                         "Send",
                                                         0,
                                                         0,
                                                         ui->txContent->value.value,
                                                         ui->txContent->value.length,
                                                         AMOUNT_UNSIGNED,
                                                         &info);
    }
    if (schema->family == FAMILY_SR && next_pair(&walk)) {
        walk.ok = render_address(&walk, "Safe", 0, 0, context->safe);
    }
    for (uint8_t i = 0; i < guards; i++) {
        const guard_t *guard = &context->guards[i];
        uint8_t number = (guards > 1) ? i + 1 : 0;
        uint8_t total = (guards > 1) ? guards : 0;

        if (next_pair(&walk)) {
            token_info(context, guard->token, false, i, &info);
            walk.ok = render_amount(&walk,
                                    schema->diffs ? "Min change" : "Min balance",
                                    number,
                                    total,
                                    guard->amount,
                                    INT256_LENGTH,
                                    schema->diffs ? AMOUNT_SIGNED : AMOUNT_MAGNITUDE,
                                    &info);
        }
        if (!allzeroes(guard->token, ADDRESS_LENGTH) && next_pair(&walk)) {
            walk.ok = render_address(&walk, "Token", number, total, guard->token);
        }
        if ((guard->flags & GUARD_FLAG_OTHER) != 0 && next_pair(&walk)) {
            walk.ok = render_address(&walk, "Other account", number, total, guard->target);
        }
    }
    for (uint8_t a = 0; a < context->approval_count; a++) {
        pulls += context->approvals[a].shown ? 1 : 0;
    }
    for (uint8_t a = 0; a < context->approval_count; a++) {
        const approval_t *approval = &context->approvals[a];
        uint8_t number;
        uint8_t total;

        if (!approval->shown) {
            continue;
        }
        pull++;
        number = (pulls > 1) ? pull : 0;
        total = (pulls > 1 && schema->diffs) ? pulls : 0;
        if (next_pair(&walk)) {
            token_info(context, approval->token, true, 0, &info);
            walk.ok = render_amount(&walk,
                                    schema->diffs ? "Pull" : "Unguarded pull",
                                    number,
                                    total,
                                    approval->amount,
                                    INT256_LENGTH,
                                    AMOUNT_UNSIGNED,
                                    &info);
        }
        if (next_pair(&walk)) {
            walk.ok = render_address(&walk, "Pull token", number, total, approval->token);
        }
    }
    if (reason != NULL) {
        *reason = !walk.hit ? E_STATE : (walk.ok ? R_OK : E_RENDER);
    }
    return walk.count;
}
