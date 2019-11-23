# name of your application
APPLICATION = iotproject

# platform for which the application will be built
PLATFORM ?= octa
PRINT_FLOATS = 1

SHIELDS += Murata-dualstack
MURATA_CONNECTOR = 1
Device = 1
USE_BOOTLOADER=1
SPG30_CONNECTOR = 3
LORAWAN_APPLICATION = lpiot
# Path to the octa-stack base directory:
STACKBASE ?= ../..

SHIELD_DEFINES += -DSPG30_CONNECTOR=$(SPG30_CONNECTOR)

SHIELD_DEFINES += -DDevice=$(Device)

include $(STACKBASE)/core/Makefile.core
