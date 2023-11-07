import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x9858e47bcbbe6fbac040519b02d7cd4b2c470c66";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x26c761bc4d990ce4c1598ae7f14d724ca8120ca1a8dc32ab6dde0aca7947c45d
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OETH with ETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x26c761bc4d990ce4c1598ae7f14d724ca8120ca1a8dc32ab6dde0aca7947c45d
  const serializedTx = txFromEtherscan("0x02f87b018204108405f5e100850a4ebc78da8307bac1949858e47bcbbe6fbac040519b02d7cd4b2c470c6689022b1c8c1227a0000084d0e30db0c001a0614fc30e51e2e801bcece0ba6b9f52d1e6ae94fa8ee9225c7e95e83070f28168a048590aab8428e959a817dca99a30cb00da53f53bdb4dff9d87bfb012f54a9733");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_zapper_deposit_eth', [right_clicks, 0]);

  await tx;
  }));
});
