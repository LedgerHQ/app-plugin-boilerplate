#include "parse_kinds.h"
#include "abi_stream.h"
#include "contracts.h"

#define SAFE_TX_HEAD_WORDS  11
#define SAFE_TX_DATA_OFFSET (SAFE_TX_HEAD_WORDS * ABI_WORD_SIZE)

#define META_ELEMENT_HEAD 0x40
#define META_BOUND_WORDS  5

enum {
    SAFE_TO = 0,
    SAFE_VALUE,
    SAFE_DATA_OFFSET,
    SAFE_OPERATION,
    SAFE_TX_GAS,
    SAFE_BASE_GAS,
    SAFE_GAS_PRICE,
    SAFE_GAS_TOKEN,
    SAFE_REFUND_RECEIVER,
    SAFE_SIGNATURES_OFFSET,
    SAFE_ROUTER_SIG_POSITION,
};

static void note_fallback(context_t *context, uint8_t reason) {
    if (context->reason == R_OK) {
        context->reason = reason;
    }
}

// Words after the current one, up to dataSize (the caller guarantees words_seen < words_total).
static uint32_t remaining_after(const context_t *context) {
    return context->words_total - context->words_seen - 1;
}

static uint8_t element_words(uint8_t kind) {
    return (kind == P_GUARDS || kind == P_WITHDRAWALS) ? 3 : 4;
}

static uint8_t feed_head(context_t *context, const selector_schema_t *schema, const uint8_t *word) {
    uint8_t slot = (uint8_t) context->words_seen;
    uint8_t kind = schema->slots[slot].kind;

    if (param_is_dynamic(kind)) {
        return abi_read_u32(word, &context->offsets[slot]) ? R_OK : E_OFFSET;
    }
    if (!abi_is_address(word)) {
        return E_PAD_ADDR;
    }
    if (kind == P_ADDR_PROXY) {
        if (memcmp(word + ABI_WORD_SIZE - ADDRESS_LENGTH,
                   CONTRACT_ADDRESSES[FAMILY_BP],
                   ADDRESS_LENGTH) != 0) {
            return E_PROXY;
        }
    } else if (kind == P_ADDR_SAFE) {
        copy_address(context->safe, word, sizeof(context->safe));
    }
    return R_OK;
}

static uint8_t start_array(context_t *context,
                           const selector_schema_t *schema,
                           uint8_t kind,
                           const uint8_t *word) {
    uint32_t n;
    uint32_t bound_words = (kind == P_META) ? META_BOUND_WORDS : element_words(kind);

    if (!abi_read_u32(word, &n)) {
        return E_PAD_UINT;
    }
    if (!abi_array_fits(n, bound_words, remaining_after(context))) {
        return E_TRUNC;
    }
    switch (kind) {
        case P_META:
            context->meta_len = n;
            if (n > MAX_GUARDS) {
                note_fallback(context, F_TOO_MANY);
            }
            break;
        case P_GUARDS:
            if (schema_is_meta(schema) && n != context->meta_len) {
                return E_META_LEN;
            }
            if (n == 0) {
                note_fallback(context, F_EMPTY);
            } else if (n > MAX_GUARDS) {
                note_fallback(context, F_TOO_MANY);
            }
            context->guard_count = (uint8_t) ((n > MAX_GUARDS) ? MAX_GUARDS : n);
            break;
        case P_APPROVALS:
            if (n > MAX_APPROVALS) {
                return E_APPROVALS_MAX;
            }
            context->approval_count = (uint8_t) n;
            break;
        case P_PERMITS:
            if (n != context->approval_count) {
                return E_PERMITS_LEN;
            }
            break;
        default:
            break;
    }
    context->count = n;
    context->item = 0;
    context->field = 0;
    if (n == 0) {
        context->sub = SUB_END;
    } else {
        context->sub = (kind == P_META) ? SUB_META_OFFSETS : SUB_ELEMENTS;
    }
    return R_OK;
}

static uint8_t start_bytes(context_t *context,
                           const uint8_t *word,
                           uint8_t content_sub,
                           uint8_t end_sub) {
    uint32_t len;
    uint32_t words;

    if (!abi_read_u32(word, &len)) {
        return E_PAD_UINT;
    }
    words = abi_bytes_words(len);
    if (words > remaining_after(context)) {
        return E_TRUNC;
    }
    context->bytes_left = words;
    context->bytes_rem = (uint8_t) (len % ABI_WORD_SIZE);
    context->sub = (words == 0) ? end_sub : content_sub;
    return R_OK;
}

static uint8_t feed_bytes(context_t *context, const uint8_t *word, uint8_t end_sub) {
    context->bytes_left--;
    if (context->bytes_left == 0) {
        if (!abi_bytes_padding_ok(word, context->bytes_rem)) {
            return E_BYTES_PAD;
        }
        context->sub = end_sub;
    }
    return R_OK;
}

// META offset table: o_i == 32n + 128i (table of n words, then i elements of 4 words).
static uint8_t feed_meta_offset(context_t *context, const uint8_t *word) {
    uint32_t offset;
    uint64_t expected = 32ULL * context->count + 128ULL * context->item;

    if (!abi_read_u32(word, &offset) || offset != expected) {
        return E_OFFSET;
    }
    context->item++;
    if (context->item == context->count) {
        context->item = 0;
        context->field = 0;
        context->sub = SUB_ELEMENTS;
    }
    return R_OK;
}

static uint8_t feed_meta_field(context_t *context, const uint8_t *word) {
    guard_t *guard = (context->item < MAX_GUARDS) ? &context->guards[context->item] : NULL;
    uint8_t decimals;
    uint32_t len;

    switch (context->field) {
        case 0:
            return abi_equals_u32(word, META_ELEMENT_HEAD) ? R_OK : E_META_HEAD;
        case 1:
            if (!abi_read_uint8(word, &decimals)) {
                return E_PAD_UINT;
            }
            if (decimals > MAX_DECIMALS) {
                return E_DECIMALS;
            }
            if (guard != NULL) {
                guard->decimals = decimals;
            }
            return R_OK;
        case 2:
            if (!abi_read_u32(word, &len)) {
                return E_PAD_UINT;
            }
            if (len == 0 || len > MAX_SYMBOL_LEN) {
                return E_SYMBOL;
            }
            context->sym_len = (uint8_t) len;
            return R_OK;
        default:
            if (!abi_symbol_ok(word, context->sym_len)) {
                return E_SYMBOL;
            }
            if (guard != NULL) {
                memcpy(guard->symbol, word, context->sym_len);
                guard->symbol[context->sym_len] = '\0';
            }
            return R_OK;
    }
}

static uint8_t feed_guard_field(context_t *context,
                                const selector_schema_t *schema,
                                const uint8_t *word) {
    guard_t *guard = (context->item < MAX_GUARDS) ? &context->guards[context->item] : NULL;

    switch (context->field) {
        case 0:
            if (!abi_is_address(word)) {
                return E_PAD_ADDR;
            }
            if (guard != NULL) {
                copy_address(guard->target, word, sizeof(guard->target));
            }
            return R_OK;
        case 1:
            if (!abi_is_address(word)) {
                return E_PAD_ADDR;
            }
            if (guard == NULL) {
                return R_OK;
            }
            copy_address(guard->token, word, sizeof(guard->token));
            if (schema_is_meta(schema) && abi_is_zero(word) &&
                !(strcmp(guard->symbol, NATIVE_TICKER) == 0 &&
                  guard->decimals == NATIVE_DECIMALS)) {
                return E_NATIVE_META;
            }
            return R_OK;
        default:
            if (guard != NULL) {
                memcpy(guard->amount, word, sizeof(guard->amount));
            }
            return R_OK;
    }
}

static uint8_t feed_approval_field(context_t *context, const uint8_t *word) {
    approval_t *approval;

    if (context->item >= MAX_APPROVALS) {
        return E_STATE;
    }
    approval = &context->approvals[context->item];
    switch (context->field) {
        case 0:
            return abi_is_address(word) ? R_OK : E_PAD_ADDR;
        case 1:
            if (!abi_is_address(word)) {
                return E_PAD_ADDR;
            }
            copy_address(approval->token, word, sizeof(approval->token));
            return R_OK;
        case 2:
            if (abi_is_negative(word)) {
                return E_APPROVAL_NEG;
            }
            memcpy(approval->amount, word, sizeof(approval->amount));
            return R_OK;
        default:
            return abi_is_bool(word) ? R_OK : E_BOOL;
    }
}

static uint8_t feed_permit_field(context_t *context, const uint8_t *word) {
    uint8_t v;

    if (context->field == 1 && !abi_read_uint8(word, &v)) {
        return E_PAD_UINT;
    }
    return R_OK;
}

static uint8_t feed_withdrawal_field(context_t *context, const uint8_t *word) {
    if (context->field < 2 && !abi_is_address(word)) {
        return E_PAD_ADDR;
    }
    return R_OK;
}

static uint8_t feed_element(context_t *context,
                            const selector_schema_t *schema,
                            uint8_t kind,
                            const uint8_t *word) {
    uint8_t reason;

    switch (kind) {
        case P_META:
            reason = feed_meta_field(context, word);
            break;
        case P_GUARDS:
            reason = feed_guard_field(context, schema, word);
            break;
        case P_APPROVALS:
            reason = feed_approval_field(context, word);
            break;
        case P_PERMITS:
            reason = feed_permit_field(context, word);
            break;
        default:
            reason = feed_withdrawal_field(context, word);
            break;
    }
    if (reason != R_OK) {
        return reason;
    }
    context->field++;
    if (context->field == element_words(kind)) {
        context->field = 0;
        context->item++;
        if (context->item == context->count) {
            context->sub = SUB_END;
        }
    }
    return R_OK;
}

static uint8_t feed_safe_head_field(context_t *context, const uint8_t *word) {
    uint32_t offset;
    uint8_t operation;

    switch (context->field) {
        case SAFE_TO:
        case SAFE_GAS_TOKEN:
        case SAFE_REFUND_RECEIVER:
            return abi_is_address(word) ? R_OK : E_PAD_ADDR;
        case SAFE_DATA_OFFSET:
            if (!abi_read_u32(word, &offset) || offset != SAFE_TX_DATA_OFFSET) {
                return E_OFFSET;
            }
            return R_OK;
        case SAFE_OPERATION:
            if (!abi_read_uint8(word, &operation)) {
                return E_PAD_UINT;
            }
            return (operation == 0) ? R_OK : E_SAFE_OP;
        case SAFE_SIGNATURES_OFFSET:
            return abi_read_u32(word, &context->safe_sig_offset) ? R_OK : E_OFFSET;
        default:
            return R_OK;
    }
}

static uint8_t feed_safe(context_t *context, const uint8_t *word) {
    uint8_t reason;
    uint64_t expected;

    switch (context->sub) {
        case SUB_SAFE_HEAD:
            reason = feed_safe_head_field(context, word);
            if (reason == R_OK) {
                context->field++;
                if (context->field == SAFE_TX_HEAD_WORDS) {
                    context->sub = SUB_SAFE_DATA_LEN;
                }
            }
            return reason;
        case SUB_SAFE_DATA_LEN:
            reason = start_bytes(context, word, SUB_SAFE_DATA, SUB_SAFE_SIG_LEN);
            context->safe_data_words = context->bytes_left;
            return reason;
        case SUB_SAFE_DATA:
            return feed_bytes(context, word, SUB_SAFE_SIG_LEN);
        case SUB_SAFE_SIG_LEN:
            expected = (uint64_t) SAFE_TX_DATA_OFFSET + ABI_WORD_SIZE +
                       (uint64_t) ABI_WORD_SIZE * context->safe_data_words;
            if (context->safe_sig_offset != expected) {
                return E_OFFSET;
            }
            return start_bytes(context, word, SUB_SAFE_SIG, SUB_END);
        case SUB_SAFE_SIG:
            return feed_bytes(context, word, SUB_END);
        default:
            return E_STATE;
    }
}

static uint8_t start_tail(context_t *context,
                          const selector_schema_t *schema,
                          uint8_t kind,
                          const uint8_t *word) {
    if (kind == P_BYTES) {
        return start_bytes(context, word, SUB_BYTES, SUB_END);
    }
    if (kind == P_SAFE_TX) {
        if (context->words_total - context->words_seen < SAFE_TX_HEAD_WORDS) {
            return E_TRUNC;
        }
        context->sub = SUB_SAFE_HEAD;
        context->field = 0;
        return feed_safe(context, word);
    }
    return start_array(context, schema, kind, word);
}

static uint8_t feed_tail(context_t *context,
                         const selector_schema_t *schema,
                         uint8_t kind,
                         const uint8_t *word) {
    switch (context->sub) {
        case SUB_ELEMENTS:
            return feed_element(context, schema, kind, word);
        case SUB_META_OFFSETS:
            return feed_meta_offset(context, word);
        case SUB_BYTES:
            return feed_bytes(context, word, SUB_END);
        case SUB_END:
            return E_STATE;
        default:
            return feed_safe(context, word);
    }
}

static void next_tail(context_t *context, const selector_schema_t *schema, uint8_t from) {
    for (uint8_t slot = from; slot < schema->head_len; slot++) {
        if (param_is_dynamic(schema->slots[slot].kind)) {
            context->slot = slot;
            context->stage = STAGE_TAIL_START;
            return;
        }
    }
    context->stage = STAGE_DONE;
}

void parser_start(context_t *context) {
    context->stage = STAGE_HEAD;
    context->words_seen = 0;
}

uint8_t parser_feed(context_t *context, const uint8_t *word) {
    const selector_schema_t *schema = &SCHEMAS[context->selector_index];
    uint8_t reason;

    switch (context->stage) {
        case STAGE_HEAD:
            reason = feed_head(context, schema, word);
            break;
        case STAGE_TAIL_START:
            if (context->offsets[context->slot] != context->words_seen * ABI_WORD_SIZE) {
                return E_OFFSET;
            }
            context->stage = STAGE_TAIL;
            reason = start_tail(context, schema, schema->slots[context->slot].kind, word);
            break;
        case STAGE_TAIL:
            reason = feed_tail(context, schema, schema->slots[context->slot].kind, word);
            break;
        default:
            return E_TRAILING;
    }
    if (reason != R_OK) {
        return reason;
    }
    context->words_seen++;
    if (context->stage == STAGE_HEAD) {
        if (context->words_seen == schema->head_len) {
            next_tail(context, schema, 0);
        }
    } else if (context->sub == SUB_END) {
        next_tail(context, schema, context->slot + 1);
    }
    return R_OK;
}

bool parser_done(const context_t *context) {
    return context->stage == STAGE_DONE;
}
