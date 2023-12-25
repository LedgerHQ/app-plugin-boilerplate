from web3 import Web3
from tests.utils import run_test, load_contract

contract_oeth_zapper = load_contract(
    "9858e47bcbbe6fbac040519b02d7cd4b2c470c66",
    "zapper"
)

def test_oeth_zapper_deposit_eth(backend, firmware, navigator, test_name):
    data = contract_oeth_zapper.encodeABI("deposit", [])

    run_test(
        contract_oeth_zapper, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name,
        value=Web3.to_wei(1, "ether")
    )

def test_oeth_zapper_deposit_sfrxeth(backend, firmware, navigator, test_name):
    data = contract_oeth_zapper.encodeABI("depositSFRXETH", [
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.991, "ether")
    ])

    run_test(contract_oeth_zapper, data, backend, firmware, navigator, test_name)
