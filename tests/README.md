# Tests

This directory contains non-regression tests for the Boilerplate plugin.
The tests craft APDUs, send them to the application, and assert the RAPDUs results and the screen content.
You will need to adapt / enrich the test database with tests relevant to your plugin.
The presence of functional testing of your plugin and an associated CI is mandatory.

The [Speculos](https://github.com/LedgerHQ/speculos) tool is used to emulate the device and its applications.
The [Ragger](https://github.com/LedgerHQ/ragger) test framework (pytest based) is used to manage the Speculos instance and provide useful helper functions (APDU exchanges, screen navigation, etc).
Both framework are developed by Ledger, if you have trouble using them, we invite you to get in touch with us on our [Discord](https://developers.ledger.com/contact/).
For this reason, the usage of the Ragger framework for your tests is greatly recommended.


## Binaries

In order to run the tests you need to provide the binaries for both the plugin and the Ethereum application.

Compiling the plugin will produce the binary for the targeted device. They will be used as-is by the test framework.

The binaries of the Ethereum application must be gathered and placed in the directory `tests/.test_dependencies/ethereum/`.
Example of the correct file tree with Ethereum compiled for all targets.
* `tests/.test_dependencies/ethereum/build/nanos/bin/app.elf`
* `tests/.test_dependencies/ethereum/build/nanos2/bin/app.elf`
* `tests/.test_dependencies/ethereum/build/nanox/bin/app.elf`
* `tests/.test_dependencies/ethereum/build/stax/bin/app.elf`

The first method is to use the Ledger VSCode extension to automatically manage dependencies
The second method is to go in the Ethereum project, compile the application, and dispatch the `build/` output directory.
A third method is to re-use the Ethereum build used in the CI, and available as artifact. 
For example in the [plugin-boilerplate CI](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml).


## Launching the tests

The plugin boilerplate app comes with functional tests 


### macOS / Windows

To test your app on macOS or Windows, it is recommended to use [Ledger's VS Code extension](#with-vscode) to quickly setup a working test environment.

You can use the following task (accessible in the **extension sidebar menu**) :

* Use `Run tests`.

### Linux (Ubuntu)

On Linux, you can use [Ledger's VS Code extension](#with-vscode) to run the tests.

Alternatively, you can run the test by command lines in your terminal.

First, run this command once to install the tests requirements:

```shell
pip install -r tests/requirements.txt
```

Then you can run the tests. For example, run the following command to run all tests on all devices:

```shell
pytest tests/
```

Please refer to the Ragger repository for a documentation on the many parameters and features offered by the framework.
