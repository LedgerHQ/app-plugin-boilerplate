# Functional tests

[//]: # (Comment)
[//]: # (This file when in the plugin-boilerplate repository is included in the Ethereum Plugin SDK Github page, keep that in mind when editing it.)

The tests craft APDUs, send them to the application, and assert the RAPDUs results and the screen content.<br/>
You will need to adapt / enrich the test database with tests relevant to your plugin.<br/>
The presence of functional testing of your plugin and an associated CI is mandatory.

## Tools used

- The [Speculos](https://github.com/LedgerHQ/speculos) tool is used to emulate the device and its applications.
- The [Ragger](https://github.com/LedgerHQ/ragger) test framework (pytest based) is used to manage the Speculos instance and provide useful helper functions (APDU exchanges, screen navigation, etc).
- The [Ledger VSCode extension](https://github.com/LedgerHQ/ledger-vscode-extension) can optionnaly be used to compile the application and run the tests. This documentation will describe using the `VSCode extension` and `bash` command lines.
- The [Ledger Ethereum client](https://pypi.org/project/ledger-app-clients.ethereum/) ([Web3](https://pypi.org/project/web3/) based) can be used to craft the smart contract transactions.

This frameworks are developed by Ledger, if you have trouble using them, we invite you to get in touch with us on our [Discord](https://developers.ledger.com/contact/).<br/>
The usage of the Ragger framework for your tests is greatly recommended.

## Compiling the binaries

In order to run the tests you need to provide the binaries for both the plugin and the Ethereum application.

### Plugin binaries

Compiling the plugin will produce the binary for the targeted device. They will be used in-place by the test framework.

### Ethereum binaries

The binaries of the Ethereum application must be gathered and placed in the directory `tests/.test_dependencies/ethereum/` of your plugin.

Example of the correct file tree with Ethereum compiled for all targets:
```shell
$ tree -a -L 3 .test_dependencies/
.test_dependencies/
└── ethereum
    ├── .ethereum_application_build_goes_there
    └── build
        ├── flex
        ├── apex_p
        ├── nanos
        ├── nanos2
        ├── nanox
        └── stax
```

Several methods are available:

- Use the Ledger VSCode extension to automatically manage dependencies.
- Clone the [Ethereum application](https://github.com/LedgerHQ/app-ethereum) in the `.test_dependencies/` directory and compile it.
	- You will need to set the compilation flags manually to compile for tests.
	- Open the [Ethereum application manifest](https://github.com/LedgerHQ/app-ethereum/blob/develop/ledger_app.toml) and use the flags from `use_test_keys`
- Re-use the Ethereum build produced by a test CI, and available as artifact. Example here with the [plugin-boilerplate CI](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml).


## Launching the tests

The plugin boilerplate application comes with some template functional tests.


### macOS / Windows

To test your application on macOS or Windows, it is recommended to use [Ledger VSCode extension](https://github.com/LedgerHQ/ledger-vscode-extension) to quickly setup a working test environment.

You can use the following task (accessible in the **extension sidebar menu**) :

* Use `Run tests`.

### Linux (Ubuntu)

On Linux, you can use [Ledger VSCode extension](https://github.com/LedgerHQ/ledger-vscode-extension) to run the tests.

Alternatively, you can run the test by command lines in your terminal.

Install the tests requirements (only needed once):

```shell
pip install -r tests/requirements.txt
```

Run the tests. For example, run the following command to run all tests on all devices:

```shell
pytest -v tests/ --device all
```

Please refer to the Ragger repository for a documentation on the many parameters and features offered by the framework.


## Developing your tests

The test provided in `test_swap.py` is a simple test of a successful parsing, display, and signature of smart contract `swapExactETHForTokens`.

When forking the boilerplate to develop your own plugin, you will need to adapt the tests to use your own smart contract.


## Using the [Ledger Ethereum client](https://pypi.org/project/ledger-app-clients.ethereum/)

The [Ledger Ethereum client](https://pypi.org/project/ledger-app-clients.ethereum/) is a Python module that abstracts the APDU crafting and response parsing. It comes from the Ethereum app itself and is what's used to test it in its CI. Conveniently, it also has everything needed for testing plugins.

### CAL signature mock

This module supports the official Ethereum appplication test signing keys, so that APDUs that are signed by the [Ledger backend CAL](https://github.com/LedgerHQ/ledger-live/tree/develop/libs/ledgerjs/packages/cryptoassets) in production can be mocked during the tests.

The Ethereum application must be compiled with the test keys flags. Please refer to the Ethereum application compilation section to know more about this.

### ABIs

The Ethereum Ragger client relies on the [Web3](https://pypi.org/project/web3/) python module for transaction crafting and it can leverage a smart contract's ABI (Application Binary Interface) to craft contracts.<br/>
ABIs are obtainable from a website like [Etherscan](https://etherscan.io/) for example.

### Signature verification

The Ragger client can also easily verify a transaction's signature by recovering the Ethereum address of the wallet that signed the transaction.<br/>
Testing the UI is crucial but so is testing the signatures we produce.
