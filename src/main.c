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

#include "os.h"
#include "cx.h"

#include "glyphs.h"

#include "boilerplate_plugin.h"

// List of selectors supported by this plugin.
// EDIT THIS: Adapt the variable names and change the `0x` values to match your selectors.
static const uint32_t SWAP_EXACT_ETH_FOR_TOKENS_SELECTOR = 0x7ff36ab5;
static const uint32_t BOILERPLATE_DUMMY_SELECTOR_2 = 0x13374242;

// Array of all the different boilerplate selectors. Make sure this follows the same order as the
// enum defined in `boilerplate_plugin.h`
// EDIT THIS: Use the names of the array declared above.
const uint32_t BOILERPLATE_SELECTORS[NUM_SELECTORS] = {
    SWAP_EXACT_ETH_FOR_TOKENS_SELECTOR,
    BOILERPLATE_DUMMY_SELECTOR_2,
};

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

// Calls the ethereum app.
void call_app_ethereum() {
    unsigned int libcall_params[5];
    libcall_params[0] = (unsigned int) "Ethereum";
    libcall_params[1] = 0x100;
    libcall_params[2] = RUN_APPLICATION;
    libcall_params[3] = (unsigned int) NULL;
#ifdef HAVE_NBGL
    caller_app_t capp;
    const char name[] = APPNAME;
    nbgl_icon_details_t icon_details;
    uint8_t bitmap[sizeof(ICONBITMAP)];

    memcpy(&icon_details, &ICONGLYPH, sizeof(ICONGLYPH));
    memcpy(&bitmap, &ICONBITMAP, sizeof(bitmap));
    icon_details.bitmap = (const uint8_t *) bitmap;
    capp.name = (const char *) name;
    capp.icon = &icon_details;
    libcall_params[4] = (unsigned int) &capp;
#else
    libcall_params[4] = (unsigned int) NULL;
#endif
    os_lib_call((unsigned int *) &libcall_params);
}

// Weird low-level black magic. No need to edit this.
__attribute__((section(".boot"))) int main(int arg0) {
    // Exit critical section
    __asm volatile("cpsie i");

    // Ensure exception will work as planned
    os_boot();

    // Try catch block. Please read the docs for more information on how to use those!
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

                // If `ETH_PLUGIN_CHECK_PRESENCE` is set, this means the caller is just trying to
                // know whether this app exists or not. We can skip `dispatch_plugin_calls`.
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
