#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `boilerPlateSelector_t`.
// EDIT THIS: Put in the number of selectors your plugin is going to support.
#define NUM_BOILERPLATE_SELECTORS 2

// Name of the plugin.
// EDIT THIS: Replace with your plugin name.
#define PLUGIN_NAME "Boilerplate"

// TODO: add doc.
#define TOKEN_SENT_FOUND     1
#define TOKEN_RECEIVED_FOUND (1 << 1)

// Enumeration of the different selectors possible.
// Should follow the array declared in main.c
typedef enum {
    BOILERPLATE_DUMMY_1,
    BOILERPLATE_DUMMY_2,
} boilerplateSelector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    TOKEN_SENT,
    TOKEN_RECEIVED,
    AMOUNT_SENT,
    AMOUNT_RECEIVED,
    BENEFICIARY,
    NONE,
} selectorField;

// Enumeration of different screens that the plugin might display.
typedef enum {
    SEND_SCREEN,
    RECEIVE_SCREEN,
    BENEFICIARY_SCREEN,
    ERROR,  // This variant indicates that an error occured. No display should occur.
} screens_t;

extern const uint8_t *const BOILERPLATE_SELECTORS[NUM_BOILERPLATE_SELECTORS];

// Number of decimals used when the token wasn't found in the Crypto Asset List.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the Crypto Asset List.
#define DEFAULT_TICKER ""

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct boilerplate_parameters_t {
    uint8_t amount_sent[INT256_LENGTH];
    uint8_t amount_received[INT256_LENGTH];
    char beneficiary[ADDRESS_LENGTH];
    uint8_t contract_address_sent[ADDRESS_LENGTH];
    uint8_t contract_address_received[ADDRESS_LENGTH];
    char ticker_sent[MAX_TICKER_LEN];
    char ticker_received[MAX_TICKER_LEN];

    uint8_t next_param;
    uint8_t tokens_found;
    uint8_t decimals_sent;
    uint8_t decimals_received;
    uint8_t selectorIndex;
} boilerplate_parameters_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(boilerplate_parameters_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);