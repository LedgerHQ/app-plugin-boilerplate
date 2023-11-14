#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "plugin.h"
#include <time.h>

#define FUZZ_INIT_CONTRACT
#define FUZZ_PROVIDE_PARAMETER
#define FUZZ_FINALIZE
#define FUZZ_PROVIDE_INFO
#define FUZZ_QUERY_CONTRACT_ID
#define FUZZ_QUERY_CONTRACT_UI

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    int message = 0x0100;
    context_t pluginContext;

    if ((!data) || (size < 1)) return 0;
    message |= data[0];

    memset(&pluginContext, 0, sizeof(context_t));

    switch (message) {
#ifdef FUZZ_INIT_CONTRACT
        case ETH_PLUGIN_INIT_CONTRACT:
            if (size < 3 + sizeof(context_t)) return 0;

            uint8_t selector[4];
            memset(selector, 0, 4);
            U4BE_ENCODE(selector, 0, SELECTORS[data[2] % SELECTOR_COUNT]);

            memcpy(&pluginContext, data + 3, sizeof(context_t) - 3);

            ethPluginInitContract_t initcontract_parameters = {
                .interfaceVersion = ETH_PLUGIN_INTERFACE_VERSION_LATEST,
                .result = data[1],

                .pluginContext = (uint8_t *) &pluginContext,
                .pluginContextLength = size - 3,

                .selector = selector,
            };

            handle_init_contract(&initcontract_parameters);
            break;
#endif /* FUZZ_INIT_CONTRACT */

#ifdef FUZZ_PROVIDE_PARAMETER
        case ETH_PLUGIN_PROVIDE_PARAMETER:
            if (size < 40 + sizeof(context_t)) return 0;

            pluginContext = (context_t){
                .offset = 1,
                .next_param = data[2],
                .selectorIndex = data[2],

                .go_to_offset = true,

            };

            ethPluginProvideParameter_t provideparameter_parameters = {
                .pluginSharedRW = NULL,
                .pluginSharedRO = NULL,
                .pluginContext = (uint8_t *) &pluginContext,
                .parameter = data + 1,
                .parameterOffset = 0,

                .result = data[5]};

            handle_provide_parameter(&provideparameter_parameters);
            break;
#endif /* FUZZ_PROVIDE_PARAMETER */

#ifdef FUZZ_FINALIZE
        case ETH_PLUGIN_FINALIZE:
            if (size < 6 + sizeof(context_t) + ADDRESS_LENGTH) return 0;

            srand(time(NULL));

            pluginContext = (context_t){
                .offset = 1,
                .next_param = data[2],
                .selectorIndex = (unsigned char) (rand() % 34),
                .go_to_offset = true,

            };

            char address[ADDRESS_LENGTH];
            memset(address, 0, ADDRESS_LENGTH);
            memcpy(address, data + 4, ADDRESS_LENGTH);

            ethPluginFinalize_t finalize_parameters = {.pluginSharedRW = NULL,
                                                       .pluginSharedRO = NULL,
                                                       .pluginContext = (uint8_t *) &pluginContext,

                                                       .tokenLookup1 = NULL,
                                                       .tokenLookup2 = NULL,

                                                       .result = data[3]};

            handle_finalize(&finalize_parameters);
            break;
#endif /* FUZZ_FINALIZE */

#ifdef FUZZ_PROVIDE_INFO
        case ETH_PLUGIN_PROVIDE_INFO:
            if (size < 3 + sizeof(context_t)) return 0;

            memcpy(&pluginContext, data + 2, sizeof(context_t) - 2);

            ethPluginProvideInfo_t provideinfo_parameters = {
                .pluginSharedRW = NULL,
                .pluginSharedRO = NULL,
                .pluginContext = (uint8_t *) &pluginContext,

                .item1 = NULL,
                .item2 = NULL,

                .additionalScreens = data[1],

                .result = data[2]};

            handle_provide_token(&provideinfo_parameters);
            break;
#endif /* FUZZ_PROVIDE_INFO */

#ifdef FUZZ_QUERY_CONTRACT_ID
        case ETH_PLUGIN_QUERY_CONTRACT_ID:
            if (size < 120 + ADDRESS_LENGTH) return 0;

            pluginContext = (context_t){
                .next_param = data[2],
                .selectorIndex = rand() % 34,
                .go_to_offset = true,
            };

            size_t nameVersionLength = size - (2 + ADDRESS_LENGTH);
            size_t nameLength = (rand() % (nameVersionLength - 1)) + 1;
            size_t versionLength = nameVersionLength - nameLength;

            char *name = calloc(sizeof(char), nameLength);
            memcpy(name, data + (2 + ADDRESS_LENGTH), nameLength);

            char *version = calloc(sizeof(char), versionLength);
            memcpy(version, data + (2 + ADDRESS_LENGTH + nameLength), versionLength);

            ethQueryContractID_t querycontractid_parameters = {
                .pluginSharedRW = NULL,
                .pluginSharedRO = NULL,
                .pluginContext = (uint8_t *) &pluginContext,

                .name = name,
                .nameLength = nameLength,
                .version = version,
                .versionLength = versionLength,

                .result = data[1]};

            handle_query_contract_id(&querycontractid_parameters);
            free(name);
            free(version);
            break;
#endif /* FUZZ_QUERY_CONTRACT_ID */

#ifdef FUZZ_QUERY_CONTRACT_UI
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            if (size < 64 + 20 + sizeof(context_t)) return 0;

            size_t titleMsgLength = size - (1 + sizeof(context_t));
            size_t titleLength = (rand() % (titleMsgLength - 1)) + 1;
            size_t msgLength = 43;

            char *title = calloc(sizeof(char), titleLength);
            memcpy(title, data + 3, titleLength);

            char *msg = calloc(sizeof(char), msgLength);
            memcpy(msg, data + 3 + titleLength, msgLength);

            pluginContext = (context_t){
                .next_param = data[2],
                .selectorIndex = rand() % 34,
                .go_to_offset = true,
            };

            ethPluginSharedRW_t PSRw = {.sha3 = (cx_sha3_t *) data + 64};

            ethQueryContractUI_t querycontract_parameters = {
                .pluginSharedRW =
                    &PSRw,  // There is a case (l24) where we need pluginSharedRW->sha3
                .pluginSharedRO = NULL,
                .item1 = NULL,
                .item2 = NULL,
                .network_ticker = "",
                .pluginContext = (uint8_t *) &pluginContext,
                .screenIndex = rand() % 5,

                .title = title,
                .titleLength = titleLength,
                .msg = msg,
                .msgLength = msgLength,

                .result = data[2]};

            handle_query_contract_ui(&querycontract_parameters);
            free(title);
            free(msg);
            break;
#endif /* FUZZ_QUERY_CONTRACT_UI */

        default:
            break;
    }

    return 0;
}
