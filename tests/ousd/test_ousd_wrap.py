from web3 import Web3
from tests.utils import run_test, load_contract

from ledger_app_clients.ethereum.client import EthAppClient
import ledger_app_clients.ethereum.response_parser as ResponseParser

contract_wousd = load_contract(
    "D2af830E8CBdFed6CC11Bab697bB25496ed6FA62",
    "4626-vault"
)

def test_ousd_wrap(backend, firmware, navigator, test_name):
    client = EthAppClient(backend)

    with client.get_public_addr(display=False):
      pass
    _, addr, _ = ResponseParser.pk_addr(client.response().data)

    data = contract_wousd.encodeABI("deposit", [
        Web3.to_wei(1, "ether"),
        addr
    ])

    run_test(contract_wousd, data, backend, firmware, navigator, test_name)

def test_ousd_wrap_different_beneficiary(backend, firmware, navigator, test_name):
    data = contract_wousd.encodeABI("deposit", [
        Web3.to_wei(1, "ether"),
        bytes.fromhex("000000000000000000000000000000000000dEaD")
    ])

    run_test(contract_wousd, data, backend, firmware, navigator, test_name)

def test_ousd_unwrap(backend, firmware, navigator, test_name):
    client = EthAppClient(backend)

    with client.get_public_addr(display=False):
      pass
    _, addr, _ = ResponseParser.pk_addr(client.response().data)

    data = contract_wousd.encodeABI("redeem", [
        Web3.to_wei(1, "ether"),
        addr,
        addr
    ])

    run_test(contract_wousd, data, backend, firmware, navigator, test_name)

def test_ousd_unwrap_different_beneficiary(backend, firmware, navigator, test_name):
    data = contract_wousd.encodeABI("redeem", [
        Web3.to_wei(1, "ether"),
        bytes.fromhex("000000000000000000000000000000000000dEaD"),
        bytes.fromhex("000000000000000000000000000000000002dEaD")
    ])

    run_test(contract_wousd, data, backend, firmware, navigator, test_name)
