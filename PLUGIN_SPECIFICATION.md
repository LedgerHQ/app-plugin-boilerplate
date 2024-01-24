# Technical Specification

## About
This documentation describes the smart contracts and functions supported by the Origin plugin for Ledger Live.

## Smart Contracts

Smart contracts covered by the plugin shall be described here:

| Contract             | Address                                      |
| -------------------- | -------------------------------------------- |
| OETH Zapper          | `0x9858e47bcbbe6fbac040519b02d7cd4b2c470c66` |
| OETH Vault           | `0x39254033945aa2e4809cc2977e7087bee48bd7ab` |
| ETH/OETH Curve Pool  | `0x94b17476a93b3262d87b9a326965d1e91f9c13e7` |
| Curve Router         | `0x99a58482bd75cbab83b27ec03ca68ff489b5788f` |
| OUSD Vault           | `0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70` |
| OUSD Flipper         | `0xcecad69d7d4ed6d52efcfa028af8732f27e08f70` |
| UniswapV3 Router     | `0xe592427a0aece92de3edee1f18e0157c05861564` |
| OUSD/3CRV Curve Pool | `0x87650d7bbfc3a9f10587d7778206671719d9910d` |
| ETH/OETH Curve Pool  | `0x94b17476a93b3262d87b9a326965d1e91f9c13e7` |
| Curve Router         | `0x99a58482bd75cbab83b27ec03ca68ff489b5788f` |
| OUSD Vault           | `0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70` |
| OUSD Flipper         | `0xcecad69d7d4ed6d52efcfa028af8732f27e08f70` |
| WOETH                | `0xdcee70654261af21c44c093c300ed3bb97b78192` |
| WOUSD                | `0xd2af830e8cbdfed6cc11bab697bb25496ed6fa62` |

## Functions implemented:

| Function            | Selector   | Displayed Parameters                                                                                                                                                                                                                                                                      |
| ------------------- | ---------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| deposit             | 0xd0e30db0 | <table> <tbody> </tbody> </table>                                                                                                                                                                                                                                                         |
| depositSFRXETH      | 0xd443e97d | <table> <tbody> <tr> <td><code>uint256 amount</code></td></tr> <tr><td><code>uint256 minOETH</code></td></tr> </tbody> </table>                                                                                                                                                           |
| mint                | 0x156e29f6 | <table> <tbody> <tr><td><code>address \_asset</code></td></tr> <tr><td><code>,uint256 \_amount</code></td></tr> <tr><td><code>uint256 \_minimumOusdAmount</code></td></tr> </tbody> </table>                                                                                              |
| redeem              | 0x7cbc2373 | <table> <tbody> <tr><td><code>uint256 \_amount</code></td></tr> <tr><td><code>uint256 \_minimumUnitAmount</code></td></tr> </tbody> </table>                                                                                                                                              |
| exchange            | 0x3df02124 | <table> <tbody> <tr><td><code>int128 i</code></td></tr> <tr><td><code>int128 j</code></td></tr> <tr><td><code>uint256 \_dx</code></td></tr> <tr><td><code>uint256 \_min_dy</code></td></tr> </tbody> </table>                                                                             |
| exchange_underlying | 0xa6417ed6 | <table> <tbody> <tr><td><code>int128 i</code></td></tr> <tr><td><code>int128 j</code></td></tr> <tr><td><code>uint256 \_dx</code></td></tr> <tr><td><code>uint256 \_min_dy</code></td></tr> </tbody> </table>                                                                             |
| exchange_multiple   | 0x353ca424 | <table> <tbody> <tr><td><code>address[9] \_route</code></td></tr> <tr><td><code>uint256 \_amount</code></td></tr> <tr><td><code>uint256 \_expected</code></td></tr> </tbody> </table>                                                                                                     |
| exactInput          | 0xc04b8d59 | <table> <tbody> <tr><td><code>bytes path</code></td></tr> <tr><td><code>address recipient</code></td></tr> <tr><td><code>uint256 amountIn</code></td></tr> <tr><td><code>uint256 amountOutMinimum</code></td></tr> </tbody> </table>                                                      |
| exactInputSingle    | 0x414bf389 | <table> <tbody> <tr><td><code>address tokenIn</code></td></tr> <tr><td><code>address tokenOut</code></td></tr> <tr><td><code>address recipient</code></td></tr> <tr><td><code>uint256 amountIn</code></td></tr> <tr><td><code>uint256 amountOutMinimum</code></td></tr> </tbody> </table> |
| buyOusdWithUsdt     | 0x35aa0b96 | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| sellOusdForUsdt     | 0xcb939053 | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| buyOusdWithDai      | 0x5981c746 | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| sellOusdForDai      | 0x8a095a0f | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| buyOusdWithUsdc     | 0xbfc11ffd | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| sellOusdForUsdc     | 0xc6b68169 | <table> <tbody> <tr><td><code>uint256 amount</code></td></tr> </tbody> </table>                                                                                                                                                                                                           |
| redeem              | 0xba087652 | <table> <tbody> <tr><td><code>uint256 assets</code></td></tr> <tr><td><code>address receiver</code></td></tr> </tbody> </table>                                                                                                                                                           |
| redeem              | 0xba087652 | <table> <tbody> <tr><td><code>uint256 shares</code></td></tr> <tr><td><code>address receiver</code></td></tr> </tbody> </table>                                                                                                                                                           |
