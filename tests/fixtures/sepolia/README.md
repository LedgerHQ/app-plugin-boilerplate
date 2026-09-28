# Real Sepolia transactions

Collected by `unit-tests/tools/fetch_sepolia_txs.py` with read-only GET requests to `https://eth-sepolia.blockscout.com/api/v2/addresses/<addr>/transactions?filter=to`. Each fixture holds `{hash, from, to, value, input, chainId, nonce, type}` plus the reference-model decision (`model`, signer = `from`, no token lookups). Re-evaluate offline with `fetch_sepolia_txs.py --rerun`.

- BP `0x800B36cc16C7572E1d600506a0d7d30890fF35f4`: 1 transactions with `to` = contract
- AR `0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8`: 20 transactions with `to` = contract
- PR `0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2`: 5 transactions with `to` = contract
- SR `0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0`: 12 transactions with `to` = contract

| Selector | Function | Found | Saved |
|---|---|---|---|
| `029a2053` | permitProxyCall | 0 | 0 |
| `2f5a71c1` | approveProxyCallWithMeta | 0 | 0 |
| `366ffd79` | permitProxyCallWithMeta | 0 | 0 |
| `4674b941` | safeExecuteWithDiffsMeta | 11 | 3 |
| `510818c0` | safeExecuteWithPostBalances | 0 | 0 |
| `7ce6cc45` | safeExecuteWithDiffs | 0 | 0 |
| `890e3add` | proxyCallDiffs | 0 | 0 |
| `b3a7f90a` | approveProxyCallDiffsWithMeta | 19 | 3 |
| `b44921d7` | permitProxyCallDiffsWithMeta | 4 | 3 |
| `bf10e28e` | permitProxyCallDiffs | 0 | 0 |
| `c6707270` | approveProxyCallDiffs | 0 | 0 |
| `ce9506f9` | proxyCall | 0 | 0 |
| `e48215b2` | safeExecuteWithPostBalancesMeta | 0 | 0 |
| `e5d77b3c` | approveProxyCall | 0 | 0 |

| Fixture | Outcome | Reason | Detail |
|---|---|---|---|
| `4674b941_0c413e24.json` | OK | R_OK |  |
| `4674b941_4f6774a1.json` | OK | R_OK |  |
| `4674b941_d890a1a4.json` | OK | R_OK |  |
| `b3a7f90a_1c89af46.json` | OK | R_OK |  |
| `b3a7f90a_224a0d20.json` | OK | R_OK |  |
| `b3a7f90a_3de5984a.json` | OK | R_OK |  |
| `b44921d7_5a2e2279.json` | ERROR | E_PROXY | balanceProxy a1a77109eaafdef3b2eb36493a98868d4bd82fef is not canonical |
| `b44921d7_c0d83ca7.json` | ERROR | E_PROXY | balanceProxy a1a77109eaafdef3b2eb36493a98868d4bd82fef is not canonical |
| `b44921d7_daf81465.json` | ERROR | E_PROXY | balanceProxy a1a77109eaafdef3b2eb36493a98868d4bd82fef is not canonical |
