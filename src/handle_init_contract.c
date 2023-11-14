#include "origin_plugin.h"

static int find_selector(uint32_t selector, const uint32_t *selectors, size_t n,
                         selector_t *out) {
  for (selector_t i = 0; i < n; i++) {
    if (selector == selectors[i]) {
      *out = i;
      return 0;
    }
  }
  return -1;
}

// Called once to init.
void handle_init_contract(void *parameters) {
  // Cast the msg to the type of structure we expect (here,
  // ethPluginInitContract_t).
  ethPluginInitContract_t *msg = (ethPluginInitContract_t *)parameters;

  // Make sure we are running a compatible version.
  if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
    // If not the case, return the `UNAVAILABLE` status.
    msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
    return;
  }

  // Double check that the `context_t` struct is not bigger than the maximum
  // size (defined by `msg->pluginContextLength`).
  if (msg->pluginContextLength < sizeof(origin_parameters_t)) {
    PRINTF("Plugin parameters structure is bigger than allowed size\n");
    msg->result = ETH_PLUGIN_RESULT_ERROR;
    return;
  }

  origin_parameters_t *context = (origin_parameters_t *)msg->pluginContext;

  // Initialize the context (to 0).
  memset(context, 0, sizeof(*context));

  uint32_t selector = U4BE(msg->selector, 0);
  if (find_selector(selector, ORIGIN_SELECTORS, NUM_SELECTORS,
                    &context->selectorIndex)) {
    msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
    return;
  }

  // Set `next_param` to be the first field we expect to parse.
  // EDIT THIS: Adapt the `cases`, and set the `next_param` to be the first
  // parameter you expect to parse.
  switch (context->selectorIndex) {
    case ZAPPER_DEPOSIT_ETH:
      context->next_param = NONE;
      break;
    case ZAPPER_DEPOSIT_SFRXETH:
      context->next_param = AMOUNT_SENT;
      break;
    case VAULT_MINT:
      context->next_param = TOKEN_SENT;
      break;
    case VAULT_REDEEM:
      context->next_param = AMOUNT_SENT;
      break;
    case CURVE_POOL_EXCHANGE:
    case CURVE_POOL_EXCHANGE_UNDERLYING:
      if (memcmp(CURVE_OETH_POOL_ADDRESS,
                 msg->pluginSharedRO->txContent->destination,
                 ADDRESS_LENGTH) == 0 ||
          memcmp(CURVE_OUSD_POOL_ADDRESS,
                 msg->pluginSharedRO->txContent->destination,
                 ADDRESS_LENGTH) == 0) {
        context->next_param = TOKEN_SENT;
        break;
      }
      PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
      msg->result = ETH_PLUGIN_RESULT_ERROR;
      return;
    case CURVE_ROUTER_EXCHANGE_MULTIPLE:
      context->next_param = TOKEN_SENT;
      break;
    case UNISWAP_ROUTER_EXACT_INPUT:
      context->skip += 2;
      context->next_param = BENEFICIARY;
      break;
    case UNISWAP_ROUTER_EXACT_INPUT_SINGLE:
      context->next_param = TOKEN_SENT;
      break;
    case FLIPPER_BUY_OUSD_WITH_USDT:
    case FLIPPER_SELL_OUSD_FOR_USDT:
    case FLIPPER_BUY_OUSD_WITH_DAI:
    case FLIPPER_SELL_OUSD_FOR_DAI:
    case FLIPPER_BUY_OUSD_WITH_USDC:
    case FLIPPER_SELL_OUSD_FOR_USDC:
    case WRAP:
    case UNWRAP:
      context->next_param = AMOUNT_SENT;
      break;
    // Keep this
    default:
      PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
      msg->result = ETH_PLUGIN_RESULT_ERROR;
      return;
  }

  // Return valid status.
  msg->result = ETH_PLUGIN_RESULT_OK;
}
