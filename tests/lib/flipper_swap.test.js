"use strict";

require("core-js/stable");
require("regenerator-runtime/runtime");
var _test = require("./test.fixture");
var _ethers = require("ethers");
var _utils = require("ethers/lib/utils");
// EDIT THIS: Replace with your contract address
const contractAddr = "0xcecad69d7d4ed6d52efcfa028af8732f27e08f70";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x1b0b3562c495b29a451b3e42d20030d30f61ce4ef56348d88f2c3b19667a1282
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Buy OUSD with USDT', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x1b0b3562c495b29a451b3e42d20030d30f61ce4ef56348d88f2c3b19667a1282
    const serializedTx = (0, _test.txFromEtherscan)("0xf889258511055921f68301496b94cecad69d7d4ed6d52efcfa028af8732f27e08f7080a435aa0b960000000000000000000000000000000000000000000000056bc75e2d6310000025a09d4c3d452778c008292e774ad9f7f8a091d951a5756a2f6f62134748a953971ca0547d2ec5ddad51cc8bdbade3abb3572b79bd83cedb7c8ef82663800fb0f2ca87");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 7 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_buy_ousd_with_usdt', [right_clicks, 0]);
    await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x27d352b8190ffe63f2bbff5ba6f3dfc89bad4c54465821ddf486581eb3278781
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Sell OUSD for USDT', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x27d352b8190ffe63f2bbff5ba6f3dfc89bad4c54465821ddf486581eb3278781
    const serializedTx = (0, _test.txFromEtherscan)("0x02f89001208405f5e100850da2cbd3a883019ad994cecad69d7d4ed6d52efcfa028af8732f27e08f7080a4cb93905300000000000000000000000000000000000000000000000daee85907715539e9c080a0774a0c9977a4318105f6e79d983e2a0315b8fae907b58cdd3229c95341ed5d8fa015a5cbe4538fe38c54af3ff16bb914db406f2b3c3fce8f7cf58e43063b335ac0");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 11 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_sell_ousd_for_usdt', [right_clicks, 0]);
    await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x45e0f295640eba21daefa11a0a9ae69bd0c2ea3c853906a2d80838ea0242bca5
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Buy OUSD with DAI', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x45e0f295640eba21daefa11a0a9ae69bd0c2ea3c853906a2d80838ea0242bca5
    const serializedTx = (0, _test.txFromEtherscan)("0x02f89001358412b01087850cd827cd1f83016b9994cecad69d7d4ed6d52efcfa028af8732f27e08f7080a45981c7460000000000000000000000000000000000000000000000001782ec079e566c4bc080a0335e2a61e69dc4a46fd08c75a221b094782463827f2a63a104de9b4cc77920eaa008f4e9833801a6d5eb9604afe035c57c3df3af27c1d0dae66e00e0f746d908f5");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 11 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_buy_ousd_with_dai', [right_clicks, 0]);
    await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x4df6a5388136aa495cebb541eac214b1c9cf49b52560cd728561394ead47e369
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Sell OUSD for DAI', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x4df6a5388136aa495cebb541eac214b1c9cf49b52560cd728561394ead47e369
    const serializedTx = (0, _test.txFromEtherscan)("0x02f89001378408fcf59f850b0679b26f830176fc94cecad69d7d4ed6d52efcfa028af8732f27e08f7080a48a095a0f00000000000000000000000000000000000000000000000009a2347c0ef4f000c001a054609f2ac554219ded75cedad0fb175028f04d783b67482f91cc96a1de520250a031187dcadcc3f303fd0e3a3cdf0ee3278a211451a5695cf6183a262e3318736a");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 7 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_sell_ousd_for_dai', [right_clicks, 0]);
    await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xfca99ebcb1efd8cd8c2cbf89f6eca2771c8d611a47ab81ea7b7f8839b0660225
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Buy OUSD with USDC', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfca99ebcb1efd8cd8c2cbf89f6eca2771c8d611a47ab81ea7b7f8839b0660225
    const serializedTx = (0, _test.txFromEtherscan)("0x02f8900150843b9aca00851010b872008301afb894cecad69d7d4ed6d52efcfa028af8732f27e08f7080a4bfc11ffd0000000000000000000000000000000000000000000000056bc75e2d63100000c001a05be74a21aeef60f01b923d2dc694b7a3aae42dbfb009e822a4e3c702c2928af4a06e56e3b527cfbe3bb8437aa6d6901aba48e5ac1acf0c524cf80fa820c3f87717");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 5 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_buy_ousd_with_usdc', [right_clicks, 0]);
    await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x16aa058002f6001e2c8229f739dd85455a56411edbcc52ab1775fbbea377fafd
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Sell OUSD for USDC', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x16aa058002f6001e2c8229f739dd85455a56411edbcc52ab1775fbbea377fafd
    const serializedTx = (0, _test.txFromEtherscan)("0x02f8910181f48405f5e100850eed2fed8f8301accc94cecad69d7d4ed6d52efcfa028af8732f27e08f7080a4c6b68169000000000000000000000000000000000000000000000002c3c465ca58ec0000c001a07a97565e093a20ad83b55e3cf52ffa17440c6c5a9660ad0bb9fd8cfb0401dacfa06768cb69e769a6b1cd8563f12be734c3c2aa097c1a6d8bebb058ff5399a0a711");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 7 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_flipper_sell_ousd_for_usdc', [right_clicks, 0]);
    await tx;
  }));
});