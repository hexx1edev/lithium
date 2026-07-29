LIBFDT_BUILD := $(BUILD_DIR)/libfdt

SOURCES := \
    fdt.c \
    fdt_ro.c \
    fdt_rw.c \
    fdt_wip.c \
    fdt_addresses.c \
    fdt_strerror.c

OBJECTS := $(patsubst %, $(LIBFDT_BUILD)/%.o, $(SOURCES))

all: $(BUILD_DIR)/libfdt.a

$(BUILD_DIR)/libfdt.a: $(OBJECTS)
	@echo "--> Linking $(shell realpath --relative-to="$(PROJECT_ROOT)" "$@")"
	@mkdir -p $(BUILD_DIR)
	@$(AR) rcs $@ $^

$(LIBFDT_BUILD)/%.o: %
	@echo "-> Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@