import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x99a58482bd75cbab83b27ec03ca68ff489b5788f";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xfe11f66cd1c739b495f96c6cb10560d721ad435d8c7bd01efc758e3e85dd4413
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDT', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfe11f66cd1c739b495f96c6cb10560d721ad435d8c7bd01efc758e3e85dd4413
  const serializedTx = txFromEtherscan("0x02f90172010e8405f5e100850955812f3c8303101594e592427a0aece92de3edee1f18e0157c0586156480b90104414bf3890000000000000000000000002a8e1e676ec238d8a992307b495b45b3feaa5e86000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec700000000000000000000000000000000000000000000000000000000000001f4000000000000000000000000a7e48829554ce0d862db02acede48d945d5ce613000000000000000000000000000000000000000000000000000001886fd8da3d000000000000000000000000000000000000000000000131615362a808bf0000000000000000000000000000000000000000000000000000000000014e8cfd300000000000000000000000000000000000000000000000000000000000000000c001a076b61ad8e6c7846707f031ddc19c892a85f3cfe77a02561f9c78f143c9a6926fa06fa27f1fba834fd012c7c1204d95ff3d42d04f3215644d735a2659a484734191");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 6;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_uniswap_beneficiary_exchange_single_ousd_for_usdt', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xfe11f66cd1c739b495f96c6cb10560d721ad435d8c7bd01efc758e3e85dd4413
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDT', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfe11f66cd1c739b495f96c6cb10560d721ad435d8c7bd01efc758e3e85dd4413
  const serializedTx = txFromEtherscan("0x02f90172010e8405f5e100850955812f3c8303101594e592427a0aece92de3edee1f18e0157c0586156480b90104414bf3890000000000000000000000002a8e1e676ec238d8a992307b495b45b3feaa5e86000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec700000000000000000000000000000000000000000000000000000000000001f4000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70c000000000000000000000000000000000000000000000000000001886fd8da3d000000000000000000000000000000000000000000000131615362a808bf0000000000000000000000000000000000000000000000000000000000014e8cfd300000000000000000000000000000000000000000000000000000000000000000c001a076b61ad8e6c7846707f031ddc19c892a85f3cfe77a02561f9c78f143c9a6926fa06fa27f1fba834fd012c7c1204d95ff3d42d04f3215644d735a2659a484734191");
  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 6 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_uniswap_exchange_single_ousd_for_usdt', [right_clicks, 0]);

  await tx;
  }));
});
