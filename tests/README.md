## Requirements for running the tests

### Binaries

The tests run using both the plugin elf and the ethereum elf, both compiled for
NanoS and X, so 4 binaries are needed. They are expected to be stored in the
`tests/elfs/` directory, and named as follow:

* `tests/elfs/plugin_nanos.elf`
* `tests/elfs/plugin_nanox.elf`
* `tests/elfs/ethereum_nanos.elf`
* `tests/elfs/ethereum_nanox.elf`

The `tests/build_local_test_elfs.sh` can be used to generate these binaries.

To use this script, you will need the
[Ethereum application](https://github.com/LedgerHQ/app-ethereum) to be cloned
somewhere, and the path of its repository exported in the `APP_ETHEREUM`
environment variable, or updated in the script, or as an argument of the script.

The script using relative paths, it must be launched from the `tests` directory:

```bash
cd tests
./build_local_test_elfs.sh
```

### Other dependencies

The tests run using the [Zemu framework](https://github.com/Zondax/zemu),
so the `yarn` manager is needed.

The JavaScript dependencies are then installed with:

```bash
cd tests
yarn install
```

## Launching the tests

```bash
cd tests
yarn test
```