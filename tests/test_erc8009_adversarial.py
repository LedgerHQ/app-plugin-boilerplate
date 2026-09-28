"""Adversarial ragger tests: the screens must reveal what the contracts actually do given their
execution order, even when the guards look reassuring."""

from eth_utils import to_checksum_address

from . import erc8009_crafter as crafter
from . import helpers as h
from .erc8009_crafter import Approval, Balance, Permit

S = h.SIGNER
USDC = crafter.USDC
ROUTER = crafter.UNISWAP_ROUTER
WETH = to_checksum_address("0xfff9976782d46cc05630d1f6ebab18b2324d6b14")
PERMIT = Permit(2**40, 27, b"\x11" * 32, b"\x22" * 32)


def _sign(backend, navigator, name, call, tokens=None):
    sc = crafter.scenario(call)
    pages = h.sign_and_compare(backend, navigator, name, sc, tokens=tokens)
    return h.review_text(pages)


class TestAdversarial:

    def test_adv_value_vs_eth_guard(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        call.value = 10**18
        text = _sign(backend, navigator, test_name, call)
        assert "Send1ETH" in text and "Minchange1/2-0.0011ETH" in text

    def test_adv_covered_pull_shown_in_diffs(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        call.approvals = [Approval(Balance(ROUTER, USDC, 100_000_000), False)]
        text = _sign(backend, navigator, test_name, call)
        assert "Pull100USDC" in text and "Minchange2/2+2.5USDC" in text

    def test_adv_pull_exceeds_guard(self, backend, navigator, test_name):
        call = crafter.canonical_call("permitProxyCallDiffsWithMeta")
        call.guards[1].amount = -1_000_000
        call.approvals = [Approval(Balance(ROUTER, USDC, 500_000_000), True)]
        call.permits = [PERMIT]
        text = _sign(backend, navigator, test_name, call)
        assert "Pull500USDC" in text and "Minchange2/2-1USDC" in text

    def test_adv_balanceproxy_no_pulls(self, backend, navigator, test_name):
        call = crafter.canonical_call("proxyCallDiffs")
        call.approvals = [Approval(Balance(ROUTER, USDC, 100_000_000), False),
                          Approval(Balance(ROUTER, WETH, 10**18), True)]
        text = _sign(backend, navigator, test_name, call, {USDC: ("USDC", 6)})
        assert "Pull" not in text
