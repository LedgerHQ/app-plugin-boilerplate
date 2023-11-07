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

// Test from replayed transaction: https://etherscan.io/tx/0x976228efba301950cf508631b3cfae12e3eb7f288c21cfa97fe9dfdb5044bfe0
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Unwrap OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x976228efba301950cf508631b3cfae12e3eb7f288c21cfa97fe9dfdb5044bfe0
  const serializedTx = txFromEtherscan("0x02f8d1015f8405f5e100850b1026ef4083017d5a94d2af830e8cbdfed6cc11bab697bb25496ed6fa6280b864ba0876520000000000000000000000000000000000000000000000000d5d3f20e0e545ca0000000000000000000000001fc65ac3448f225ba073a59edfe51e1e3699f5970000000000000000000000001fc65ac3448f225ba073a59edfe51e1e3699f597c001a0eac678ade48327c882b20f4e5dabc445358a9205f426afcc1fef215a7c9f8493a00ef44550c6789212f51292b8ddb5e26c572664405ca05a713a984eb242107d72");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 10 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_unwrap_beneficiary_ousd', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x976228efba301950cf508631b3cfae12e3eb7f288c21cfa97fe9dfdb5044bfe0
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Unwrap OUSD', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x976228efba301950cf508631b3cfae12e3eb7f288c21cfa97fe9dfdb5044bfe0
  const serializedTx = txFromEtherscan("0x02f8d1015f8405f5e100850b1026ef4083017d5a94d2af830e8cbdfed6cc11bab697bb25496ed6fa6280b864ba0876520000000000000000000000000000000000000000000000000d5d3f20e0e545ca000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70c0000000000000000000000001fc65ac3448f225ba073a59edfe51e1e3699f597c001a0eac678ade48327c882b20f4e5dabc445358a9205f426afcc1fef215a7c9f8493a00ef44550c6789212f51292b8ddb5e26c572664405ca05a713a984eb242107d72");
  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_unwrap_ousd', [right_clicks, 0]);

  await tx;
  }));
});
