from web3 import Web3
from tests.utils import run_test, load_contract

contract_oeth_vault = load_contract(
    "39254033945AA2E4809Cc2977E7087BEE48bd7Ab",
    "vault-core"
)

def test_oeth_vault_mint_weth(backend, firmware, navigator, test_name):
    data = contract_oeth_vault.encodeABI("mint", [
        bytes.fromhex("C02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2"),
        Web3.to_wei(1, "ether"),
        Web3.to_wei(1, "ether")
    ])

    run_test(contract_oeth_vault, data, backend, firmware, navigator, test_name)

def test_oeth_vault_mint_reth(backend, firmware, navigator, test_name):
    data = contract_oeth_vault.encodeABI("mint", [
        bytes.fromhex("ae78736cd615f374d3085123a210448e74fc6393"),
        Web3.to_wei(1, "ether"),
        Web3.to_wei(1, "ether")
    ])

    run_test(contract_oeth_vault, data, backend, firmware, navigator, test_name)

def test_oeth_vault_mint_steth(backend, firmware, navigator, test_name):
    data = contract_oeth_vault.encodeABI("mint", [
        bytes.fromhex("ae7ab96520de3a18e5e111b5eaab095312d7fe84"),
        Web3.to_wei(1, "ether"),
        Web3.to_wei(1, "ether")
    ])

    run_test(contract_oeth_vault, data, backend, firmware, navigator, test_name)

def test_oeth_vault_mint_frxeth(backend, firmware, navigator, test_name):
    data = contract_oeth_vault.encodeABI("mint", [
        bytes.fromhex("5e8422345238f34275888049021821e8e08caa1f"),
        Web3.to_wei(1, "ether"),
        Web3.to_wei(1, "ether")
    ])

    run_test(contract_oeth_vault, data, backend, firmware, navigator, test_name)

def test_oeth_vault_redeem(backend, firmware, navigator, test_name):
    data = contract_oeth_vault.encodeABI("redeem", [
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.88, "ether")
    ])

    run_test(contract_oeth_vault, data, backend, firmware, navigator, test_name)
