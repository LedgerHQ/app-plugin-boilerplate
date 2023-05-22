#*******************************************************************************
#   Ledger App
#   (c) 2017 Ledger
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif

include $(BOLOS_SDK)/Makefile.defines

# EDIT THIS: Put your plugin name
APPNAME = "Boilerplate"

ifeq ($(ETHEREUM_PLUGIN_SDK),)
ETHEREUM_PLUGIN_SDK=ethereum-plugin-sdk
endif

APP_LOAD_PARAMS += --appFlags 0x800 --path "44'/60'" --curve secp256k1

APP_LOAD_PARAMS += $(COMMON_LOAD_PARAMS)

APPVERSION_M     = 1
APPVERSION_N     = 0
APPVERSION_P     = 0
APPVERSION       = "$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)"

# EDIT THIS: Change the name of the gif, and generate you own GIFs!
ifeq ($(TARGET_NAME), TARGET_NANOS)
ICONNAME=icons/nanos_app_boilerplate.gif
else
ICONNAME=icons/nanox_app_boilerplate.gif
endif

################
# Default rule #
################
all: default

############
# Platform #
############

DEFINES   += OS_IO_SEPROXYHAL
DEFINES   += HAVE_SPRINTF
DEFINES   += LEDGER_MAJOR_VERSION=$(APPVERSION_M) LEDGER_MINOR_VERSION=$(APPVERSION_N) LEDGER_PATCH_VERSION=$(APPVERSION_P)
DEFINES   += IO_HID_EP_LENGTH=64

DEFINES   += UNUSED\(x\)=\(void\)x
DEFINES   += APPVERSION=\"$(APPVERSION)\"
CFLAGS    += -DAPPNAME=\"$(APPNAME)\"

ifneq (,$(filter $(TARGET_NAME),TARGET_NANOX TARGET_STAX))
DEFINES   += HAVE_BLE BLE_COMMAND_TIMEOUT_MS=2000
DEFINES   += HAVE_BLE_APDU # basic ledger apdu transport over BLE
endif

ifeq ($(TARGET_NAME),TARGET_NANOS)
DEFINES   += IO_SEPROXYHAL_BUFFER_SIZE_B=128
else
DEFINES   += IO_SEPROXYHAL_BUFFER_SIZE_B=300
endif

ifneq  ($(TARGET_NAME),TARGET_STAX)
DEFINES   += HAVE_BAGL
ifneq ($(TARGET_NAME),TARGET_NANOS)
DEFINES   += HAVE_GLO096
DEFINES   += HAVE_BAGL BAGL_WIDTH=128 BAGL_HEIGHT=64
DEFINES   += HAVE_BAGL_ELLIPSIS # long label truncation feature
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX
DEFINES   += HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX
DEFINES   += HAVE_UX_FLOW
endif
endif

# Enabling debug PRINTF
ifneq ($(DEBUG),0)
        DEFINES += HAVE_STACK_OVERFLOW_CHECK
        SDK_SOURCE_PATH  += lib_stusb lib_stusb_impl
        DEFINES   += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=4 IO_HID_EP_LENGTH=64 HAVE_USB_APDU

        ifeq ($(DEBUG),10)
                $(warning Using semihosted PRINTF. Only run with speculos!)
                CFLAGS    += -include src/dbg/debug.h
                DEFINES   += HAVE_PRINTF PRINTF=semihosted_printf
        else
                ifeq ($(TARGET_NAME),TARGET_NANOS)
                        DEFINES   += HAVE_PRINTF PRINTF=screen_printf
                else
                        DEFINES   += HAVE_PRINTF PRINTF=mcu_usb_printf
                endif

        endif
else
        DEFINES   += PRINTF\(...\)=
endif

##############
#  Compiler  #
##############
ifneq ($(BOLOS_ENV),)
$(info BOLOS_ENV=$(BOLOS_ENV))
CLANGPATH := $(BOLOS_ENV)/clang-arm-fropi/bin/
GCCPATH := $(BOLOS_ENV)/gcc-arm-none-eabi-5_3-2016q1/bin/
else
$(info BOLOS_ENV is not set: falling back to CLANGPATH and GCCPATH)
endif
ifeq ($(CLANGPATH),)
$(info CLANGPATH is not set: clang will be used from PATH)
endif
ifeq ($(GCCPATH),)
$(info GCCPATH is not set: arm-none-eabi-* will be used from PATH)
endif

CC       := $(CLANGPATH)clang

AS     := $(GCCPATH)arm-none-eabi-gcc

LD       := $(GCCPATH)arm-none-eabi-gcc
LDLIBS   += -lm -lgcc -lc

# import rules to compile glyphs(/pone)
include $(BOLOS_SDK)/Makefile.glyphs

### variables processed by the common makefile.rules of the SDK to grab source files and include dirs
APP_SOURCE_PATH  += src $(ETHEREUM_PLUGIN_SDK)
ifneq ($(TARGET_NAME), TARGET_STAX)
SDK_SOURCE_PATH  += lib_ux
endif
ifneq (,$(findstring HAVE_BLE,$(DEFINES)))
SDK_SOURCE_PATH  += lib_blewbxx lib_blewbxx_impl
endif

### initialize plugin SDK submodule if needed
ifneq ($(shell git submodule status | grep '^[-+]'),)
$(info INFO: Need to reinitialize git submodules)
$(shell git submodule update --init)
endif

load: all
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

# import generic rules from the sdk
include $(BOLOS_SDK)/Makefile.rules


#add dependency on custom makefile filename
dep/%.d: %.c Makefile

listvariants:
        # EDIT THIS: replace `boilerplate` by the lowercase name of your plugin
	@echo VARIANTS NONE boilerplate
