from web3 import Web3
from tests.utils import run_test, load_contract

contract_ousd_flipper = load_contract(
    "cecaD69d7D4Ed6D52eFcFA028aF8732F27e08F70",
    "flipper"
)

def test_ousd_flipper_flip_with_usdc(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("buyOusdWithUsdc", [
        Web3.to_wei(10000, "ether")
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_flipper_flip_to_usdc(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("sellOusdForUsdc", [
        Web3.to_wei(10000, "ether")
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_flipper_flip_with_usdt(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("buyOusdWithUsdt", [
        Web3.to_wei(10000, "ether"),
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_flipper_flip_to_usdt(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("sellOusdForUsdt", [
        Web3.to_wei(10000, "ether")
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_flipper_flip_with_dai(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("buyOusdWithDai", [
        Web3.to_wei(10000, "ether")
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_flipper_flip_to_dai(backend, firmware, navigator, test_name):
    data = contract_ousd_flipper.encodeABI("sellOusdForDai", [
        Web3.to_wei(10000, "ether")
    ])

    run_test(
        contract_ousd_flipper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )
