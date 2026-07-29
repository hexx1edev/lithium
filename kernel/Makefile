KERNEL_BUILD := $(BUILD_DIR)/kernel

SOURCES_C := $(shell find . \
  -path './arch' -prune -o \
  -type f -name "*.c" -print)
SOURCES_ARCH_C=$(shell find arch/$(ARCH) -name "*.c")

SOURCES_ASM := $(shell find . \
  -path './arch' -prune -o \
  -type f -name "*.S" -print)
SOURCES_ARCH_ASM=$(shell find arch/$(ARCH) -name "*.S")

OBJECTS_C := $(patsubst %, $(KERNEL_BUILD)/%.o, $(SOURCES_C) $(SOURCES_ARCH_C))
OBJECTS_ASM := $(patsubst %, $(KERNEL_BUILD)/%.o, $(SOURCES_ASM) $(SOURCES_ARCH_ASM))

all: $(BUILD_DIR)/kernel.elf

$(BUILD_DIR)/kernel.elf: $(OBJECTS_C) $(OBJECTS_ASM) $(BUILD_DIR)/libk.a $(BUILD_DIR)/libfdt.a
	@echo "--> Linking $(shell realpath --relative-to="$(PROJECT_ROOT)" "$@")"
	@mkdir -p $(BUILD_DIR)
	@$(LD) $(LDFLAGS) -T arch/$(ARCH)/link.ld $^ -o $@

$(KERNEL_BUILD)/%.o: %
	@mkdir -p $(dir $@)
	@echo "-> Compiling $<"
	@$(CC) $(CFLAGS) -I$(KERNEL_DIR) -c $< -o $@