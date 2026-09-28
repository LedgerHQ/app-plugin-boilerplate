# ERC8009 Plugin Specification

## About

This is the Ledger Ethereum plugin `ERC8009`: it clear-signs calls into the ERC-8009 "guarded
call" contracts, showing on the Stax screen **exactly the balance guarantees the contract will
enforce** on the signer's address (and, for the Safe flow, on the Safe).

## Smart contracts

Sepolia only (chainId `11155111` / `0xaa36a7`). The plugin binds to exactly these 4 addresses;
any other `to` is `ERROR`.

| Contract | Address |
|---|---|
| BalanceProxy | `0x800B36cc16C7572E1d600506a0d7d30890fF35f4` |
| ApproveRouter | `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8` |
| PermitRouter | `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2` |
| SafeRouter | `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0` |

## Functions

Generated from the deployed ABIs in `tests/abis/*.abi.json`; the same 14 functions and
selectors are exercised by `tests/erc8009_model.py`.

<!-- BEGIN GENERATED SELECTORS -->
| Contract | Address | Function | Selector | Mode |
|---|---|---|---|---|
| BalanceProxy | `0x800B36cc16C7572E1d600506a0d7d30890fF35f4` | `proxyCall((address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0xce9506f9` | min balance |
| BalanceProxy | `0x800B36cc16C7572E1d600506a0d7d30890fF35f4` | `proxyCallDiffs((address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0x890e3add` | diffs |
| ApproveRouter | `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8` | `approveProxyCall(address,(address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0xe5d77b3c` | min balance |
| ApproveRouter | `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8` | `approveProxyCallDiffs(address,(address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0xc6707270` | diffs |
| ApproveRouter | `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8` | `approveProxyCallWithMeta(address,(string,uint8)[],(address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0x2f5a71c1` | min balance |
| ApproveRouter | `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8` | `approveProxyCallDiffsWithMeta(address,(string,uint8)[],(address,address,int256)[],((address,address,int256),bool)[],address,bytes,(address,address,int256)[])` | `0xb3a7f90a` | diffs |
| PermitRouter | `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2` | `permitProxyCall(address,(address,address,int256)[],((address,address,int256),bool)[],(uint256,uint8,bytes32,bytes32)[],address,bytes,(address,address,int256)[])` | `0x029a2053` | min balance |
| PermitRouter | `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2` | `permitProxyCallDiffs(address,(address,address,int256)[],((address,address,int256),bool)[],(uint256,uint8,bytes32,bytes32)[],address,bytes,(address,address,int256)[])` | `0xbf10e28e` | diffs |
| PermitRouter | `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2` | `permitProxyCallWithMeta(address,(string,uint8)[],(address,address,int256)[],((address,address,int256),bool)[],(uint256,uint8,bytes32,bytes32)[],address,bytes,(address,address,int256)[])` | `0x366ffd79` | min balance |
| PermitRouter | `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2` | `permitProxyCallDiffsWithMeta(address,(string,uint8)[],(address,address,int256)[],((address,address,int256),bool)[],(uint256,uint8,bytes32,bytes32)[],address,bytes,(address,address,int256)[])` | `0xb44921d7` | diffs |
| SafeRouter | `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0` | `safeExecuteWithPostBalances(address,(address,address,int256)[],address,(address,uint256,bytes,uint8,uint256,uint256,uint256,address,address,bytes,uint256))` | `0x510818c0` | min balance |
| SafeRouter | `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0` | `safeExecuteWithDiffs(address,(address,address,int256)[],address,(address,uint256,bytes,uint8,uint256,uint256,uint256,address,address,bytes,uint256))` | `0x7ce6cc45` | diffs |
| SafeRouter | `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0` | `safeExecuteWithPostBalancesMeta(address,(string,uint8)[],(address,address,int256)[],address,(address,uint256,bytes,uint8,uint256,uint256,uint256,address,address,bytes,uint256))` | `0xe48215b2` | min balance |
| SafeRouter | `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0` | `safeExecuteWithDiffsMeta(address,(string,uint8)[],(address,address,int256)[],address,(address,uint256,bytes,uint8,uint256,uint256,uint256,address,address,bytes,uint256))` | `0x4674b941` | diffs |
<!-- END GENERATED SELECTORS -->

`executeSafeTransaction(address,(address,uint256,bytes,uint8,uint256,uint256,uint256,address,address,bytes,uint256))`
(`0xece10c06`) is a self-guarded callback on SafeRouter, not a user-facing entry point, and is
**not** supported: the plugin returns `UNAVAILABLE` for it, same as for any other selector.

## Screens

One `nbgl_useCaseAdvancedReview` (advanced review list), built by the plugin's
`Send`/`Safe`/guard/pull pairs, framed by app-ethereum's own `From` (before) and
`Network` / `Max fees` (after) pairs, then "Hold to sign":

```
From                              (app-ethereum)
[Send]                            payable families, only if tx value > 0
[Safe]                            SafeRouter only
for each guard i = 1..N (calldata order):
    Min change i/N | Min balance i/N
    [Token i/N]                   only if the guard's token != 0x0 (native)
    [Other account i/N]           only if target != owner account
for each displayed pull j = 1..M (ApproveRouter/PermitRouter only, never BalanceProxy):
    Pull j/M | Unguarded pull j
    Pull token j/M | Pull token j
Network                           (app-ethereum)
Max fees                          (app-ethereum)
Hold to sign
```

**Title.** `Review transaction to execute guarded call on ERC-8009` for BalanceProxy /
ApproveRouter / PermitRouter. For SafeRouter the plugin's `handle_query_contract_id` returns
the version string `execute guarded Safe tx`, but app-ethereum lower-cases the `version` part of
the title before display (name `ERC-8009` is left as-is) — so the title actually shown on the
Safe variants is `Review transaction to execute guarded safe tx on ERC-8009`.

**`Send`** — payable families (BalanceProxy, ApproveRouter, PermitRouter) only, and only when
the transaction's `value > 0`: the network ticker amount, e.g. `0.001 ETH` (`value > 0` on a
SafeRouter call is `ERROR`, since SafeRouter calls are not payable).

**`Safe`** — SafeRouter only: the EIP-55 checksummed address of `safe`.

**Per guard (`i = 1..N`, `N` ≤ 8, calldata order).** The counter suffix `" i/N"` is present
**only when `N > 1`** — with a single guard the pairs are titled plainly `Min change` /
`Min balance` / `Token` / `Other account`.
- `Min change i/N` (diffs mode) — signed two's-complement delta: `-` + magnitude when negative,
  `+` when positive, no sign at zero (e.g. `-0.001 ETH`, `+2.5 USDC`, `0 USDC`).
- `Min balance i/N` (min balance mode) — unsigned magnitude (the contract checks `abs(value)`,
  so the sign carries no information): e.g. `5 USDC`.
- `Token i/N` — shown only when the guard's token is an ERC-20 (`!= 0x0`): the EIP-55
  checksummed token address.
- `Other account i/N` — shown only when the guard's `target` differs from the owner account
  (the signer, or `safe` for SafeRouter calls): the EIP-55 checksummed target address.

**Pulls.** Shown only for ApproveRouter / PermitRouter calls (never for BalanceProxy — its
approvals draw on the proxy's own funds, not the signer's, so they are validated but not
displayed). `M` = the number of *displayed* pulls; the counter suffix is dropped when
`M == 1`.
- diffs mode: **every** approval is a displayed pull — `Pull j/M` (unsigned amount) +
  `Pull token j/M` (EIP-55 address), or `Pull` / `Pull token` when `M == 1`.
- min balance mode: only approvals **not covered** by a guard on the owner account for the same
  token are displayed — `Unguarded pull j` + `Pull token j`, or `Unguarded pull` / `Pull token`
  when `M == 1`.

**Amounts.** A token is *known* when it is the native ticker (`ETH`, 18 decimals), or the
selector carries `meta` for it, or an app-ethereum ticker lookup for it was accepted (at most 2
distinct unknown ERC-20 tokens are looked up, guard tokens first in guard order, then pull
tokens in pull order). Known amounts render as `<decimal> <TICKER>` with trailing zeros
trimmed (e.g. `0.001 ETH`, `0 USDC`); unknown tokens render as `<raw base units> ???`. `Send`
and pull amounts are always unsigned.

**Example** (swap 0.001 ETH → USDC via `approveProxyCallDiffsWithMeta`, one guard on USDC, no
approvals):
```
Review transaction to execute guarded call on ERC-8009
From              0x<your address>
Send              0.001 ETH
Min change        +2.5 USDC
Token             0x1c7D4B196Cb0C7B01d743Fbc6116a902379C7238
Network           Sepolia
Max fees          …
Hold to sign
```

## Outcomes

Four outcomes, in order of severity. The full list of reason codes (`erc8009_reason_t`) is
defined in `src/plugin.h`; this is only a summary of what the user sees.

| Outcome | What the device does | When |
|---|---|---|
| `OK` | The screens above; hold to sign. | Canonical ABI, known network/contract/family, ≤ 8 guards, ≤ 2 approvals, no other violation. |
| `FALLBACK` | Blind signing **OFF**: refuses immediately with "This transaction cannot be clear-signed" (`0x6A80`), no review. Blind signing **ON**: a blind-signing review instead of the plugin's screens. | 0 guards, or more than 8 guards / meta entries. |
| `ERROR` | Refuses immediately with `0x6A80` (verify on first run against the device; no clear-sign or blind-sign review happens). | Wrong chain; `to` outside the 4 addresses; selector family ≠ contract; non-canonical calldata (bad offsets, padding, truncation, trailing bytes); `balanceProxy` argument ≠ the canonical BalanceProxy; bad `meta` (length, symbol charset/length/padding, decimals, native-token mismatch); more than 2 approvals; a negative approval amount; `value > 0` on a SafeRouter call; and every other structural rule enforced by the parser (see `erc8009_reason_t` in `src/plugin.h`). |
| `UNAVAILABLE` | app-ethereum never invokes the plugin for this call; the Ethereum app falls back to its own default handling. | Plugin interface version mismatch, or the selector is not one of the 14 above (including `0xece10c06`). |
