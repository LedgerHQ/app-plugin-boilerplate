#include "boilerplate_plugin.h"
#include "eth_internals.h"

void get_public_key(uint8_t bip32PathLength,
                    uint32_t *bip32Path,
                    cx_sha3_t *sha3Context,
                    chain_config_t *chain_config,
                    uint8_t *out,
                    uint8_t outLength) {
    uint8_t privateKeyData[INT256_LENGTH];
    cx_ecfp_private_key_t privateKey;
    cx_ecfp_public_key_t publicKey;

    if (outLength != ADDRESS_LENGTH) {
        memset(out, 0, outLength);
        return;
    }

    os_perso_derive_node_bip32(CX_CURVE_256K1, bip32Path, bip32PathLength, privateKeyData, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);
    cx_ecfp_generate_pair(CX_CURVE_256K1, &publicKey, &privateKey, 1);
    explicit_bzero(&privateKey, sizeof(privateKey));
    explicit_bzero(privateKeyData, sizeof(privateKeyData));
    getEthAddressFromKey(&publicKey, (uint8_t *) out, &sha3Context, chain_config);
}