/*******************************************************************************
 *   Ethereum 2 Deposit Application
 *   (c) 2020 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cx.h"
#include "origin_plugin.h"
#include "os.h"

// List of selectors supported by this plugin.
// EDIT THIS: Adapt the variable names and change the `0x` values to match your
// selectors.
// static const uint32_t SWAP_EXACT_ETH_FOR_TOKENS_SELECTOR = 0x7ff36ab5;
// static const uint32_t BOILERPLATE_DUMMY_SELECTOR_2 = 0x13374242;
static const uint32_t ZAPPER_DEPOSIT_ETH_SELECTOR = 0xd0e30db0;  // deposit(),
static const uint32_t ZAPPER_DEPOSIT_SFRXETH_SELECTOR =
    0xd443e97d;  // depositSFRXETH(uint256 amount,uint256 minOETH)
static const uint32_t VAULT_MINT_SELECTOR = 0x156e29f6;  // mint(address _asset,uint256
                                                         // _amount,uint256 _minimumOusdAmount)
static const uint32_t VAULT_REDEEM_SELECTOR =
    0x7cbc2373;  // redeem(uint256 _amount,uint256 _minimumUnitAmount)
static const uint32_t CURVE_POOL_EXCHANGE_SELECTOR =
    0x3df02124;  // exchange(int128 i,int128 j,uint256 _dx,uint256 _min_dy)
static const uint32_t CURVE_POOL_EXCHANGE_UNDERLYING_SELECTOR =
    0xa6417ed6;  // exchange_underlying(int128 i, int128 j, uint256 dx, uint256
                 // min_dy)
static const uint32_t CURVE_ROUTER_EXCHANGE_MULTIPLE_SELECTOR =
    0x353ca424;  // exchange_multiple(address[9] _route,uint256[3][4]
                 // _swap_params,uint256 _amount,uint256 _expected)
static const uint32_t UNISWAP_ROUTER_EXACT_INPUT_SELECTOR = 0xc04b8d59;  // exactInput(tuple params)
static const uint32_t UNISWAP_ROUTER_EXACT_INPUT_SINGLE_SELECTOR =
    0x414bf389;  // exactInputSingle(tuple params)
static const uint32_t FLIPPER_BUY_OUSD_WITH_USDT_SELECTOR =
    0x35aa0b96;  // buyOusdWithUsdt(uint256 amount)
static const uint32_t FLIPPER_SELL_OUSD_FOR_USDT_SELECTOR =
    0xcb939053;  // sellOusdForUsdt(uint256 amount)
static const uint32_t FLIPPER_BUY_OUSD_WITH_DAI_SELECTOR =
    0x5981c746;  // buyOusdWithDai(uint256 amount)
static const uint32_t FLIPPER_SELL_OUSD_FOR_DAI_SELECTOR =
    0x8a095a0f;  // sellOusdForDai(uint256 amount)
static const uint32_t FLIPPER_BUY_OUSD_WITH_USDC_SELECTOR =
    0xbfc11ffd;  // buyOusdWithUsdc(uint256 amount)
static const uint32_t FLIPPER_SELL_OUSD_FOR_USDC_SELECTOR =
    0xc6b68169;                                    // sellOusdForUsdc(uint256 amount)
static const uint32_t WRAP_SELECTOR = 0x6e553f65;  // deposit(uint256 assets,address receiver)
static const uint32_t UNWRAP_SELECTOR =
    0xba087652;  // redeem(uint256 shares,address receiver,address owner)

// contract address check for:
// VAULT_MINT_SELECTOR
// VAULT_REDEEM_SELECTOR
// CURVE_POOL_EXCHANGE_SELECTOR
// CURVE_POOL_EXCHANGE_UNDERLYING_SELECTOR

// Array of all the different boilerplate selectors. Make sure this follows the
// same order as the enum defined in `boilerplate_plugin.h` EDIT THIS: Use the
// names of the array declared above.
const uint32_t ORIGIN_SELECTORS[NUM_SELECTORS] = {ZAPPER_DEPOSIT_ETH_SELECTOR,
                                                  ZAPPER_DEPOSIT_SFRXETH_SELECTOR,
                                                  VAULT_MINT_SELECTOR,
                                                  VAULT_REDEEM_SELECTOR,
                                                  CURVE_POOL_EXCHANGE_SELECTOR,
                                                  CURVE_POOL_EXCHANGE_UNDERLYING_SELECTOR,
                                                  CURVE_ROUTER_EXCHANGE_MULTIPLE_SELECTOR,
                                                  UNISWAP_ROUTER_EXACT_INPUT_SELECTOR,
                                                  UNISWAP_ROUTER_EXACT_INPUT_SINGLE_SELECTOR,
                                                  FLIPPER_BUY_OUSD_WITH_USDT_SELECTOR,
                                                  FLIPPER_SELL_OUSD_FOR_USDT_SELECTOR,
                                                  FLIPPER_BUY_OUSD_WITH_DAI_SELECTOR,
                                                  FLIPPER_SELL_OUSD_FOR_DAI_SELECTOR,
                                                  FLIPPER_BUY_OUSD_WITH_USDC_SELECTOR,
                                                  FLIPPER_SELL_OUSD_FOR_USDC_SELECTOR,
                                                  WRAP_SELECTOR,
                                                  UNWRAP_SELECTOR};

const uint8_t OETH_ADDRESS[ADDRESS_LENGTH] = {0x85, 0x6c, 0x4e, 0xfb, 0x76, 0xc1, 0xd1,
                                              0xae, 0x02, 0xe2, 0x0c, 0xeb, 0x03, 0xa2,
                                              0xa6, 0xa0, 0x8b, 0x0b, 0x8d, 0xc3};

const uint8_t STETH_ADDRESS[ADDRESS_LENGTH] = {0xae, 0x7a, 0xb9, 0x65, 0x20, 0xde, 0x3a,
                                               0x18, 0xe5, 0xe1, 0x11, 0xb5, 0xea, 0xab,
                                               0x09, 0x53, 0x12, 0xd7, 0xfe, 0x84};

const uint8_t OUSD_ADDRESS[ADDRESS_LENGTH] = {0x2a, 0x8e, 0x1e, 0x67, 0x6e, 0xc2, 0x38,
                                              0xd8, 0xa9, 0x92, 0x30, 0x7b, 0x49, 0x5b,
                                              0x45, 0xb3, 0xfe, 0xaa, 0x5e, 0x86};

const uint8_t DAI_ADDRESS[ADDRESS_LENGTH] = {0x6b, 0x17, 0x54, 0x74, 0xe8, 0x90, 0x94,
                                             0xc4, 0x4d, 0xa9, 0x8b, 0x95, 0x4e, 0xed,
                                             0xea, 0xc4, 0x95, 0x27, 0x1d, 0x0f};

const uint8_t USDC_ADDRESS[ADDRESS_LENGTH] = {0xa0, 0xb8, 0x69, 0x91, 0xc6, 0x21, 0x8b,
                                              0x36, 0xc1, 0xd1, 0x9d, 0x4a, 0x2e, 0x9e,
                                              0xb0, 0xce, 0x36, 0x06, 0xeb, 0x48};

const uint8_t USDT_ADDRESS[ADDRESS_LENGTH] = {0xda, 0xc1, 0x7f, 0x95, 0x8d, 0x2e, 0xe5,
                                              0x23, 0xa2, 0x20, 0x62, 0x06, 0x99, 0x45,
                                              0x97, 0xc1, 0x3d, 0x83, 0x1e, 0xc7};

const uint8_t OETH_VAULT_ADDRESS[ADDRESS_LENGTH] = {0x39, 0x25, 0x40, 0x33, 0x94, 0x5a, 0xa2,
                                                    0xe4, 0x80, 0x9c, 0xc2, 0x97, 0x7e, 0x70,
                                                    0x87, 0xbe, 0xe4, 0x8b, 0xd7, 0xab};

const uint8_t WOETH_ADDRESS[ADDRESS_LENGTH] = {0xdc, 0xee, 0x70, 0x65, 0x42, 0x61, 0xaf,
                                               0x21, 0xc4, 0x4c, 0x09, 0x3c, 0x30, 0x0e,
                                               0xd3, 0xbb, 0x97, 0xb7, 0x81, 0x92};

const uint8_t CURVE_OETH_POOL_ADDRESS[ADDRESS_LENGTH] = {0x94, 0xb1, 0x74, 0x76, 0xa9, 0x3b, 0x32,
                                                         0x62, 0xd8, 0x7b, 0x9a, 0x32, 0x69, 0x65,
                                                         0xd1, 0xe9, 0x1f, 0x9c, 0x13, 0xe7};

const uint8_t CURVE_OUSD_POOL_ADDRESS[ADDRESS_LENGTH] = {0x87, 0x65, 0x0d, 0x7b, 0xbf, 0xc3, 0xa9,
                                                         0xf1, 0x05, 0x87, 0xd7, 0x77, 0x82, 0x06,
                                                         0x67, 0x17, 0x19, 0xd9, 0x91, 0x0d};

// Function to dispatch calls from the ethereum app.
void dispatch_plugin_calls(int message, void *parameters) {
    switch (message) {
        case ETH_PLUGIN_INIT_CONTRACT:
            handle_init_contract(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_PARAMETER:
            handle_provide_parameter(parameters);
            break;
        case ETH_PLUGIN_FINALIZE:
            handle_finalize(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_INFO:
            handle_provide_token(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_ID:
            handle_query_contract_id(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            handle_query_contract_ui(parameters);
            break;
        default:
            PRINTF("Unhandled message %d\n", message);
            break;
    }
}

void handle_query_ui_exception(unsigned int *args) {
    switch (args[0]) {
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            ((ethQueryContractUI_t *) args[1])->result = ETH_PLUGIN_RESULT_ERROR;
            break;
        default:
            break;
    }
}

const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Calls the ethereum app.
void call_app_ethereum() {
    unsigned int libcall_params[3];
    libcall_params[0] = (unsigned int) "Ethereum";
    libcall_params[1] = 0x100;
    libcall_params[2] = RUN_APPLICATION;
    os_lib_call((unsigned int *) &libcall_params);
}

// Weird low-level black magic. No need to edit this.
__attribute__((section(".boot"))) int main(int arg0) {
    // Exit critical section
    __asm volatile("cpsie i");

    // Ensure exception will work as planned
    os_boot();

    // Try catch block. Please read the docs for more information on how to use
    // those!
    BEGIN_TRY {
        TRY {
            // Low-level black magic.
            check_api_level(CX_COMPAT_APILEVEL);

            // Check if we are called from the dashboard.
            if (!arg0) {
                // Called from dashboard, launch Ethereum app
                call_app_ethereum();
                return 0;
            } else {
                // Not called from dashboard: called from the ethereum app!
                const unsigned int *args = (const unsigned int *) arg0;

                // If `ETH_PLUGIN_CHECK_PRESENCE` is set, this means the caller is just
                // trying to know whether this app exists or not. We can skip
                // `dispatch_plugin_calls`.
                if (args[0] != ETH_PLUGIN_CHECK_PRESENCE) {
                    dispatch_plugin_calls(args[0], (void *) args[1]);
                }
            }
        }
        CATCH_OTHER(e) {
            switch (e) {
                // These exceptions are only generated on handle_query_contract_ui()
                case 0x6502:
                case EXCEPTION_OVERFLOW:
                    handle_query_ui_exception((unsigned int *) arg0);
                    break;
                default:
                    break;
            }
            PRINTF("Exception 0x%x caught\n", e);
        }
        FINALLY {
            // Call `os_lib_end`, go back to the ethereum app.
            os_lib_end();
        }
    }
    END_TRY;

    // Will not get reached.
    return 0;
}
