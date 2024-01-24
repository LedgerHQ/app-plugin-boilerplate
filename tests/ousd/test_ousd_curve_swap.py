from web3 import Web3
from tests.utils import run_test, load_contract

contract_curve_pool = load_contract(
    "87650D7bbfC3A9F10587d7778206671719d9910D",
    "ousd-curve-pool"
)

def test_ousd_curve_swap_from_dai(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        1,
        0,
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9750, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_ousd_curve_swap_to_dai(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        0,
        1,
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9750, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_ousd_curve_swap_from_usdt(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        3,
        0,
        Web3.to_wei(10000, "mwei"),
        Web3.to_wei(9750, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_ousd_curve_swap_to_usdt(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        0,
        3,
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9750, "mwei")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_ousd_curve_swap_from_usdc(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        2,
        0,
        Web3.to_wei(10000, "mwei"),
        Web3.to_wei(9750, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_ousd_curve_swap_to_usdc(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange_underlying", [
        0,
        2,
        Web3.to_wei(10000, "ether"),
        Web3.to_wei(9750, "mwei")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)
