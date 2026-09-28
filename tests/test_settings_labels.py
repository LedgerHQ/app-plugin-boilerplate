"""Snapshot of the Ethereum app settings pages on Stax, with the exact labels the docs and the
friend's instructions refer to."""

from ragger.navigator import NavInsID

from . import helpers as h

LABELS = ("Blind signing", "Raw messages", "Debug smart contracts", "Transaction Check")


def test_settings_labels(backend, navigator, test_name):
    h.assert_home_screen(backend)
    with h.recorded_screens(backend) as pages:
        navigator.navigate_and_compare(
            h.ROOT_SCREENSHOT_PATH, test_name,
            [NavInsID.USE_CASE_HOME_SETTINGS, NavInsID.USE_CASE_SETTINGS_NEXT,
             NavInsID.USE_CASE_SETTINGS_NEXT, NavInsID.USE_CASE_SETTINGS_NEXT,
             NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT],
            screen_change_before_first_instruction=False)
    shown = [t for idx in sorted(pages) for t in pages[idx]]
    for label in LABELS:
        assert label in shown, f"setting label '{label}' not displayed: {shown}"
    h.assert_home_screen(backend)
