"use strict";

require("core-js/stable");
require("regenerator-runtime/runtime");
var _test = require("./test.fixture");
var _ethers = require("ethers");
var _utils = require("ethers/lib/utils");
// EDIT THIS: Replace with your contract address
const contractAddr = "0xe75d77b1865ae93c7eaa3040b038d7aa7bc02f70";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origin";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0xfbb0557cc85b244a68a1bf030379c43117723375326e44c43db44af54a31ece0
// EDIT THIS: build your own test
_test.nano_models.forEach(function (model) {
  jest.setTimeout(100000);
  test('[Nano ' + model.letter + '] Redeem OUSD', (0, _test.zemu)(model, async (sim, eth) => {
    // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfbb0557cc85b244a68a1bf030379c43117723375326e44c43db44af54a31ece0
    const serializedTx = (0, _test.txFromEtherscan)("0xf8aa0485055ae826008328155794e75d77b1865ae93c7eaa3040b038d7aa7bc02f7080b8447cbc2373000000000000000000000000000000000000000000003ea6080a7f4ba08e59ff000000000000000000000000000000000000000000003e2a03f3da153c51000025a0e98bbec6aa4ed8480923d5c5b74dcadf09d0bcd6b1c794500da9e5f12b32def3a069f50d96250a062339968570da2233676acc9886286a1f2ead8d5909efbe284b");
    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
    const right_clicks = model.letter === 'S' ? 7 : 5;

    // Wait for the application to actually load and parse the transaction
    await (0, _test.waitForAppScreen)(sim);
    // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
    await sim.navigateAndCompareSnapshots('.', model.name + '_vault_redeem_ousd', [right_clicks, 0]);
    await tx;
  }));
});

// Test from constructed transaction
// EDIT THIS: build your own test
/*nano_models.forEach(function(model) {
  jest.setTimeout(20000)
  test('[Nano ' + model.letter + '] Swap Exact Eth For Tokens', zemu(model, async (sim, eth) => {
  const contract = new ethers.Contract(contractAddr, abi);

  // Constants used to create the transaction
  // EDIT THIS: Remove what you don't need
  const amountOutMin = parseUnits("28471151959593036279", 'wei');
  const WETH = "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2";
  const SUSHI = "0x6b3595068778dd592e39a122f4f5a5cf09c90fe2";
  const path = [WETH, SUSHI];
  const deadline = Number(1632843280);
  // We set beneficiary to the default address of the emulator, so it maches sender address
  const beneficiary = SPECULOS_ADDRESS;

  // EDIT THIS: adapt the signature to your method
  // signature: swapExactETHForTokens(uint amountOutMin, address[] calldata path, address to, uint deadline)
  // EDIT THIS: don't call `swapExactETHForTokens` but your own method and adapt the arguments.
  const {data} = await contract.populateTransaction.swapExactETHForTokens(amountOutMin, path, beneficiary ,deadline);

  // Get the generic transaction template
  let unsignedTx = genericTx;
  // Modify `to` to make it interact with the contract
  unsignedTx.to = contractAddr;
  // Modify the attached data
  unsignedTx.data = data;
  // EDIT THIS: get rid of this if you don't wish to modify the `value` field.
  // Modify the number of ETH sent
  unsignedTx.value = parseEther("0.1");

  // Create serializedTx and remove the "0x" prefix
  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
  // EDIT THIS: modify `10` to fix the number of screens you are expecting to navigate through.
  await sim.navigateAndCompareSnapshots('.', model.name + '_swap_exact_eth_for_tokens', [right_clicks, 0]);

  await tx;
  }));
});*/