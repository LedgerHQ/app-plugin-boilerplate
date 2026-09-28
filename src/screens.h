#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "plugin.h"

typedef enum token_source_e {
    TOKEN_NATIVE = 0,
    TOKEN_META,
    TOKEN_UNKNOWN,
} token_source_t;

const uint8_t *lookup_token(const context_t *context, uint8_t source);

// `guard` is the guard index for a guard token; ignored for a pull token. For TOKEN_META,
// *meta_guard is the guard whose META applies.
uint8_t token_source(const context_t *context,
                     const uint8_t *token,
                     bool is_pull,
                     uint8_t guard,
                     uint8_t *meta_guard);

// When `ui` is NULL, only counts the pairs; otherwise renders pair ui->screenIndex into
// ui->title / ui->msg and sets *reason (R_OK, E_RENDER, or E_STATE if there is no such pair).
uint8_t resolve_screen(const context_t *context, const ethQueryContractUI_t *ui, uint8_t *reason);
