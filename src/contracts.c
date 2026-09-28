#include "contracts.h"

#ifdef ERC8009_SCHEMA_NAMES
#define SLOT(kind, abi_name) {kind, abi_name}
#define FUNCTION(abi_name)   .function = abi_name,
#else
#define SLOT(kind, abi_name) {kind}
#define FUNCTION(abi_name)
#endif

// clang-format off
const selector_schema_t SCHEMAS[SELECTOR_COUNT] = {
    [PROXY_CALL] = {FUNCTION("proxyCall")
        .family = FAMILY_BP, .diffs = 0, .head_len = 5,
        .slots = {SLOT(P_GUARDS, "postBalances"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [PROXY_CALL_DIFFS] = {FUNCTION("proxyCallDiffs")
        .family = FAMILY_BP, .diffs = 1, .head_len = 5,
        .slots = {SLOT(P_GUARDS, "diffs"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [APPROVE_PROXY_CALL] = {FUNCTION("approveProxyCall")
        .family = FAMILY_AR, .diffs = 0, .head_len = 6,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "postBalances"),
                  SLOT(P_APPROVALS, "approvals"), SLOT(P_ADDR_TARGET, "target"),
                  SLOT(P_BYTES, "data"), SLOT(P_WITHDRAWALS, "withdrawals")}},
    [APPROVE_PROXY_CALL_DIFFS] = {FUNCTION("approveProxyCallDiffs")
        .family = FAMILY_AR, .diffs = 1, .head_len = 6,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "diffs"),
                  SLOT(P_APPROVALS, "approvals"), SLOT(P_ADDR_TARGET, "target"),
                  SLOT(P_BYTES, "data"), SLOT(P_WITHDRAWALS, "withdrawals")}},
    [APPROVE_PROXY_CALL_META] = {FUNCTION("approveProxyCallWithMeta")
        .family = FAMILY_AR, .diffs = 0, .head_len = 7,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "balances"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [APPROVE_PROXY_CALL_DIFFS_META] = {FUNCTION("approveProxyCallDiffsWithMeta")
        .family = FAMILY_AR, .diffs = 1, .head_len = 7,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "diffs"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [PERMIT_PROXY_CALL] = {FUNCTION("permitProxyCall")
        .family = FAMILY_PR, .diffs = 0, .head_len = 7,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "postBalances"),
                  SLOT(P_APPROVALS, "approvals"), SLOT(P_PERMITS, "permits"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [PERMIT_PROXY_CALL_DIFFS] = {FUNCTION("permitProxyCallDiffs")
        .family = FAMILY_PR, .diffs = 1, .head_len = 7,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "diffs"),
                  SLOT(P_APPROVALS, "approvals"), SLOT(P_PERMITS, "permits"),
                  SLOT(P_ADDR_TARGET, "target"), SLOT(P_BYTES, "data"),
                  SLOT(P_WITHDRAWALS, "withdrawals")}},
    [PERMIT_PROXY_CALL_META] = {FUNCTION("permitProxyCallWithMeta")
        .family = FAMILY_PR, .diffs = 0, .head_len = 8,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "balances"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_PERMITS, "permits"), SLOT(P_ADDR_TARGET, "target"),
                  SLOT(P_BYTES, "data"), SLOT(P_WITHDRAWALS, "withdrawals")}},
    [PERMIT_PROXY_CALL_DIFFS_META] = {FUNCTION("permitProxyCallDiffsWithMeta")
        .family = FAMILY_PR, .diffs = 1, .head_len = 8,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "diffs"), SLOT(P_APPROVALS, "approvals"),
                  SLOT(P_PERMITS, "permits"), SLOT(P_ADDR_TARGET, "target"),
                  SLOT(P_BYTES, "data"), SLOT(P_WITHDRAWALS, "withdrawals")}},
    [SAFE_PROXY_CALL] = {FUNCTION("safeExecuteWithPostBalances")
        .family = FAMILY_SR, .diffs = 0, .head_len = 4,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "postBalances"),
                  SLOT(P_ADDR_SAFE, "safe"), SLOT(P_SAFE_TX, "safeTx")}},
    [SAFE_PROXY_CALL_DIFFS] = {FUNCTION("safeExecuteWithDiffs")
        .family = FAMILY_SR, .diffs = 1, .head_len = 4,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_GUARDS, "diffs"),
                  SLOT(P_ADDR_SAFE, "safe"), SLOT(P_SAFE_TX, "safeTx")}},
    [SAFE_PROXY_CALL_META] = {FUNCTION("safeExecuteWithPostBalancesMeta")
        .family = FAMILY_SR, .diffs = 0, .head_len = 5,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "postBalances"), SLOT(P_ADDR_SAFE, "safe"),
                  SLOT(P_SAFE_TX, "safeTx")}},
    [SAFE_PROXY_CALL_DIFFS_META] = {FUNCTION("safeExecuteWithDiffsMeta")
        .family = FAMILY_SR, .diffs = 1, .head_len = 5,
        .slots = {SLOT(P_ADDR_PROXY, "balanceProxy"), SLOT(P_META, "meta"),
                  SLOT(P_GUARDS, "diffs"), SLOT(P_ADDR_SAFE, "safe"),
                  SLOT(P_SAFE_TX, "safeTx")}},
};

const uint8_t CONTRACT_ADDRESSES[FAMILY_COUNT][ADDRESS_LENGTH] = {
    [FAMILY_BP] = {0x80, 0x0b, 0x36, 0xcc, 0x16, 0xc7, 0x57, 0x2e, 0x1d, 0x60,
                   0x05, 0x06, 0xa0, 0xd7, 0xd3, 0x08, 0x90, 0xff, 0x35, 0xf4},
    [FAMILY_AR] = {0xf3, 0xc8, 0x2e, 0x21, 0x76, 0xd8, 0x94, 0xed, 0x32, 0xc5,
                   0x74, 0x8c, 0x22, 0x23, 0x8e, 0xbf, 0xcc, 0x68, 0xcd, 0xd8},
    [FAMILY_PR] = {0xfe, 0xa6, 0x07, 0xc2, 0xf7, 0x4d, 0x7a, 0xc4, 0xac, 0x35,
                   0x36, 0x0e, 0x51, 0xf3, 0xfe, 0x08, 0xe3, 0x19, 0xeb, 0xa2},
    [FAMILY_SR] = {0x7d, 0x7e, 0xae, 0x4d, 0x58, 0xe9, 0xd2, 0xa9, 0x2a, 0x48,
                   0xee, 0x55, 0xf2, 0x49, 0xa2, 0x90, 0x3f, 0xd1, 0xd0, 0xd0},
};
// clang-format on

bool contract_family(const uint8_t *address, uint8_t *family) {
    for (uint8_t i = 0; i < FAMILY_COUNT; i++) {
        if (memcmp(address, CONTRACT_ADDRESSES[i], ADDRESS_LENGTH) == 0) {
            *family = i;
            return true;
        }
    }
    return false;
}

bool param_is_dynamic(uint8_t kind) {
    return kind >= P_META;
}

bool schema_is_meta(const selector_schema_t *schema) {
    for (uint8_t i = 0; i < schema->head_len; i++) {
        if (schema->slots[i].kind == P_META) {
            return true;
        }
    }
    return false;
}

bool chain_id_ok(const uint8_t *value, uint8_t length) {
    if (length > sizeof(uint64_t)) {
        return false;
    }
    return u64_from_BE(value, length) == CHAIN_ID;
}

bool tx_value_is_zero(const txInt256_t *value) {
    uint8_t length = (value->length > INT256_LENGTH) ? INT256_LENGTH : value->length;

    return allzeroes(value->value, length) != 0;
}
