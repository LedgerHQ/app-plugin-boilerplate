[![Ensure compliance with Ledger guidelines](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml)


# app-plugin-origin

Ledger's recommended [plugin guide](https://developers.ledger.com/docs/dapp/embedded-plugin/code-overview/) is out-dated and doesn't work since they introduced a lot of new changes. Here's a simple way to get started with this repo:
1. Clone this repo (along with git submodules)
2. Install [Xquartz](https://www.xquartz.org/) and make sure you have enabled "Allow connections from network clients" enabled under "Security" settings.
3. Install and start Docker (Note: If Docker is already running, quit and start it after starting Xquartz, otherwise docker cannot connect to Xquartz).
4. Install the [Ledger Dev Tools VS Code plugin](https://marketplace.visualstudio.com/items?itemName=LedgerHQ.ledger-dev-tools#:~:text=ledger%2Dvscode%2Dextension,Plus%2C%20Nano%20X%2C%20Stax) and makes sure it's enabled
5. Once you have installed the plugin and open the repo, the plugin should by default try to create and start the containers. If it doesn't, you can simply click "Update Container" under "Ledger Dev Tools" in the Activity Side Bar on VS Code.
6. On the "Ledger Dev Tools" side bar, Select a target and then click on Build. 
7. Once build is complete, click on "Run tests" to run the tests

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk).
You can use the `ETHEREUM_PLUGIN_SDK` variable to point to the directory where you cloned
this repository. By default, the `Makefile` expects it to be at the root directory of this
plugin repository by the `ethereum-plugin-sdk` name.

You can see that this [CI](https://github.com/LedgerHQ/app-plugin-boilerplate/blob/develop/.github/workflows/check_sdk.yml) workflow
verifies that the SDK used is either on the latest `master` or `develop` references. This ensures
the code is compiled and tested on the latest version of the SDK.

## Documentation

The documentation about the plugin is in [PLUGIN_SPECIFICATON.md](https://github.com/LedgerHQ/app-plugin-boilerplate/blob/develop/PLUGIN_SPECIFICATION.md). It includes the smart contracts and functions supported by the plugin.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.

```
clang-format -i src/*
```