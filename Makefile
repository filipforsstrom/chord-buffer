# Project Name
TARGET = ChordBuffer

# Sources
CPP_SOURCES = main.cpp register.cpp synth.cpp midi.cpp seq.cpp hid.cpp

# Library Locations
LIBDAISY_DIR = ${DAISYEXAMPLES_PATH}/libDaisy
DAISYSP_DIR = ${DAISYEXAMPLES_PATH}/DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
