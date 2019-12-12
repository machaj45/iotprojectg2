# name of your application
APPLICATION = murata-dualstack-example-ns

# platform for which the application will be built
PLATFORM ?= octa

# Add shields used in application
# Murata
SHIELDS += Murata-dualstack
MURATA_CONNECTOR = 1
LORAWAN_APPLICATION = theultimateapplication

Device = 3

LOW_POWER = 1

# Path to the octa-stack base directory:
STACKBASE ?= ../..

SHIELD_DEFINES += -DDevice=$(Device)

include $(STACKBASE)/core/Makefile.core


