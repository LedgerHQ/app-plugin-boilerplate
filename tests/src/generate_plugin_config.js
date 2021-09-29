// EDIT THIS: replace with the name of your plugin (lowercase)
// You will need to create a folder with this name, and put in the abis of your contracts in `abis/`.
// You will also need to create a `b2c.json` file that will hold the methodIDs and location of
// the erc20 tokens that should get displayed.
const pluginFolder = "boilerplate";

function serialize_data(pluginName, contractAddress, selector) {
	const len = Buffer.from([pluginName.length]);
	const name = Buffer.from(pluginName)
	const address = Buffer.from(contractAddress.slice(2), "hex");
	const methodid = Buffer.from(selector.slice(2), "hex");

	// Taking .slice(2) to remove the "0x" prefix
	return Buffer.concat([len, name, address, methodid]);
}

function assert(condition, message) {
	if (!condition) {
		throw message || "Assertion failed";
	}
}

// Function to generate the plugin configuration.
function generate_plugin_config() {
	
	var fs = require('fs');
	var files = fs.readdirSync(`${pluginFolder}/abis/`);
	
	// `contracts_to_abis` holds a maping of contract addresses to abis
	let contracts_to_abis = {};
	for (let abiFileName of files) {
		assert(abiFileName.toLocaleLowerCase() == abiFileName, `FAILED: File ${abiFileName} should be lower case.`);

		// Strip ".json" suffix
		let contractAddress = abiFileName.slice(0, abiFileName.length - ".json".length);
		// Load abi
		let abi = require(`../${pluginFolder}/abis/${abiFileName}`);
		// Add it to contracts
		contracts_to_abis[contractAddress] = abi;
	}
	
	// Load the b2c.json file
	const b2c = require(`../boilerplate/b2c.json`);
	
	let res = {};
	
	// Place holder signature
	const PLACE_HOLDER_SIGNATURE = "3045022100f6e1a922c745e244fa3ed9a865491672808ef93f492ee0410861d748c5de201f0220160d6522499f3a84fa3e744b3b81e49e129e997b28495e58671a1169b16fa777";

	// Iterate through contracts in b2c.json file
	for (let contract of b2c["contracts"]) {
		let methods_info = {};
		const contractAddress = contract["address"];
		assert(contractAddress.toLowerCase() == contractAddress, `FAILED: Contract Address ${contractAddress} should be lower case`);

		for (let [selector, values] of Object.entries(contract["selectors"])) {

			assert(selector.toLowerCase() == selector, `FAILED: Selector ${selector} should be lower case`);

			// Gather up the info needed for the end result
			const pluginName = values["plugin"];
			const serializedData = serialize_data(pluginName, contractAddress, selector);
			const signature = PLACE_HOLDER_SIGNATURE;

			// Put them in `methods_info`
			methods_info[selector] = {"erc20OfInterest": values["erc20OfInterest"], "plugin": pluginName, "serialized_data": serializedData, "signature": signature};
		}
		// Add the abi to methods_info
		methods_info["abi"] = contracts_to_abis[contractAddress];
		// Add the methods_info to the end result
		res[contractAddress] = methods_info;
	}

	assert(res.length == contracts_to_abis.length, `FAILED: ${res.length} contracts in b2c.json and ${contracts_to_abis.length} contracts in abis/ folder`);

	return res;
}

module.exports = {
	generate_plugin_config,
};
