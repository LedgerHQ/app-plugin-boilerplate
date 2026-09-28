"""Reference model of the ERC8009 plugin: outcome, reason code and screens for a calldata."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Dict, List, Optional, Sequence, Tuple, Union

from eth_utils import keccak

CHAIN_ID = 11155111
BALANCE_PROXY = bytes.fromhex("800B36cc16C7572E1d600506a0d7d30890fF35f4")
FAMILY_ADDRESSES = {
    "BP": BALANCE_PROXY,
    "AR": bytes.fromhex("f3c82E2176D894ED32C5748C22238ebfcc68CDd8"),
    "PR": bytes.fromhex("fEa607c2f74D7ac4ac35360e51f3fe08E319ebA2"),
    "SR": bytes.fromhex("7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0"),
}
FAMILY_OF_ADDRESS = {addr: fam for fam, addr in FAMILY_ADDRESSES.items()}
MAX_GUARDS = 8
MAX_APPROVALS = 2
MAX_SYMBOL_LEN = 16
MAX_DECIMALS = 77
NATIVE_TICKER = b"ETH"
NATIVE_DECIMALS = 18
UNKNOWN_TICKER = b"???"
ZERO_ADDRESS = bytes(20)
TITLE_BUFFER = 43
VALUE_BUFFER = 100
TICKER_BUFFER = 51
PLUGIN_NAME = "ERC-8009"
VERSION_BY_FAMILY = {
    "BP": "execute guarded call",
    "AR": "execute guarded call",
    "PR": "execute guarded call",
    "SR": "execute guarded Safe tx",
}

OK, ERROR, FALLBACK, UNAVAILABLE = "OK", "ERROR", "FALLBACK", "UNAVAILABLE"
REASONS: Dict[str, Tuple[int, str]] = {
    "R_OK": (0, OK),
    "U_VERSION": (1, UNAVAILABLE),
    "U_SELECTOR": (2, UNAVAILABLE),
    "E_CONTEXT": (3, ERROR),
    "E_DEST": (4, ERROR),
    "E_FAMILY": (5, ERROR),
    "E_CHAIN": (6, ERROR),
    "E_DATASIZE": (7, ERROR),
    "E_SAFE_VALUE": (8, ERROR),
    "E_SEQ": (9, ERROR),
    "E_PROXY": (10, ERROR),
    "E_OFFSET": (11, ERROR),
    "E_PAD_ADDR": (12, ERROR),
    "E_PAD_UINT": (13, ERROR),
    "E_BOOL": (14, ERROR),
    "E_TRUNC": (15, ERROR),
    "E_TRAILING": (16, ERROR),
    "E_META_LEN": (17, ERROR),
    "E_META_HEAD": (18, ERROR),
    "E_SYMBOL": (19, ERROR),
    "E_DECIMALS": (20, ERROR),
    "E_NATIVE_META": (21, ERROR),
    "E_APPROVALS_MAX": (22, ERROR),
    "E_APPROVAL_NEG": (23, ERROR),
    "E_PERMITS_LEN": (24, ERROR),
    "E_SAFE_OP": (25, ERROR),
    "E_BYTES_PAD": (26, ERROR),
    "E_RENDER": (27, ERROR),
    "E_STATE": (28, ERROR),
    "F_EMPTY": (29, FALLBACK),
    "F_TOO_MANY": (30, FALLBACK),
}
REASON_BY_CODE = {code: name for name, (code, _) in REASONS.items()}

STATIC_KINDS = ("ADDR_PROXY", "ADDR_TARGET", "ADDR_SAFE")
TAIL_KINDS = ("META", "GUARDS", "APPROVALS", "PERMITS", "BYTES", "WITHDRAWALS", "SAFE_TX")


@dataclass(frozen=True)
class Schema:
    selector: str
    name: str
    family: str
    mode: str
    head: Tuple[str, ...]

    @property
    def meta(self) -> bool:
        return "META" in self.head


_BP = ("GUARDS", "APPROVALS", "ADDR_TARGET", "BYTES", "WITHDRAWALS")
_AR = ("ADDR_PROXY", "GUARDS", "APPROVALS", "ADDR_TARGET", "BYTES", "WITHDRAWALS")
_AR_META = ("ADDR_PROXY", "META", "GUARDS", "APPROVALS", "ADDR_TARGET", "BYTES", "WITHDRAWALS")
_PR = ("ADDR_PROXY", "GUARDS", "APPROVALS", "PERMITS", "ADDR_TARGET", "BYTES", "WITHDRAWALS")
_PR_META = ("ADDR_PROXY", "META", "GUARDS", "APPROVALS", "PERMITS", "ADDR_TARGET", "BYTES",
            "WITHDRAWALS")
_SR = ("ADDR_PROXY", "GUARDS", "ADDR_SAFE", "SAFE_TX")
_SR_META = ("ADDR_PROXY", "META", "GUARDS", "ADDR_SAFE", "SAFE_TX")

SCHEMAS: Dict[str, Schema] = {
    s.selector: s
    for s in (
        Schema("ce9506f9", "proxyCall", "BP", "min", _BP),
        Schema("890e3add", "proxyCallDiffs", "BP", "diffs", _BP),
        Schema("e5d77b3c", "approveProxyCall", "AR", "min", _AR),
        Schema("c6707270", "approveProxyCallDiffs", "AR", "diffs", _AR),
        Schema("2f5a71c1", "approveProxyCallWithMeta", "AR", "min", _AR_META),
        Schema("b3a7f90a", "approveProxyCallDiffsWithMeta", "AR", "diffs", _AR_META),
        Schema("029a2053", "permitProxyCall", "PR", "min", _PR),
        Schema("bf10e28e", "permitProxyCallDiffs", "PR", "diffs", _PR),
        Schema("366ffd79", "permitProxyCallWithMeta", "PR", "min", _PR_META),
        Schema("b44921d7", "permitProxyCallDiffsWithMeta", "PR", "diffs", _PR_META),
        Schema("510818c0", "safeExecuteWithPostBalances", "SR", "min", _SR),
        Schema("7ce6cc45", "safeExecuteWithDiffs", "SR", "diffs", _SR),
        Schema("e48215b2", "safeExecuteWithPostBalancesMeta", "SR", "min", _SR_META),
        Schema("4674b941", "safeExecuteWithDiffsMeta", "SR", "diffs", _SR_META),
    )
}
SCHEMA_BY_NAME = {s.name: s for s in SCHEMAS.values()}

SAFE_TX_HEAD_WORDS = 11
SAFE_TX_DATA_OFFSET = 0x160


def _adjust_decimals(src: str, decimals: int) -> str:
    if src == "0":
        return "0"
    if len(src) <= decimals:
        target = "0." + "0" * (decimals - len(src))
        start = len(target)
        target += src
    else:
        delta = len(src) - decimals
        target = src[:delta]
        if decimals != 0:
            target += "."
        start = len(target)
        target += src[delta:]
    last_zero = 0
    for i in range(start, len(target)):
        if target[i] == "0":
            if last_zero == 0:
                last_zero = i
        else:
            last_zero = 0
    if last_zero != 0:
        target = target[:last_zero]
        if target[last_zero - 1] == ".":
            target = target[: last_zero - 1]
    return target


def amount_to_string(amount: int, decimals: int, ticker: Union[str, bytes]) -> str:
    if amount < 0 or amount >= 1 << 256:
        raise ValueError("amount must be a uint256")
    if isinstance(ticker, bytes):
        ticker = ticker.decode("latin-1")
    return _adjust_decimals(str(amount), decimals) + " " + ticker


def eip55(address: Union[bytes, str]) -> str:
    raw = _addr_bytes(address)
    hexaddr = raw.hex()
    digest = keccak(hexaddr.encode("ascii"))
    out = []
    for i, ch in enumerate(hexaddr):
        nibble = (digest[i // 2] >> (4 * (1 - i % 2))) & 0x0F
        out.append(ch.upper() if ch in "abcdef" and nibble >= 8 else ch)
    return "0x" + "".join(out)


def _addr_bytes(value: Union[bytes, str, None]) -> bytes:
    if value is None:
        return b""
    if isinstance(value, str):
        v = value[2:] if value[:2] in ("0x", "0X") else value
        return bytes.fromhex(v)
    return bytes(value)


def _int_bytes(value: Union[int, bytes, str, None]) -> bytes:
    if value is None:
        return b""
    if isinstance(value, (bytes, bytearray)):
        return bytes(value)
    if isinstance(value, str):
        value = int(value, 0)
    if value < 0:
        raise ValueError("negative integer")
    return value.to_bytes((value.bit_length() + 7) // 8, "big")


class ModelStop(Exception):
    def __init__(self, reason: str, stage: str, word: Optional[int], detail: str):
        super().__init__(f"{reason} at {stage} word={word}: {detail}")
        self.reason = reason
        self.stage = stage
        self.word = word
        self.detail = detail


@dataclass
class _Guard:
    target: bytes
    token: bytes
    amount: bytes


@dataclass
class _Approval:
    token: bytes
    amount: bytes


class _Plugin:
    def __init__(self, schema: Schema, data_size: int):
        self.schema = schema
        self.data_size = data_size
        self.total_words = (data_size - 4) // 32
        self.calls = 0
        self.cur = -1
        self.done = False
        self.fallback: Optional[Tuple[str, int]] = None
        self.offsets: Dict[int, int] = {}
        self.meta: List[Tuple[bytes, int]] = []
        self.meta_len: Optional[int] = None
        self.guards: List[_Guard] = []
        self.n_guards: Optional[int] = None
        self.approvals: List[_Approval] = []
        self.n_approvals: Optional[int] = None
        self.safe: Optional[bytes] = None
        self._gen = self._parse()
        next(self._gen)

    def _err(self, reason: str, detail: str) -> ModelStop:
        return ModelStop(reason, "param", self.cur, detail)

    def _sticky(self, reason: str) -> None:
        if self.fallback is None:
            self.fallback = (reason, self.cur)

    def _address(self, w: bytes, what: str) -> bytes:
        if any(w[:12]):
            raise self._err("E_PAD_ADDR", f"{what}: non-zero upper 12 bytes")
        return w[12:]

    def _uint8(self, w: bytes, what: str) -> int:
        if any(w[:31]):
            raise self._err("E_PAD_UINT", f"{what}: non-zero upper 31 bytes")
        return w[31]

    def _length(self, w: bytes, what: str) -> int:
        if any(w[:28]):
            raise self._err("E_PAD_UINT", f"{what}: length word with non-zero upper 28 bytes")
        return int.from_bytes(w[28:], "big")

    def _offset(self, w: bytes, what: str) -> int:
        if any(w[:28]):
            raise self._err("E_OFFSET", f"{what}: offset word with non-zero upper 28 bytes")
        return int.from_bytes(w[28:], "big")

    def _bool(self, w: bytes, what: str) -> bool:
        v = int.from_bytes(w, "big")
        if v not in (0, 1):
            raise self._err("E_BOOL", f"{what}: bool word is {v:#x}")
        return v == 1

    def _bounds(self, n: int, per_element: int, what: str) -> None:
        remaining = self.total_words - (self.cur + 1)
        if n > remaining // per_element:
            raise self._err("E_TRUNC", f"{what}: {n} x {per_element} words > {remaining} left")

    def _tail_start(self, expected_offset: int, what: str) -> None:
        pos = 32 * self.cur
        if expected_offset != pos:
            raise self._err("E_OFFSET", f"{what}: offset {expected_offset:#x} != position {pos:#x}")

    def provide_parameter(self, parameter_offset: int, word: bytes) -> None:
        k = self.calls
        self.cur = k
        if parameter_offset != 4 + 32 * k:
            raise self._err("E_SEQ", f"parameterOffset {parameter_offset} != {4 + 32 * k}")
        if 4 + 32 * (k + 1) > self.data_size:
            raise self._err("E_SEQ", f"word {k} beyond dataSize {self.data_size}")
        if self.done:
            raise self._err("E_TRAILING", "word after the last tail")
        try:
            self._gen.send(word)
        except StopIteration:
            self.done = True
        self.calls = k + 1

    def _parse(self):
        head = self.schema.head
        for slot, kind in enumerate(head):
            w = yield
            if kind == "ADDR_PROXY":
                addr = self._address(w, "balanceProxy")
                if addr != BALANCE_PROXY:
                    raise self._err("E_PROXY", f"balanceProxy {addr.hex()} is not canonical")
            elif kind == "ADDR_TARGET":
                self._address(w, "target")
            elif kind == "ADDR_SAFE":
                self.safe = self._address(w, "safe")
            else:
                self.offsets[slot] = self._offset(w, f"head slot {slot} ({kind})")
        for slot, kind in enumerate(head):
            if kind in STATIC_KINDS:
                continue
            w = yield
            self._tail_start(self.offsets[slot], f"{kind} tail start")
            if kind == "META":
                yield from self._meta(w)
            elif kind == "GUARDS":
                yield from self._guards(w)
            elif kind == "APPROVALS":
                yield from self._approvals(w)
            elif kind == "PERMITS":
                yield from self._permits(w)
            elif kind == "BYTES":
                yield from self._bytes(w, "data")
            elif kind == "WITHDRAWALS":
                yield from self._withdrawals(w)
            elif kind == "SAFE_TX":
                yield from self._safe_tx(w)
            else:  # pragma: no cover - schema table is static
                raise AssertionError(kind)

    def _meta(self, w: bytes):
        n = self._length(w, "meta length")
        self._bounds(n, 5, "meta")
        if n > MAX_GUARDS:
            self._sticky("F_TOO_MANY")
        self.meta_len = n
        for i in range(n):
            w = yield
            o = self._offset(w, f"meta offset[{i}]")
            if o != 32 * n + 128 * i:
                raise self._err("E_OFFSET", f"meta offset[{i}] {o:#x} != {32 * n + 128 * i:#x}")
        for i in range(n):
            w = yield
            if int.from_bytes(w, "big") != 0x40:
                raise self._err("E_META_HEAD", f"meta[{i}] head word != 0x40")
            w = yield
            decimals = self._uint8(w, f"meta[{i}].decimals")
            if decimals > MAX_DECIMALS:
                raise self._err("E_DECIMALS", f"meta[{i}].decimals {decimals} > {MAX_DECIMALS}")
            w = yield
            ln = self._length(w, f"meta[{i}].symbol length")
            if ln == 0 or ln > MAX_SYMBOL_LEN:
                raise self._err("E_SYMBOL", f"meta[{i}].symbol length {ln}")
            w = yield
            symbol = w[:ln]
            if any(b < 0x20 or b > 0x7E for b in symbol):
                raise self._err("E_SYMBOL", f"meta[{i}].symbol has a non-printable byte")
            if any(w[ln:]):
                raise self._err("E_SYMBOL", f"meta[{i}].symbol has non-zero padding")
            if i < MAX_GUARDS:
                self.meta.append((symbol, decimals))

    def _guards(self, w: bytes):
        n = self._length(w, "guards length")
        self._bounds(n, 3, "guards")
        if self.schema.meta and n != self.meta_len:
            raise self._err("E_META_LEN", f"len(guards) {n} != len(meta) {self.meta_len}")
        if n == 0:
            self._sticky("F_EMPTY")
        elif n > MAX_GUARDS:
            self._sticky("F_TOO_MANY")
        self.n_guards = n
        for i in range(n):
            w = yield
            target = self._address(w, f"guards[{i}].target")
            w = yield
            token = self._address(w, f"guards[{i}].token")
            if (
                self.schema.meta
                and i < MAX_GUARDS
                and token == ZERO_ADDRESS
                and self.meta[i] != (NATIVE_TICKER, NATIVE_DECIMALS)
            ):
                raise self._err("E_NATIVE_META",
                                f"guards[{i}] native token with meta {self.meta[i]}")
            w = yield
            if i < MAX_GUARDS:
                self.guards.append(_Guard(target, token, w))

    def _approvals(self, w: bytes):
        n = self._length(w, "approvals length")
        self._bounds(n, 4, "approvals")
        if n > MAX_APPROVALS:
            raise self._err("E_APPROVALS_MAX", f"{n} approvals")
        self.n_approvals = n
        for i in range(n):
            w = yield
            self._address(w, f"approvals[{i}].target")
            w = yield
            token = self._address(w, f"approvals[{i}].token")
            w = yield
            if w[0] & 0x80:
                raise self._err("E_APPROVAL_NEG", f"approvals[{i}] amount has the top bit set")
            amount = w
            w = yield
            self._bool(w, f"approvals[{i}].useTransfer")
            self.approvals.append(_Approval(token, amount))

    def _permits(self, w: bytes):
        n = self._length(w, "permits length")
        self._bounds(n, 4, "permits")
        if n != self.n_approvals:
            raise self._err("E_PERMITS_LEN",
                            f"len(permits) {n} != len(approvals) {self.n_approvals}")
        for i in range(n):
            yield
            w = yield
            self._uint8(w, f"permits[{i}].v")
            yield
            yield

    def _bytes(self, w: bytes, what: str):
        ln = self._length(w, f"{what} length")
        words = (ln + 31) // 32
        self._bounds(words, 1, what)
        for j in range(words):
            w = yield
            if j == words - 1 and ln % 32 and any(w[ln % 32:]):
                raise self._err("E_BYTES_PAD", f"{what}: non-zero padding after byte {ln}")
        return ln

    def _withdrawals(self, w: bytes):
        n = self._length(w, "withdrawals length")
        self._bounds(n, 3, "withdrawals")
        for i in range(n):
            w = yield
            self._address(w, f"withdrawals[{i}].target")
            w = yield
            self._address(w, f"withdrawals[{i}].token")
            yield

    def _safe_tx(self, w: bytes):
        remaining_incl = self.total_words - self.cur
        if SAFE_TX_HEAD_WORDS > remaining_incl:
            raise self._err("E_TRUNC", f"SafeTx head needs 11 words, {remaining_incl} left")
        self._address(w, "safeTx.to")
        yield
        w = yield
        data_off = self._offset(w, "safeTx.data offset")
        if data_off != SAFE_TX_DATA_OFFSET:
            raise self._err("E_OFFSET", f"safeTx.data offset {data_off:#x} != 0x160")
        w = yield
        op = self._uint8(w, "safeTx.operation")
        if op != 0:
            raise self._err("E_SAFE_OP", f"safeTx.operation {op}")
        yield
        yield
        yield
        w = yield
        self._address(w, "safeTx.gasToken")
        w = yield
        self._address(w, "safeTx.refundReceiver")
        w = yield
        sig_off = self._offset(w, "safeTx.signatures offset")
        yield
        w = yield
        data_len = yield from self._bytes(w, "safeTx.data")
        w = yield
        expected = SAFE_TX_DATA_OFFSET + 32 + 32 * ((data_len + 31) // 32)
        if sig_off != expected:
            raise self._err("E_OFFSET", f"safeTx.signatures offset {sig_off:#x} != {expected:#x}")
        yield from self._bytes(w, "safeTx.signatures")


def _result(reason: str, stage: Optional[str], word: Optional[int], detail: str = "") -> dict:
    code, outcome = REASONS[reason]
    return {
        "outcome": outcome,
        "reason": reason,
        "reason_code": code,
        "stage": stage,
        "word": word,
        "detail": detail,
        "lookups": [],
        "name": None,
        "version": None,
        "num_screens": 0,
        "pairs": [],
    }


def _init(schema_sel: bytes, to: bytes, value: int, chain_id: bytes, data_size: int,
          interface_version_ok: bool, context_ok: bool) -> Schema:
    def stop(reason: str, detail: str) -> ModelStop:
        return ModelStop(reason, "init", None, detail)

    if not interface_version_ok:
        raise stop("U_VERSION", "interface version != LATEST")
    if not context_ok:
        raise stop("E_CONTEXT", "pluginContextLength < sizeof(context_t)")
    schema = SCHEMAS.get(schema_sel.hex())
    if schema is None:
        raise stop("U_SELECTOR", f"selector {schema_sel.hex()} not supported")
    if len(to) != 20 or to not in FAMILY_OF_ADDRESS:
        raise stop("E_DEST", f"destination {to.hex() or '<none>'} not in allowlist")
    if FAMILY_OF_ADDRESS[to] != schema.family:
        raise stop("E_FAMILY", f"destination family {FAMILY_OF_ADDRESS[to]} != {schema.family}")
    if len(chain_id) > 0:
        if len(chain_id) > 8:
            raise stop("E_CHAIN", f"chainID length {len(chain_id)} > 8")
        if int.from_bytes(chain_id, "big") != CHAIN_ID:
            raise stop("E_CHAIN", f"chainID {int.from_bytes(chain_id, 'big')} != {CHAIN_ID}")
    if data_size < 4 or data_size > 0xFFFFFFFF or (data_size - 4) % 32 != 0:
        raise stop("E_DATASIZE", f"dataSize {data_size}")
    if schema.family == "SR" and value != 0:
        raise stop("E_SAFE_VALUE", "Safe family with non-zero tx value")
    return schema


def _accept_ticker(ticker: Union[str, bytes], decimals: int) -> Optional[Tuple[bytes, int]]:
    raw = ticker.encode("latin-1") if isinstance(ticker, str) else bytes(ticker)
    buf = raw[:TICKER_BUFFER]
    if b"\x00" not in buf and len(raw) >= TICKER_BUFFER:
        return None
    s = buf.split(b"\x00", 1)[0]
    if not 1 <= len(s) <= MAX_SYMBOL_LEN:
        return None
    if any(b < 0x20 or b > 0x7E for b in s):
        return None
    if decimals > MAX_DECIMALS or decimals < 0:
        return None
    return s, decimals


def _signed(word: bytes) -> int:
    v = int.from_bytes(word, "big")
    return v - (1 << 256) if v >> 255 else v


def run(
    calldata: Union[bytes, str],
    *,
    to: Union[bytes, str, None],
    value: int = 0,
    chain_id: Union[int, bytes, None] = CHAIN_ID,
    v: Union[int, bytes, None] = None,
    data_size: Optional[int] = None,
    signer: Union[bytes, str, None] = None,
    interface_version_ok: bool = True,
    context_ok: bool = True,
    lookups: Optional[Dict[Union[bytes, str], Sequence]] = None,
    items: Optional[Sequence[Optional[Sequence]]] = None,
    param_offsets: Optional[Sequence[int]] = None,
) -> dict:
    """chain_id / v: typed tx -> chain_id=<id>, v=None; legacy tx -> chain_id=None, v=<id> (the
    EIP-155 unsigned `v` field carries the chain id). lookups: {token_address: (ticker,
    decimals)} or (ticker, decimals, reported_address) -- what app-ethereum knows and passes as
    item1/item2 for the requested tokenLookup1/2. items: explicit (item1, item2), each None or
    (reported_address, ticker, decimals); overrides `lookups`."""
    calldata = _addr_bytes(calldata) if isinstance(calldata, str) else bytes(calldata)
    to_b = _addr_bytes(to)
    chain_b = _int_bytes(chain_id)
    v_b = _int_bytes(v)
    signer_b = _addr_bytes(signer) if signer is not None else ZERO_ADDRESS
    if data_size is None:
        data_size = len(calldata)
    selector = calldata[:4].ljust(4, b"\x00")

    try:
        schema = _init(selector, to_b, value, chain_b, data_size, interface_version_ok,
                       context_ok)
    except ModelStop as stop:
        return _result(stop.reason, stop.stage, stop.word, stop.detail)

    plugin = _Plugin(schema, data_size)
    body = calldata[4:]
    n_words = (len(body) + 31) // 32
    if param_offsets is not None:
        n_words = len(param_offsets)
    try:
        for k in range(n_words):
            word = body[32 * k: 32 * k + 32].ljust(32, b"\x00")
            offset = param_offsets[k] if param_offsets is not None else 4 + 32 * k
            plugin.provide_parameter(offset, word)
    except ModelStop as stop:
        return _result(stop.reason, stop.stage, stop.word, stop.detail)

    if not plugin.done:
        return _result("E_TRUNC", "finalize", None, "calldata ended before the parser finished")
    if plugin.calls != plugin.total_words:
        return _result("E_TRAILING", "finalize", None,
                       f"consumed {plugin.calls} words != {plugin.total_words}")
    if (len(chain_b) > 0) == (len(v_b) > 0):
        return _result("E_CHAIN", "finalize", None, "need exactly one of chainID / v")
    net = chain_b if chain_b else v_b
    if len(net) > 8:
        return _result("E_CHAIN", "finalize", None, f"network length {len(net)} > 8")
    if int.from_bytes(net, "big") != CHAIN_ID:
        return _result("E_CHAIN", "finalize", None, f"network {int.from_bytes(net, 'big')}")
    if plugin.fallback is not None:
        reason, word = plugin.fallback
        return _result(reason, "finalize", word, "sticky fallback")

    family = schema.family
    owner = plugin.safe if family == "SR" else signer_b
    guards = plugin.guards
    if family in ("AR", "PR"):
        if schema.mode == "diffs":
            pulls = list(plugin.approvals)
        else:
            pulls = [
                a for a in plugin.approvals
                if not any(g.target == signer_b and g.token == a.token for g in guards)
            ]
    else:
        pulls = []

    def guard_meta_for(token: bytes) -> Optional[Tuple[bytes, int]]:
        for i, g in enumerate(guards):
            if g.token == token:
                return plugin.meta[i]
        return None

    unknown: List[bytes] = []
    for g in guards:
        if g.token != ZERO_ADDRESS and not schema.meta and g.token not in unknown:
            unknown.append(g.token)
    for p in pulls:
        known = schema.meta and guard_meta_for(p.token) is not None
        if p.token != ZERO_ADDRESS and not known and p.token not in unknown:
            unknown.append(p.token)
    requested = unknown[:2]

    if items is None:
        items_l: List[Optional[Tuple[bytes, Union[str, bytes], int]]] = []
        table = {_addr_bytes(k): tuple(val) for k, val in (lookups or {}).items()}
        for addr in requested:
            entry = table.get(addr)
            if entry is None:
                items_l.append(None)
            elif len(entry) >= 3:
                items_l.append((_addr_bytes(entry[2]), entry[0], entry[1]))
            else:
                items_l.append((addr, entry[0], entry[1]))
    else:
        items_l = [None if it is None else (_addr_bytes(it[0]), it[1], it[2]) for it in items]
    accepted: Dict[bytes, Tuple[bytes, int]] = {}
    for idx, addr in enumerate(requested):
        item = items_l[idx] if idx < len(items_l) else None
        if item is None or item[0] != addr:
            continue
        ok = _accept_ticker(item[1], item[2])
        if ok is not None:
            accepted[addr] = ok

    def info(token: bytes, guard_index: Optional[int]) -> Tuple[bytes, int]:
        if token == ZERO_ADDRESS:
            return NATIVE_TICKER, NATIVE_DECIMALS
        if schema.meta:
            if guard_index is not None:
                return plugin.meta[guard_index]
            m = guard_meta_for(token)
            if m is not None:
                return m
        if token in accepted:
            return accepted[token]
        return UNKNOWN_TICKER, 0

    pairs: List[List[str]] = []
    if family != "SR" and value != 0:
        pairs.append(["Send", amount_to_string(value, NATIVE_DECIMALS, NATIVE_TICKER)])
    if family == "SR":
        pairs.append(["Safe", eip55(plugin.safe)])
    n = len(guards)
    for i, g in enumerate(guards):
        sfx = f" {i + 1}/{n}" if n > 1 else ""
        ticker, decimals = info(g.token, i)
        amount = _signed(g.amount)
        if schema.mode == "diffs":
            sign = "-" if amount < 0 else ("+" if amount > 0 else "")
            text = sign + amount_to_string(abs(amount), decimals, ticker)
            pairs.append(["Min change" + sfx, text])
        else:
            pairs.append(["Min balance" + sfx, amount_to_string(abs(amount), decimals, ticker)])
        if g.token != ZERO_ADDRESS:
            pairs.append(["Token" + sfx, eip55(g.token)])
        if g.target != owner:
            pairs.append(["Other account" + sfx, eip55(g.target)])
    m = len(pulls)
    for j, p in enumerate(pulls):
        ticker, decimals = info(p.token, None)
        text = amount_to_string(int.from_bytes(p.amount, "big"), decimals, ticker)
        if schema.mode == "diffs":
            t1, t2 = ("Pull", "Pull token") if m == 1 else (f"Pull {j + 1}/{m}",
                                                            f"Pull token {j + 1}/{m}")
        else:
            t1, t2 = ("Unguarded pull", "Pull token") if m == 1 else (f"Unguarded pull {j + 1}",
                                                                      f"Pull token {j + 1}")
        pairs.append([t1, text])
        pairs.append([t2, eip55(p.token)])

    if not pairs:
        return _result("E_STATE", "finalize", None, "no screens")
    for title, text in pairs:
        if len(title) + 1 > TITLE_BUFFER or len(text) + 1 > VALUE_BUFFER:
            return _result("E_RENDER", "ui", None, f"{title!r} does not fit")

    res = _result("R_OK", "finalize", None, "")
    res.update(
        lookups=[eip55(a) for a in requested],
        name=PLUGIN_NAME,
        version=VERSION_BY_FAMILY[family],
        num_screens=len(pairs),
        pairs=pairs,
    )
    return res


def run_tx(tx: dict, *, signer: Union[bytes, str, None] = None, lookups=None, items=None,
           **kw) -> dict:
    data = tx.get("data", b"")
    legacy = "gasPrice" in tx and "maxFeePerGas" not in tx
    chain = tx.get("chainId")
    args = dict(
        to=tx.get("to"),
        value=int(tx.get("value", 0)),
        chain_id=None if legacy else chain,
        v=chain if legacy else None,
        signer=signer,
        lookups=lookups,
        items=items,
    )
    args.update(kw)
    return run(data, **args)
