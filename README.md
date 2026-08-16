# Lithium

Lithium is a small and lightweight 64-bit monolithic multi-architecture kernel, written in C.

## How it works

All kernel sources are placed in `kernel` directory. libk and libfdt are placed in dedicated directories.

Lithium splits into 3 parts:
- Architecture-specific code
- Drivers code
- Generic kernel code

### Arch code

Architecture-specific code is placed in `arch/<architecture>` directory, it contains linker script,  
entry point and HAL implementation, that allows kernel do miscellaneous things without worrying,  
what platform it's running on.

### Drivers code

Drivers code is placed in `drivers` directory, it contains code, that allows kernel to  
interact(initialize, deinitialize, use) different devices without worrying about how they work.

To init drivers kernel calls probe, that walks over FDT and matches known device drivers.

### Generic kernel code

Generic kernel code is placed in `kernel` directory, it contains actual kernel code,  
that orchestrates drivers, manages memory, processes and power(in future), provides API for apps,  
handles exceptions and does generally all work.

### libfdt

libfdt is a small standalone library, that kernel uses to work with FDT.

### libk

libk implements essential functions from libc, that kernel uses.

## Building

Before build, you can set target architecture in `config.mk` by setting ARCH variable to one of the following values:

- riscv

Install `riscv64-unknown-elf-gcc and binutils, make, cpio`, run `make all` to build project.

## Testing

You can run riscv-built kernel in QEMU using  
`qemu-system-riscv64 -M virt -m 512M -nographic -bios default -kernel build/kernel.elf -initrd rd.img`.  
Feel free to add any device, that kernel supports, here's list of supported devices:

- UART NS16500A char dev (enabled in QEMU)
- PLIC (enabled in QEMU)