import pytest

from ragger.conftest import configuration
from .utils import WalletAddr


###########################
### CONFIGURATION START ###
###########################

# You can configure optional parameters by overriding the value of ragger.configuration.OPTIONAL_CONFIGURATION
# Please refer to ragger/conftest/configuration.py for their descriptions and accepted values

configuration.OPTIONAL.APP_DIR = "tests/.test_dependencies/"

configuration.OPTIONAL.LOAD_MAIN_APP_AS_LIBRARY = True

configuration.OPTIONAL.BACKEND_SCOPE = "class"


#########################
### CONFIGURATION END ###
#########################

# Pull all features from the base ragger conftest using the overridden configuration
pytest_plugins = ("ragger.conftest.base_conftest", )

@pytest.fixture
def wallet_addr(backend):
    return WalletAddr(backend)
