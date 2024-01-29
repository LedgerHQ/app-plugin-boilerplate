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

with open ("%s/ERC20.json" % (ABIS_FOLDER)) as file:
    contract = Web3().eth.contract(
        abi=json.load(file),
        address=bytes.fromhex("A0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48")
    )

def test_approve_erc20(backend, firmware, navigator, test_name):
    client = EthAppClient(backend)

    data = contract.encodeABI("approve", [
        bytes.fromhex("cc78b5eC02BCD1A94c223a0Ee75a816D9A1De72C"),
        1000000000
    ])
    print(data)
    selctor = get_selector_from_data(data)
    hex_string =''.join([f'{ byte:02X}' for byte in selctor])
    print("selector:", hex_string)
    print(contract)
    # return



    with client.set_external_plugin(PLUGIN_NAME,
                                    contract.address,
                                    get_selector_from_data(data)):
        pass

    with client.sign("m/44'/60'/1'/0/0", {
        "nonce": 23,
        "maxFeePerGas": Web3.to_wei(145, "gwei"),
        "maxPriorityFeePerGas": Web3.to_wei(1.5, "gwei"),
        "gas": 180000,
        "to": contract.address,
        "value": Web3.to_wei(0, "ether"),
        "chainId": ChainId.ETH,
        "data": data
    }):
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
                                                       "Hold to Sign",
                                                       ROOT_SCREENSHOT_PATH,
                                                       test_name)




