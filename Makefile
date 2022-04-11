# Name of your project, will set the name of your ROM.
PROJECT_NAME := CrossConnect
# Run "rgbfix --mbc-type help" for possible MBC types
MBC     := MBC5+RAM+BATTERY
# Target should be a combination of DMG, CGB and SGB
TARGETS := DMG CGB

include gbsdk/rules.mk

# Game fits in 32k, so put everything in ROM0
LDFLAGS += --tiny

FIXFLAGS += --non-japanese
FIXFLAGS += --rom-version 0
FIXFLAGS += --new-licensee HB

# Convert levels
$(BUILD)/levels/%.bin: levelgen/levels/%.txt
	@echo Converting $<
	@mkdir -p $(dir $@)
	@python3 levelgen/levelconvert.py $< $@

# Convert tilemaps
$(BUILD)/assets/%.2bpp $(BUILD)/assets/%.tilemap: tilemaps/%.png
	@echo Converting $<
	@mkdir -p $(dir $@)
	$(Q)rgbgfx $< -u -o $(BUILD)/assets/$*.2bpp -t $(BUILD)/assets/$*.tilemap