import Zemu from "@zondax/zemu";
import Eth from "@ledgerhq/hw-app-eth";
import { generate_plugin_config } from "./generate_plugin_config";
import { parseEther, parseUnits, RLP } from "ethers/lib/utils";
import { ethers } from "ethers";


const transactionUploadDelay = 60000;

const sim_options_generic = {
  logging: true,
  X11: true,
  startDelay: 5000,
  startText: 'is ready',
  custom: "",
};

const Resolve = require("path").resolve;

const NANOS_ETH_PATH = Resolve("elfs/ethereum_nanos.elf");
const NANOX_ETH_PATH = Resolve("elfs/ethereum_nanox.elf");

const NANOS_PLUGIN_PATH = Resolve("elfs/plugin_nanos.elf");
const NANOX_PLUGIN_PATH = Resolve("elfs/plugin_nanox.elf");

const NANOS_PLUGIN = { Paraswap: NANOS_PLUGIN_PATH };
const NANOX_PLUGIN = { Paraswap: NANOX_PLUGIN_PATH };

const SPECULOS_ADDRESS = "0xFE984369CE3919AA7BB4F431082D027B4F8ED70C";
const RANDOM_ADDRESS = "0xaaaabbbbccccddddeeeeffffgggghhhhiiiijjjj";

let genericTx = {
  nonce: Number(0),
  gasLimit: Number(21000),
  gasPrice: parseUnits("1", "gwei"),
  value: parseEther("1"),
  chainId: 1,
  to: RANDOM_ADDRESS,
  data: null,
};

const TIMEOUT = 1000000;

/**
 * Generates a serializedTransaction from a rawHexTransaction copy pasted from etherscan.
 * @param {string} rawTx Raw transaction
 * @returns {string} serializedTx
 */
function txFromEtherscan(rawTx) {
  // Remove 0x prefix
  rawTx = rawTx.slice(2);

  let txType = rawTx.slice(0, 2);
  if (txType == "02" || txType == "01") {
    // Remove "02" prefix
    rawTx = rawTx.slice(2);
  } else {
    txType = "";
  }

  let decoded = RLP.decode("0x" + rawTx);
  if (txType != "") {
    decoded = decoded.slice(0, decoded.length - 3); // remove v, r, s
  } else {
    decoded[decoded.length - 1] = "0x"; // empty
    decoded[decoded.length - 2] = "0x"; // empty
    decoded[decoded.length - 3] = "0x01"; // chainID 1
  }

  // Encode back the data, drop the '0x' prefix
  let encoded = RLP.encode(decoded).slice(2);

  // Don't forget to prepend the txtype
  return txType + encoded;
}

/**
 * Emulation of the device using zemu
 * @param {string} device name of the device to emulate (nanos, nanox)
 * @param {function} func
 * @param {boolean} signed the plugin is already signed
 * @returns {Promise}
 */
function zemu(device, func, signed = false, testNetwork="ethereum") {
  return async () => {
    jest.setTimeout(TIMEOUT);
    let eth_path;
    let plugin;
    let sim_options = sim_options_generic;

    if (device === "nanos") {
      eth_path = NANOS_ETH_PATH;
      plugin = NANOS_PLUGIN;
      sim_options.model = "nanos";
    } else {
      eth_path = NANOX_ETH_PATH;
      plugin = NANOX_PLUGIN;
      sim_options.model = "nanox";
    }

    const sim = new Zemu(eth_path, plugin);

    try {
      await sim.start(sim_options);
      const transport = await sim.getTransport();
      const eth = new Eth(transport);

      if(!signed){
        eth.setPluginsLoadConfig({
          baseURL: null,
          extraPlugins: generate_plugin_config(testNetwork),
        });
      }
      await func(sim, eth);
    } finally {
      await sim.close();
    }
  };
}

/**
 * Process the trasaction through the full test process in interaction with the simulator
 * @param {Eth} eth Device to test (nanos, nanox)
 * @param {function} sim Zemu simulator
 * @param {int} steps Number of steps to push right button
 * @param {string} label directory against which the test snapshots must be checked.
 * @param {string} rawTxHex RawTransaction Hex to process
 */
async function processTransaction(eth, sim, steps, label, rawTxHex,srlTx="") {

  let serializedTx;

  if(srlTx == "")
    serializedTx = txFromEtherscan(rawTxHex);
  else
    serializedTx = srlTx;

  let tx = eth.signTransaction("44'/60'/0'/0/0", serializedTx);

  await sim.waitUntilScreenIsNot(
    sim.getMainMenuSnapshot(),
    transactionUploadDelay
  );
  await sim.navigateAndCompareSnapshots(".", label, [steps, 0]);

  await tx;
}

/**
 * Function to execute test with the simulator
 * @param {Object} device Device including its name, its label, and the number of steps to process the use case
 * @param {string} contractName Name of the contract
 * @param {string} testLabel Name of the test case
 * @param {string} testDirSuffix Name of the folder suffix for snapshot comparison
 * @param {string} rawTxHex RawTx Hex to test
 * @param {boolean} signed The plugin is already signed and existing in Ledger database
 */
function processTest(device, contractName, testLabel, testDirSuffix, rawTxHex, signed, serializedTx, testNetwork="ethereum" ) {
  test(
    "[" + contractName + "] - " + device.label + " - " + testLabel,
    zemu(device.name, async (sim, eth) => {
      await processTransaction(
        eth,
        sim,
        device.steps,
        testNetwork+ "_" + device.name + "_" + testDirSuffix,
        rawTxHex,
        serializedTx
      );
    },signed, testNetwork)
  );
}


function populateTransaction(contractAddr, inputData, chainId, value="0.1"){
  // Get the generic transaction template
  let unsignedTx = genericTx;
  //adapt to the appropriate network
  unsignedTx.chainId = chainId;
  // Modify `to` to make it interact with the contract
  unsignedTx.to = contractAddr;
  // Modify the attached data
  unsignedTx.data = inputData;
  // Modify the number of ETH sent
  unsignedTx.value = parseEther(value);
  // Create serializedTx and remove the "0x" prefix
  return ethers.utils.serializeTransaction(unsignedTx).slice(2);
}


module.exports = {
  processTest,
  genericTx,
  populateTransaction
};