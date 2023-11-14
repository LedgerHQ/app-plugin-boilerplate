import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x39254033945aa2e4809cc2977e7087bee48bd7ab";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x24587806cb75a6159e4d8329289a6c140ba12ecee8934586d4d3452932271ce7
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Redeem OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x24587806cb75a6159e4d8329289a6c140ba12ecee8934586d4d3452932271ce7
  const serializedTx = txFromEtherscan("0x02f8b1015c8405f5e1008509f8630ef48308d7289439254033945aa2e4809cc2977e7087bee48bd7ab80b8447cbc237300000000000000000000000000000000000000000000000009d2d4b74c9da3c500000000000000000000000000000000000000000000000009b6e64a8ec60000c080a0617e25a8c320e4bce9321cb9d731bd934c6beeed226e49c50651dcd57599e3eca060b6ec97000e85dfd54de86c224131e51ab47026b5bf0ab044631f16e36f7240");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_redeem_oeth', [right_clicks, 0]);

  await tx;
  }));
});
