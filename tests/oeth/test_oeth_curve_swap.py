from web3 import Web3
from tests.utils import run_test, load_contract

contract_curve_pool = load_contract(
    "94b17476a93b3262d87b9a326965d1e91f9c13e7",
    "curve-pool"
)

contract_curve_router = load_contract(
    "94b17476a93b3262d87b9a326965d1e91f9c13e7",
    "curve-router"
)

def test_oeth_curve_swap_from_oeth(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange", [
        1,
        0,
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.975, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_oeth_curve_swap_to_oeth(backend, firmware, navigator, test_name):
    data = contract_curve_pool.encodeABI("exchange", [
        0,
        1,
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.995, "ether")
    ])

    run_test(contract_curve_pool, data, backend, firmware, navigator, test_name)

def test_oeth_curve_swap_multiple_oeth_to_reth(backend, firmware, navigator, test_name):
    data = contract_curve_router.encodeABI("exchange_multiple", [
        [
            bytes.fromhex("856c4Efb76C1D1AE02e20CEB03A2A6a08b0b8dC3"),
            bytes.fromhex("94b17476a93b3262d87b9a326965d1e91f9c13e7"),
            bytes.fromhex("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"),
            bytes.fromhex("0f3159811670c117c372428d4e69ac32325e4d0f"),
            bytes.fromhex("ae78736cd615f374d3085123a210448e74fc6393"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
        ],
        [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ],
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.941, "ether"),
    ])

    run_test(contract_curve_router, data, backend, firmware, navigator, test_name)

def test_oeth_curve_swap_multiple_reth_to_oeth(backend, firmware, navigator, test_name):
    data = contract_curve_router.encodeABI("exchange_multiple", [
        [
            bytes.fromhex("ae78736cd615f374d3085123a210448e74fc6393"),
            bytes.fromhex("0f3159811670c117c372428d4e69ac32325e4d0f"),
            bytes.fromhex("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"),
            bytes.fromhex("94b17476a93b3262d87b9a326965d1e91f9c13e7"),
            bytes.fromhex("856c4Efb76C1D1AE02e20CEB03A2A6a08b0b8dC3"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
        ],
        [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ],
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.941, "ether"),
    ])

    run_test(contract_curve_router, data, backend, firmware, navigator, test_name)

def test_oeth_curve_swap_multiple_eth_to_oeth(backend, firmware, navigator, test_name):
    data = contract_curve_router.encodeABI("exchange_multiple", [
        [
            bytes.fromhex("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"),
            bytes.fromhex("94b17476a93b3262d87b9a326965d1e91f9c13e7"),
            bytes.fromhex("856c4Efb76C1D1AE02e20CEB03A2A6a08b0b8dC3"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
        ],
        [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ],
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.941, "ether"),
    ])

    run_test(contract_curve_router, data, backend, firmware, navigator, test_name)

def test_oeth_curve_swap_multiple_oeth_to_eth(backend, firmware, navigator, test_name):
    data = contract_curve_router.encodeABI("exchange_multiple", [
        [
            bytes.fromhex("856c4Efb76C1D1AE02e20CEB03A2A6a08b0b8dC3"),
            bytes.fromhex("94b17476a93b3262d87b9a326965d1e91f9c13e7"),
            bytes.fromhex("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
            bytes.fromhex("0000000000000000000000000000000000000000"),
        ],
        [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0]
        ],
        Web3.to_wei(1, "ether"),
        Web3.to_wei(0.941, "ether"),
    ])

    run_test(contract_curve_router, data, backend, firmware, navigator, test_name)