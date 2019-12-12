# name of your application
APPLICATION = iotproject

# platform for which the application will be built
PLATFORM ?= octa
PRINT_FLOATS = 1

SHIELDS += Murata-dualstack
MURATA_CONNECTOR = 1

Device = 3
# 1 jan
# 2 jola
# 3 ruben

USE_BOOTLOADER=1
SHIELDS += SPG30
SPG30_CONNECTOR = 3

LORAWAN_APPLICATION = lpiot
# Path to the octa-stack base directory:
STACKBASE ?= ../..

SHIELD_DEFINES += -DSPG30_CONNECTOR=$(SPG30_CONNECTOR)

SHIELD_DEFINES += -DDevice=$(Device)

include $(STACKBASE)/core/Makefile.core
