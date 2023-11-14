import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xfbb0557cc85b244a68a1bf030379c43117723375326e44c43db44af54a31ece0
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Redeem OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfbb0557cc85b244a68a1bf030379c43117723375326e44c43db44af54a31ece0
  const serializedTx = txFromEtherscan("0xf8aa0485055ae826008328155794e75d77b1865ae93c7eaa3040b038d7aa7bc02f7080b8447cbc2373000000000000000000000000000000000000000000003ea6080a7f4ba08e59ff000000000000000000000000000000000000000000003e2a03f3da153c51000025a0e98bbec6aa4ed8480923d5c5b74dcadf09d0bcd6b1c794500da9e5f12b32def3a069f50d96250a062339968570da2233676acc9886286a1f2ead8d5909efbe284b");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_redeem_ousd', [right_clicks, 0]);

  await tx;
  }));
});
