[![Ensure compliance with Ledger guidelines](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/guidelines_enforcer.yml)
[![Compilation & tests](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml/badge.svg?branch=develop)](https://github.com/LedgerHQ/app-plugin-boilerplate/actions/workflows/build_and_functional_tests.yml)

# Origin DeFi Ledger Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing an OUSD or OETH transaction on their respective dapps.

Plugins are lightweight applications that go hand-in-hand with the Ethereum
Application on a Nano S / X device.

They allow users to safely interact with smart contracts by parsing the
transaction data and displaying its content in a human readable way. This is
done on a "per contract" basis, meaning a plugin is required for every DApp.

## Prerequisite

Clone the plugin to a new folder.

```shell
git clone https://github.com/OriginProtocol/origin-app-plugin.git
```

Then in the same folder clone two more repositories, which is the plugin-tools and app-ethereum.

```shell
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum
sudo docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

## Build

The script will build a docker image and attach a console.
When the docker image is running go to the "app-plugin-origindefi" folder and build the ".elf" files.
```shell
cd app-plugin-origindefi/tests       # go to the tests folder in app-plugin-origindefi
./build_local_test_elfs.sh      # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder in a new console window from the "app-plugin-origindefi" and run the script "test"
```shell
cd app-plugin-origindefi/tests       # go to the tests folder in app-plugin-origindefi
yarn install
yarn test                       # run the script test
```

## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.asc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Contract | Address |
| ---       | --- |
| OETH Zapper  | `0x9858e47bcbbe6fbac040519b02d7cd4b2c470c66`|
| OETH Vault  | `0x39254033945aa2e4809cc2977e7087bee48bd7ab`|
| ETH/OETH Curve Pool  | `0x94b17476a93b3262d87b9a326965d1e91f9c13e7`|
| Curve Router   | `0x99a58482bd75cbab83b27ec03ca68ff489b5788f`|
| OUSD Vault   | `0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70`|
| OUSD Flipper   | `0xcecad69d7d4ed6d52efcfa028af8732f27e08f70`|
| UniswapV3 Router  | `0xe592427a0aece92de3edee1f18e0157c05861564`|
| OUSD/3CRV Curve Pool  | `0x87650d7bbfc3a9f10587d7778206671719d9910d`|
| ETH/OETH Curve Pool  | `0x94b17476a93b3262d87b9a326965d1e91f9c13e7`|
| Curve Router   | `0x99a58482bd75cbab83b27ec03ca68ff489b5788f`|
| OUSD Vault   | `0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70`|
| OUSD Flipper   | `0xcecad69d7d4ed6d52efcfa028af8732f27e08f70`|
| WOETH   | `0xdcee70654261af21c44c093c300ed3bb97b78192`|
| WOUSD   | `0xd2af830e8cbdfed6cc11bab697bb25496ed6fa62`|

## Functions implemented:


|    Function   | Selector  | Displayed Parameters |
| ---           | ---       | --- |
|deposit | 0xd0e30db0| <table>  <tbody> </tbody> </table> |
|depositSFRXETH | 0xd443e97d| <table>  <tbody>  <tr> <td><code>uint256 amount</code></td></tr> <tr><td><code>uint256 minOETH</code></td></tr> </tbody> </table> |
|mint  | 0x156e29f6| <table>  <tbody>  <tr><td><code>address _asset</code></td></tr> <tr><td><code>,uint256 _amount</code></td></tr> <tr><td><code>uint256 _minimumOusdAmount</code></td></tr> </tbody> </table>|
|redeem | 0x7cbc2373| <table>  <tbody>  <tr><td><code>uint256 _amount</code></td></tr> <tr><td><code>uint256 _minimumUnitAmount</code></td></tr> </tbody> </table>|
|exchange  | 0x3df02124| <table>  <tbody>  <tr><td><code>int128 i</code></td></tr> <tr><td><code>int128 j</code></td></tr> <tr><td><code>uint256 _dx</code></td></tr> <tr><td><code>uint256 _min_dy</code></td></tr> </tbody> </table>|
|exchange_underlying  | 0xa6417ed6| <table>  <tbody>  <tr><td><code>int128 i</code></td></tr> <tr><td><code>int128 j</code></td></tr> <tr><td><code>uint256 _dx</code></td></tr> <tr><td><code>uint256 _min_dy</code></td></tr> </tbody> </table>|
|exchange_multiple | 0x353ca424| <table>  <tbody> <tr><td><code>address[9] _route</code></td></tr> <tr><td><code>uint256 _amount</code></td></tr> <tr><td><code>uint256 _expected</code></td></tr> </tbody> </table>|
|exactInput  | 0xc04b8d59| <table>  <tbody>  <tr><td><code>bytes path</code></td></tr> <tr><td><code>address recipient</code></td></tr> <tr><td><code>uint256 amountIn</code></td></tr> <tr><td><code>uint256 amountOutMinimum</code></td></tr> </tbody> </table> |
|exactInputSingle  | 0x414bf389| <table>  <tbody>  <tr><td><code>address tokenIn</code></td></tr> <tr><td><code>address tokenOut</code></td></tr> <tr><td><code>address recipient</code></td></tr> <tr><td><code>uint256 amountIn</code></td></tr> <tr><td><code>uint256 amountOutMinimum</code></td></tr> </tbody> </table>|
|buyOusdWithUsdt  | 0x35aa0b96| <table>  <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>|
|sellOusdForUsdt  | 0xcb939053| <table>  <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>|
|buyOusdWithDai  | 0x5981c746| <table>  <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>|
|sellOusdForDai  | 0x8a095a0f| <table>  <tbody>  <tr><td><code>uint256 amount</code></td></tr> </tbody> </table> |
|buyOusdWithUsdc  | 0xbfc11ffd| <table>  <tbody>  <tr><td><code>uint256 amount</code></td></tr> </tbody> </table> |
|sellOusdForUsdc  | 0xc6b68169| <table>  <tbody>  <tr><td><code>uint256 amount</code></td></tr> </tbody> </table> |
|redeem  | 0xba087652| <table>  <tbody>  <tr><td><code>uint256 assets</code></td></tr> <tr><td><code>address receiver</code></td></tr> </tbody> </table> |
|redeem  | 0xba087652| <table>  <tbody>  <tr><td><code>uint256 shares</code></td></tr> <tr><td><code>address receiver</code></td></tr> </tbody> </table> |

## Ethereum SDK

Ethereum plugins need the [Ethereum SDK](https://github.com/LedgerHQ/ethereum-plugin-sdk).
You can use the `ETHEREUM_PLUGIN_SDK` variable to point to the directory where you cloned
this repository. By default, the `Makefile` expects it to be at the root directory of this
plugin repository, by the `ethereum-plugin-sdk` name.

This repository is deliberately **not** a submodule. You can see that the CI workflows
clone and checkout either the latest `master` or on `develop` references. This ensures
the code is compiled and tested on the latest version of the SDK.

## Formatting

The C source code is expected to be formatted with `clang-format` 11.0.0 or higher.
