# app-plugin-boilerplate

This repo is meant to be a forkable example of a plugin.

Plugins are lightweight applications that go hand-in-hand with the Ethereum
Application on a Nano S / X device.

They allow users to safely interact with smart contracts by parsing the
transaction data and displaying its content in a human-readable way. This is
done on a "per contract" basis, meaning a plugin is required for every DApp.

The code has been commented, and special "EDIT THIS" comments indicate where
developers are expected to adapt the code to their own needs.

It is STRONGLY recommended to follow the
[plugin guide](https://developers.ledger.com/docs/dapp/requirements/)
in order to better understand the flow and the context for plugins.

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk).
You can use the `ETHEREUM_PLUGIN_SDK` variable to point to the directory where you cloned
this repository. By default, the `Makefile` expects it to be at the root directory of this
plugin repository by the `ethereum-plugin-sdk` name.

This repository is deliberately **not** a submodule. You can see that the CI workflows
clone and checkout either the latest `master` or `develop` references. This ensures
the code is compiled and tested on the latest version of the SDK.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
