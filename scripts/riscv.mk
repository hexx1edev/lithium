export ARCH_CFLAGS := \
	--target=riscv64-unknown-elf \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany

export ARCH_LDFLAGS := \
	--target=riscv64-unknown-elf \
    -march=rv64gc \
    -mabi=lp64 \
	-mcmodel=medany