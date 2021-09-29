import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, SPECULOS_ADDRESS} from './test.fixture';
import { ethers } from "ethers";
import { parseEther, parseUnits} from "ethers/lib/utils";

// EDIT THIS: Replace with your contract address
const contractAddr = "0x7a250d5630b4cf539739df2c5dacb4c659f2488d";
// EDIT THIS: Replace `boilerplate` with your plugin name
const abi_path = '../boilerplate/abis/' + contractAddr + '.json';
const abi = require(abi_path);


// Reference transaction for this test:
// https://etherscan.io/tx/0x0160b3aec12fd08e6be0040616c7c38248efb4413168a3372fc4d2db0e5961bb

// Nanos S test
test('[Nano S] Swap Exact Eth For Tokens with beneficiary', zemu("nanos", async (sim, eth) => {
  // Constants used to create the transaction
  const amountOutMin = parseUnits("28471151959593036279", 'wei');
  const WETH = "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2";
  const SUSHI = "0x6b3595068778dd592e39a122f4f5a5cf09c90fe2";
  const path = [WETH, SUSHI];
  const deadline = Number(1632843280);
  const beneficiary = "0x15557c8b7246C38EE71eA6dc69e4347F5DAc2104";
  const contract = new ethers.Contract(contractAddr, abi);

  // signature: swapExactETHForTokens(uint amountOutMin, address[] calldata path, address to, uint deadline)
  const {data} = await contract.populateTransaction.swapExactETHForTokens(amountOutMin, path, beneficiary, deadline);

  // Get the generic transaction template
  let unsignedTx = genericTx;
  // Modify `to` to make it interact with the contract
  unsignedTx.to = contractAddr;
  // Modify the attached data
  unsignedTx.data = data;
  // Modify the number of ETH sent
  unsignedTx.value = parseEther("0.1");

  // Create serializedTx and remove the "0x" prefix
  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx
  );

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button 7 times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', 'nanos_swap_exact_eth_for_tokens_with_beneficiary', [10, 0]);

  await tx;
}));

// NanoX test
test('[Nano X] Swap Exact Eth For Tokens with beneficiary', zemu("nanox", async (sim, eth) => {

  // Rather than constructing the tx ourselves, one can can obtain it directly through etherscan (with a little bit of editing)
  const serializedTx = "02f901170181d38459682f0085215d7c1e598302a4e9947a250d5630b4cf539739df2c5dacb4c659f2488d88016345785d8a0000b8e47ff36ab50000000000000000000000000000000000000000000000018b1dd9dc51b5a9f7000000000000000000000000000000000000000000000000000000000000008000000000000000000000000015557c8b7246c38ee71ea6dc69e4347f5dac210400000000000000000000000000000000000000000000000000000000615336100000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000006b3595068778dd592e39a122f4f5a5cf09c90fe2c0";

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button 7 times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', 'nanox_swap_exact_eth_for_tokens_with_beneficiary', [6, 0]);

  await tx;
}));
