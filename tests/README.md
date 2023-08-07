# Tests

## Binaries

The tests run using both the plugin elf and the ethereum elf, both compiled for
Nano (S, S plus, X) and Stax devices, so 8 binaries are needed. The plugin binaries are expected to be stored in the
`tests/lib_binaries/` directory, and named as follow:

* `tests/lib_binaries/pluginBoilerplate_nanos.elf`
* `tests/lib_binaries/pluginBoilerplate_nanos2.elf`
* `tests/lib_binaries/pluginBoilerplate_nanox.elf`
* `tests/lib_binaries/pluginBoilerplate_stax.elf`

The ethereum binaries are expected to be stored in the
`build/` directory, and named as follow:

* `build/nanos/bin/app.elf`
* `build/nanos2/bin/app.elf`
* `build/nanox/bin/app.elf`
* `build/stax/bin/app.elf`

This [CI](https://github.com/LedgerHQ/app-plugin-boilerplate/blob/develop/.github/workflows/build_and_functional_tests.yml) can be used to generate the binaries artifacts.


## Launching the tests

The plugin boilerplate app comes with functional tests implemented with Ledger's [Ragger](https://github.com/LedgerHQ/ragger) test framework.

### macOS / Windows

To test your app on macOS or Windows, it is recommended to use [Ledger's VS Code extension](#with-vscode) to quickly setup a working test environment.

You can use the following task (all accessible in the **extension sidebar menu**) :

* Use `Run tests`.

### Linux (Ubuntu)

On Linux, you can use [Ledger's VS Code extension](#with-vscode) to run the tests. If you prefer not to, open a terminal and follow the steps below.

Install the tests requirements :

```shell
pip install -r tests/requirements.txt 
```

Then you can :

Run the functional tests (here for nanos but available for any device once you have built the binaries) :

```shell
pytest tests/ --tb=short -v --device nanos
```
