# Name of your project, will set the name of your ROM.
PROJECT_NAME := CrossConnect
# Run "rgbfix --mbc-type help" for possible MBC types
MBC     := MBC5+RAM+BATTERY
# Target should be a combination of DMG, CGB and SGB
TARGETS := DMG CGB

include gbsdk/rules.mk

# Game fits in 32k, so put everything in ROM0
LDFLAGS += --tiny

FIXFLAGS += --rom-version 1
FIXFLAGS += --new-licensee HB

tools/gbcompress/gbcompress:
	@echo Building gbcompress
	@mkdir -p $(dir $@)
	@$(MAKE) -C tools/gbcompress

# Convert levels
$(BUILD)/levels/%.bin: levelgen/levels/%.txt
	@echo Converting $<
	@mkdir -p $(dir $@)
	@python3 levelgen/levelconvert.py $< $@

# Convert tilemaps
$(BUILD)/assets/%.gbcompress $(BUILD)/assets/%.tilemap: tilemaps/%.png
	@echo Converting $<
	@mkdir -p $(dir $@)
	$(Q)rgbgfx $< -u -o $(BUILD)/assets/$*.2bpp -t $(BUILD)/assets/$*.tilemap
	@./tools/gbcompress/gbcompress $(BUILD)/assets/$*.2bpp $@

# Convert compressed assets
$(BUILD)/assets/%.gbcompress: assets/%.c.png tools/gbcompress/gbcompress
	@echo Converting $<
	@mkdir -p $(dir $@)
	$(Q)rgbgfx $< -o $(BUILD)/assets/$*.2bpp
	@./tools/gbcompress/gbcompress $(BUILD)/assets/$*.2bpp $@

# Add this new clean as a prerequisite for the GBSDK clean.
clean: clean2
clean2:
	@$(MAKE) -C tools/gbcompress clean
