from pathlib import Path
import json
import os

import datetime

from web3 import Web3
from eth_typing import ChainId

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ledger_app_clients.ethereum.utils import get_selector_from_data
from ragger.navigator import NavInsID

from .utils import get_appname_from_makefile


ROOT_SCREENSHOT_PATH = Path(__file__).parent
ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))

PLUGIN_NAME = get_appname_from_makefile()

with open("%s/0x0fa7c8350c8f028bc55899dbc1446ecd342958fe.abi.json" % (ABIS_FOLDER)) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        # Get address from filename
        address=bytes.fromhex(os.path.basename(file.name).split(".")[0].split("x")[-1])
    )

def test_batch_deposit(backend, firmware, navigator, test_name):
    client = EthAppClient(backend)

    # values taken from https://goerli.etherscan.io/tx/0xf711055fa03e17137c9891960237033711c1b75e5812a1edce1cf7f7c2d0d91b
    data = contract.encodeABI("batchDeposit", [
        bytes.fromhex('86cf59bf1295b5484d153e4cebf6132b65719958511ec6ff8f1e8c2dd4f704cd449b0984d5168a1eddc0e3221eef2bcc'),
        bytes.fromhex('010000000000000000000000a1237efe3159197537f41f510f01d09394780f08'),
        bytes.fromhex('a547573d76cdf47f843f16522f4312133fe78560f682e3659344ead855a7f309ebd6ccb882e4ddfae1b8b05c17b605b302276a226d3f6129fe23358e333beb350d95bf08bbbed85b03004198d31db7df4e2c347833c97d51d6c030528c28b60c'),
        [
            b'0xa36e68c1081f5a546314ebc866f29e277e6052f20a0fac20cb9cb4bf00fae242'
        ]
    ])

    # first setup the external plugin
    with client.set_external_plugin(PLUGIN_NAME,
                                    contract.address,
                                    # Extract function selector from the encoded data
                                    get_selector_from_data(data)):
        pass

    # send the transaction
    with client.sign("m/44'/60'/1'/0/0", {
             "nonce": 20,
             "maxFeePerGas": Web3.to_wei(145, "gwei"),
             "maxPriorityFeePerGas": Web3.to_wei(1.5, "gwei"),
             "gas": 173290,
             "to": contract.address,
             "value": Web3.to_wei(32, "ether"),
             "chainId": ChainId.GOR,
             "data": data
         }):
        # Validate the on-screen request by performing the navigation appropriate for this device
        if firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
                                                      [NavInsID.BOTH_CLICK],
                                                      "Accept",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)
        else:
            navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
                                                      [NavInsID.USE_CASE_REVIEW_CONFIRM,
                                                       NavInsID.USE_CASE_STATUS_DISMISS],
                                                      "Hold to sign",
                                                      ROOT_SCREENSHOT_PATH,
                                                      test_name)