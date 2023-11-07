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

// Test from replayed transaction: https://etherscan.io/tx/0xea93ac0f288e6a986495570a489008da0cd4660e2565787e7b33b33c9c088dfb
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDC', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xea93ac0f288e6a986495570a489008da0cd4660e2565787e7b33b33c9c088dfb
  const serializedTx = txFromEtherscan("0x02f901b4018201368405f5e100850a91f951218304c7e994e592427a0aece92de3edee1f18e0157c0586156480b90144c04b8d59000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000042aabd3a2f2b64a44b99e0ab025657ba73b871a200000000000000000000000000000000000000000000000000000188820cd9f500000000000000000000000000000000000000000000006c6b935b8bbd4000000000000000000000000000000000000000000000000000000000000076b603c000000000000000000000000000000000000000000000000000000000000000422a8e1e676ec238d8a992307b495b45b3feaa5e860001f4dac17f958d2ee523a2206206994597c13d831ec7000064a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48000000000000000000000000000000000000000000000000000000000000c080a080e5058df4e6f21c2ffb34988b47289f23ea5d68fafd04206b68e17a58658a4ea012321ed088e8062c8a2fffd06959526a3fbca75a047d3e14ff2d273a7f25d1d6");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 10 : 6;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_uniswap_beneficiary_exchange_ousd_for_usdc', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xea93ac0f288e6a986495570a489008da0cd4660e2565787e7b33b33c9c088dfb
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Exchange OUSD for USDC', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xea93ac0f288e6a986495570a489008da0cd4660e2565787e7b33b33c9c088dfb
  const serializedTx = txFromEtherscan("0x02f901b4018201368405f5e100850a91f951218304c7e994e592427a0aece92de3edee1f18e0157c0586156480b90144c04b8d59000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000fe984369ce3919aa7bb4f431082d027b4f8ed70c00000000000000000000000000000000000000000000000000000188820cd9f500000000000000000000000000000000000000000000006c6b935b8bbd4000000000000000000000000000000000000000000000000000000000000076b603c000000000000000000000000000000000000000000000000000000000000000422a8e1e676ec238d8a992307b495b45b3feaa5e860001f4dac17f958d2ee523a2206206994597c13d831ec7000064a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48000000000000000000000000000000000000000000000000000000000000c080a080e5058df4e6f21c2ffb34988b47289f23ea5d68fafd04206b68e17a58658a4ea012321ed088e8062c8a2fffd06959526a3fbca75a047d3e14ff2d273a7f25d1d6");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_uniswap_exchange_ousd_for_usdc', [right_clicks, 0]);

  await tx;
  }));
});
