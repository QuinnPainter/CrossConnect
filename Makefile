# Name of your project, will set the name of your ROM.
PROJECT_NAME := CrossConnect
# Run "rgbfix --mbc-type help" for possible MBC types
MBC     := ROM
# Target should be a combination of DMG, CGB and SGB
TARGETS := DMG CGB

include gbsdk/rules.mk

$(BUILD)/levels/%.bin: levelgen/levels/%.txt
	@echo Converting $<
	@mkdir -p $(dir $@)
	@python3 levelgen/levelconvert.py $< $@