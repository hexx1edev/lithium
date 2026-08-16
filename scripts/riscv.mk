export ARCH_CFLAGS := \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany \
	-mno-relax

export ARCH_LDFLAGS := \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany \
	-mno-relax \
	-Wl,--no-relax

export CC := riscv64-unknown-elf-gcc
export LD := riscv64-unknown-elf-gcc
export AR := riscv64-unknown-elf-ar
export OBJCOPY := riscv64-unknown-elf-objcopy