[![Ensure compliance with Ledger guidelines](https://github.com/EIP-8009-Proxy-Clear-Signing/app-plugin-erc8009/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/EIP-8009-Proxy-Clear-Signing/app-plugin-erc8009/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/EIP-8009-Proxy-Clear-Signing/app-plugin-erc8009/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/EIP-8009-Proxy-Clear-Signing/app-plugin-erc8009/actions/workflows/build_and_functional_tests.yml)

# app-plugin-erc8009

Ledger Ethereum plugin `ERC8009`. It clear-signs calls into the ERC-8009 "guarded call"
contracts: it decodes the transaction's calldata and shows, on the Stax screen,
**exactly the balance guarantees the contract will enforce** on the signer's address (and, for
the Safe flow, on the Safe) — no more, no less. See [`PLUGIN_SPECIFICATION.md`](PLUGIN_SPECIFICATION.md)
for the full contract/function/screen reference.

**Network:** Sepolia only (chainId `11155111`). **Contracts:** BalanceProxy, ApproveRouter,
PermitRouter, SafeRouter — see `PLUGIN_SPECIFICATION.md` for addresses and the 14 supported
functions.

## `Send` vs `Min change`

In `diffs` mode, a guard's `Min change i/N` is the router's `after − before` delta for the
owner account's balance of that token. The router only takes the `before[]` snapshot *after*
it has already spent the transaction's `value` (native currency) and pulled any ERC-20
approvals (`transferFrom`) into the proxy, so `Min change` never includes the value sent or the
tokens pulled via approvals: those show up as their own pairs instead, `Send` (the native
`value`) and `Pull j/M` / `Unguarded pull j` (the approvals). The guard amount plus `Send` plus
the pulls together are the full picture, not the guard amount alone. This is a property of the
contracts, not a plugin limitation.

## Device settings

Before reviewing an ERC-8009 transaction on Stax, make sure these Ethereum app settings (exact
on-device labels) are set as below — otherwise the plugin's screens won't appear as documented,
or extra screens will be inserted that don't match the golden snapshots:

| Setting | Required state | Why |
|---|---|---|
| **Blind signing** | OFF | The plugin clear-signs every supported call; blind signing is only relevant to the `FALLBACK` case (see `PLUGIN_SPECIFICATION.md` → Outcomes) and should stay off during normal use. |
| **Raw messages** | OFF | Only needed for EIP-712 flows outside this plugin (front-end permit/SafeTx signing); leave off for plugin review. |
| **Debug smart contracts** | OFF | If on, app-ethereum skips plugin dispatch entirely and the transaction is never clear-signed by this plugin. |
| **Transaction Check** | OFF | Adds an extra warning screen ahead of the plugin's review, which will not match `tests/snapshots/`. |

## Build

Build inside the `ledger-dev` container (`ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools`),
same as any Ledger app/plugin:

```sh
docker exec -w /app/app-plugin-erc8009 ledger-dev bash -c 'BOLOS_SDK=$STAX_SDK make DEBUG=1 -j'
```

Swap `$STAX_SDK` for `$NANOX_SDK` / `$NANOSP_SDK` / `$FLEX_SDK` / `$APEX_P_SDK` to target another
device, and drop `DEBUG=1` for a release build.

The Ethereum plugin interface is provided by [`ethereum-plugin-sdk`](https://github.com/LedgerHQ/ethereum-plugin-sdk),
vendored as a submodule at `ethereum-plugin-sdk/`.

Source is formatted with `clang-format` (config in `.clang-format`).

## Tests

- **`tests/`** — `pytest` + [ragger](https://github.com/LedgerHQ/ragger) functional tests against
  Speculos, run inside the `ledger-dev` container with an app-ethereum `use_test_keys` build
  placed in `tests/.test_dependencies/`:

  ```sh
  pytest tests/ --device stax
  ```

  Golden snapshots live in `tests/snapshots/`. See [`tests/README.md`](tests/README.md) for the
  full test layout, fixtures, and the calldata crafter / reference model.
- **`fuzzing/`** — a libFuzzer harness (ASan/UBSan) over the calldata parser, seeded from
  `fuzzing/seeds/`. See [`fuzzing/README.md`](fuzzing/README.md).

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk). The
`Makefile` expects it at `ethereum-plugin-sdk/` (a git submodule of this repository).

It is recommended to read the [plugin guide](https://ethereum-plugin-sdk.ledger.com/) to
understand the general flow and context for Ethereum plugins.
