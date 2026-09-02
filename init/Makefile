INIT_BUILD := $(BUILD_DIR)/init
RAMDISK_STAGING := $(BUILD_DIR)/ramdisk

SOURCES_ASM := $(wildcard *.S)
OBJECTS := $(patsubst %, $(INIT_BUILD)/%.o, $(SOURCES_ASM))

all: $(RAMDISK_STAGING)/init

$(RAMDISK_STAGING)/init: $(INIT_BUILD)/init.elf
	@mkdir -p $(RAMDISK_STAGING)
	@cp $< $@

$(INIT_BUILD)/init.elf: $(OBJECTS)
	@echo "--> Linking $(shell realpath --relative-to="$(PROJECT_ROOT)" "$@")"
	@mkdir -p $(INIT_BUILD)
	@$(LD) $(LDFLAGS) -T link.ld $^ -o $@

$(INIT_BUILD)/%.o: %
	@mkdir -p $(dir $@)
	@echo "-> Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@
