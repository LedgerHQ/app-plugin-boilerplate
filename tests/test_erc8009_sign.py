"""Positive ragger tests (Stax): the 14 selectors, the review-matrix cases, 29 pairs, legacy tx,
unsupported selector and the real Sepolia calldatas.

Every positive test binds the plugin (E0 12), provides the ERC-20 descriptors (E0 0A) when the
selector has no meta, signs with Blind signing OFF, snapshots every review screen up to
"Hold to sign", checks that the screens show exactly the pairs of the reference model
and that the signature recovers to the Speculos wallet."""

import json
from pathlib import Path

import pytest
from eth_utils import to_checksum_address

from . import erc8009_crafter as crafter
from . import helpers as h
from .erc8009_crafter import Approval, Balance, Call, Meta, Permit

S = h.SIGNER
USDC = crafter.USDC
NATIVE = crafter.NATIVE
WETH = to_checksum_address("0xfff9976782d46cc05630d1f6ebab18b2324d6b14")
LINK = to_checksum_address("0x779877a7b0d9e8603169ddbd7836e478b4624789")
EURC = to_checksum_address("0x08210f9170f89ab7658f0b5e3ff39b0e03c594d4")
OTHER = to_checksum_address("0xa11ce00000000000000000000000000000000001")
ROUTER = crafter.UNISWAP_ROUTER
TOKENS = {USDC: ("USDC", 6), WETH: ("WETH", 18), LINK: ("LINK", 18), EURC: ("EURC", 6)}
USDC_ONLY = {USDC: TOKENS[USDC]}
PERMIT = Permit(2**40, 27, b"\x11" * 32, b"\x22" * 32)
SEPOLIA_DIR = Path(__file__).parent / "fixtures" / "sepolia"
SEPOLIA_FIXTURES = sorted(SEPOLIA_DIR.glob("*.json"))


def _tokens_for(call: Call):
    return None if call.is_meta else USDC_ONLY


def _sign(backend, navigator, name, call, tokens=None, legacy=False):
    return h.sign_and_compare(backend, navigator, name, crafter.scenario(call, legacy),
                              tokens=tokens)


def _approval(token, amount, use_transfer=False):
    return Approval(Balance(ROUTER, token, amount), use_transfer)


class TestSignSelectors:

    def _canonical(self, backend, navigator, test_name, function):
        call = crafter.canonical_call(function)
        _sign(backend, navigator, test_name, call, _tokens_for(call))

    def test_sign_proxy_call(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "proxyCall")

    def test_sign_proxy_call_diffs(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "proxyCallDiffs")

    def test_sign_approve_proxy_call(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "approveProxyCall")

    def test_sign_approve_proxy_call_diffs(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "approveProxyCallDiffs")

    def test_sign_approve_proxy_call_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "approveProxyCallWithMeta")

    def test_sign_approve_proxy_call_diffs_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "approveProxyCallDiffsWithMeta")

    def test_sign_permit_proxy_call(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "permitProxyCall")

    def test_sign_permit_proxy_call_diffs(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "permitProxyCallDiffs")

    def test_sign_permit_proxy_call_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "permitProxyCallWithMeta")

    def test_sign_permit_proxy_call_diffs_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "permitProxyCallDiffsWithMeta")

    def test_sign_safe_post_balances(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "safeExecuteWithPostBalances")

    def test_sign_safe_diffs(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "safeExecuteWithDiffs")

    def test_sign_safe_post_balances_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "safeExecuteWithPostBalancesMeta")

    def test_sign_safe_diffs_meta(self, backend, navigator, test_name):
        self._canonical(backend, navigator, test_name, "safeExecuteWithDiffsMeta")


class TestSignMatrix:

    def test_matrix_nonmeta_unknown_token(self, backend, navigator, test_name):
        _sign(backend, navigator, test_name, crafter.canonical_call("approveProxyCallDiffs"))

    def test_matrix_three_erc20(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffs")
        call.guards = [Balance(S, USDC, 2_500_000), Balance(S, WETH, -5 * 10**17),
                       Balance(S, LINK, 10 * 10**18)]
        _sign(backend, navigator, test_name, call,
              {USDC: TOKENS[USDC], WETH: TOKENS[WETH], LINK: TOKENS[LINK]})

    def test_matrix_single_guard_no_counters(self, backend, navigator, test_name):
        _sign(backend, navigator, test_name, crafter.spec_example_call())

    def test_matrix_eight_guards(self, backend, navigator, test_name):
        call = crafter.canonical_call("safeExecuteWithDiffsMeta")
        safe = call.safe
        rows = [(safe, NATIVE, -10**16, "ETH", 18), (safe, USDC, 1_000_000, "USDC", 6),
                (safe, WETH, 0, "WETH", 18), (OTHER, USDC, -2_000_000, "USDC", 6),
                (safe, LINK, 5 * 10**18, "LINK", 18), (safe, EURC, -123_456, "EURC", 6),
                (S, NATIVE, 10**15, "ETH", 18), (safe, USDC, 7_000_000, "USDC", 6)]
        call.guards = [Balance(t, tok, amt) for t, tok, amt, _, _ in rows]
        call.meta = [Meta(sym, dec) for _, _, _, sym, dec in rows]
        _sign(backend, navigator, test_name, call)

    def test_matrix_data_4kb(self, backend, navigator, test_name):
        call = crafter.canonical_call("proxyCall")
        call.data = bytes(i % 251 for i in range(4096))
        _sign(backend, navigator, test_name, call, USDC_ONLY)

    def test_matrix_other_account(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallWithMeta")
        call.guards[1].target = OTHER
        _sign(backend, navigator, test_name, call)

    def test_matrix_safe_target_is_safe(self, backend, navigator, test_name):
        _sign(backend, navigator, test_name,
              crafter.canonical_call("safeExecuteWithPostBalancesMeta"))

    def test_matrix_safe_target_not_safe(self, backend, navigator, test_name):
        call = crafter.canonical_call("safeExecuteWithPostBalancesMeta")
        call.guards[0].target = S
        call.guards[1].target = OTHER
        _sign(backend, navigator, test_name, call)

    def test_matrix_pull_diffs_one(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        call.approvals = [_approval(USDC, 3_000_000)]
        _sign(backend, navigator, test_name, call)

    def test_matrix_pull_diffs_two(self, backend, navigator, test_name):
        call = crafter.canonical_call("permitProxyCallDiffs")
        call.approvals = [_approval(USDC, 3_000_000), _approval(WETH, 25 * 10**16, True)]
        call.permits = [PERMIT, PERMIT]
        _sign(backend, navigator, test_name, call, {USDC: TOKENS[USDC], WETH: TOKENS[WETH]})

    def test_matrix_unguarded_pull_one(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallWithMeta")
        call.approvals = [_approval(USDC, 3_000_000), _approval(WETH, 10**18)]
        _sign(backend, navigator, test_name, call, {WETH: TOKENS[WETH]})

    def test_matrix_unguarded_pull_two(self, backend, navigator, test_name):
        call = crafter.canonical_call("permitProxyCallWithMeta")
        call.approvals = [_approval(WETH, 10**18), _approval(LINK, 2 * 10**18, True)]
        call.permits = [PERMIT, PERMIT]
        _sign(backend, navigator, test_name, call, {WETH: TOKENS[WETH], LINK: TOKENS[LINK]})

    def test_matrix_post_negative_abs(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallWithMeta")
        call.guards[1].amount = -5_000_000
        _sign(backend, navigator, test_name, call)

    def test_matrix_diff_zero(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        call.guards[1].amount = 0
        _sign(backend, navigator, test_name, call)

    def test_max_pairs(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        tokens = [(USDC, "USDC", 6), (WETH, "WETH", 18), (LINK, "LINK", 18), (EURC, "EURC", 6)]
        call.guards, call.meta = [], []
        for i in range(8):
            token, symbol, decimals = tokens[i % 4]
            target = to_checksum_address(f"0x{i + 1:02x}" + "b0b" * 12 + f"{i + 1:02x}")
            call.guards.append(Balance(target, token, (-1) ** i * (i + 1) * 10**(decimals - 1)))
            call.meta.append(Meta(symbol, decimals))
        call.approvals = [_approval(USDC, 10**6), _approval(WETH, 10**18, True)]
        sc = crafter.scenario(call)
        assert len(h.expected_pairs(sc)) == 29
        h.sign_and_compare(backend, navigator, test_name, sc)


class TestSignOther:

    def test_legacy_tx_ok(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffsWithMeta")
        _sign(backend, navigator, test_name, call, legacy=True)

    def test_unknown_selector(self, backend, navigator, test_name):
        m = crafter.mutate("unknown_selector", crafter.canonical_call("safeExecuteWithDiffsMeta"))
        assert m.scenario.calldata[:4] == crafter.EXECUTE_SAFE_TRANSACTION_SELECTOR
        res = h.model_decision(m.scenario)
        assert (res["outcome"], res["reason"]) == ("UNAVAILABLE", "U_SELECTOR")
        h.ensure_blind_signing(backend, navigator, True)
        mark = h.log_mark(backend)
        h.blind_sign(backend, navigator, test_name, m.scenario)
        assert h.plugin_reasons(backend, mark) == [h.MODEL.REASONS["U_SELECTOR"][0]]


def _load_fixture(path: Path):
    fx = json.loads(path.read_text())
    sc = crafter.Scenario(bytes.fromhex(fx["input"][2:]), fx["to"], int(fx["value"]),
                          chain_id=int(fx["chainId"]))
    return fx, sc


class TestSepoliaFixtures:
    """Real Sepolia calldatas (read-only fetched). Signed by the Speculos wallet (fixed
    nonce/fees), so `Other account` is computed for that signer."""

    @pytest.mark.parametrize("fixture", SEPOLIA_FIXTURES, ids=[p.stem for p in SEPOLIA_FIXTURES])
    def test_sepolia_fixture(self, backend, navigator, fixture):
        fx, sc = _load_fixture(fixture)
        res = h.model_decision(sc)
        assert (res["outcome"], res["reason"]) == (fx["model"]["outcome"], fx["model"]["reason"])
        if res["outcome"] == "OK":
            h.sign_and_compare(backend, navigator, f"test_sepolia_fixture_{fixture.stem}", sc)
        else:
            h.expect_plugin_error(backend, navigator, sc, res["reason"])
