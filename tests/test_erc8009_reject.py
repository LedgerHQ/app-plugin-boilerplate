"""Negative ragger tests: every rule the plugin enforces with ERROR, built from the crafter
mutators.

Each negative test runs with Blind signing ON (set through the settings UI, re-read from
GET_APP_CONFIGURATION, re-checked after the plugin binding), expects 0x6A80 on some APDU chunk
and no signature, then checks LIVE (Speculos /events) that the device is back on the home
screen, and that the plugin printed the reason code the reference model expects. Each class
starts with a FALLBACK control: with the same settings a FALLBACK must reach the blind-signing
warning, so a 6A80 in that class cannot be a FALLBACK with Blind signing OFF."""

import pytest
from ledger_app_clients.ethereum.client import EthAppClient
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID

from . import erc8009_crafter as crafter
from . import helpers as h


def _negative(backend, navigator, mutator, function):
    m = crafter.mutate(mutator, crafter.canonical_call(function))
    assert m is not None, f"{mutator} does not apply to {function}"
    return h.expect_plugin_error(backend, navigator, m.scenario, m.expected)


def _fallback_control(backend, navigator, mutator, function):
    m = crafter.mutate(mutator, crafter.canonical_call(function))
    h.assert_blind_review_offered(backend, navigator, m.scenario)


class TestRejectTransaction:

    def test_control_fallback_blind_review(self, backend, navigator):
        _fallback_control(backend, navigator, "fallback_empty", "approveProxyCallDiffsWithMeta")

    def test_wrong_chain(self, backend, navigator):
        _negative(backend, navigator, "wrong_chain", "approveProxyCallDiffsWithMeta")

    def test_legacy_tx_wrong_chain(self, backend, navigator):
        idx, count = _negative(backend, navigator, "legacy_tx_wrong_chain",
                               "approveProxyCallDiffsWithMeta")
        assert idx == count - 1, "a legacy chain id is only known in finalize"

    def test_wrong_to(self, backend, navigator):
        _negative(backend, navigator, "wrong_to", "proxyCallDiffs")

    def test_family_mismatch(self, backend, navigator):
        _negative(backend, navigator, "family_mismatch", "permitProxyCallDiffsWithMeta")

    def test_misaligned_data(self, backend, navigator):
        _negative(backend, navigator, "misaligned_data", "safeExecuteWithDiffsMeta")

    def test_safe_value(self, backend, navigator):
        _negative(backend, navigator, "safe_value", "safeExecuteWithPostBalances")

    def test_error_intermediate_chunk(self, backend, navigator):
        m = crafter.mutate("wrong_balance_proxy",
                           crafter.canonical_call("approveProxyCallDiffsWithMeta"))
        assert len(m.scenario.calldata) > 255
        idx, count = h.expect_plugin_error(backend, navigator, m.scenario, m.expected)
        assert count >= 2 and idx < count - 1, f"6A80 on chunk {idx + 1}/{count}"

    def test_reject_6985(self, backend, navigator, test_name):
        h.ensure_blind_signing(backend, navigator, True)
        sc = crafter.scenario(crafter.canonical_call("approveProxyCallDiffsWithMeta"))
        h.set_plugin(backend, sc)
        client = EthAppClient(backend)
        with pytest.raises(ExceptionRAPDU) as err:
            with client.sign(h.DERIVATION_PATH, sc.tx()):
                # "Yes, reject" answers the APDU: no screen wait after it, so that ragger's
                # navigation is not interrupted by the 6985 (it would leave the ticker paused)
                h.sync_on_text(backend, h.REVIEW_START)
                navigator.navigate_until_text_and_compare(
                    NavInsID.SWIPE_CENTER_TO_LEFT,
                    [NavInsID.USE_CASE_REVIEW_REJECT, NavInsID.USE_CASE_CHOICE_CONFIRM],
                    h.HOLD_TO_SIGN, h.ROOT_SCREENSHOT_PATH, test_name,
                    screen_change_before_first_instruction=False,
                    screen_change_after_last_instruction=False)
        assert err.value.status == h.SW_REJECTED
        h.sync_on_text(backend, "Transaction rejected")
        navigator.navigate([NavInsID.USE_CASE_STATUS_DISMISS],
                           screen_change_before_first_instruction=False)
        h.assert_home_screen(backend)


class TestRejectLayout:

    def test_control_fallback_blind_review(self, backend, navigator):
        _fallback_control(backend, navigator, "fallback_nine", "permitProxyCallDiffsWithMeta")

    def test_wrong_balance_proxy(self, backend, navigator):
        _negative(backend, navigator, "wrong_balance_proxy", "permitProxyCallDiffsWithMeta")

    def test_offset_shift_plus32(self, backend, navigator):
        _negative(backend, navigator, "offset_shift_plus32", "approveProxyCallWithMeta")

    def test_offset_shift_minus32(self, backend, navigator):
        _negative(backend, navigator, "offset_shift_minus32", "approveProxyCallWithMeta")

    def test_offset_overlap(self, backend, navigator):
        _negative(backend, navigator, "offset_overlap", "permitProxyCall")

    def test_offset_out_of_bounds(self, backend, navigator):
        _negative(backend, navigator, "offset_out_of_bounds", "proxyCall")

    def test_offset_overflow(self, backend, navigator):
        _negative(backend, navigator, "offset_overflow", "safeExecuteWithDiffs")

    def test_padding_address(self, backend, navigator):
        _negative(backend, navigator, "padding_address", "approveProxyCallDiffs")

    def test_padding_uint8(self, backend, navigator):
        _negative(backend, navigator, "padding_uint8", "permitProxyCallDiffs")

    def test_padding_bool(self, backend, navigator):
        _negative(backend, navigator, "padding_bool", "approveProxyCallDiffsWithMeta")

    def test_padding_length(self, backend, navigator):
        _negative(backend, navigator, "padding_length", "safeExecuteWithPostBalancesMeta")

    def test_truncated(self, backend, navigator):
        _negative(backend, navigator, "truncated", "permitProxyCallWithMeta")

    def test_trailing_garbage(self, backend, navigator):
        _negative(backend, navigator, "trailing_garbage", "proxyCallDiffs")

    def test_bytes_padding(self, backend, navigator):
        _negative(backend, navigator, "bytes_padding", "approveProxyCall")

    def test_safe_operation(self, backend, navigator):
        _negative(backend, navigator, "safe_operation", "safeExecuteWithDiffsMeta")

    def test_safe_offsets(self, backend, navigator):
        _negative(backend, navigator, "safe_offsets", "safeExecuteWithPostBalancesMeta")


class TestRejectMeta:

    def test_control_fallback_blind_review(self, backend, navigator):
        _fallback_control(backend, navigator, "fallback_empty", "safeExecuteWithDiffsMeta")

    def test_meta_len_mismatch(self, backend, navigator):
        _negative(backend, navigator, "meta_len_mismatch", "approveProxyCallDiffsWithMeta")

    def test_meta_head_not_0x40(self, backend, navigator):
        _negative(backend, navigator, "meta_head_not_0x40", "permitProxyCallWithMeta")

    def test_meta_offset_table(self, backend, navigator):
        _negative(backend, navigator, "meta_offset_table", "safeExecuteWithDiffsMeta")

    def test_symbol_empty(self, backend, navigator):
        _negative(backend, navigator, "symbol_empty", "approveProxyCallWithMeta")

    def test_symbol_17_chars(self, backend, navigator):
        _negative(backend, navigator, "symbol_17_chars", "permitProxyCallDiffsWithMeta")

    def test_symbol_nonprintable(self, backend, navigator):
        _negative(backend, navigator, "symbol_nonprintable", "safeExecuteWithPostBalancesMeta")

    def test_symbol_padding(self, backend, navigator):
        _negative(backend, navigator, "symbol_padding", "approveProxyCallDiffsWithMeta")

    def test_decimals_78(self, backend, navigator):
        _negative(backend, navigator, "decimals_78", "permitProxyCallWithMeta")

    def test_native_meta_mismatch(self, backend, navigator):
        _negative(backend, navigator, "native_meta_mismatch", "safeExecuteWithDiffsMeta")


class TestRejectApprovals:

    def test_control_fallback_blind_review(self, backend, navigator):
        _fallback_control(backend, navigator, "fallback_empty", "proxyCall")

    def test_three_approvals(self, backend, navigator):
        _negative(backend, navigator, "three_approvals", "proxyCall")

    def test_negative_approval(self, backend, navigator):
        _negative(backend, navigator, "negative_approval", "approveProxyCallDiffs")

    def test_permits_len(self, backend, navigator):
        _negative(backend, navigator, "permits_len", "permitProxyCallWithMeta")
