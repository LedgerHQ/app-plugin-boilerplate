# ****************************************************************************
#    Ledger App Boilerplate
#    (c) 2023 Ledger SAS.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ****************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif

include $(BOLOS_SDK)/Makefile.defines

########################################
#        Mandatory configuration       #
########################################
# Application name
APPNAME = "Plugin Boilerplate"

# Application version
APPVERSION_M = 1
APPVERSION_N = 0
APPVERSION_P = 0
APPVERSION = "$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)"

# Initialize plugin SDK submodule if needed
ifneq ($(shell git submodule status | grep '^[-+]'),)
$(info INFO: Need to reinitialize git submodules)
$(shell git submodule update --init)
endif

ifeq ($(ETHEREUM_PLUGIN_SDK),)
ETHEREUM_PLUGIN_SDK=ethereum-plugin-sdk
endif

# Application source files
APP_SOURCE_PATH += src $(ETHEREUM_PLUGIN_SDK)

# Application icons following guidelines:
# https://developers.ledger.com/docs/embedded-app/design-requirements/#device-icon
ICON_ID = $(shell echo -n "$(APPNAME)" | tr " " "_" | tr "[:upper:]" "[:lower:]")
ICON_NANOS = icons/nanos_app_$(ICON_ID).gif
ICON_NANOX = icons/nanox_app_$(ICON_ID).gif
ICON_NANOSP = $(ICON_NANOX)
ICON_STAX = icons/stax_app_$(ICON_ID).gif

ifeq ($(TARGET_NAME),TARGET_STAX)
	DEFINES += ICONGLYPH=C_stax_$(ICON_ID)_64px
	DEFINES += ICONBITMAP=C_stax_$(ICON_ID)_64px_bitmap
endif

# Application allowed derivation curves.
# Possibles curves are: secp256k1, secp256r1, ed25519 and bls12381g1
# If your app needs it, you can specify multiple curves by using:
# `CURVE_APP_LOAD_PARAMS = <curve1> <curve2>`
CURVE_APP_LOAD_PARAMS = secp256k1

# Application allowed derivation paths.
# You should request a specific path for your app.
# This serve as an isolation mechanism.
# Most application will have to request a path according to the BIP-0044
# and SLIP-0044 standards.
# If your app needs it, you can specify multiple path by using:
# `PATH_APP_LOAD_PARAMS = "44'/1'" "45'/1'"`
PATH_APP_LOAD_PARAMS = "44'/60'"   # purpose=coin(44) / coin_type=Testnet(1)

# Setting to allow building variant applications
# - <VARIANT_PARAM> is the name of the parameter which should be set
#   to specify the variant that should be build.
# - <VARIANT_VALUES> a list of variant that can be build using this app code.
#   * It must at least contains one value.
#   * Values can be the app ticker or anything else but should be unique.
VARIANT_PARAM = COIN
VARIANT_VALUES = pbol

# Enabling DEBUG flag will enable PRINTF and disable optimizations
#DEBUG = 1

########################################
#     Application custom permissions   #
########################################
# See SDK `include/appflags.h` for the purpose of each permission
#HAVE_APPLICATION_FLAG_DERIVE_MASTER = 1
#HAVE_APPLICATION_FLAG_GLOBAL_PIN = 1
#HAVE_APPLICATION_FLAG_BOLOS_SETTINGS = 1
HAVE_APPLICATION_FLAG_LIBRARY = 1

########################################
# Application communication interfaces #
########################################
#ENABLE_BLUETOOTH = 1
#ENABLE_NFC = 1

########################################
#         NBGL custom features         #
########################################
#ENABLE_NBGL_QRCODE = 1
#ENABLE_NBGL_KEYBOARD = 1
#ENABLE_NBGL_KEYPAD = 1

########################################
#          Features disablers          #
########################################
# These advanced settings allow to disable some feature that are by
# default enabled in the SDK `Makefile.standard_app`.

DISABLE_STANDARD_APP_FILES = 1

# To allow custom size declaration
#DISABLE_DEFAULT_IO_SEPROXY_BUFFER_SIZE = 1

# Will set all the following disablers
#DISABLE_STANDARD_APP_DEFINES = 1

DISABLE_STANDARD_SNPRINTF = 1
#DISABLE_STANDARD_USB = 1
DISABLE_STANDARD_WEBUSB = 1
DISABLE_STANDARD_BAGL_UX_FLOW = 1

include $(BOLOS_SDK)/Makefile.standard_app
