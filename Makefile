# System specifications
F_CRYSTAL      = 8000000L
F_CPU          = 72000000L
SYSCLOCK       = SYSCLK_FREQ_72MHz
FAMILY         = f10x
DENSITY        = md
MEMORY_MODE    = flash
# USB            = enabled
SAMPLE_RATE    = 8192

APPLICATION    = TRUE

# Preferred upload command
UPLOAD_COMMAND  = upload_combo_jtag

# Packages to build
TARGET         = batumi
BOOTLOADER     = batumi_bootloader
PACKAGES       = . drivers stmlib/utils stmlib/system
RESOURCES      = resources
ORIGINAL_BIN  = $(RESOURCES)/original_firmware.bin

include stmlib/makefile.inc

# Rule for uploading the original firmware
upload_original_serial:
	python2.7 $(STM32LOADER_PATH)stm32loader.py \
		-p $(PGM_SERIAL_PORT) \
		-b $(PGM_SERIAL_BAUD_RATE) \
		-e $(PGM_SERIAL_VERIFY) \
		-a $(BASE_ADDRESS) \
		-w $(ORIGINAL_BIN)

upload_original_jtag:
	openocd \
	-f $(OPENOCD_SCRIPTS_PATH)interface.cfg \
	-f $(OPENOCD_SCRIPTS_PATH)stm32$(FAMILY).cfg \
	-f $(OPENOCD_SCRIPTS_PATH)prelude_$(FAMILY).cfg \
	-f $(OPENOCD_SCRIPTS_PATH)erase_$(FAMILY).cfg \
	-c "flash write_bank 0 $(ORIGINAL_BIN) 0x0" \
	-c "verify_image $(ORIGINAL_BIN)" \
	-f $(OPENOCD_SCRIPTS_PATH)postlude.cfg
