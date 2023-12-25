from web3 import Web3
from tests.utils import run_test, load_contract

contract_ousd_vault = load_contract(
    "E75D77B1865Ae93c7eaa3040B038D7aA7BC02F70",
    "vault-core"
)

def test_ousd_vault_mint_dai(backend, firmware, navigator, test_name):
    data = contract_ousd_vault.encodeABI("mint", [
        bytes.fromhex("6b175474e89094c44da98b954eedeac495271d0f"),
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9500, "ether")
    ])

    run_test(contract_ousd_vault, data, backend, firmware, navigator, test_name)

def test_ousd_vault_mint_usdc(backend, firmware, navigator, test_name):
    data = contract_ousd_vault.encodeABI("mint", [
        bytes.fromhex("a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48"),
        Web3.to_wei(10000, "mwei"),
        Web3.to_wei(9500, "ether")
    ])

    run_test(contract_ousd_vault, data, backend, firmware, navigator, test_name)

def test_ousd_vault_mint_usdt(backend, firmware, navigator, test_name):
    data = contract_ousd_vault.encodeABI("mint", [
        bytes.fromhex("dac17f958d2ee523a2206206994597c13d831ec7"),
        Web3.to_wei(10000, "mwei"),
        Web3.to_wei(9500, "ether")
    ])

    run_test(contract_ousd_vault, data, backend, firmware, navigator, test_name)

def test_ousd_vault_redeem(backend, firmware, navigator, test_name):
    data = contract_ousd_vault.encodeABI("redeem", [
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9088, "ether")
    ])

    run_test(contract_ousd_vault, data, backend, firmware, navigator, test_name)
