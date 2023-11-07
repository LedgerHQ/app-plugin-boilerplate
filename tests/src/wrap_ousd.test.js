import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0xd2af830e8cbdfed6cc11bab697bb25496ed6fa62";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x73729a1a6fd5ecaadbd3a8df9ffd24e00a0dd8b35874f2c88f05a72dda8bff81
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Wrap OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x73729a1a6fd5ecaadbd3a8df9ffd24e00a0dd8b35874f2c88f05a72dda8bff81
  const serializedTx = txFromEtherscan("0x02f8b1010c8405f5e10085098356220b83018f6e94d2af830e8cbdfed6cc11bab697bb25496ed6fa6280b8446e553f6500000000000000000000000000000000000000000000001b2d3e4baa5320bef90000000000000000000000008485b04aa0ee38f7c8c4a1dc80ed4259066a6d67c001a0e778365e30572f81f0c86a94f7a503cbc81c0097611415c18962c1ecc3510fa7a05c812baa8b16c9f20c8e6525be400e290aa3f91e0b352f350b3ba3193b368071");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 11 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_wrap_beneficiary_ousd', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x73729a1a6fd5ecaadbd3a8df9ffd24e00a0dd8b35874f2c88f05a72dda8bff81
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Wrap OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x73729a1a6fd5ecaadbd3a8df9ffd24e00a0dd8b35874f2c88f05a72dda8bff81
  const serializedTx = txFromEtherscan("0x02f8b1010c8405f5e10085098356220b83018f6e94d2af830e8cbdfed6cc11bab697bb25496ed6fa6280b8446e553f6500000000000000000000000000000000000000000000001b2d3e4baa5320bef9000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70cc001a0e778365e30572f81f0c86a94f7a503cbc81c0097611415c18962c1ecc3510fa7a05c812baa8b16c9f20c8e6525be400e290aa3f91e0b352f350b3ba3193b368071");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 8 : 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_wrap_ousd', [right_clicks, 0]);

  await tx;
  }));
});
