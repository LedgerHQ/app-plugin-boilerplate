#include "plugin.h"

// set a small size to detect possible overflows
#define NAME_LENGTH    3u
#define VERSION_LENGTH 3u

void handle_init_contract(ethPluginInitContract_t *parameters);
void handle_provide_parameter(ethPluginProvideParameter_t *parameters);
void handle_finalize(ethPluginFinalize_t *parameters);
void handle_provide_token(ethPluginProvideInfo_t *parameters);
void handle_query_contract_id(ethQueryContractID_t *parameters);
void handle_query_contract_ui(ethQueryContractUI_t *parameters);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    ethPluginInitContract_t init_contract = {0};
    ethPluginProvideParameter_t provide_param = {0};
    ethPluginFinalize_t finalize = {0};
    ethPluginProvideInfo_t provide_info = {0};
    ethQueryContractID_t query_id = {0};
    ethQueryContractUI_t query_ui = {0};
    txContent_t content = {0};

    // Fake sha3 context
    cx_sha3_t sha3;

    ethPluginSharedRO_t shared_ro;
    shared_ro.txContent = &content;

    ethPluginSharedRW_t shared_rw;
    shared_rw.sha3 = &sha3;

    context_t context;
    const uint8_t address[ADDRESS_LENGTH] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                             0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                             0xee, 0xee, 0xee, 0xee, 0xee, 0xee};

    // see fullAmount / fullAddress in
    char title[32] = {0};
    char msg[79] = {0};  // 2^256 is 78 digits long

    // for token lookups
    extraInfo_t item1 = {0};
    extraInfo_t item2 = {0};

    char name[NAME_LENGTH] = {0};
    char version[VERSION_LENGTH] = {0};

    // data must be big enough to hold a selector
    if (size < 4) {
        return 0;
    }

    context.next_param = 99;

    init_contract.interfaceVersion = ETH_PLUGIN_INTERFACE_VERSION_LATEST;
    init_contract.selector = data;
    init_contract.pluginContext = (uint8_t *) &context;
    init_contract.pluginContextLength = sizeof(context);
    init_contract.pluginSharedRO = &shared_ro;

    handle_init_contract(&init_contract);
    if (init_contract.result != ETH_PLUGIN_RESULT_OK) {
        return 0;
    }

    size_t i = 4;
    // potentially save space for token lookups
    while (size - i >= 32 + sizeof(extraInfo_t) * 2) {
        provide_param.parameter = data + i;
        provide_param.parameterOffset = i;
        provide_param.pluginContext = (uint8_t *) &context;
        handle_provide_parameter(&provide_param);
        if (provide_param.result != ETH_PLUGIN_RESULT_OK) {
            return 0;
        }
        i += 32;
    }

    finalize.pluginContext = (uint8_t *) &context;
    finalize.address = address;
    handle_finalize(&finalize);
    if (finalize.result != ETH_PLUGIN_RESULT_OK) {
        return 0;
    }

    provide_info.pluginContext = (uint8_t *) &context;
    if (finalize.tokenLookup1) {
        if (size - i >= sizeof(extraInfo_t)) {
            provide_info.item1 = &item1;

            memcpy(provide_info.item1, data + i, sizeof(extraInfo_t));
            provide_info.item1->token.ticker[MAX_TICKER_LEN - 1] = '\0';
            i += sizeof(extraInfo_t);
        }
    }

    if (finalize.tokenLookup2) {
        if (size - i >= sizeof(extraInfo_t)) {
            provide_info.item2 = &item2;

            memcpy(provide_info.item2, data + i, sizeof(extraInfo_t));
            provide_info.item2->token.ticker[MAX_TICKER_LEN - 1] = '\0';
            i += sizeof(extraInfo_t);
        }
    }

    handle_provide_token(&provide_info);
    if (provide_info.result != ETH_PLUGIN_RESULT_OK) {
        return 0;
    }

    query_id.pluginContext = (uint8_t *) &context;
    query_id.name = name;
    query_id.nameLength = sizeof(name);
    query_id.version = version;
    query_id.versionLength = sizeof(version);
    handle_query_contract_id(&query_id);

    if (query_id.result != ETH_PLUGIN_RESULT_OK) {
        return 0;
    }

    printf("name:    %s\n", query_id.name);
    printf("version: %s\n", query_id.version);

    for (int i = 0; i < finalize.numScreens + provide_info.additionalScreens; i++) {
        query_ui.title = title;
        query_ui.titleLength = sizeof(title);
        query_ui.msg = msg;
        query_ui.msgLength = sizeof(msg);
        query_ui.pluginContext = (uint8_t *) &context;
        query_ui.pluginSharedRO = &shared_ro;
        query_ui.pluginSharedRW = &shared_rw;

        query_ui.screenIndex = i;
        handle_query_contract_ui(&query_ui);
        if (query_ui.result != ETH_PLUGIN_RESULT_OK) {
            return 0;
        }
        printf("%s: %s\n", title, msg);
    }

    return 0;
}
