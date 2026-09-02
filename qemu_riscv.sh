#!/bin/sh
set -e

qemu-system-riscv64 -M virt -m 512M -nographic -bios default -kernel build/kernel.bin -initrd build/rd.img
