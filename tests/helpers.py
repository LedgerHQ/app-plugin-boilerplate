"""Shared helpers of the ERC8009 ragger tests (Stax, Speculos)."""

from __future__ import annotations

import json
import re
import time
from contextlib import contextmanager
from pathlib import Path
from typing import Dict, Iterator, List, Optional, Sequence, Tuple

import requests
from elftools.elf.elffile import ELFFile
from ledger_app_clients.ethereum import response_parser as ResponseParser
from ledger_app_clients.ethereum.client import EthAppClient, SignMode
from ledger_app_clients.ethereum.settings import SettingID, settings_toggle
from ledger_app_clients.ethereum.utils import recover_transaction
from ragger.backend import BackendInterface
from ragger.error import ExceptionRAPDU
from ragger.navigator import Navigator, NavInsID

from . import erc8009_crafter as crafter
from . import erc8009_model as MODEL
from .utils import DERIVATION_PATH, get_appname_from_makefile

TESTS_DIR = Path(__file__).resolve().parent
ROOT_SCREENSHOT_PATH = TESTS_DIR
PLUGIN_NAME = get_appname_from_makefile()
SIGNER = crafter.SPECULOS_ADDRESS

SW_OK = 0x9000
SW_PLUGIN_ERROR = 0x6A80
SW_REJECTED = 0x6985
APP_FLAG_BLIND_SIGNING = 0x01

HOME_SCREEN = ["Ethereum", "This app enables signing", "transactions on the Ethereum",
               "network.", "Quit app"]
HOLD_TO_SIGN = "Hold to sign"
REVIEW_START = "^Review transaction"
BLIND_WARNING = "Blind signing ahead"
CANNOT_CLEAR_SIGN = "This transaction cannot be clear-signed"
SCREEN_TIMEOUT = 10.0
_FOOTER = re.compile(r"^(Reject|\d+ of \d+)$")
_REASON_LINE = re.compile(rb"ERC8009 E:(\d+) off=(\d+)")


def model_decision(sc: crafter.Scenario, *, signer: str = SIGNER,
                   lookups: Optional[Dict[str, Tuple[str, int]]] = None) -> dict:
    return MODEL.run(signer=signer, lookups=lookups, **sc.model_kwargs())


def expected_pairs(sc: crafter.Scenario, *, signer: str = SIGNER,
                   lookups: Optional[Dict[str, Tuple[str, int]]] = None) -> List[List[str]]:
    res = model_decision(sc, signer=signer, lookups=lookups)
    assert res["outcome"] == "OK", f"the model does not accept the scenario: {res}"
    return res["pairs"]


def review_title(sc: crafter.Scenario) -> str:
    version = MODEL.VERSION_BY_FAMILY[crafter.FUNCTIONS_BY_SELECTOR[sc.calldata[:4].hex()].family]
    return f"Review transaction to {version.lower()} on {MODEL.PLUGIN_NAME}"


def _nvram_file_range(elf_path: Path) -> Tuple[int, int]:
    with open(elf_path, "rb") as fh:
        elf = ELFFile(fh)
        symtab = elf.get_section_by_name(".symtab")
        start = symtab.get_symbol_by_name("_nvram_data")[0]["st_value"]
        end = (symtab.get_symbol_by_name("_envram_data")
               or symtab.get_symbol_by_name("_envram"))[0]["st_value"]
        for seg in elf.iter_segments():
            vaddr = seg["p_vaddr"]
            if seg["p_type"] == "PT_LOAD" and vaddr <= start and end <= vaddr + seg["p_filesz"]:
                return seg["p_offset"] + start - vaddr, end - start
    raise AssertionError(f"NVRAM of {elf_path} is not backed by file bytes")


def sync_speculos_nvram(backend: BackendInterface) -> None:
    instance = backend._client  # pylint: disable=protected-access
    nvram = instance.erc8009_run_dir / "main_nvram.bin"
    if not nvram.exists():
        return
    offset, size = _nvram_file_range(instance.erc8009_main_elf)
    image = nvram.read_bytes()
    assert len(image) <= size, f"{nvram} larger than the NVRAM ({len(image)} > {size})"
    with open(instance.erc8009_main_elf, "r+b") as fh:
        fh.seek(offset)
        fh.write(image.ljust(size, b"\x00"))


def current_screen_texts(backend: BackendInterface) -> List[str]:
    with requests.get(f"{backend.url}/events?currentscreenonly=true", timeout=5) as rsp:
        rsp.raise_for_status()
        events = rsp.json().get("events", [])
    return [e["text"].strip() for e in events if e.get("text", "").strip()]


def wait_for_live_text(backend: BackendInterface, text: str,
                       timeout: float = SCREEN_TIMEOUT) -> List[str]:
    deadline = time.time() + timeout
    texts: List[str] = []
    while time.time() < deadline:
        texts = current_screen_texts(backend)
        if _norm(text) in "".join(_norm(t) for t in texts):
            return texts
        time.sleep(0.2)
    raise AssertionError(f"'{text}' not on the live screen after {timeout}s: {texts}")


def sync_on_text(backend: BackendInterface, pattern: str,
                 timeout: float = SCREEN_TIMEOUT) -> None:
    backend.wait_for_text_on_screen(pattern, timeout)


def assert_home_screen(backend: BackendInterface, timeout: float = SCREEN_TIMEOUT) -> None:
    deadline = time.time() + timeout
    texts: List[str] = []
    while time.time() < deadline:
        texts = current_screen_texts(backend)
        if texts == HOME_SCREEN:
            return
        time.sleep(0.2)
    raise AssertionError(f"not the Ethereum home screen after {timeout}s: {texts}")


def app_flags(backend: BackendInterface) -> int:
    rapdu = EthAppClient(backend).get_app_configuration()
    assert rapdu.status == SW_OK and len(rapdu.data) == 4, rapdu
    return rapdu.data[0]


def blind_signing_enabled(backend: BackendInterface) -> bool:
    return bool(app_flags(backend) & APP_FLAG_BLIND_SIGNING)


def ensure_blind_signing(backend: BackendInterface, navigator: Navigator, enabled: bool) -> None:
    assert_home_screen(backend)
    if blind_signing_enabled(backend) != enabled:
        settings_toggle(backend.device, navigator, [SettingID.BLIND_SIGNING])
        assert_home_screen(backend)
        sync_speculos_nvram(backend)
    flags = app_flags(backend)
    assert bool(flags & APP_FLAG_BLIND_SIGNING) == enabled, \
        f"Blind signing is not {'ON' if enabled else 'OFF'} (flags 0x{flags:02x})"


def _addr_bytes(address: str) -> bytes:
    return bytes.fromhex(address[2:] if address.startswith("0x") else address)


def set_plugin(backend: BackendInterface, sc: crafter.Scenario) -> None:
    EthAppClient(backend).set_external_plugin(PLUGIN_NAME, _addr_bytes(sc.to), sc.calldata[:4])


def provide_tokens(backend: BackendInterface,
                   tokens: Optional[Dict[str, Tuple[str, int]]]) -> None:
    client = EthAppClient(backend)
    for address, (ticker, decimals) in (tokens or {}).items():
        client.provide_token_metadata(ticker, _addr_bytes(address), decimals, crafter.CHAIN_ID)


def sign_chunks(backend: BackendInterface, tx: dict) -> List[bytes]:
    client = EthAppClient(backend)
    encoded, _ = client.serialize_tx(tx)
    return client._cmd_builder.sign(DERIVATION_PATH, encoded,  # pylint: disable=protected-access
                                    SignMode.BASIC)


def assert_signed_by_wallet(tx: dict, response_data: bytes) -> None:
    vrs = ResponseParser.signature(response_data)
    assert recover_transaction(tx, vrs) == _addr_bytes(SIGNER), "signature does not recover"


@contextmanager
def recorded_screens(backend: BackendInterface) -> Iterator[Dict[int, List[str]]]:
    pages: Dict[int, List[str]] = {}
    original = backend.compare_screen_with_snapshot

    def spy(golden_snap_path, crop=None, tmp_snap_path=None, golden_run=False):
        result = original(golden_snap_path, crop, tmp_snap_path=tmp_snap_path,
                          golden_run=golden_run)
        path = Path(tmp_snap_path) if tmp_snap_path is not None else None
        if path is not None and path.parent.parent.name == backend.device.name \
                and path.stem.isdigit():
            pages[int(path.stem)] = current_screen_texts(backend)
        return result

    backend.compare_screen_with_snapshot = spy
    try:
        yield pages
    finally:
        del backend.compare_screen_with_snapshot


def _norm(text: str) -> str:
    return re.sub(r"\s+", "", text)


def review_text(pages: Dict[int, List[str]]) -> str:
    return "".join(_norm(t) for idx in sorted(pages) for t in pages[idx]
                   if not _FOOTER.match(t))


def assert_pairs_on_screens(pages: Dict[int, List[str]], pairs: Sequence[Sequence[str]]) -> None:
    text = review_text(pages)
    head = _norm("From" + SIGNER)
    start = text.find(head)
    assert start >= 0, f"'From {SIGNER}' not found in the review: {text}"
    start += len(head)
    end = text.find("Network", start)
    assert end >= 0, f"'Network' not found after From: {text}"
    shown = text[start:end]
    want = "".join(_norm(t) + _norm(v) for t, v in pairs)
    assert shown == want, f"screens differ from the model:\n shown: {shown}\n model: {want}"


def sign_and_compare(backend: BackendInterface, navigator: Navigator, snap_name: str,
                     sc: crafter.Scenario, *,
                     tokens: Optional[Dict[str, Tuple[str, int]]] = None,
                     tx: Optional[dict] = None) -> Dict[int, List[str]]:
    pairs = expected_pairs(sc, lookups=tokens)
    tx = tx or sc.tx()
    ensure_blind_signing(backend, navigator, False)
    set_plugin(backend, sc)
    provide_tokens(backend, tokens)
    client = EthAppClient(backend)
    with recorded_screens(backend) as pages:
        with client.sign(DERIVATION_PATH, tx):
            sync_on_text(backend, REVIEW_START)
            navigator.navigate_until_text_and_compare(
                NavInsID.SWIPE_CENTER_TO_LEFT,
                [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS],
                HOLD_TO_SIGN, ROOT_SCREENSHOT_PATH, snap_name,
                screen_change_before_first_instruction=False)
    _save_screens(backend, snap_name, {"title": review_title(sc), "model_pairs": pairs,
                                       "pages": pages})
    assert_signed_by_wallet(tx, client.response().data)
    first = "".join(_norm(t) for t in pages.get(0, []))
    assert first.startswith(_norm(review_title(sc))), f"review title: {pages.get(0)}"
    assert_pairs_on_screens(pages, pairs)
    assert_home_screen(backend)
    return pages


def _save_screens(backend: BackendInterface, snap_name: Optional[str], data: dict) -> None:
    if not snap_name:
        return
    out = ROOT_SCREENSHOT_PATH / "snapshots-tmp" / backend.device.name / snap_name
    out.mkdir(parents=True, exist_ok=True)
    (out / "screens.json").write_text(json.dumps(data, indent=1, sort_keys=True))


def blind_sign(backend: BackendInterface, navigator: Navigator, snap_name: Optional[str],
               sc: crafter.Scenario, *, bind: bool = True,
               tokens: Optional[Dict[str, Tuple[str, int]]] = None) -> Dict[int, List[str]]:
    tx = sc.tx()
    if bind:
        set_plugin(backend, sc)
    provide_tokens(backend, tokens)
    assert blind_signing_enabled(backend), "Blind signing reverted before signing"
    client = EthAppClient(backend)
    path = ROOT_SCREENSHOT_PATH if snap_name else None
    with recorded_screens(backend) as pages:
        with client.sign(DERIVATION_PATH, tx):
            sync_on_text(backend, BLIND_WARNING)
            pages[-1] = current_screen_texts(backend)
            navigator.navigate_and_compare(path, snap_name, [NavInsID.USE_CASE_CHOICE_REJECT],
                                           screen_change_before_first_instruction=False,
                                           screen_change_after_last_instruction=False)
            navigator.navigate_until_text_and_compare(
                NavInsID.SWIPE_CENTER_TO_LEFT,
                [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS],
                HOLD_TO_SIGN, path, snap_name, snap_start_idx=1)
    _save_screens(backend, snap_name, {"blind": True, "pages": pages})
    assert_signed_by_wallet(tx, client.response().data)
    text = "".join(_norm(t) for t in pages.get(-1, []))
    assert _norm(BLIND_WARNING) in text, f"no blind-signing warning: {pages.get(-1)}"
    assert "ERC-8009" not in review_text(pages), "plugin screens in a blind review"
    assert_home_screen(backend)
    return pages


def assert_blind_review_offered(backend: BackendInterface, navigator: Navigator,
                                sc: crafter.Scenario) -> None:
    ensure_blind_signing(backend, navigator, True)
    assert model_decision(sc)["outcome"] == "FALLBACK"
    set_plugin(backend, sc)
    assert blind_signing_enabled(backend), "Blind signing reverted by the plugin binding"
    client = EthAppClient(backend)
    try:
        with client.sign(DERIVATION_PATH, sc.tx()):
            wait_for_live_text(backend, BLIND_WARNING)
            navigator.navigate([NavInsID.USE_CASE_CHOICE_CONFIRM],
                               screen_change_before_first_instruction=False,
                               screen_change_after_last_instruction=False)
    except ExceptionRAPDU as err:
        assert err.status == SW_REJECTED, f"SW 0x{err.status:04X}"
    else:
        raise AssertionError("the blind review was not refused")
    sync_on_text(backend, "Transaction rejected")
    navigator.navigate([NavInsID.USE_CASE_STATUS_DISMISS],
                       screen_change_before_first_instruction=False)
    assert_home_screen(backend)


def log_mark(backend: BackendInterface) -> int:
    return backend._client.erc8009_log_path.stat().st_size  # pylint: disable=protected-access


def plugin_reasons(backend: BackendInterface, mark: int, wait: float = 3.0) -> List[int]:
    path = backend._client.erc8009_log_path  # pylint: disable=protected-access
    deadline = time.time() + wait
    while True:
        with open(path, "rb") as fh:
            fh.seek(mark)
            codes = [int(m.group(1)) for m in _REASON_LINE.finditer(fh.read())]
        if codes or time.time() > deadline:
            return codes
        time.sleep(0.1)


def send_sign_expect_sw(backend: BackendInterface, navigator: Navigator, tx: dict,
                        expected_sw: int = SW_PLUGIN_ERROR,
                        wait: float = 5.0) -> Tuple[int, int, int]:
    chunks = sign_chunks(backend, tx)
    for idx, chunk in enumerate(chunks[:-1]):
        try:
            backend.exchange_raw(chunk)
        except ExceptionRAPDU as err:
            return idx, len(chunks), err.status
    try:
        with backend.exchange_async_raw(chunks[-1]):
            deadline = time.time() + wait
            while backend.last_async_response is None and time.time() < deadline:
                backend._check_async_error()  # pylint: disable=protected-access
                time.sleep(0.1)
            if backend.last_async_response is None:
                texts = current_screen_texts(backend)
                _leave_screen(navigator, texts)
                raise AssertionError(f"expected SW 0x{expected_sw:04X}; the device displayed "
                                     f"{texts} instead")
    except ExceptionRAPDU as err:
        return len(chunks) - 1, len(chunks), err.status
    return len(chunks) - 1, len(chunks), SW_OK


def _leave_screen(navigator: Navigator, texts: List[str]) -> None:
    joined = "".join(_norm(t) for t in texts)
    if _norm(BLIND_WARNING) in joined:
        moves = [NavInsID.USE_CASE_CHOICE_CONFIRM]
    elif _norm(CANNOT_CLEAR_SIGN) in joined:
        moves = [NavInsID.USE_CASE_CHOICE_REJECT]
    else:
        moves = [NavInsID.USE_CASE_REVIEW_REJECT, NavInsID.USE_CASE_CHOICE_CONFIRM]
    try:
        navigator.navigate(moves, screen_change_before_first_instruction=False,
                           screen_change_after_last_instruction=False)
    except Exception:  # pylint: disable=broad-except
        pass


def expect_plugin_error(backend: BackendInterface, navigator: Navigator,
                        sc: crafter.Scenario, expected_reason: str) -> Tuple[int, int]:
    ensure_blind_signing(backend, navigator, True)
    res = model_decision(sc)
    assert (res["outcome"], res["reason"]) == ("ERROR", expected_reason), res
    set_plugin(backend, sc)
    assert blind_signing_enabled(backend), "Blind signing reverted by the plugin binding"
    mark = log_mark(backend)
    idx, count, sw = send_sign_expect_sw(backend, navigator, sc.tx())
    assert sw == SW_PLUGIN_ERROR, f"SW 0x{sw:04X} on chunk {idx + 1}/{count}"
    assert_home_screen(backend)
    code = MODEL.REASONS[expected_reason][0]
    codes = plugin_reasons(backend, mark)
    assert codes and codes[0] == code, \
        f"plugin reason {codes} != {expected_reason} ({code}) (chunk {idx + 1}/{count})"
    return idx, count
