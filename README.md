# app-plugin-boilerplate

This repo is a meant to be a forkable example of a plugin.

Plugins are lightweight applications that go hand-in-hand with the Ethereum Application on a Nano S / X device.

They allow users to safely interact with smart contracts by parsing the transaction data and displaying its content in a human readable way. This is done on a "per contract" basis, meaning a plugin is required for every DApp.

The code has been commented, and special "EDIT THIS" comments indicate where developers are expected to adapt the code to their own needs.

It is STRONGLY recommended to follow the [plugin guide](https://developers.ledger.com/docs/dapp/nano-plugin/overview/) in order to better understand the flow and the context for plugins.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
