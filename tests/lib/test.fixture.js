"use strict";

var _zemu = _interopRequireWildcard(require("@zondax/zemu"));
var _hwAppEth = _interopRequireDefault(require("@ledgerhq/hw-app-eth"));
var _generate_plugin_config = require("./generate_plugin_config");
var _utils = require("ethers/lib/utils");
function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }
function _getRequireWildcardCache(e) { if ("function" != typeof WeakMap) return null; var r = new WeakMap(), t = new WeakMap(); return (_getRequireWildcardCache = function (e) { return e ? t : r; })(e); }
function _interopRequireWildcard(e, r) { if (!r && e && e.__esModule) return e; if (null === e || "object" != typeof e && "function" != typeof e) return { default: e }; var t = _getRequireWildcardCache(r); if (t && t.has(e)) return t.get(e); var n = { __proto__: null }, a = Object.defineProperty && Object.getOwnPropertyDescriptor; for (var u in e) if ("default" !== u && Object.prototype.hasOwnProperty.call(e, u)) { var i = a ? Object.getOwnPropertyDescriptor(e, u) : null; i && (i.get || i.set) ? Object.defineProperty(n, u, i) : n[u] = e[u]; } return n.default = e, t && t.set(e, n), n; }
const transactionUploadDelay = 60000;
async function waitForAppScreen(sim) {
  await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot(), transactionUploadDelay);
}
const sim_options_nano = {
  ..._zemu.DEFAULT_START_OPTIONS,
  logging: true,
  X11: true,
  startDelay: 5000,
  startText: 'is ready'
};
const Resolve = require('path').resolve;
const NANOS_ETH_PATH = Resolve('elfs/ethereum_nanos.elf');
const NANOSP_ETH_PATH = Resolve('elfs/ethereum_nanosp.elf');
const NANOX_ETH_PATH = Resolve('elfs/ethereum_nanox.elf');
const NANOS_PLUGIN_PATH = Resolve('elfs/plugin_nanos.elf');
const NANOSP_PLUGIN_PATH = Resolve('elfs/plugin_nanosp.elf');
const NANOX_PLUGIN_PATH = Resolve('elfs/plugin_nanox.elf');
const nano_models = [{
  name: 'nanos',
  letter: 'S',
  path: NANOS_PLUGIN_PATH,
  eth_path: NANOS_ETH_PATH
}, {
  name: 'nanosp',
  letter: 'SP',
  path: NANOSP_PLUGIN_PATH,
  eth_path: NANOSP_ETH_PATH
}, {
  name: 'nanox',
  letter: 'X',
  path: NANOX_PLUGIN_PATH,
  eth_path: NANOX_ETH_PATH
}];
const originDefiJSON = (0, _generate_plugin_config.generate_plugin_config)();
const SPECULOS_ADDRESS = '0xFE984369CE3919AA7BB4F431082D027B4F8ED70C';
const RANDOM_ADDRESS = '0xaaaabbbbccccddddeeeeffffgggghhhhiiiijjjj';
let genericTx = {
  nonce: Number(0),
  gasLimit: Number(21000),
  gasPrice: (0, _utils.parseUnits)('1', 'gwei'),
  value: (0, _utils.parseEther)('1'),
  chainId: 1,
  to: RANDOM_ADDRESS,
  data: null
};
const TIMEOUT = 1000000;

// Generates a serializedTransaction from a rawHexTransaction copy pasted from etherscan.
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
  let decoded = _utils.RLP.decode("0x" + rawTx);
  if (txType != "") {
    decoded = decoded.slice(0, decoded.length - 3); // remove v, r, s
  } else {
    decoded[decoded.length - 1] = "0x"; // empty
    decoded[decoded.length - 2] = "0x"; // empty
    decoded[decoded.length - 3] = "0x01"; // chainID 1
  }

  // Encode back the data, drop the '0x' prefix
  let encoded = _utils.RLP.encode(decoded).slice(2);

  // Don't forget to prepend the txtype
  return txType + encoded;
}
function zemu(device, func) {
  return async () => {
    jest.setTimeout(TIMEOUT);
    let elf_path;
    let lib_elf;
    elf_path = device.eth_path;
    // Edit this: replace `Boilerplate` by your plugin name
    lib_elf = {
      'origindefi': device.path
    };
    const sim = new _zemu.default(elf_path, lib_elf);
    try {
      await sim.start({
        ...sim_options_nano,
        model: device.name
      });
      const transport = await sim.getTransport();
      const eth = new _hwAppEth.default(transport);
      eth.setLoadConfig({
        baseURL: null,
        extraPlugins: originDefiJSON
      });
      await func(sim, eth);
    } finally {
      await sim.close();
    }
  };
}
module.exports = {
  zemu,
  waitForAppScreen,
  genericTx,
  nano_models,
  SPECULOS_ADDRESS,
  RANDOM_ADDRESS,
  txFromEtherscan
};