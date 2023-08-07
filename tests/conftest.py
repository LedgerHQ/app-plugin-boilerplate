from ragger.conftest import configuration

###########################
### CONFIGURATION START ###
###########################

# You can configure optional parameters by overriding the value of ragger.configuration.OPTIONAL_CONFIGURATION
# Please refer to ragger/conftest/configuration.py for their descriptions and accepted values

# configuration.OPTIONAL.APP_NAME = "Ethereum"
# configuration.OPTIONAL.APP_DIR="tests/bin/"

configuration.OPTIONAL.SIDELOADED_APPS = {
    "PluginBoilerplate": "PluginBoilerplate",
}

configuration.OPTIONAL.SIDELOADED_APPS_DIR = "tests/lib_binaries"

configuration.OPTIONAL.BACKEND_SCOPE = "class"

#########################
### CONFIGURATION END ###
#########################

# Pull all features from the base ragger conftest using the overridden configuration
pytest_plugins = ("ragger.conftest.base_conftest", )
