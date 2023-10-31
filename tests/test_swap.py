from pathlib import Path

from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
from ragger.navigator import NavInsID

from .utils import get_appname_from_makefile


ROOT_SCREENSHOT_PATH = Path(__file__).parent

PLUGIN_NAME = get_appname_from_makefile()

PLACE_HOLDER_SIGNATURE = bytes.fromhex('3045022100f6e1a922c745e244fa3ed9a865491672808ef93f492ee0410861d748c5de201f0220160d6522499f3a84fa3e744b3b81e49e129e997b28495e58671a1169b16fa777')

# In this test we verify "Swap Exact Eth For Tokens with beneficiary"
# Test from replayed transaction: https://etherscan.io/tx/0x0160b3aec12fd08e6be0040616c7c38248efb4413168a3372fc4d2db0e5961bb
# Function: swapExactETHForTokens(uint256 amountOutMin, address[] path, address to, uint256 deadline)
#
# MethodID: 0x7ff36ab5
# [0]:  0000000000000000000000000000000000000000000000018b1dd9dc51b5a9f7
# [1]:  0000000000000000000000000000000000000000000000000000000000000080
# [2]:  00000000000000000000000015557c8b7246c38ee71ea6dc69e4347f5dac2104
# [3]:  0000000000000000000000000000000000000000000000000000000061533610
# [4]:  0000000000000000000000000000000000000000000000000000000000000002
# [5]:  000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2
# [6]:  0000000000000000000000006b3595068778dd592e39a122f4f5a5cf09c90fe2

# EDIT THIS: build your own test
def test_swap_exact_eth_for_token(backend, firmware, navigator, test_name):
    # EDIT THIS: Replace with your contract address
    contract_address = bytes.fromhex('7a250d5630b4cf539739df2c5dacb4c659f2488d')
    # EDIT THIS: Replace with your method selector
    method_selector = bytes.fromhex('7ff36ab5')
    # The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0x0160b3aec12fd08e6be0040616c7c38248efb4413168a3372fc4d2db0e5961bb
    # EDIT THIS: Replace with your Tx payload
    tx_payload = bytes.fromhex('02f9015a0181d38459682f0085215d7c1e598302a4e9947a250d5630b4cf539739df2c5dacb4c659f2488d88016345785d8a0000b8e47ff36ab50000000000000000000000000000000000000000000000018b1dd9dc51b5a9f7000000000000000000000000000000000000000000000000000000000000008000000000000000000000000015557c8b7246c38ee71ea6dc69e4347f5dac210400000000000000000000000000000000000000000000000000000000615336100000000000000000000000000000000000000000000000000000000000000002000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc20000000000000000000000006b3595068778dd592e39a122f4f5a5cf09c90fe2c001a089c5ce4ce199f7d93ea1d54c08133fab9407d8de63a9885d59d8ce69a59573dda045f6a0e0d3288dfdfddc23ad0afb9577c4011801f598d581a46cd0b0e2bd0571')

    client = EthAppClient(backend)

    # first setup the external plugin
    with client.set_external_plugin(PLUGIN_NAME, contract_address, method_selector, PLACE_HOLDER_SIGNATURE):
        pass

    assert client.response().status == StatusWord.OK

    # send the transaction
    # !!!! to be replaced by a dedicated plugin sign function !!!
    with client._sign("m/44'/60'/1'/0/0", tx_payload):
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

    assert client.response().status == StatusWord.OK
