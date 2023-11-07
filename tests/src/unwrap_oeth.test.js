import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0xdcee70654261af21c44c093c300ed3bb97b78192";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xbe8f9ea873c501f414f0817fd1aa41538c3487eb1ef27778bfcb96f37459eafd
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Unwrap OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xbe8f9ea873c501f414f0817fd1aa41538c3487eb1ef27778bfcb96f37459eafd
  const serializedTx = txFromEtherscan("0x02f8d3018202e88405f5e100851657a2004c8301396994dcee70654261af21c44c093c300ed3bb97b7819280b864ba087652000000000000000000000000000000000000000000000000038da9fb8d95b7ab0000000000000000000000000546af2f351e70a9e2a43d81f8098b6a572eef0c0000000000000000000000000546af2f351e70a9e2a43d81f8098b6a572eef0cc080a009e47d737ea18d10ebdd505fe4362cdb699cb84f2e31e5d4b146858b39354893a00f3182736359d5b6d8262dccad5ae18682dd3fb54e75aede1ce097204cb4cc68");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 11 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_unwrap_beneficiary_oeth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xbe8f9ea873c501f414f0817fd1aa41538c3487eb1ef27778bfcb96f37459eafd
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Unwrap OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xbe8f9ea873c501f414f0817fd1aa41538c3487eb1ef27778bfcb96f37459eafd
  const serializedTx = txFromEtherscan("0x02f8d3018202e88405f5e100851657a2004c8301396994dcee70654261af21c44c093c300ed3bb97b7819280b864ba087652000000000000000000000000000000000000000000000000038da9fb8d95b7ab000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70c0000000000000000000000000546af2f351e70a9e2a43d81f8098b6a572eef0cc080a009e47d737ea18d10ebdd505fe4362cdb699cb84f2e31e5d4b146858b39354893a00f3182736359d5b6d8262dccad5ae18682dd3fb54e75aede1ce097204cb4cc68");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 8 : 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_unwrap_oeth', [right_clicks, 0]);

  await tx;
  }));
});
