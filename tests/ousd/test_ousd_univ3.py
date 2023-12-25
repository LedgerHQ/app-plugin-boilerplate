from web3 import Web3
from tests.utils import run_test, load_contract

contract_univ3_router = load_contract(
    "e592427a0aece92de3edee1f18e0157c05861564",
    "uniswap-v3"
)

def test_ousd_univ3_swap_to_usdc(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInput", [
        [
            bytes.fromhex("2a8e1e676ec238d8a992307b495b45b3feaa5e860001f4dac17f958d2ee523a2206206994597c13d831ec7000064a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48"),
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(10000, "ether"),
            Web3.to_wei(9500, "mwei"),
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_univ3_swap_from_usdc(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInput", [
        [
            bytes.fromhex("a0b86991c6218b36c1d19d4a2e9eb0ce3606eb480001f46b175474e89094c44da98b954eedeac495271d0f0001f42a8e1e676ec238d8a992307b495b45b3feaa5e86"),
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(9500, "mwei"),
            Web3.to_wei(10000, "ether"),
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_univ3_swap_to_usdt(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInputSingle", [
        [
            bytes.fromhex("2a8e1e676ec238d8a992307b495b45b3feaa5e86"),
            bytes.fromhex("dac17f958d2ee523a2206206994597c13d831ec7"),
            500,
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(10000, "ether"),
            Web3.to_wei(9550, "mwei"),
            0
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_univ3_swap_from_usdt(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInputSingle", [
        [
            bytes.fromhex("dac17f958d2ee523a2206206994597c13d831ec7"),
            bytes.fromhex("2a8e1e676ec238d8a992307b495b45b3feaa5e86"),
            500,
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(9550, "mwei"),
            Web3.to_wei(10000, "ether"),
            0
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_univ3_swap_to_dai(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInput", [
        [
            bytes.fromhex("2a8e1e676ec238d8a992307b495b45b3feaa5e860001f4dac17f958d2ee523a2206206994597c13d831ec70000646b175474e89094c44da98b954eedeac495271d0f"),
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(10000, "ether"),
            Web3.to_wei(9500, "ether"),
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )

def test_ousd_univ3_swap_from_dai(backend, firmware, navigator, test_name):
    data = contract_univ3_router.encodeABI("exactInput", [
        [
            bytes.fromhex("6b175474e89094c44da98b954eedeac495271d0f0001f46b175474e89094c44da98b954eedeac495271d0f0001f42a8e1e676ec238d8a992307b495b45b3feaa5e86"),
            bytes.fromhex("dead0000000dead000000dead00000000000dead"),
            0,
            Web3.to_wei(9600, "ether"),
            Web3.to_wei(10000, "ether"),
        ]
    ])

    run_test(
        contract_univ3_router, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name
    )