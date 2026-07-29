LIBK_BUILD := $(BUILD_DIR)/libk

SOURCES := $(shell find . -name "*.c")
OBJECTS := $(patsubst %, $(LIBK_BUILD)/%.o, $(SOURCES))

all: $(BUILD_DIR)/libk.a

$(BUILD_DIR)/libk.a: $(OBJECTS)
	@echo "--> Linking $(shell realpath --relative-to="$(PROJECT_ROOT)" "$@")"
	@mkdir -p $(BUILD_DIR)
	@$(AR) rcs $@ $^

$(LIBK_BUILD)/%.o: %
	@echo "-> Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@