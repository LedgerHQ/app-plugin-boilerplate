#include "origin_plugin.h"

static void handle_token_sent(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    memset(context->contract_address_sent, 0, sizeof(context->contract_address_sent));

    if (context->selectorIndex == CURVE_POOL_EXCHANGE ||
        context->selectorIndex == CURVE_POOL_EXCHANGE_UNDERLYING) {
        if (memcmp(CURVE_OETH_POOL_ADDRESS,
                   msg->pluginSharedRO->txContent->destination,
                   ADDRESS_LENGTH) == 0) {
            switch (msg->parameter[PARAMETER_LENGTH - 1]) {
                case 0:
                    memcpy(context->contract_address_sent, NULL_ETH_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 1:
                    memcpy(context->contract_address_sent, OETH_ADDRESS, ADDRESS_LENGTH);
                    break;
                default:
                    PRINTF("Param not supported\n");
                    break;
            }
        } else if (memcmp(CURVE_OUSD_POOL_ADDRESS,
                          msg->pluginSharedRO->txContent->destination,
                          ADDRESS_LENGTH) == 0) {
            switch (msg->parameter[PARAMETER_LENGTH - 1]) {
                case 0:
                    memcpy(context->contract_address_sent, OUSD_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 1:
                    memcpy(context->contract_address_sent, DAI_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 2:
                    memcpy(context->contract_address_sent, USDC_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 3:
                    memcpy(context->contract_address_sent, USDT_ADDRESS, ADDRESS_LENGTH);
                    break;
                default:
                    PRINTF("Param not supported\n");
                    break;
            }
        }
    } else {
        memcpy(context->contract_address_sent,
               &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
               ADDRESS_LENGTH);
    }
    printf_hex_array("TOKEN SENT: ", ADDRESS_LENGTH, context->contract_address_sent);
}

static void handle_token_received(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    memset(context->contract_address_received, 0, sizeof(context->contract_address_received));

    // determine token addresses of curve pools based on contract address and
    // value of i/j params
    if (context->selectorIndex == CURVE_POOL_EXCHANGE ||
        context->selectorIndex == CURVE_POOL_EXCHANGE_UNDERLYING) {
        if (memcmp(CURVE_OETH_POOL_ADDRESS,
                   msg->pluginSharedRO->txContent->destination,
                   ADDRESS_LENGTH) == 0) {
            switch (msg->parameter[PARAMETER_LENGTH - 1]) {
                case 0:
                    memcpy(context->contract_address_received, NULL_ETH_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 1:
                    memcpy(context->contract_address_received, OETH_ADDRESS, ADDRESS_LENGTH);
                    break;
                default:
                    PRINTF("Param not supported\n");
                    break;
            }
        } else if (memcmp(CURVE_OUSD_POOL_ADDRESS,
                          msg->pluginSharedRO->txContent->destination,
                          ADDRESS_LENGTH) == 0) {
            switch (msg->parameter[PARAMETER_LENGTH - 1]) {
                case 0:
                    memcpy(context->contract_address_received, OUSD_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 1:
                    memcpy(context->contract_address_received, DAI_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 2:
                    memcpy(context->contract_address_received, USDC_ADDRESS, ADDRESS_LENGTH);
                    break;
                case 3:
                    memcpy(context->contract_address_received, USDT_ADDRESS, ADDRESS_LENGTH);
                    break;
                default:
                    PRINTF("Param not supported\n");
                    break;
            }
        }
    } else {
        memcpy(context->contract_address_received,
               &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
               ADDRESS_LENGTH);
    }
    printf_hex_array("TOKEN RECEIVED: ", ADDRESS_LENGTH, context->contract_address_received);
}

static void handle_amount_sent(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    memcpy(context->amount_sent, msg->parameter, INT256_LENGTH);
}

static void handle_min_amount_received(ethPluginProvideParameter_t *msg,
                                       origin_parameters_t *context) {
    memcpy(context->min_amount_received, msg->parameter, PARAMETER_LENGTH);
}

static void handle_beneficiary(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    memset(context->beneficiary, 0, sizeof(context->beneficiary));
    memcpy(context->beneficiary,
           &msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
           sizeof(context->beneficiary));
    printf_hex_array("BENEFICIARY: ", ADDRESS_LENGTH, context->beneficiary);
}

// deposit()
static void handle_zapper_deposit_eth(ethPluginProvideParameter_t *msg,
                                      origin_parameters_t *context) {
    switch (context->next_param) {
        case NONE:
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// depositSFRXETH(uint256 amount,uint256 minOETH)
static void handle_zapper_deposit_sfrxeth(ethPluginProvideParameter_t *msg,
                                          origin_parameters_t *context) {
    switch (context->next_param) {
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// mint(address _asset,uint256 _amount,uint256 _minimumOusdAmount)
static void handle_vault_mint(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// redeem(uint256 _amount,uint256 _minimumUnitAmount)
static void handle_vault_redeem(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    switch (context->next_param) {
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// exchange(int128 i,int128 j,uint256 _dx,uint256 _min_dy)
static void handle_curve_pool_exchange(ethPluginProvideParameter_t *msg,
                                       origin_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:
            handle_token_received(msg, context);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// exchange_multiple(address[9] _route,uint256[3][4] _swap_params,uint256
// _amount,uint256 _expected)
static void handle_curve_router_exchange(ethPluginProvideParameter_t *msg,
                                         origin_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:
            PRINTF("Counter: %d\n", context->counter);
            context->counter += 1;
            if (memcmp(&msg->parameter[PARAMETER_LENGTH - ADDRESS_LENGTH],
                       NULL_ETH_ADDRESS,
                       ADDRESS_LENGTH) == 0) {
                context->skip += 20 - context->counter;
                context->counter = 0;
                context->next_param = AMOUNT_SENT;
            } else {
                handle_token_received(msg, context);
                context->next_param = TOKEN_RECEIVED;
            }
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// exactInput(tuple params)
static void handle_uniswap_exchange(ethPluginProvideParameter_t *msg,
                                    origin_parameters_t *context) {
    switch (context->next_param) {
        case BENEFICIARY:
            handle_beneficiary(msg, context);
            context->skip += 1;
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = PATH_LENGTH;
            break;
        case PATH_LENGTH:
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("OFFSET: %d\n", context->offset);
            context->next_param = TOKEN_SENT;
            break;
        case TOKEN_SENT:
            // first 20 bytes of path is token sent
            memcpy(context->contract_address_sent, msg->parameter, ADDRESS_LENGTH);
            printf_hex_array("TOKEN_SENT: ", ADDRESS_LENGTH, context->contract_address_sent);
            context->skip = (context->offset - ADDRESS_LENGTH) / 32 - 1;
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:
            // address of token received starts at 20 bytes from end of path
            memcpy(&context->contract_address_received[0],
                   &msg->parameter[(context->offset - ADDRESS_LENGTH) % PARAMETER_LENGTH],
                   ADDRESS_LENGTH);
            context->next_param = TOKEN_RECEIVED_REST;
            break;
        case TOKEN_RECEIVED_REST:
            // copy rest of address in case it overflows into the next param
            memcpy(&context->contract_address_received[PARAMETER_LENGTH -
                                                       (context->offset - ADDRESS_LENGTH) %
                                                           PARAMETER_LENGTH],
                   &msg->parameter[0],
                   (context->offset - ADDRESS_LENGTH) % PARAMETER_LENGTH + ADDRESS_LENGTH -
                       PARAMETER_LENGTH);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// exactInputSingle(tuple params)
static void handle_uniswap_exchange_single(ethPluginProvideParameter_t *msg,
                                           origin_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            context->next_param = TOKEN_RECEIVED;
            break;
        case TOKEN_RECEIVED:
            handle_token_received(msg, context);
            context->skip += 1;
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:
            handle_beneficiary(msg, context);
            context->skip += 1;
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = MIN_AMOUNT_RECEIVED;
            break;
        case MIN_AMOUNT_RECEIVED:
            handle_min_amount_received(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// buyOusdWithUsdt(uint256 amount)
// sellOusdForUsdt(uint256 amount)
// buyOusdWithDai(uint256 amount)
// sellOusdForDai(uint256 amount)
// buyOusdWithUsdc(uint256 amount)
// sellOusdForUsdc(uint256 amount)
static void handle_flipper_exchange(ethPluginProvideParameter_t *msg,
                                    origin_parameters_t *context) {
    switch (context->next_param) {
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

// deposit(uint256 assets,address receiver)
// redeem(uint256 shares,address receiver,address owner)
static void handle_wrap(ethPluginProvideParameter_t *msg, origin_parameters_t *context) {
    switch (context->next_param) {
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:
            handle_beneficiary(msg, context);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    origin_parameters_t *context = (origin_parameters_t *) msg->pluginContext;
    printf_hex_array("oeth plugin provide parameter: ", PARAMETER_LENGTH, msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    if (context->skip) {
        // Skip this step, and don't forget to decrease skipping counter.
        context->skip--;
    } else {
        switch (context->selectorIndex) {
            case ZAPPER_DEPOSIT_ETH:
                handle_zapper_deposit_eth(msg, context);
                break;
            case ZAPPER_DEPOSIT_SFRXETH:
                handle_zapper_deposit_sfrxeth(msg, context);
                break;
            case VAULT_MINT:
                handle_vault_mint(msg, context);
                break;
            case VAULT_REDEEM:
                handle_vault_redeem(msg, context);
                break;
            case CURVE_POOL_EXCHANGE:
            case CURVE_POOL_EXCHANGE_UNDERLYING:
                handle_curve_pool_exchange(msg, context);
                break;
            case CURVE_ROUTER_EXCHANGE_MULTIPLE:
                handle_curve_router_exchange(msg, context);
                break;
            case UNISWAP_ROUTER_EXACT_INPUT:
                handle_uniswap_exchange(msg, context);
                break;
            case UNISWAP_ROUTER_EXACT_INPUT_SINGLE:
                handle_uniswap_exchange_single(msg, context);
                break;
            case FLIPPER_BUY_OUSD_WITH_USDT:
            case FLIPPER_SELL_OUSD_FOR_USDT:
            case FLIPPER_BUY_OUSD_WITH_DAI:
            case FLIPPER_SELL_OUSD_FOR_DAI:
            case FLIPPER_BUY_OUSD_WITH_USDC:
            case FLIPPER_SELL_OUSD_FOR_USDC:
                handle_flipper_exchange(msg, context);
                break;
            case WRAP:
            case UNWRAP:
                handle_wrap(msg, context);
                break;
            default:
                PRINTF("Selector Index %d not supported\n", context->selectorIndex);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
    }
}