import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models,SPECULOS_ADDRESS, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x39254033945aa2e4809cc2977e7087bee48bd7ab";
// EDIT THIS: Replace `boilerplate` with your plugin name
const pluginName = "origindefi";
const testNetwork = "ethereum";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://etherscan.io/tx/0x89d42b27050c5dc32eb097b1a99a3f46f3b0a2b8f0b22b0da0a0c13c5a76feef
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OETH with stETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x89d42b27050c5dc32eb097b1a99a3f46f3b0a2b8f0b22b0da0a0c13c5a76feef
  const serializedTx = txFromEtherscan("0x02f8d101038405f5e100850996f7e83d83087a279439254033945aa2e4809cc2977e7087bee48bd7ab80b864156e29f6000000000000000000000000ae7ab96520de3a18e5e111b5eaab095312d7fe8400000000000000000000000000000000000000000000000ac20dade5e477d3b500000000000000000000000000000000000000000000000abfd3986a20350000c001a0108e5f743bbbb1fa9ce47263a6c36a2a73fb5121c2a9890c7034560e6d6da6f7a07b5bcc6454f4a42e701b7547bded67a7001340d94da2c730d54efa85278fb783");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_steth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x10ce133ad3aff5e0c8fb86eae5a481215beb7b87505f7114eb5ec2d23aba056a
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OETH with WETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x10ce133ad3aff5e0c8fb86eae5a481215beb7b87505f7114eb5ec2d23aba056a
  const serializedTx = txFromEtherscan("0x02f8d101028405f5e1008508bba4df26830258f69439254033945aa2e4809cc2977e7087bee48bd7ab80b864156e29f6000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000000000000000000000000000000429d069189e000000000000000000000000000000000000000000000000000004064976a8dd0000c080a0b95178faf5aec8eb30d37d0af2ecee369ecda658ef0d56012108519b24d1d1dba03a4f4af7a5314eeb719a6ae317099386982d0de58de1ededc09bd602b6365a0c");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 7 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_weth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0x76fe3dc19782ba2e1274f81dd4347dcacc151960918b1b52bb91582a9d49ab50
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OETH with RETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x76fe3dc19782ba2e1274f81dd4347dcacc151960918b1b52bb91582a9d49ab50
  const serializedTx = txFromEtherscan("0xf8cc820118850aaa268efa830719f19439254033945aa2e4809cc2977e7087bee48bd7ab80b864156e29f6000000000000000000000000ae78736cd615f374d3085123a210448e74fc63930000000000000000000000000000000000000000000000000e09d8ed81f0ccd70000000000000000000000000000000000000000000000000efcee47256c000025a072a5b8dd0c954324064a047bf1890ae0e90b6ca2b3c0a2d8e7de2f4b0e7e72d4a064bbae768cf360a8c636b44edda93607db40e74efa1dfae1549b5337fb407e7a");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 9 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_reth', [right_clicks, 0]);

  await tx;
  }));
});

// Test from replayed transaction: https://etherscan.io/tx/0xfe4fbb405068e95c4ef4d0be3372dae1398b94b5ec1a1ff10300c045c4a7a2fd
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(100000)
  test('[Nano ' + model.letter + '] Mint OETH with frxETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xfe4fbb405068e95c4ef4d0be3372dae1398b94b5ec1a1ff10300c045c4a7a2fd
  const serializedTx = txFromEtherscan("0x02f8d201028502e318fc00851cdef9d8008303b3229439254033945aa2e4809cc2977e7087bee48bd7ab80b864156e29f60000000000000000000000005e8422345238f34275888049021821e8e08caa1f00000000000000000000000000000000000000000000000000b1a2bc2ec50000000000000000000000000000000000000000000000000000008e1bc9bf040000c001a0723c679978a180603104a9f2351d7ec3678ef14e8a083407ba417daac89f09dfa0723fa673a23502a826b9b83916b66bffd7910aaaf5adb51c4812d1ff1c7b93b2");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 5 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_vault_deposit_frxeth', [right_clicks, 0]);

  await tx;
  }));
});
