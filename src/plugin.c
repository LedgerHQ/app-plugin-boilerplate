/*******************************************************************************
 *   ERC-8009 plugin
 *   (c) 2023 Ledger
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

#include <stdint.h>
#include "plugin.h"

const uint32_t SELECTORS[SELECTOR_COUNT] = {SELECTORS_LIST(TO_VALUE)};

eth_plugin_result_t erc8009_error(context_t *context, uint8_t reason, uint32_t offset) {
    // The first ERROR is frozen: later calls keep reporting it.
    if (context->outcome != OUTCOME_ERROR) {
        context->outcome = OUTCOME_ERROR;
        context->reason = reason;
    }
    PRINTF("ERC8009 E:%u off=%u\n", (unsigned) context->reason, (unsigned) offset);
    (void) offset;
    return ETH_PLUGIN_RESULT_ERROR;
}
