# Name of your project, will set the name of your ROM.
PROJECT_NAME := Template
# Run "rgbfix --mbc-type help" for possible MBC types
MBC     := ROM
# Target should be a combination of DMG, CGB and SGB
TARGETS := DMG

include gbsdk/rules.mk
