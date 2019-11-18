# name of your application
APPLICATION = iotproject

# platform for which the application will be built
PLATFORM ?= octa
PRINT_FLOATS = 1

SHIELDS += Murata-dualstack
MURATA_CONNECTOR = 2
SPG30_CONNECTOR = 3
LORAWAN_APPLICATION = lpiot
# Path to the octa-stack base directory:
STACKBASE ?= ../..

SHIELD_DEFINES += -DSPG30_CONNECTOR=$(SPG30_CONNECTOR)

include $(STACKBASE)/core/Makefile.core
