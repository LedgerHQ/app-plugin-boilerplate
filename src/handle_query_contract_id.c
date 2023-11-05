#include "origin_defi_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
  ethQueryContractID_t *msg = (ethQueryContractID_t *)parameters;
  const origin_defi_parameters_t *context =
      (const origin_defi_parameters_t *)msg->pluginContext;
  // msg->name will be the upper sentence displayed on the screen.
  // msg->version will be the lower sentence displayed on the screen.

  // For the first screen, display the plugin name.
  strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

  switch (context->selectorIndex) {
  case ZAPPER_DEPOSIT_ETH:
  case ZAPPER_DEPOSIT_SFRXETH:
  case VAULT_MINT:
    strlcpy(msg->version, "Mint", msg->versionLength);
    break;
  case VAULT_REDEEM:
    strlcpy(msg->version, "Redeem", msg->versionLength);
    break;
  case CURVE_POOL_EXCHANGE:
  case CURVE_POOL_EXCHANGE_UNDERLYING:
  case CURVE_ROUTER_EXCHANGE_MULTIPLE:
  case UNISWAP_ROUTER_EXACT_INPUT:
  case UNISWAP_ROUTER_EXACT_INPUT_SINGLE:
  case FLIPPER_BUY_OUSD_WITH_USDT:
  case FLIPPER_SELL_OUSD_FOR_USDT:
  case FLIPPER_BUY_OUSD_WITH_DAI:
  case FLIPPER_SELL_OUSD_FOR_DAI:
  case FLIPPER_BUY_OUSD_WITH_USDC:
  case FLIPPER_SELL_OUSD_FOR_USDC:
    strlcpy(msg->version, "Swap", msg->versionLength);
    break;
  case WRAP:
    strlcpy(msg->version, "Wrap", msg->versionLength);
    break;
  case UNWRAP:
    strlcpy(msg->version, "Unwrap", msg->versionLength);
    break;
  default:
    PRINTF("Selector Index :%d not supported\n", context->selectorIndex);
    msg->result = ETH_PLUGIN_RESULT_ERROR;
    return;
  }

  msg->result = ETH_PLUGIN_RESULT_OK;
}