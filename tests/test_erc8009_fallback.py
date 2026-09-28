"""FALLBACK ragger tests: 0 guards and 9 guards. Blind signing OFF: "This transaction cannot
be clear-signed" + 0x6A80. Blind signing ON: the app's generic blind-signing review (never an
empty plugin review)."""

from eth_utils import to_checksum_address
from ragger.navigator import NavInsID

from . import erc8009_crafter as crafter
from . import helpers as h
from .erc8009_crafter import Balance

S = h.SIGNER
USDC = crafter.USDC
WETH = to_checksum_address("0xfff9976782d46cc05630d1f6ebab18b2324d6b14")


def _fallback(mutator, function):
    m = crafter.mutate(mutator, crafter.canonical_call(function))
    res = h.model_decision(m.scenario)
    assert (res["outcome"], res["reason"]) == ("FALLBACK", m.expected), res
    return m.scenario


def _fallback_off(backend, navigator, name, sc):
    h.ensure_blind_signing(backend, navigator, False)
    h.set_plugin(backend, sc)
    idx, count, sw = h.send_sign_expect_sw(backend, navigator, sc.tx())
    assert (idx, sw) == (count - 1, h.SW_PLUGIN_ERROR), f"SW 0x{sw:04X} on {idx + 1}/{count}"
    h.wait_for_live_text(backend, h.CANNOT_CLEAR_SIGN)
    h.sync_on_text(backend, "^This transaction")
    navigator.navigate_and_compare(h.ROOT_SCREENSHOT_PATH, name,
                                   [NavInsID.USE_CASE_CHOICE_REJECT],
                                   screen_change_before_first_instruction=False)
    h.assert_home_screen(backend)


def _fallback_on(backend, navigator, name, sc, tokens=None):
    h.ensure_blind_signing(backend, navigator, True)
    h.blind_sign(backend, navigator, name, sc, tokens=tokens)


class TestFallback:

    def test_fallback_empty_off(self, backend, navigator, test_name):
        _fallback_off(backend, navigator, test_name,
                      _fallback("fallback_empty", "approveProxyCallDiffsWithMeta"))

    def test_fallback_empty_on(self, backend, navigator, test_name):
        _fallback_on(backend, navigator, test_name,
                     _fallback("fallback_empty", "approveProxyCallDiffsWithMeta"))

    def test_fallback_nine_off(self, backend, navigator, test_name):
        _fallback_off(backend, navigator, test_name,
                      _fallback("fallback_nine", "safeExecuteWithDiffsMeta"))

    def test_fallback_nine_on(self, backend, navigator, test_name):
        _fallback_on(backend, navigator, test_name,
                     _fallback("fallback_nine", "safeExecuteWithDiffsMeta"))

    def test_fallback_with_lookup_stays_fallback(self, backend, navigator, test_name):
        call = crafter.canonical_call("approveProxyCallDiffs")
        call.guards = [Balance(S, USDC if i % 2 else WETH, (i + 1) * 10**6) for i in range(9)]
        sc = crafter.scenario(call)
        res = h.model_decision(sc, lookups={USDC: ("USDC", 6), WETH: ("WETH", 18)})
        assert (res["outcome"], res["reason"], res["lookups"]) == ("FALLBACK", "F_TOO_MANY", [])
        _fallback_on(backend, navigator, test_name, sc,
                     tokens={USDC: ("USDC", 6), WETH: ("WETH", 18)})

    def test_fallback_then_error_is_error(self, backend, navigator):
        m = crafter.mutate("fallback_then_error_is_error",
                           crafter.canonical_call("approveProxyCallWithMeta"))
        h.expect_plugin_error(backend, navigator, m.scenario, m.expected)
