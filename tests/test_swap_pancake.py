# from pathlib import Path
# import json
# import os
# import datetime
# from web3 import Web3
# from eth_typing import ChainId
# from ledger_app_clients.ethereum.client import EthAppClient
# from ledger_app_clients.ethereum.utils import get_selector_from_data
# from ragger.navigator import NavInsID
# from .utils import get_appname_from_makefile

# ROOT_SCREENSHOT_PATH = Path(__file__).parent
# ABIS_FOLDER = "%s/abis" % (os.path.dirname(__file__))
# PLUGIN_NAME = get_appname_from_makefile()

# with open ("%s/ERC20.json" % (ABIS_FOLDER)) as file:
#     contract = Web3().eth.contract(
#         abi= json.load(file),
#         address=bytes.fromhex("10ED43C718714eb63d5aA57B78B54704E256024E")
#     )

# def test_swap_pancake(backend, firmware, navigator, test_name):
#     client = EthAppClient(backend)

#     data = contract.encodeABI("swapExactTokensForTokens"
#     , [
#         40000000,
#         1000000000000,
#         [
#             bytes.fromhex("55d398326f99059fF775485246999027B3197955"),
#             bytes.fromhex("0xcD1B51b87a8a7137D6421bA5A976225187a26777")
#         ],
#         bytes.fromhex("d8dA6BF26964aF9D7eEd9e03E53415D37aA96045"),
#         int(datetime.datetime(2025,12,25,0,0).timestamp())
#     ])
#     print("data: ", data)

#     with client.set_external_plugin(PLUGIN_NAME,
#                                     contract.address,
#                                     get_selector_from_data(data)):
#         pass

#     with client.sign("m/44'/60'/1'/0/0", {
#         "nonce": 24,
#         "maxFeePerGas": Web3.to_wei(1.5, "gwei"),
#         "maxPriorityFeePerGas": Web3.to_wei(1.5, "gwei"),
#         "gas": 173290,
#         "to": contract.address,
#         "value": Web3.to_wei(0, "ether"),
#         "chainId": ChainId.ETH,
#         "data": data
#     }):
#         if firmware.device.startswith("nano"):
#             navigator.navigate_until_text_and_compare(NavInsID.RIGHT_CLICK,
#                                                       [NavInsID.BOTH_CLICK],
#                                                       "Accept",
#                                                       ROOT_SCREENSHOT_PATH,
#                                                       test_name)
#         else:
#             navigator.navigate_until_text_and_compare(NavInsID.USE_CASE_REVIEW_TAP,
#                                                       [NavInsID.USE_CASE_REVIEW_CONFIRM,
#                                                        NavInsID.USE_CASE_STATUS_DISMISS],
#                                                       "Hold to sign",
#                                                       ROOT_SCREENSHOT_PATH,
                                                        # testname)

                                                    