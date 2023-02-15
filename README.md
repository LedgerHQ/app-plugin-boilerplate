# Ledger pSTAKE Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a transaction on pSTAKE's stkBNB platform.

## Smart Contracts

Smart contracts covered by this plugin are:

| Network | Smart Contract Name | Address                                      | Selectors                                                                                                     |
|---------|---------------------|----------------------------------------------|---------------------------------------------------------------------------------------------------------------|
| BSC     | Stake Pool          | `0xc228cefdf841defdbd5b3a18dfd414cc0dbfa0d8` | **deposit (0xd0e30db0)**: Shows deposited BNB amount,<br/> **claimAll (0xd1058e59)**: Shows a static info msg |
| BSC     | stkBNB Token        | `0xc2e9d07f66a89c44062459a47a0d2dc038e4fb16` | **send (0x9bd9bbc6)**: Shows stkBNB amount on unstake                                                         |
