![GitHub repo size](https://img.shields.io/github/repo-size/liskhq/lisk-app-plugin)
![GitHub issues](https://img.shields.io/github/issues-raw/liskhq/lisk-app-plugin)
![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/liskhq/lisk-app-plugin)
[![Ensure compliance with Ledger guidelines](https://github.com/liskhq/lisk-app-plugin/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/liskhq/lisk-app-plugin/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/liskhq/lisk-app-plugin/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/liskhq/lisk-app-plugin/actions/workflows/build_and_functional_tests.yml)


# Lisk App Plugin

Lisk App Plugin is a plugin for the Ethereum application specific to support lisk smart contracts. It helps parsing and displaying relevant information when signing transaction.

Plugins are lightweight applications that go hand-in-hand with the Ethereum
Application on a Nano (S, S plus, X) and Stax devices.

They allow users to safely interact with smart contracts by parsing the
transaction data and displaying its content in a human-readable way. This is
done on a "per contract" basis, meaning a plugin is required for every DApp.

The code has been commented, and special "EDIT THIS" comments indicate where
developers are expected to adapt the code to their own needs.

It is STRONGLY recommended to follow the
[plugin guide](https://developers.ledger.com/docs/dapp/embedded-plugin/code-overview/)
in order to better understand the flow and the context for plugins.

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk).
You can use the `ETHEREUM_PLUGIN_SDK` variable to point to the directory where you cloned
this repository. By default, the `Makefile` expects it to be at the root directory of this
plugin repository by the `ethereum-plugin-sdk` name.

You can see that this [CI](https://github.com/LedgerHQ/app-plugin-boilerplate/blob/develop/.github/workflows/check_sdk.yml) workflow
verifies that the SDK used is either on the latest `master` or `develop` references. This ensures
the code is compiled and tested on the latest version of the SDK.

## Prerequisite
To start with, follow the steps below:
- Clone the plugin to a new folder.

    ```shell
    git clone --recurse-submodules https://github.com/LiskHQ/lisk-app-plugin.git
    ```

    Then in the same folder clone the app-ethereum.

    ```shell
    git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git
    ```
- Install [Xquartz](https://www.xquartz.org/) and make sure you have enabled "Allow connections from network clients" enabled under "Security" settings.
- Install and start Docker.
- Install and enable [Ledger Dev Tools VS Code Extension](https://marketplace.visualstudio.com/items?itemName=LedgerHQ.ledger-dev-tools).
- To build, select a target and click on Build on the "Ledger Dev Tools" VS code extension.
- Once build is complete, click on "Run tests" to run the tests

## Documentation

The documentation about the plugin can be found in [PLUGIN_SPECIFICATON.md](PLUGIN_SPECIFICATION.md). It includes the smart contracts and functions supported by the plugin.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
