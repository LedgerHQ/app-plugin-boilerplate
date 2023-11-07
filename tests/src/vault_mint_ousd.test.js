import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xb41bb0e34d106e2fd52c7b7f150b11f1e3a2a5dc343088716a58bc4be2c599e0
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OUSD with DAI', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xb41bb0e34d106e2fd52c7b7f150b11f1e3a2a5dc343088716a58bc4be2c599e0
  const serializedTx = txFromEtherscan("0xf8cb81e38503fe1c8524832dc7e294e75d77b1865ae93c7eaa3040b038d7aa7bc02f7080b864156e29f60000000000000000000000006b175474e89094c44da98b954eedeac495271d0f00000000000000000000000000000000000000000000054b40b1f852bda000000000000000000000000000000000000000000000000005497daac683fd6e000025a06629d3c025a81bee907259c2a708cf509cffc5ce0aff3ff5039743e3c2f2e2eba00cfadc35f7c820b07c2840c2dc2366f3e5614e51035ab0748ba1038a245f3e85");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_dai', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x9c081693473f135e261155c6306a003213c94344739622cac176839ed5011baf
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OUSD with USDC', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x9c081693473f135e261155c6306a003213c94344739622cac176839ed5011baf
  const serializedTx = txFromEtherscan("0x02f8d3018215778405f5e1008508c6fa02158327b33294e75d77b1865ae93c7eaa3040b038d7aa7bc02f7080b864156e29f6000000000000000000000000a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48000000000000000000000000000000000000000000000000000000ba4245b51700000000000000000000000000000000000000000000a9377d164818e1d00000c001a09f624f2f1f98e52f03b78e98d44b7acf5839b777526ea4671a8b6987d32155cba03071a6c8c2673cbc093bafcfe0c43f208811129e8f5b6ef8242df7d7023de2a4");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_usdc', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x56d2d00df85071f63eeac04a68f9884947d4529b36603e7cc0ffbf5829139cd3
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OUSD with USDT', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x56d2d00df85071f63eeac04a68f9884947d4529b36603e7cc0ffbf5829139cd3
  const serializedTx = txFromEtherscan("0x02f8d3018215728405f5e100850ab2694e2d832c3af694e75d77b1865ae93c7eaa3040b038d7aa7bc02f7080b864156e29f6000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec700000000000000000000000000000000000000000000000000000045d964b800000000000000000000000000000000000000000000003f76c5018958ba500000c080a09f6a46c46270b936770e0077177dea6eb38fc37dd896c8e2fcaf6269d8768b4ea06262cbccfd26d7ebaf58a5c33d74bf88ede3d9ef21f568be63863e142fef7f56");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_usdt', [right_clicks, 0]);

  await tx;
  }));
});
