import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x94b17476a93b3262d87b9a326965d1e91f9c13e7";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x22c7ec2b750a7ebb952de23b7b5a3b9edd6186f0fe2df5d6cd758babf0342afc
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OETH for ETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x22c7ec2b750a7ebb952de23b7b5a3b9edd6186f0fe2df5d6cd758babf0342afc
  const serializedTx = txFromEtherscan("0x02f8f101308405f5e100850d01e664278302534d9494b17476a93b3262d87b9a326965d1e91f9c13e780b8843df021240000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000076259b9e6f2d36f600000000000000000000000000000000000000000000000075d28a049f110000c001a0ccbd5897a8c00a7adf97f3ba96d821651b9be3be8f9e241c88b40202905dcefaa0148d6afdb1b078ebbc56bc913a762d3eece879b540b61c79154bf7cc9cdbd4ea");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_exchange_oeth_for_eth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xee3caa668e5cc936aab2263ff88485864f7c589fa8cfa5839858858452b654f7
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange ETH for OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xee3caa668e5cc936aab2263ff88485864f7c589fa8cfa5839858858452b654f7
  const serializedTx = txFromEtherscan("0x02f8f90105843b9aca00852152e8d9c88302d24a9494b17476a93b3262d87b9a326965d1e91f9c13e788429d069189e00000b8843df0212400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000429d069189e00000000000000000000000000000000000000000000000000000429059f4c8f1460bc001a0e8e0485b020454731eb65e905ecd318c82d9f44c0e3adc84a81e4662464c1f89a02d8bfb04fe59a1a43009603eb481351ce6f3549778afe36757142196ef36ffe3");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_curve_exchange_eth_for_oeth', [right_clicks, 0]);

  await tx;
  }));
});
