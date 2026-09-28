import shutil
import subprocess
import sys
from pathlib import Path

import pytest

from ragger.conftest import configuration
from speculos.client import SpeculosInstance

from .helpers import sync_speculos_nvram
from .utils import WalletAddr


###########################
### CONFIGURATION START ###
###########################

# You can configure optional parameters by overriding the value of ragger.configuration.OPTIONAL_CONFIGURATION
# Please refer to ragger/conftest/configuration.py for their descriptions and accepted values

configuration.OPTIONAL.MAIN_APP_DIR = "tests/.test_dependencies/"

configuration.OPTIONAL.BACKEND_SCOPE = "class"


#########################
### CONFIGURATION END ###
#########################

# Pull all features from the base ragger conftest using the overridden configuration
pytest_plugins = ("ragger.conftest.base_conftest", )

# Speculos 0.27 re-maps the app from its ELF file at every os_lib_end, reverting NVRAM;
# helpers.sync_speculos_nvram() copies NVRAM back into main.elf so settings persist across calls.
SPECULOS_RUN_ROOT = Path(__file__).parent / "snapshots-tmp"
_speculos_runs = {"count": 0}


def _start_speculos(self: SpeculosInstance) -> None:
    _speculos_runs["count"] += 1
    run_dir = SPECULOS_RUN_ROOT / f"speculos-{_speculos_runs['count']:03d}"
    shutil.rmtree(run_dir, ignore_errors=True)
    run_dir.mkdir(parents=True)
    main_elf = run_dir / "main.elf"
    shutil.copyfile(self.app, main_elf)
    log_path = run_dir / "speculos.log"
    cmd = [sys.executable or "python3", "-m", "speculos", *self.args, str(main_elf)]
    with open(log_path, "wb") as log:
        self.process = subprocess.Popen(cmd, stdout=log, stderr=subprocess.STDOUT,  # noqa: S603
                                        cwd=run_dir)
    self.erc8009_run_dir = run_dir
    self.erc8009_main_elf = main_elf
    self.erc8009_log_path = log_path
    self._wait_until_ready()  # pylint: disable=protected-access


SpeculosInstance.start = _start_speculos


@pytest.fixture(scope=configuration.OPTIONAL.BACKEND_SCOPE)
def additional_speculos_arguments():
    return ["--save-nvram"]


@pytest.fixture(autouse=True)
def speculos_output(request):
    if "backend" not in request.fixturenames:
        yield None
        return
    backend = request.getfixturevalue("backend")
    sync_speculos_nvram(backend)
    log_path = backend._client.erc8009_log_path  # pylint: disable=protected-access
    start = log_path.stat().st_size
    yield log_path
    # same directory as the test's tmp snapshots: test_x[stax] -> test_x,
    # test_x[stax-<param>] -> test_x_<param>
    base, _, params = request.node.name.partition("[")
    params = [p for p in params.rstrip("]").split("-") if p and p != backend.device.name]
    out_dir = SPECULOS_RUN_ROOT / backend.device.name / "_".join([base] + params)
    out_dir.mkdir(parents=True, exist_ok=True)
    with open(log_path, "rb") as src:
        src.seek(start)
        (out_dir / "speculos.log").write_bytes(src.read())


@pytest.fixture
def wallet_addr(backend):
    return WalletAddr(backend)
