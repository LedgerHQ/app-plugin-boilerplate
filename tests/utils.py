import os
import re
from pathlib import Path
from typing import Optional

from ledger_app_clients.ethereum.client import EthAppClient
import ledger_app_clients.ethereum.response_parser as ResponseParser

DERIVATION_PATH = "m/44'/60'/0'/0/0"
makefile_relative_path = "../Makefile"

makefile_path = (Path(os.path.dirname(os.path.realpath(__file__))) / Path(makefile_relative_path)).resolve()

pattern = r'.*APPNAME.*=.*'

default_strip_parameter = " \t\n\r\x0b\x0c"


def get_appname_from_makefile() -> str:
    '''
    Parse the app Makefile to automatically get the APPNAME value
    '''
    APPNAME: Optional[str] = None
    with open(makefile_path) as file:
        for line in file:
            if re.search(pattern, line):
                _, var = line.partition("=")[::2]
                APPNAME = var.strip(default_strip_parameter + '"')

    if APPNAME is None:
        raise AssertionError("Unable to find APPNAME in the Makefile")

    return APPNAME


class WalletAddr:
    client: EthAppClient

    def __init__(self, backend):
        self.client = EthAppClient(backend)

    def get(self, path=DERIVATION_PATH) -> bytes:
        with self.client.get_public_addr(display=False, bip32_path=path):
            pass
        return ResponseParser.pk_addr(self.client.response().data)[1]
