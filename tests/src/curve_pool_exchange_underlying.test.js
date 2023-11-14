import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x87650d7bbfc3a9f10587d7778206671719d9910d";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xb01c59662cb3f94a7d7eecf64669360ac6b7878063d8a2d94ccb1f0f2bdff143
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for DAI', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xb01c59662cb3f94a7d7eecf64669360ac6b7878063d8a2d94ccb1f0f2bdff143
  const serializedTx = txFromEtherscan("0x02f8f301820267843b9aca008519ec7019e48306b48d9487650d7bbfc3a9f10587d7778206671719d9910d80b884a6417ed60000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000d9b17e813d9e085ffad600000000000000000000000000000000000000000000d90df249c515b3118395c080a0312c3e1c5f652a97adb857eece5ea1474661e4252fc23a13fbde177ffccfc0cfa026552c245fbfada31ebf86648ef4455b9f49041d0ec466146cf28f2eaca02d72");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 11 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_pool_exchange_ousd_for_dai', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x7596c9b60e74bc0d877b363cd8b4a1e33651abc3b1c85a95eb46ae6ea1f002e7
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDC', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x7596c9b60e74bc0d877b363cd8b4a1e33651abc3b1c85a95eb46ae6ea1f002e7
  const serializedTx = txFromEtherscan("0x02f8f101098459682f0085051daca6e9830645bd9487650d7bbfc3a9f10587d7778206671719d9910d80b884a6417ed60000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000039ffd39a76f011e4dc200000000000000000000000000000000000000000000000000000003f84eb690c001a093d7bd2825766f775ca9f97e597904fc6bcc007eac206b3329bc688486f0099fa01d8f2718d2c2fdc7f208795e06d0d6a6862aa7c24d2a2afcac9929ee55159e47");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_pool_exchange_ousd_for_usdc', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x97a4abae75f2ec4450dfd0a5fdfb2b88605b92c61e12a7124f663a7a0f48b80d
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDT', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x97a4abae75f2ec4450dfd0a5fdfb2b88605b92c61e12a7124f663a7a0f48b80d
  const serializedTx = txFromEtherscan("0x02f8f1011f8405f5e1008507f6e455a3830653f49487650d7bbfc3a9f10587d7778206671719d9910d80b884a6417ed6000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000003f870857a3e0e380000000000000000000000000000000000000000000000000000000000045a0339b0c080a0465b0d87bdf116f2604083add9e06e2d3816c282f1af7abc8b6b30ef8caa7ab5a041a33d436ab8368136e89fd8e79b70f7842bd0467fccb9373d528c40de7a8cf5");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_pool_exchange_ousd_for_usdt', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xff98b29afe1fbf6a1134d903f82bfb8888dc9428ea81b27a7b8f382d28dcbb8f
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange DAI for OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xff98b29afe1fbf6a1134d903f82bfb8888dc9428ea81b27a7b8f382d28dcbb8f
  const serializedTx = txFromEtherscan("0xf8ec8201d78504c13cddf783065b7c9487650d7bbfc3a9f10587d7778206671719d9910d80b884a6417ed60000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000370ea0d47cf61a800000000000000000000000000000000000000000000000003704cf2575a113ff000025a04fb9946c7b7a8515cea0110fcb8df7d1ba1cf4518dc9451661a836fa4db466f4a0600335c825cc727fa9988df68165358d579026f96ae76870acb42497a90518d3");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_pool_exchange_dai_for_ousd', [right_clicks, 0]);

  await tx;
  }));
});
