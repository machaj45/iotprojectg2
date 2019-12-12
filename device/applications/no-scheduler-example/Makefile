# name of your application
APPLICATION = no-scheduler-example

# platform for which the application will be built
PLATFORM ?= octa

# use the SBSFU bootloader
BOOTLOADER ?= 0
SHIELDS += Murata-dualstack
MURATA_CONNECTOR = 1
LORAWAN_APPLICATION = theultimateapplication

# LORAWAN_APPLICATION = lpiot

Device = 3

SHIELDS += SPG30
SPG30_CONNECTOR = 3
#LOW_POWER = 1

SHIELD_DEFINES += -DDevice=$(Device)

# Path to the octa-stack base directory:
STACKBASE ?= ../..

# Set PRINT_FLOATS to 1 to enable linker flags enabling float printf & sprintf
PRINT_FLOATS = 1

include $(STACKBASE)/core/Makefile.core