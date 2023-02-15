import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx, nano_models, txFromEtherscan} from './test.fixture';
import { ethers } from "ethers";
import { parseEther} from "ethers/lib/utils";

const contractAddr = "0xc228cefdf841defdbd5b3a18dfd414cc0dbfa0d8";
const pluginName = "pstake";
const testNetwork = "bsc";
const abi_path = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abi_path);

// Test from replayed transaction: https://bscscan.com/tx/0x402104398b7ec82762c110bfa20cba59ceea4ea89db2044ec81636add2d0368a
nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Stake: On-chain tx', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is not accessible through: https://bscscan.com/getRawTx?tx=0x402104398b7ec82762c110bfa20cba59ceea4ea89db2044ec81636add2d0368a
  // So, using:
  // curl -H "Content-Type: application/json" -X POST --data \
  // '{"jsonrpc":"2.0","method":"eth_getRawTransactionByHash","params":["0x402104398b7ec82762c110bfa20cba59ceea4ea89db2044ec81636add2d0368a"],"id":1}' https://<RPC-Node-URL-here>
  const serializedTx = txFromEtherscan("0xf87582014285012a05f200830154b994c228cefdf841defdbd5b3a18dfd414cc0dbfa0d889018493fba64ef0000084d0e30db08194a0d4ee4f4d82d46d7b107264b663e3a9f5952f5292582b97dfbdeda05ce65a19f7a01036678d6f646529c73a5f5946817da78373ab29d855f6b5cecd8729259e0a93");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_stake_on_chain_tx', [right_clicks, 0]);

  await tx;
  }));
});

// Test from constructed transaction
nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Stake: Custom tx', zemu(model, async (sim, eth) => {
  const contract = new ethers.Contract(contractAddr, abi);

  const {data} = await contract.populateTransaction.deposit();

  // Get the generic transaction template
  let unsignedTx = genericTx;
  // Modify `to` to make it interact with the contract
  unsignedTx.to = contractAddr;
  // Modify the attached data
  unsignedTx.data = data;
  // Modify the number of ETH sent
  unsignedTx.value = parseEther("1.23");

  // Create serializedTx and remove the "0x" prefix
  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx
  );

  const right_clicks = 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button 4 times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_stake_custom_tx', [right_clicks, 0]);

  await tx;
  }));
});

