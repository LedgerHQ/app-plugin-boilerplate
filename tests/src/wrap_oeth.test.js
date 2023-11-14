import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0xdcee70654261af21c44c093c300ed3bb97b78192";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x159ed616f5914352ea2f72d92afac27afa953bdce162b52e882df523bd27f253
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Wrap OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x159ed616f5914352ea2f72d92afac27afa953bdce162b52e882df523bd27f253
  const serializedTx = txFromEtherscan("0x02f8b301821ee38405f5e10085085af8af9183018ef394dcee70654261af21c44c093c300ed3bb97b7819280b8446e553f650000000000000000000000000000000000000000000000004192927743b87fff000000000000000000000000fdbbfb0fe2986672af97eca0e797d76a0bbf35c9c080a0d971478250211b657496dee7241ea80f249789f03816db5809eb2b99f8648a47a0131ff1d325a92153e23e251f0215accd15d30c376406e6f516af4b3bf53ce260");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 11 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_wrap_beneficiary_oeth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x159ed616f5914352ea2f72d92afac27afa953bdce162b52e882df523bd27f253
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Wrap OETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x159ed616f5914352ea2f72d92afac27afa953bdce162b52e882df523bd27f253
  const serializedTx = txFromEtherscan("0x02f8b301821ee38405f5e10085085af8af9183018ef394dcee70654261af21c44c093c300ed3bb97b7819280b8446e553f650000000000000000000000000000000000000000000000004192927743b87fff000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70cc080a0d971478250211b657496dee7241ea80f249789f03816db5809eb2b99f8648a47a0131ff1d325a92153e23e251f0215accd15d30c376406e6f516af4b3bf53ce260");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 8 : 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_wrap_oeth', [right_clicks, 0]);

  await tx;
  }));
});
