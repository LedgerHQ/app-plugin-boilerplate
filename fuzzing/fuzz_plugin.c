#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plugin.h"

#define NAME_LENGTH    43u
#define VERSION_LENGTH 131u
#define TITLE_LENGTH   43u
#define MSG_LENGTH     100u

#define FLAG_CONTINUE    0x01
#define FLAG_BAD_VERSION 0x02
#define FLAG_SMALL_CTX   0x04

#define DEST_RANDOM  0x80
#define SIGNER_GUARD 0x80
#define TICKER_SRC   20u
#define LOOKUP_ITEM1 0x01
#define LOOKUP_RAW1  0x04

typedef struct item_in_s {
    uint8_t addr[ADDRESS_LENGTH];
    uint8_t ticker_len;
    uint8_t ticker[TICKER_SRC];
    uint8_t decimals;
} item_in_t;

typedef struct header_s {
    uint8_t selector[SELECTOR_SIZE];
    uint8_t flags;
    uint8_t dest_sel;
    uint8_t dest_len;
    uint8_t dest[ADDRESS_LENGTH];
    uint8_t value_len;
    uint8_t value[INT256_LENGTH];
    uint8_t chain_len;
    uint8_t chain[INT256_LENGTH];
    uint8_t v_len;
    uint8_t v[8];
    uint8_t signer_sel;
    uint8_t signer[ADDRESS_LENGTH];
    uint8_t ds_delta[8];
    uint8_t chunk_seed[4];
    uint8_t seq_word[2];
    uint8_t seq_delta[4];
    uint8_t lookup_flags;
    item_in_t item[2];
} header_t;

_Static_assert(sizeof(item_in_t) == 42, "item layout");
_Static_assert(sizeof(header_t) == 226, "header layout");

// Duplicated on purpose: the fuzzer's expectations must not depend on src/contracts.c.
static const uint8_t ALLOWLIST[4][ADDRESS_LENGTH] = {
    {0x80, 0x0b, 0x36, 0xcc, 0x16, 0xc7, 0x57, 0x2e, 0x1d, 0x60,
     0x05, 0x06, 0xa0, 0xd7, 0xd3, 0x08, 0x90, 0xff, 0x35, 0xf4},
    {0xf3, 0xc8, 0x2e, 0x21, 0x76, 0xd8, 0x94, 0xed, 0x32, 0xc5,
     0x74, 0x8c, 0x22, 0x23, 0x8e, 0xbf, 0xcc, 0x68, 0xcd, 0xd8},
    {0xfe, 0xa6, 0x07, 0xc2, 0xf7, 0x4d, 0x7a, 0xc4, 0xac, 0x35,
     0x36, 0x0e, 0x51, 0xf3, 0xfe, 0x08, 0xe3, 0x19, 0xeb, 0xa2},
    {0x7d, 0x7e, 0xae, 0x4d, 0x58, 0xe9, 0xd2, 0xa9, 0x2a, 0x48,
     0xee, 0x55, 0xf2, 0x49, 0xa2, 0x90, 0x3f, 0xd1, 0xd0, 0xd0},
};

typedef struct buffers_s {
    uint8_t *selector;
    uint8_t *word;
    context_t *context;
    txContent_t *tx;
    uint8_t *signer;
    extraInfo_t *item[2];
    char *name;
    char *version;
    char *title;
    char *msg;
} buffers_t;

static uint64_t le(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    for (size_t i = n; i > 0; i--) {
        v = (v << 8) | p[i - 1];
    }
    return v;
}

static uint32_t next_rand(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static void fill_tx(txContent_t *tx, const header_t *h) {
    memset(tx, 0, sizeof(*tx));
    if (h->dest_sel & DEST_RANDOM) {
        memcpy(tx->destination, h->dest, ADDRESS_LENGTH);
    } else {
        memcpy(tx->destination, ALLOWLIST[h->dest_sel & 3], ADDRESS_LENGTH);
    }
    tx->destinationLength = h->dest_len;
    memcpy(tx->value.value, h->value, INT256_LENGTH);
    tx->value.length = h->value_len;
    memcpy(tx->chainID.value, h->chain, INT256_LENGTH);
    tx->chainID.length = h->chain_len;
    memcpy(tx->v, h->v, sizeof(tx->v));
    tx->vLength = h->v_len;
    tx->dataPresent = true;
}

static extraInfo_t *fill_item(extraInfo_t *item,
                              const header_t *h,
                              unsigned i,
                              const uint8_t *requested) {
    const item_in_t *in = &h->item[i];
    size_t n = in->ticker_len < MAX_TICKER_LEN ? in->ticker_len : MAX_TICKER_LEN;

    if ((h->lookup_flags & (LOOKUP_ITEM1 << i)) == 0) {
        return NULL;
    }
    memset(item, 0xA5, sizeof(*item));
    memset(&item->token, 0, sizeof(item->token));
    if (requested != NULL && (h->lookup_flags & (LOOKUP_RAW1 << i)) == 0) {
        memcpy(item->token.address, requested, ADDRESS_LENGTH);
    } else {
        memcpy(item->token.address, in->addr, ADDRESS_LENGTH);
    }
    for (size_t j = 0; j < n; j++) {
        item->token.ticker[j] = (char) in->ticker[j % TICKER_SRC];
    }
    item->token.decimals = in->decimals;
    return item;
}

static void run(buffers_t *b, const header_t *h, const uint8_t *args, size_t args_len) {
    const bool cont = (h->flags & FLAG_CONTINUE) != 0;
    ethPluginInitContract_t init = {0};
    ethPluginProvideParameter_t param = {0};
    ethPluginFinalize_t finalize = {0};
    ethPluginProvideInfo_t info = {0};
    ethQueryContractID_t query_id = {0};
    ethQueryContractUI_t query_ui = {0};
    bip32_path_t bip32 = {0};
    uint32_t rng = (uint32_t) le(h->chunk_seed, 4) | 1u;
    uint32_t seq_word = (uint32_t) le(h->seq_word, 2);
    uint32_t seq_delta = (uint32_t) le(h->seq_delta, 4);
    size_t nwords = (args_len + PARAMETER_LENGTH - 1) / PARAMETER_LENGTH;
    unsigned screens;

    bip32.length = 5;
    bip32.path[0] = 44 | 0x80000000;
    bip32.path[1] = 60 | 0x80000000;
    bip32.path[2] = 0 | 0x80000000;

    fill_tx(b->tx, h);
    memcpy(b->selector, h->selector, SELECTOR_SIZE);
    // Filled with garbage (not zero): init must not rely on a pre-zeroed context.
    memset(b->context, 0xA5, sizeof(*b->context));

    init.interfaceVersion = (h->flags & FLAG_BAD_VERSION) ? ETH_PLUGIN_INTERFACE_VERSION_LATEST - 1
                                                          : ETH_PLUGIN_INTERFACE_VERSION_LATEST;
    init.txContent = b->tx;
    init.pluginContextLength = sizeof(context_t) - ((h->flags & FLAG_SMALL_CTX) ? 1 : 0);
    init.selector = b->selector;
    init.dataSize = (size_t) ((uint64_t) (SELECTOR_SIZE + args_len) + le(h->ds_delta, 8));
    init.bip32 = &bip32;
    init.pluginContext = (uint8_t *) b->context;
    handle_init_contract(&init);
    if (init.result != ETH_PLUGIN_RESULT_OK) {
        return;
    }

    for (size_t k = 0; k < nwords; k++) {
        size_t chunk = args_len - k * PARAMETER_LENGTH;
        if (chunk > PARAMETER_LENGTH) {
            chunk = PARAMETER_LENGTH;
        }
        memset(b->word, 0, PARAMETER_LENGTH);
        memcpy(b->word, args + k * PARAMETER_LENGTH, chunk);

        memset(&param, 0, sizeof(param));
        param.parameter = b->word;
        param.parameterOffset = (uint32_t) (SELECTOR_SIZE + PARAMETER_LENGTH * k);
        if (k == seq_word) {
            param.parameterOffset += seq_delta;
        }
        param.parameter_size = (uint8_t) (1 + next_rand(&rng) % PARAMETER_LENGTH);
        param.pluginContext = (uint8_t *) b->context;
        param.txContent = b->tx;
        handle_provide_parameter(&param);
        if (param.result != ETH_PLUGIN_RESULT_OK && param.result != ETH_PLUGIN_RESULT_FALLBACK &&
            !cont) {
            return;
        }
    }

    if (h->signer_sel & SIGNER_GUARD) {
        memcpy(b->signer, b->context->guards[h->signer_sel & 7].target, ADDRESS_LENGTH);
    } else {
        memcpy(b->signer, h->signer, ADDRESS_LENGTH);
    }
    finalize.pluginContext = (uint8_t *) b->context;
    finalize.txContent = b->tx;
    finalize.address = b->signer;
    handle_finalize(&finalize);
    if (finalize.result != ETH_PLUGIN_RESULT_OK && !cont) {
        return;
    }

    if (finalize.tokenLookup1 != NULL || finalize.tokenLookup2 != NULL || cont) {
        info.pluginContext = (uint8_t *) b->context;
        info.txContent = b->tx;
        info.item1 = fill_item(b->item[0], h, 0, finalize.tokenLookup1);
        info.item2 = fill_item(b->item[1], h, 1, finalize.tokenLookup2);
        handle_provide_token(&info);
        if (info.result != ETH_PLUGIN_RESULT_OK && !cont) {
            return;
        }
    }

    query_id.pluginContext = (uint8_t *) b->context;
    query_id.txContent = b->tx;
    query_id.name = b->name;
    query_id.nameLength = NAME_LENGTH;
    query_id.version = b->version;
    query_id.versionLength = VERSION_LENGTH;
    memset(b->name, 0, NAME_LENGTH);
    memset(b->version, 0, VERSION_LENGTH);
    handle_query_contract_id(&query_id);
    if (query_id.result == ETH_PLUGIN_RESULT_OK) {
        printf("name: %.*s\n", (int) NAME_LENGTH, b->name);
        printf("version: %.*s\n", (int) VERSION_LENGTH, b->version);
    } else if (!cont) {
        return;
    }

    screens = (unsigned) finalize.numScreens + info.additionalScreens + (cont ? 1 : 0);
    for (unsigned i = 0; i < screens && i <= UINT8_MAX; i++) {
        memset(&query_ui, 0, sizeof(query_ui));
        query_ui.txContent = b->tx;
        query_ui.item1 = info.item1;
        query_ui.item2 = info.item2;
        memcpy(query_ui.network_ticker, "ETH", sizeof("ETH"));
        query_ui.screenIndex = (uint8_t) i;
        query_ui.title = b->title;
        query_ui.titleLength = TITLE_LENGTH;
        query_ui.msg = b->msg;
        query_ui.msgLength = MSG_LENGTH;
        query_ui.pluginContext = (uint8_t *) b->context;
        memset(b->title, 0, TITLE_LENGTH);
        memset(b->msg, 0, MSG_LENGTH);
        handle_query_contract_ui(&query_ui);
        if (query_ui.result == ETH_PLUGIN_RESULT_OK) {
            printf("%.*s: %.*s\n", (int) TITLE_LENGTH, b->title, (int) MSG_LENGTH, b->msg);
        } else if (!cont) {
            return;
        }
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    header_t h;
    buffers_t b;

    if (size < sizeof(header_t)) {
        return 0;
    }
    memcpy(&h, data, sizeof(h));

    b.selector = malloc(SELECTOR_SIZE);
    b.word = malloc(PARAMETER_LENGTH);
    b.context = malloc(sizeof(context_t));
    b.tx = malloc(sizeof(txContent_t));
    b.signer = malloc(ADDRESS_LENGTH);
    b.item[0] = malloc(sizeof(extraInfo_t));
    b.item[1] = malloc(sizeof(extraInfo_t));
    b.name = malloc(NAME_LENGTH);
    b.version = malloc(VERSION_LENGTH);
    b.title = malloc(TITLE_LENGTH);
    b.msg = malloc(MSG_LENGTH);
    if (b.selector && b.word && b.context && b.tx && b.signer && b.item[0] && b.item[1] && b.name &&
        b.version && b.title && b.msg) {
        run(&b, &h, data + sizeof(header_t), size - sizeof(header_t));
    }
    free(b.selector);
    free(b.word);
    free(b.context);
    free(b.tx);
    free(b.signer);
    free(b.item[0]);
    free(b.item[1]);
    free(b.name);
    free(b.version);
    free(b.title);
    free(b.msg);
    return 0;
}
