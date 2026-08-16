export ARCH_CFLAGS := \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany

export ARCH_LDFLAGS := \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany

export CC := riscv64-unknown-elf-gcc
export LD := riscv64-unknown-elf-gcc
export AR := riscv64-unknown-elf-ar
export OBJCOPY := riscv64-unknown-elf-objcopy