"""Canonical calldata crafter for the ERC8009 plugin (ERC-8009 guarded calls, Sepolia)."""

from __future__ import annotations

import copy
import json
import random
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Dict, List, Optional, Tuple, Union

from eth_abi import decode as abi_decode
from eth_abi import encode as abi_encode
from eth_utils import keccak, to_checksum_address

ABIS_DIR = Path(__file__).resolve().parent / "abis"

CHAIN_ID = 11155111
FAMILY_ADDRESS = {
    "BP": "0x800B36cc16C7572E1d600506a0d7d30890fF35f4",
    "AR": "0xf3c82E2176D894ED32C5748C22238ebfcc68CDd8",
    "PR": "0xfEa607c2f74D7ac4ac35360e51f3fe08E319ebA2",
    "SR": "0x7d7Eae4d58e9D2A92a48EE55f249a2903fD1d0d0",
}
BALANCE_PROXY = FAMILY_ADDRESS["BP"]
OLD_BALANCE_PROXY = "0xa1A77109EaAFDEf3b2Eb36493A98868D4bD82FEf"
ZERO_ADDRESS = "0x0000000000000000000000000000000000000000"
NATIVE = ZERO_ADDRESS
USDC = "0x1c7D4B196Cb0C7B01d743Fbc6116a902379C7238"
UNISWAP_ROUTER = "0x3A9D48AB9751398BbFa63ad67599Bb04e4BdF98b"
SPECULOS_ADDRESS = "0xDad77910DbDFdE764fC21FCD4E74D71bBACA6D8D"
EXAMPLE_SAFE = to_checksum_address("0x5afe" + "00" * 16 + "5afe")
DEAD = "0x000000000000000000000000000000000000dEaD"
EXECUTE_SAFE_TRANSACTION_SELECTOR = bytes.fromhex("ece10c06")
UNKNOWN_SELECTOR = bytes.fromhex("deadbeef")

ROLE_BY_NAME = {
    "balanceProxy": "ADDR_PROXY",
    "meta": "META",
    "postBalances": "GUARDS",
    "diffs": "GUARDS",
    "balances": "GUARDS",
    "approvals": "APPROVALS",
    "permits": "PERMITS",
    "target": "ADDR_TARGET",
    "data": "BYTES",
    "withdrawals": "WITHDRAWALS",
    "safe": "ADDR_SAFE",
    "safeTx": "SAFE_TX",
}
STATIC_ROLES = ("ADDR_PROXY", "ADDR_TARGET", "ADDR_SAFE")
MAX_GUARDS = 8
INT256_MIN = -(1 << 255)
INT256_MAX = (1 << 255) - 1
UINT256_MAX = (1 << 256) - 1


def canonical_type(param: dict) -> str:
    t = param["type"]
    if t.startswith("tuple"):
        return "(" + ",".join(canonical_type(c) for c in param["components"]) + ")" + t[5:]
    return t


@dataclass(frozen=True)
class Param:
    name: str
    type: str
    role: str


@dataclass(frozen=True)
class Function:
    name: str
    selector: bytes
    family: str
    address: str
    params: Tuple[Param, ...]

    @property
    def mode(self) -> str:
        return "diffs" if "Diffs" in self.name else "min"

    @property
    def meta(self) -> bool:
        return any(p.role == "META" for p in self.params)

    @property
    def types(self) -> List[str]:
        return [p.type for p in self.params]

    @property
    def roles(self) -> List[str]:
        return [p.role for p in self.params]

    def slot(self, role: str) -> int:
        return self.roles.index(role)


def load_abi(address: str) -> list:
    with open(ABIS_DIR / f"{address}.abi.json", encoding="utf-8") as fh:
        return json.load(fh)


def _signature(entry: dict) -> str:
    return entry["name"] + "(" + ",".join(canonical_type(p) for p in entry["inputs"]) + ")"


def _load_functions() -> Tuple[Dict[str, Function], dict]:
    functions: Dict[str, Function] = {}
    execute_safe = None
    for family, address in FAMILY_ADDRESS.items():
        for entry in load_abi(address):
            if entry.get("type") != "function":
                continue
            selector = keccak(text=_signature(entry))[:4]
            if entry["name"] == "executeSafeTransaction":
                execute_safe = entry
            names = [p["name"] for p in entry["inputs"]]
            if not any(ROLE_BY_NAME.get(n) == "GUARDS" for n in names):
                continue
            params = tuple(Param(p["name"], canonical_type(p), ROLE_BY_NAME[p["name"]])
                           for p in entry["inputs"])
            functions[entry["name"]] = Function(entry["name"], selector, family, address, params)
    return functions, execute_safe


FUNCTIONS, _EXECUTE_SAFE_ABI = _load_functions()
FUNCTIONS_BY_SELECTOR = {f.selector.hex(): f for f in FUNCTIONS.values()}


def _cs(address: Union[str, bytes]) -> str:
    return to_checksum_address(address)


def _raw(address: Union[str, bytes]) -> bytes:
    return bytes.fromhex(_cs(address)[2:])


@dataclass
class Balance:
    target: str
    token: str
    amount: int


@dataclass
class Approval:
    balance: Balance
    use_transfer: bool = False


@dataclass
class Meta:
    symbol: str
    decimals: int


@dataclass
class Permit:
    deadline: int
    v: int
    r: bytes
    s: bytes


@dataclass
class SafeTx:
    to: str
    value: int = 0
    data: bytes = b""
    operation: int = 0
    safe_tx_gas: int = 0
    base_gas: int = 0
    gas_price: int = 0
    gas_token: str = ZERO_ADDRESS
    refund_receiver: str = ZERO_ADDRESS
    signatures: bytes = b""
    router_sig_position: int = 0

    def as_tuple(self) -> tuple:
        return (_raw(self.to), self.value, self.data, self.operation, self.safe_tx_gas,
                self.base_gas, self.gas_price, _raw(self.gas_token), _raw(self.refund_receiver),
                self.signatures, self.router_sig_position)


@dataclass
class Call:
    function: str
    guards: List[Balance] = field(default_factory=list)
    meta: List[Meta] = field(default_factory=list)
    approvals: List[Approval] = field(default_factory=list)
    permits: List[Permit] = field(default_factory=list)
    target: str = ZERO_ADDRESS
    data: bytes = b""
    withdrawals: List[Balance] = field(default_factory=list)
    balance_proxy: str = BALANCE_PROXY
    safe: str = ZERO_ADDRESS
    safe_tx: SafeTx = field(default_factory=lambda: SafeTx(to=ZERO_ADDRESS))
    value: int = 0

    @property
    def spec(self) -> Function:
        return FUNCTIONS[self.function]

    @property
    def selector(self) -> bytes:
        return self.spec.selector

    @property
    def family(self) -> str:
        return self.spec.family

    @property
    def mode(self) -> str:
        return self.spec.mode

    @property
    def is_meta(self) -> bool:
        return self.spec.meta

    @property
    def to(self) -> str:
        return self.spec.address

    @property
    def owner(self) -> Optional[str]:
        return self.safe if self.family == "SR" else None


def _bal(b: Balance) -> tuple:
    return (_raw(b.target), _raw(b.token), b.amount)


def _args(call: Call) -> list:
    out = []
    for p in call.spec.params:
        r = p.role
        if r == "ADDR_PROXY":
            out.append(_raw(call.balance_proxy))
        elif r == "META":
            out.append([(m.symbol, m.decimals) for m in call.meta])
        elif r == "GUARDS":
            out.append([_bal(b) for b in call.guards])
        elif r == "APPROVALS":
            out.append([(_bal(a.balance), a.use_transfer) for a in call.approvals])
        elif r == "PERMITS":
            out.append([(p_.deadline, p_.v, p_.r, p_.s) for p_ in call.permits])
        elif r == "ADDR_TARGET":
            out.append(_raw(call.target))
        elif r == "BYTES":
            out.append(call.data)
        elif r == "WITHDRAWALS":
            out.append([_bal(b) for b in call.withdrawals])
        elif r == "ADDR_SAFE":
            out.append(_raw(call.safe))
        elif r == "SAFE_TX":
            out.append(call.safe_tx.as_tuple())
        else:  # pragma: no cover
            raise AssertionError(r)
    return out


def encode(call: Call) -> bytes:
    body = abi_encode(call.spec.types, _args(call))
    assert len(body) // 32 == layout(call).total_words, "layout mismatch"
    return call.selector + body


def decode(calldata: Union[bytes, str], value: int = 0) -> Call:
    if isinstance(calldata, str):
        calldata = bytes.fromhex(calldata[2:] if calldata.startswith("0x") else calldata)
    spec = FUNCTIONS_BY_SELECTOR[calldata[:4].hex()]
    values = abi_decode(spec.types, calldata[4:])
    call = Call(spec.name, value=value)

    def bal(t) -> Balance:
        return Balance(_cs(t[0]), _cs(t[1]), t[2])

    for p, v in zip(spec.params, values):
        r = p.role
        if r == "ADDR_PROXY":
            call.balance_proxy = _cs(v)
        elif r == "META":
            call.meta = [Meta(s, d) for s, d in v]
        elif r == "GUARDS":
            call.guards = [bal(t) for t in v]
        elif r == "APPROVALS":
            call.approvals = [Approval(bal(b), u) for b, u in v]
        elif r == "PERMITS":
            call.permits = [Permit(d, v_, bytes(r_), bytes(s_)) for d, v_, r_, s_ in v]
        elif r == "ADDR_TARGET":
            call.target = _cs(v)
        elif r == "BYTES":
            call.data = bytes(v)
        elif r == "WITHDRAWALS":
            call.withdrawals = [bal(t) for t in v]
        elif r == "ADDR_SAFE":
            call.safe = _cs(v)
        elif r == "SAFE_TX":
            call.safe_tx = SafeTx(_cs(v[0]), v[1], bytes(v[2]), v[3], v[4], v[5], v[6],
                                  _cs(v[7]), _cs(v[8]), bytes(v[9]), v[10])
    return call


def encode_execute_safe_transaction(safe: str, safe_tx: SafeTx) -> bytes:
    types = [canonical_type(p) for p in _EXECUTE_SAFE_ABI["inputs"]]
    selector = keccak(text=_signature(_EXECUTE_SAFE_ABI))[:4]
    assert selector == EXECUTE_SAFE_TRANSACTION_SELECTOR
    return selector + abi_encode(types, [_raw(safe), safe_tx.as_tuple()])


# All fields below are word indexes into calldata after the 4-byte selector, not byte offsets.
@dataclass
class Layout:
    head: Dict[str, int]
    tail: Dict[str, int]
    meta_offsets: List[int]
    meta_elems: List[int]
    guard_elems: List[int]
    approval_elems: List[int]
    permit_elems: List[int]
    withdrawal_elems: List[int]
    data_len: Optional[int]
    safe_data_len: Optional[int]
    safe_sigs_len: Optional[int]
    total_words: int


def _words(n_bytes: int) -> int:
    return (n_bytes + 31) // 32


def _symbol_bytes(symbol: Union[str, bytes]) -> bytes:
    return symbol.encode("utf-8") if isinstance(symbol, str) else symbol


def layout(call: Call) -> Layout:
    spec = call.spec
    lay = Layout({}, {}, [], [], [], [], [], [], None, None, None, 0)
    pos = len(spec.params)
    for slot, p in enumerate(spec.params):
        lay.head[p.role] = slot
    for p in spec.params:
        r = p.role
        if r in STATIC_ROLES:
            continue
        lay.tail[r] = pos
        if r == "META":
            n = len(call.meta)
            lay.meta_offsets = [pos + 1 + i for i in range(n)]
            cur = pos + 1 + n
            for m in call.meta:
                lay.meta_elems.append(cur)
                cur += 3 + _words(len(_symbol_bytes(m.symbol)))
            pos = cur
        elif r == "GUARDS":
            lay.guard_elems = [pos + 1 + 3 * i for i in range(len(call.guards))]
            pos += 1 + 3 * len(call.guards)
        elif r == "APPROVALS":
            lay.approval_elems = [pos + 1 + 4 * i for i in range(len(call.approvals))]
            pos += 1 + 4 * len(call.approvals)
        elif r == "PERMITS":
            lay.permit_elems = [pos + 1 + 4 * i for i in range(len(call.permits))]
            pos += 1 + 4 * len(call.permits)
        elif r == "BYTES":
            lay.data_len = pos
            pos += 1 + _words(len(call.data))
        elif r == "WITHDRAWALS":
            lay.withdrawal_elems = [pos + 1 + 3 * i for i in range(len(call.withdrawals))]
            pos += 1 + 3 * len(call.withdrawals)
        elif r == "SAFE_TX":
            lay.safe_data_len = pos + 11
            lay.safe_sigs_len = lay.safe_data_len + 1 + _words(len(call.safe_tx.data))
            pos = lay.safe_sigs_len + 1 + _words(len(call.safe_tx.signatures))
    lay.total_words = pos
    return lay


def get_word(calldata: bytes, k: int) -> bytes:
    return calldata[4 + 32 * k: 4 + 32 * k + 32]


def set_word(calldata: bytes, k: int, word: Union[int, bytes]) -> bytes:
    if isinstance(word, int):
        word = (word % (1 << 256)).to_bytes(32, "big")
    assert len(word) == 32
    return calldata[: 4 + 32 * k] + word + calldata[4 + 32 * k + 32:]


def set_byte(calldata: bytes, k: int, index: int, value: int) -> bytes:
    w = bytearray(get_word(calldata, k))
    w[index] = value
    return set_word(calldata, k, bytes(w))


def word_int(calldata: bytes, k: int) -> int:
    return int.from_bytes(get_word(calldata, k), "big")


def _hex(data: bytes) -> str:
    return "0x" + data.hex()


def tx_eip1559(data: bytes, to: Optional[str], value: int = 0, *, chain_id: int = CHAIN_ID,
               nonce: int = 7, gas: int = 400_000, max_fee_per_gas: int = 30 * 10**9,
               max_priority_fee_per_gas: int = 10**9) -> dict:
    tx = {
        "nonce": nonce,
        "maxFeePerGas": max_fee_per_gas,
        "maxPriorityFeePerGas": max_priority_fee_per_gas,
        "gas": gas,
        "value": value,
        "chainId": chain_id,
        "data": _hex(data),
    }
    if to is not None:
        tx["to"] = _cs(to)
    return tx


def tx_legacy(data: bytes, to: Optional[str], value: int = 0, *, chain_id: int = CHAIN_ID,
              nonce: int = 7, gas: int = 400_000, gas_price: int = 30 * 10**9) -> dict:
    tx = {
        "nonce": nonce,
        "gasPrice": gas_price,
        "gas": gas,
        "value": value,
        "chainId": chain_id,
        "data": _hex(data),
    }
    if to is not None:
        tx["to"] = _cs(to)
    return tx


@dataclass
class Scenario:
    calldata: bytes
    to: Optional[str]
    value: int = 0
    chain_id: Union[int, bytes, None] = CHAIN_ID
    legacy_v: Union[int, bytes, None] = None
    knobs: Dict[str, object] = field(default_factory=dict)

    @property
    def legacy(self) -> bool:
        return self.chain_id is None and self.legacy_v is not None

    def model_kwargs(self) -> dict:
        kw = dict(calldata=self.calldata, to=self.to, value=self.value, chain_id=self.chain_id,
                  v=self.legacy_v)
        kw.update(self.knobs)
        return kw

    def tx(self, **kw) -> dict:
        if self.knobs:
            raise ValueError(f"model-only scenario (knobs {sorted(self.knobs)})")
        if self.legacy:
            if not isinstance(self.legacy_v, int):
                raise ValueError("raw legacy v bytes cannot be expressed as a tx dict")
            return tx_legacy(self.calldata, self.to, self.value, chain_id=self.legacy_v, **kw)
        if not isinstance(self.chain_id, int) or self.legacy_v is not None:
            raise ValueError("raw / dual network fields cannot be expressed as a tx dict")
        return tx_eip1559(self.calldata, self.to, self.value, chain_id=self.chain_id, **kw)


def scenario(call: Call, legacy: bool = False) -> Scenario:
    data = encode(call)
    if legacy:
        return Scenario(data, call.to, call.value, chain_id=None, legacy_v=CHAIN_ID)
    return Scenario(data, call.to, call.value)


def _swap_data() -> bytes:
    return bytes.fromhex("3593564c") + bytes(31) + b"\x01" + bytes(31) + b"\x02"


def _erc20_transfer(to: str, amount: int) -> bytes:
    return bytes.fromhex("a9059cbb") + abi_encode(["address", "uint256"], [_raw(to), amount])


def canonical_call(function: str, signer: str = SPECULOS_ADDRESS) -> Call:
    spec = FUNCTIONS[function]
    owner = EXAMPLE_SAFE if spec.family == "SR" else _cs(signer)
    if spec.mode == "diffs":
        eth, usdc = -1_100_000_000_000_000, 2_500_000
    else:
        eth, usdc = 50_000_000_000_000_000, 2_500_000
    call = Call(function, guards=[Balance(owner, NATIVE, eth), Balance(owner, USDC, usdc)])
    if spec.meta:
        call.meta = [Meta("ETH", 18), Meta("USDC", 6)]
    if spec.family == "SR":
        call.safe = EXAMPLE_SAFE
        call.safe_tx = SafeTx(to=USDC, data=_erc20_transfer(DEAD, 1_000_000),
                              signatures=bytes(range(65)))
    else:
        call.target = UNISWAP_ROUTER
        call.data = _swap_data()
        call.value = 10**15
    return call


def spec_example_call(signer: str = SPECULOS_ADDRESS) -> Call:
    return Call("approveProxyCallDiffsWithMeta", guards=[Balance(_cs(signer), USDC, 2_500_000)],
                meta=[Meta("USDC", 6)], target=UNISWAP_ROUTER, data=_swap_data(), value=10**15)


def _rand_addr(rng: random.Random) -> str:
    return _cs(rng.getrandbits(160).to_bytes(20, "big"))


def _rand_bytes(rng: random.Random, n: int) -> bytes:
    return rng.getrandbits(8 * n).to_bytes(n, "big") if n else b""


def _rand_int256(rng: random.Random) -> int:
    kind = rng.randrange(8)
    if kind == 0:
        return 0
    if kind == 1:
        return rng.choice([INT256_MIN, INT256_MAX, -1, 1, INT256_MIN + 1])
    if kind in (2, 3):
        return rng.randint(-(10**24), 10**24)
    if kind == 4:
        return rng.randint(-(10**7), 10**7)
    return rng.randint(INT256_MIN, INT256_MAX)


def _rand_amount(rng: random.Random) -> int:
    kind = rng.randrange(5)
    if kind == 0:
        return rng.choice([0, 1, INT256_MAX])
    if kind in (1, 2):
        return rng.randint(0, 10**24)
    return rng.randint(0, INT256_MAX)


def _rand_symbol(rng: random.Random) -> str:
    return "".join(chr(rng.randint(0x20, 0x7E)) for _ in range(rng.randint(1, 16)))


def random_call(function: str, rng: random.Random, *, signer: Optional[str] = None,
                n_guards: Optional[int] = None) -> Call:
    spec = FUNCTIONS[function]
    signer = _cs(signer) if signer else _rand_addr(rng)
    safe = _rand_addr(rng)
    owner = safe if spec.family == "SR" else signer
    tokens = [NATIVE, USDC] + [_rand_addr(rng) for _ in range(rng.randint(1, 4))]
    token_meta: Dict[str, Meta] = {NATIVE: Meta("ETH", 18)}
    n = n_guards if n_guards is not None else rng.randint(1, MAX_GUARDS)
    call = Call(function, safe=safe if spec.family == "SR" else ZERO_ADDRESS)
    for _ in range(n):
        target = owner if rng.random() < 0.6 else _rand_addr(rng)
        token = rng.choice(tokens)
        call.guards.append(Balance(target, token, _rand_int256(rng)))
        if spec.meta:
            if token not in token_meta:
                token_meta[token] = Meta(_rand_symbol(rng), rng.randint(0, 77))
            call.meta.append(copy.copy(token_meta[token]))
    if spec.family != "SR":
        for _ in range(rng.randint(0, 2)):
            token = rng.choice(tokens + [_rand_addr(rng)])
            target = _rand_addr(rng) if rng.random() < 0.7 else signer
            call.approvals.append(Approval(Balance(target, token, _rand_amount(rng)),
                                           rng.random() < 0.5))
        if spec.family == "PR":
            call.permits = [Permit(rng.getrandbits(256), rng.randint(0, 255),
                                   _rand_bytes(rng, 32), _rand_bytes(rng, 32))
                            for _ in call.approvals]
        call.target = _rand_addr(rng) if rng.random() < 0.9 else ZERO_ADDRESS
        size = rng.choice([0, rng.randint(1, 64), rng.randint(1, 4096)])
        call.data = _rand_bytes(rng, size)
        call.withdrawals = [Balance(_rand_addr(rng), rng.choice(tokens), _rand_int256(rng))
                            for _ in range(rng.choice([0, 1, 1, 2, 3]))]
        call.value = rng.choice([0, rng.randint(1, 10**18), rng.randint(1, UINT256_MAX)])
    else:
        call.safe_tx = SafeTx(
            to=_rand_addr(rng),
            value=rng.choice([0, rng.getrandbits(256)]),
            data=_rand_bytes(rng, rng.choice([0, rng.randint(1, 600)])),
            operation=0,
            safe_tx_gas=rng.getrandbits(rng.choice([8, 64, 256])),
            base_gas=rng.getrandbits(rng.choice([8, 64, 256])),
            gas_price=rng.getrandbits(rng.choice([8, 64, 256])),
            gas_token=rng.choice([ZERO_ADDRESS, _rand_addr(rng)]),
            refund_receiver=rng.choice([ZERO_ADDRESS, _rand_addr(rng)]),
            signatures=_rand_bytes(rng, 65 * rng.randint(0, 3)),
            router_sig_position=rng.choice([0, 1, 2, rng.getrandbits(256)]),
        )
    return call


def random_lookups(call: Call, rng: random.Random) -> Dict[str, Tuple[str, int]]:
    tokens = {b.token for b in call.guards} | {a.balance.token for a in call.approvals}
    return {t: (_rand_symbol(rng), rng.randint(0, 77))
            for t in sorted(tokens) if t != NATIVE and rng.random() < 0.6}


def hypothesis_strategy(function: str):
    from hypothesis import strategies as st  # pylint: disable=import-outside-toplevel

    return st.integers(0, 2**64 - 1).map(lambda seed: random_call(function,
                                                                  random.Random(seed)))


def fuzz_seeds(per_selector: int = 4, seed: int = 0) -> List[Tuple[str, bytes]]:
    rng = random.Random(seed)
    out: List[Tuple[str, bytes]] = []
    for name, spec in sorted(FUNCTIONS.items(), key=lambda kv: kv[1].selector):
        out.append((f"ok_{spec.selector.hex()}_0.bin", encode(canonical_call(name))))
        for n in range(1, per_selector):
            out.append((f"ok_{spec.selector.hex()}_{n}.bin", encode(random_call(name, rng))))
    counter: Dict[str, int] = {}
    seen = {data for _, data in out}
    for name in sorted(FUNCTIONS):
        for m in all_mutations(canonical_call(name)):
            if m.scenario.knobs or m.scenario.calldata in seen:
                continue
            seen.add(m.scenario.calldata)
            cls = m.name.replace("@", "_")
            idx = counter.get(cls, 0)
            counter[cls] = idx + 1
            out.append((f"bad_{cls}_{idx}.bin", m.scenario.calldata))
    return out


@dataclass
class Mutation:
    name: str
    expected: str
    scenario: Scenario
    call: Call
    note: str = ""


MutatorFn = Callable[[Call], Optional[Tuple[Scenario, str]]]
MUTATORS: Dict[str, Tuple[MutatorFn, str]] = {}


def _mutator(name: str, expected: str):
    def deco(fn: MutatorFn) -> MutatorFn:
        MUTATORS[name] = (fn, expected)
        return fn

    return deco


def mutate(name: str, call: Call) -> Optional[Mutation]:
    fn, expected = MUTATORS[name]
    base = copy.deepcopy(call)
    res = fn(base)
    if res is None:
        return None
    sc, note = res
    return Mutation(name, expected, sc, base, note)


def all_mutations(call: Call) -> List[Mutation]:
    out = []
    for name in MUTATORS:
        m = mutate(name, call)
        if m is not None:
            out.append(m)
    return out


def _patched(call: Call, patch: Callable[[bytes, Layout], bytes], note: str,
             **overrides) -> Tuple[Scenario, str]:
    sc = scenario(call)
    sc.calldata = patch(sc.calldata, layout(call))
    for k, v in overrides.items():
        setattr(sc, k, v)
    return sc, note


def _ensure_approval(call: Call) -> bool:
    if call.family == "SR":
        return False
    if not call.approvals:
        call.approvals.append(Approval(Balance(UNISWAP_ROUTER, USDC, 1_000_000), False))
    if call.family == "PR":
        while len(call.permits) < len(call.approvals):
            call.permits.append(Permit(2**40, 27, b"\x11" * 32, b"\x22" * 32))
    return True


def _nine_guards(call: Call) -> None:
    owner = call.safe if call.family == "SR" else (call.guards[0].target if call.guards
                                                   else SPECULOS_ADDRESS)
    while len(call.guards) < MAX_GUARDS + 1:
        i = len(call.guards)
        call.guards.append(Balance(owner, NATIVE, 10**15 * (i + 1)))
        if call.is_meta:
            call.meta.append(Meta("ETH", 18))


@_mutator("wrong_chain", "E_CHAIN")
def _m_wrong_chain(call):
    sc = scenario(call)
    sc.chain_id = 1
    return sc, "typed tx on chain 1"


@_mutator("chain_id_too_long", "E_CHAIN")
def _m_chain_long(call):
    sc = scenario(call)
    sc.chain_id = bytes(6) + CHAIN_ID.to_bytes(3, "big")
    return sc, "chainID.length 9 (> 8) with the right value"


@_mutator("legacy_tx_wrong_chain", "E_CHAIN")
def _m_legacy_wrong_chain(call):
    sc = scenario(call, legacy=True)
    sc.legacy_v = 1
    return sc, "legacy tx, EIP-155 v carries chain 1 (checked in finalize)"


@_mutator("legacy_v_too_long", "E_CHAIN")
def _m_legacy_v_long(call):
    sc = scenario(call, legacy=True)
    sc.legacy_v = bytes(6) + CHAIN_ID.to_bytes(3, "big")
    return sc, "legacy v length 9"


@_mutator("network_absent", "E_CHAIN")
def _m_network_absent(call):
    sc = scenario(call)
    sc.chain_id = None
    return sc, "neither chainID nor v present (not expressible as a tx dict)"


@_mutator("network_both", "E_CHAIN")
def _m_network_both(call):
    sc = scenario(call)
    sc.legacy_v = CHAIN_ID
    return sc, "both chainID and v present (not expressible as a tx dict)"


@_mutator("wrong_to", "E_DEST")
def _m_wrong_to(call):
    sc = scenario(call)
    sc.to = DEAD
    return sc, "destination outside the allowlist"


@_mutator("wrong_to@old_balance_proxy", "E_DEST")
def _m_wrong_to_old(call):
    sc = scenario(call)
    sc.to = OLD_BALANCE_PROXY
    return sc, "legacy BalanceProxy deployment"


@_mutator("wrong_to@none", "E_DEST")
def _m_wrong_to_none(call):
    sc = scenario(call)
    sc.to = None
    return sc, "no destination (contract creation)"


@_mutator("family_mismatch", "E_FAMILY")
def _m_family(call):
    fams = list(FAMILY_ADDRESS)
    other = fams[(fams.index(call.family) + 1) % len(fams)]
    sc = scenario(call)
    sc.to = FAMILY_ADDRESS[other]
    return sc, f"{call.family} selector sent to the {other} contract"


@_mutator("misaligned_data", "E_DATASIZE")
def _m_misaligned(call):
    sc = scenario(call)
    sc.calldata += b"\x00"
    return sc, "(dataSize - 4) % 32 == 1"


@_mutator("datasize_too_large", "E_DATASIZE")
def _m_datasize_large(call):
    sc = scenario(call)
    sc.knobs["data_size"] = 0x1_0000_0004
    return sc, "dataSize > 0xFFFFFFFF (model-only)"


@_mutator("datasize_exceeds_calldata", "E_TRAILING")
def _m_datasize_exceeds(call):
    sc = scenario(call)
    sc.knobs["data_size"] = len(sc.calldata) + 32
    return sc, "parser done but consumed words != (dataSize-4)/32 (model-only)"


@_mutator("safe_value", "E_SAFE_VALUE")
def _m_safe_value(call):
    if call.family != "SR":
        return None
    sc = scenario(call)
    sc.value = 1
    return sc, "1 wei on a Safe selector"


@_mutator("interface_version", "U_VERSION")
def _m_version(call):
    sc = scenario(call)
    sc.knobs["interface_version_ok"] = False
    return sc, "model-only"


@_mutator("context_too_small", "E_CONTEXT")
def _m_context(call):
    sc = scenario(call)
    sc.knobs["context_ok"] = False
    return sc, "model-only"


@_mutator("unknown_selector", "U_SELECTOR")
def _m_unknown_ece(call):
    safe = call.safe if call.family == "SR" else EXAMPLE_SAFE
    stx = call.safe_tx if call.family == "SR" else SafeTx(to=USDC)
    sc = Scenario(encode_execute_safe_transaction(safe, stx), FAMILY_ADDRESS["SR"], 0)
    return sc, "executeSafeTransaction 0xece10c06 to SafeRouter"


@_mutator("unknown_selector@random", "U_SELECTOR")
def _m_unknown_random(call):
    sc = scenario(call)
    sc.calldata = UNKNOWN_SELECTOR + sc.calldata[4:]
    return sc, "0xdeadbeef + canonical body"


@_mutator("param_offset_gap", "E_SEQ")
def _m_seq(call):
    sc = scenario(call)
    n = (len(sc.calldata) - 4) // 32
    offs = [4 + 32 * k for k in range(n)]
    offs[1] += 32
    sc.knobs["param_offsets"] = offs
    return sc, "parameterOffset of the 2nd call skips a word (model-only)"


@_mutator("wrong_balance_proxy", "E_PROXY")
def _m_proxy(call):
    if call.family == "BP":
        return None
    call.balance_proxy = OLD_BALANCE_PROXY
    return scenario(call), "legacy BalanceProxy as the balanceProxy argument"


def _shift_guards(delta: int):
    def fn(call):
        return _patched(call, lambda cd, lay: set_word(cd, lay.head["GUARDS"],
                                                       word_int(cd, lay.head["GUARDS"]) + delta),
                        f"guards head offset {delta:+d}")
    return fn


_mutator("offset_shift_plus32", "E_OFFSET")(_shift_guards(32))
_mutator("offset_shift_minus32", "E_OFFSET")(_shift_guards(-32))


@_mutator("offset_overlap", "E_OFFSET")
def _m_overlap(call):
    role = "APPROVALS" if "APPROVALS" in call.spec.roles else "SAFE_TX"

    def patch(cd, lay):
        return set_word(cd, lay.head[role], get_word(cd, lay.head["GUARDS"]))

    return _patched(call, patch, f"{role} offset points at the guards tail")


@_mutator("offset_out_of_bounds", "E_OFFSET")
def _m_oob(call):
    return _patched(call, lambda cd, lay: set_word(cd, lay.head["GUARDS"],
                                                   32 * (lay.total_words + 8)),
                    "guards offset beyond dataSize")


@_mutator("offset_overflow", "E_OFFSET")
def _m_overflow(call):
    return _patched(call, lambda cd, lay: set_word(cd, lay.head["GUARDS"],
                                                   word_int(cd, lay.head["GUARDS"]) + (1 << 32)),
                    "guards offset + 2^32 (byte 27 of the word non-zero)")


@_mutator("offset_overflow@max", "E_OFFSET")
def _m_overflow_max(call):
    return _patched(call, lambda cd, lay: set_word(cd, lay.head["GUARDS"], UINT256_MAX - 31),
                    "guards offset 2^256 - 32")


def _padding_address(where: str):
    def fn(call):
        spec = call.spec

        def word(lay: Layout) -> Optional[int]:
            if where == "guard_target":
                return lay.guard_elems[0]
            if where == "guard_token":
                return lay.guard_elems[0] + 1
            if where == "head_proxy":
                return lay.head.get("ADDR_PROXY")
            if where == "head_target":
                return lay.head.get("ADDR_TARGET")
            if where == "head_safe":
                return lay.head.get("ADDR_SAFE")
            if where == "approval_target":
                return lay.approval_elems[0] if lay.approval_elems else None
            if where == "withdrawal_token":
                return lay.withdrawal_elems[0] + 1 if lay.withdrawal_elems else None
            if where == "safe_tx_to":
                return lay.tail.get("SAFE_TX")
            if where == "safe_refund_receiver":
                return lay.tail["SAFE_TX"] + 8 if "SAFE_TX" in lay.tail else None
            raise AssertionError(where)

        if where == "approval_target" and not _ensure_approval(call):
            return None
        if where == "withdrawal_token":
            if spec.family == "SR":
                return None
            if not call.withdrawals:
                call.withdrawals.append(Balance(DEAD, USDC, 1))
        k = word(layout(call))
        if k is None:
            return None
        return _patched(call, lambda cd, lay: set_byte(cd, k, 11, 0x01),
                        f"address word {k} ({where}) with byte 11 set")
    return fn


_mutator("padding_address", "E_PAD_ADDR")(_padding_address("guard_target"))
for _w in ("guard_token", "head_proxy", "head_target", "head_safe", "approval_target",
           "withdrawal_token", "safe_tx_to", "safe_refund_receiver"):
    _mutator(f"padding_address@{_w}", "E_PAD_ADDR")(_padding_address(_w))


def _padding_uint8(where: Optional[str]):
    def fn(call):
        spec = call.spec
        choice = where
        if choice is None:
            choice = ("meta_decimals" if spec.meta else
                      "permit_v" if spec.family == "PR" else
                      "safe_operation" if spec.family == "SR" else None)
        if choice is None:
            return None
        if choice == "meta_decimals":
            if not spec.meta:
                return None
            k = lambda lay: lay.meta_elems[0] + 1  # noqa: E731
        elif choice == "permit_v":
            if spec.family != "PR":
                return None
            _ensure_approval(call)
            k = lambda lay: lay.permit_elems[0] + 1  # noqa: E731
        else:
            if spec.family != "SR":
                return None
            k = lambda lay: lay.tail["SAFE_TX"] + 3  # noqa: E731
        return _patched(call, lambda cd, lay: set_byte(cd, k(lay), 30, 0x01),
                        f"uint8 word ({choice}) with byte 30 set")
    return fn


_mutator("padding_uint8", "E_PAD_UINT")(_padding_uint8(None))
for _w in ("meta_decimals", "permit_v", "safe_operation"):
    _mutator(f"padding_uint8@{_w}", "E_PAD_UINT")(_padding_uint8(_w))


@_mutator("padding_bool", "E_BOOL")
def _m_bool(call):
    if not _ensure_approval(call):
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.approval_elems[0] + 3, 2),
                    "useTransfer == 2")


@_mutator("padding_bool@high", "E_BOOL")
def _m_bool_high(call):
    if not _ensure_approval(call):
        return None
    return _patched(call, lambda cd, lay: set_byte(cd, lay.approval_elems[0] + 3, 0, 0x80),
                    "useTransfer with the top byte set")


def _padding_length(where: str):
    def fn(call):
        spec = call.spec
        if where == "approvals" and not _ensure_approval(call):
            return None
        if where == "permits" and spec.family != "PR":
            return None
        if where in ("meta", "symbol") and not spec.meta:
            return None
        if where in ("data", "withdrawals", "approvals") and spec.family == "SR":
            return None
        if where in ("safe_data", "safe_sigs") and spec.family != "SR":
            return None

        def k(lay: Layout) -> int:
            return {
                "guards": lambda: lay.tail["GUARDS"],
                "meta": lambda: lay.tail.get("META"),
                "symbol": lambda: lay.meta_elems[0] + 2,
                "approvals": lambda: lay.tail.get("APPROVALS"),
                "permits": lambda: lay.tail.get("PERMITS"),
                "data": lambda: lay.data_len,
                "withdrawals": lambda: lay.tail.get("WITHDRAWALS"),
                "safe_data": lambda: lay.safe_data_len,
                "safe_sigs": lambda: lay.safe_sigs_len,
            }[where]()

        return _patched(call, lambda cd, lay: set_byte(cd, k(lay), 27, 0x01),
                        f"{where} length word + 2^32 (byte 27 set)")
    return fn


_mutator("padding_length", "E_PAD_UINT")(_padding_length("guards"))
for _w in ("meta", "symbol", "approvals", "permits", "data", "withdrawals", "safe_data",
           "safe_sigs"):
    _mutator(f"padding_length@{_w}", "E_PAD_UINT")(_padding_length(_w))


@_mutator("truncated", "E_TRUNC")
def _m_trunc(call):
    sc = scenario(call)
    sc.calldata = sc.calldata[:-32]
    return sc, "last word removed"


@_mutator("truncated@half", "E_TRUNC")
def _m_trunc_half(call):
    sc = scenario(call)
    n = (len(sc.calldata) - 4) // 32
    sc.calldata = sc.calldata[: 4 + 32 * (n // 2)]
    return sc, "cut at half of the words"


@_mutator("truncated@head", "E_TRUNC")
def _m_trunc_head(call):
    sc = scenario(call)
    sc.calldata = sc.calldata[: 4 + 32 * (len(call.spec.params) - 1)]
    return sc, "calldata ends inside the head (finalize)"


@_mutator("trailing_garbage", "E_TRAILING")
def _m_trailing(call):
    sc = scenario(call)
    sc.calldata += b"\xde" * 32
    return sc, "one extra word after the last tail"


@_mutator("meta_len_mismatch", "E_META_LEN")
def _m_meta_len(call):
    if not call.is_meta:
        return None
    call.meta.append(Meta("XTRA", 6))
    return scenario(call), "len(meta) == len(guards) + 1"


@_mutator("meta_len_mismatch@fewer", "E_META_LEN")
def _m_meta_len_fewer(call):
    if not call.is_meta:
        return None
    call.meta.pop()
    return scenario(call), "len(meta) == len(guards) - 1"


@_mutator("meta_head_not_0x40", "E_META_HEAD")
def _m_meta_head(call):
    if not call.is_meta:
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.meta_elems[0], 0x60),
                    "meta[0] head word 0x60")


@_mutator("meta_offset_table", "E_OFFSET")
def _m_meta_table(call):
    if not call.is_meta:
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.meta_offsets[0],
                                                   word_int(cd, lay.meta_offsets[0]) + 32),
                    "meta offset table entry 0 + 32")


@_mutator("meta_offset_table@last", "E_OFFSET")
def _m_meta_table_last(call):
    if not call.is_meta:
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.meta_offsets[-1],
                                                   word_int(cd, lay.meta_offsets[-1]) - 32),
                    "last meta offset table entry - 32")


@_mutator("symbol_empty", "E_SYMBOL")
def _m_sym_empty(call):
    if not call.is_meta:
        return None
    call.meta[-1] = Meta("", call.meta[-1].decimals)
    return scenario(call), "last meta symbol empty (canonical 3-word element)"


@_mutator("symbol_17_chars", "E_SYMBOL")
def _m_sym_17(call):
    if not call.is_meta:
        return None
    call.meta[0] = Meta("ABCDEFGHIJKLMNOPQ", call.meta[0].decimals)
    return scenario(call), "meta[0] symbol of 17 chars"


@_mutator("symbol_nonprintable", "E_SYMBOL")
def _m_sym_nl(call):
    if not call.is_meta:
        return None
    call.meta[0] = Meta("US\nDC", call.meta[0].decimals)
    return scenario(call), "meta[0] symbol with a newline"


@_mutator("symbol_nonprintable@del", "E_SYMBOL")
def _m_sym_del(call):
    if not call.is_meta:
        return None
    call.meta[0] = Meta("US\x7fDC", call.meta[0].decimals)
    return scenario(call), "meta[0] symbol with 0x7f"


@_mutator("symbol_padding", "E_SYMBOL")
def _m_sym_pad(call):
    if not call.is_meta:
        return None
    ln = len(_symbol_bytes(call.meta[0].symbol))
    return _patched(call, lambda cd, lay: set_byte(cd, lay.meta_elems[0] + 3, ln, 0x41),
                    "meta[0] symbol with a non-zero byte after its length")


@_mutator("decimals_78", "E_DECIMALS")
def _m_dec78(call):
    if not call.is_meta:
        return None
    call.meta[0] = Meta(call.meta[0].symbol, 78)
    return scenario(call), "meta[0] decimals 78"


@_mutator("native_meta_mismatch", "E_NATIVE_META")
def _m_native_meta(call):
    if not call.is_meta:
        return None
    call.guards[0].token = NATIVE
    call.meta[0] = Meta("ETH", 6)
    return scenario(call), "native guard with meta (ETH, 6)"


@_mutator("native_meta_mismatch@symbol", "E_NATIVE_META")
def _m_native_meta_sym(call):
    if not call.is_meta:
        return None
    call.guards[0].token = NATIVE
    call.meta[0] = Meta("WETH", 18)
    return scenario(call), "native guard with meta (WETH, 18)"


@_mutator("three_approvals", "E_APPROVALS_MAX")
def _m_three(call):
    if not _ensure_approval(call):
        return None
    while len(call.approvals) < 3:
        call.approvals.append(Approval(Balance(UNISWAP_ROUTER, USDC, 10**6), True))
    _ensure_approval(call)
    return scenario(call), "3 approvals"


@_mutator("negative_approval", "E_APPROVAL_NEG")
def _m_neg(call):
    if not _ensure_approval(call):
        return None
    call.approvals[0].balance.amount = -1
    return scenario(call), "approval amount -1"


@_mutator("permits_len", "E_PERMITS_LEN")
def _m_permits(call):
    if call.family != "PR":
        return None
    _ensure_approval(call)
    call.permits = []
    return scenario(call), "approvals >= 1, permits == 0"


@_mutator("permits_len@more", "E_PERMITS_LEN")
def _m_permits_more(call):
    if call.family != "PR":
        return None
    call.permits = [Permit(2**40, 27, b"\x11" * 32, b"\x22" * 32)
                    for _ in range(len(call.approvals) + 1)]
    return scenario(call), "permits == approvals + 1"


@_mutator("safe_operation", "E_SAFE_OP")
def _m_safe_op(call):
    if call.family != "SR":
        return None
    call.safe_tx.operation = 1
    return scenario(call), "SafeTx.operation DELEGATECALL"


@_mutator("safe_offsets", "E_OFFSET")
def _m_safe_off(call):
    if call.family != "SR":
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.tail["SAFE_TX"] + 2, 0x180),
                    "SafeTx.data offset 0x180")


@_mutator("safe_offsets@signatures", "E_OFFSET")
def _m_safe_off_sig(call):
    if call.family != "SR":
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.tail["SAFE_TX"] + 9,
                                                   word_int(cd, lay.tail["SAFE_TX"] + 9) + 32),
                    "SafeTx.signatures offset + 32")


@_mutator("safe_offsets@head", "E_OFFSET")
def _m_safe_off_head(call):
    if call.family != "SR":
        return None
    return _patched(call, lambda cd, lay: set_word(cd, lay.head["SAFE_TX"],
                                                   word_int(cd, lay.head["SAFE_TX"]) + 32),
                    "safeTx head offset + 32")


@_mutator("bytes_padding", "E_BYTES_PAD")
def _m_bytes_pad(call):
    if call.family == "SR":
        call.safe_tx.data = b"\xab" * 5
        return _patched(call, lambda cd, lay: set_byte(cd, lay.safe_data_len + 1, 31, 0x01),
                        "SafeTx.data (5 bytes) with a non-zero padding byte")
    call.data = b"\xab" * 5
    return _patched(call, lambda cd, lay: set_byte(cd, lay.data_len + 1, 31, 0x01),
                    "data (5 bytes) with a non-zero padding byte")


@_mutator("bytes_padding@signatures", "E_BYTES_PAD")
def _m_bytes_pad_sig(call):
    if call.family != "SR":
        return None
    call.safe_tx.signatures = b"\x5a" * 65
    return _patched(call, lambda cd, lay: set_byte(cd, lay.safe_sigs_len + 3, 1, 0x01),
                    "SafeTx.signatures (65 bytes) with a non-zero padding byte")


@_mutator("fallback_empty", "F_EMPTY")
def _m_fb_empty(call):
    call.guards = []
    call.meta = []
    return scenario(call), "0 guards"


@_mutator("fallback_nine", "F_TOO_MANY")
def _m_fb_nine(call):
    _nine_guards(call)
    return scenario(call), "9 guards"


@_mutator("fallback_then_error_is_error", "E_TRAILING")
def _m_fb_then_err(call):
    _nine_guards(call)
    sc = scenario(call)
    sc.calldata += b"\xde" * 32
    return sc, "9 guards (sticky FALLBACK) followed by trailing garbage"


@_mutator("fallback_then_error_is_error@empty_padding", "E_PAD_UINT")
def _m_fb_empty_then_err(call):
    call.guards = []
    call.meta = []
    k_role = "WITHDRAWALS" if call.family != "SR" else None

    def patch(cd, lay):
        k = lay.tail[k_role] if k_role else lay.safe_sigs_len
        return set_byte(cd, k, 0, 0x01)

    return _patched(call, patch, "0 guards (sticky) then a bad length word")


@_mutator("datasize_short", "E_DATASIZE")
def _m_datasize_short(call):
    sc = scenario(call)
    sc.knobs["data_size"] = 3
    return sc, "dataSize < 4 with a readable selector (model-only)"


@_mutator("datasize_below_calldata", "E_SEQ")
def _m_datasize_below(call):
    if call.family == "SR":
        call.safe_tx.signatures = b""
    else:
        call.withdrawals = []
    sc = scenario(call)
    sc.knobs["data_size"] = len(sc.calldata) - 32
    return sc, "last word (an empty tail's length) lies beyond dataSize (model-only)"


@_mutator("meta_offset_table@overflow", "E_OFFSET")
def _m_meta_table_overflow(call):
    if not call.is_meta:
        return None
    return _patched(call, lambda cd, lay: set_byte(cd, lay.meta_offsets[0], 0, 0x01),
                    "meta offset table entry 0 with a non-zero upper byte")


@_mutator("safe_offsets@signatures_overflow", "E_OFFSET")
def _m_safe_sig_overflow(call):
    if call.family != "SR":
        return None
    return _patched(call, lambda cd, lay: set_byte(cd, lay.tail["SAFE_TX"] + 9, 0, 0x01),
                    "SafeTx.signatures offset word with a non-zero upper byte")


@_mutator("padding_address@safe_gas_token", "E_PAD_ADDR")
def _m_pad_gas_token(call):
    if call.family != "SR":
        return None
    return _patched(call, lambda cd, lay: set_byte(cd, lay.tail["SAFE_TX"] + 7, 0, 0xFF),
                    "SafeTx.gasToken with a non-zero upper byte")


@_mutator("truncated@safe_head", "E_TRUNC")
def _m_trunc_safe_head(call):
    if call.family != "SR":
        return None
    sc = scenario(call)
    sc.calldata = sc.calldata[: 4 + 32 * (layout(call).tail["SAFE_TX"] + 10)]
    return sc, "only 10 of the 11 SafeTx head words fit (E_TRUNC at the SafeTx first word)"
