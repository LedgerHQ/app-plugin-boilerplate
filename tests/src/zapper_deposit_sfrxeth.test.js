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

// Test from replayed transaction: https://etherscan.io/tx/0x6cd9a8dfffef39305f34415e62c8f743589dc638cbbacb7b743465da07e45e53
// EDIT THIS: build your own test
nano_models.forEach(function(model) {
  jest.setTimeout(50000)
  test('[Nano ' + model.letter + '] Mint OETH with sfrxETH', zemu(model, async (sim, eth) => {

  // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x6cd9a8dfffef39305f34415e62c8f743589dc638cbbacb7b743465da07e45e53
  const serializedTx = txFromEtherscan("0x02f8b1016a8405f5e100850c697f53768307568a949858e47bcbbe6fbac040519b02d7cd4b2c470c6680b844d443e97d0000000000000000000000000000000000000000000000006d56392667c40d630000000000000000000000000000000000000000000000006d37db4d8e530000c080a02e33189b78b5f34df3deb27a69a738fec9ec0b81adadcf9d2d4d0cb51e2bf792a074c08c1ce63b1353ddfbe062b9b3454c6404cb5d3d47f333d417f50a78c4dc9e");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = model.letter === 'S' ? 8 : 5;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_zapper_deposit_sfrxeth', [right_clicks, 0]);

  await tx;
  }));
});
