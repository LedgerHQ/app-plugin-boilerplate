#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "plugin.h"

// Head, then the tail of every dynamic slot in head-slot order, each tail starting exactly
// where the previous one ended.

typedef enum parse_stage_e {
    STAGE_HEAD = 0,
    STAGE_TAIL_START,
    STAGE_TAIL,
    STAGE_DONE,
} parse_stage_t;

typedef enum parse_sub_e {
    SUB_ELEMENTS = 0,
    SUB_META_OFFSETS,
    SUB_BYTES,
    SUB_SAFE_HEAD,
    SUB_SAFE_DATA_LEN,
    SUB_SAFE_DATA,
    SUB_SAFE_SIG_LEN,
    SUB_SAFE_SIG,
    SUB_END,
} parse_sub_t;

void parser_start(context_t *context);

// Returns R_OK or the first E_* reason; sticky F_* reasons are recorded in context->reason.
uint8_t parser_feed(context_t *context, const uint8_t *word);

bool parser_done(const context_t *context);
