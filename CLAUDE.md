# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Lithium is a from-scratch RISC-V (rv64gc) kernel, currently targeting the QEMU
`virt` machine and booted via OpenSBI. It is bare-metal/freestanding: no libc,
no OS underneath it.

## Build commands

Built with a cross-compiling `clang`/LLVM toolchain (`--target=riscv64-unknown-elf`),
driven by a small recursive Makefile setup rooted at `Makefile` + `config.mk`.

- `make` (or `make kernel`) — builds `libk`, `libfdt`, then the kernel; final
  artifact is `build/kernel.elf`.
- `make clean` — removes `build/`.
- Sub-libraries can be built individually: `make libk`, `make libfdt`.

There is no test suite or linter in this repo.

### Running

Boot the kernel in QEMU:

```
qemu-system-riscv64 -machine virt -nographic -kernel build/kernel.elf
```

`-nographic` routes the emulated 16550 UART to the terminal. The kernel is
interrupt-driven and idles in `wfi` — type characters and they're echoed back
(proves the trap/PLIC/UART path works end-to-end). Exit QEMU with `Ctrl-A X`.

To inspect the DTB QEMU actually hands the kernel:
`qemu-system-riscv64 -machine virt -machine dumpdtb=virt.dtb -kernel build/kernel.elf`
then `dtc -I dtb -O dts virt.dtb`.

Useful inspection commands after a build:
- `readelf -h build/kernel.elf` — check the entry point (should be `0x80200000`).
- `llvm-objdump -d build/kernel.elf` — disassemble to confirm what actually got linked in; useful when something built doesn't match the source you expect (the `build/` tree is not automatically cleaned between edits, so stale `.o`/`.elf` files can persist if a rebuild is skipped).

## Architecture

**Directory layout**: `kernel/` has three buckets, all compiled with
`-I$(KERNEL_DIR)` (see `kernel/Makefile`) so any file includes any other by
its path relative to `kernel/` regardless of nesting (e.g.
`<kernel/hal.h>`, `<drivers/ns16550a/ns16550a.h>`, `"trap.h"` for a sibling).
- `kernel/kernel/`: portable, arch-agnostic core (`kmain.c`, `panic.c`,
  `fdt_util.c`, `boot_info.h`). No RISC-V-specific code may live here — note
  `boot_info` has an `arch` sub-struct (`arch_boot_info`, e.g. `riscv_hartid`)
  that only arch code should read.
- `kernel/hal/`: the portable HAL interface headers (`hal.h`, `irq.h`,
  `interrupts.h`) — declarations only, implemented per-arch.
- `kernel/drivers/`: portable device drivers usable from any arch
  (`ns16550a/`), plus `probe/` which owns the two driver tables.
- `kernel/arch/riscv/`: everything RISC-V-specific, including the interrupt
  controller (PLIC) — not every arch has one, so it's arch-local rather than
  a generic "driver".

**Boot flow**: QEMU loads `build/kernel.elf` and hands control to OpenSBI, which
reads the kernel's real ELF entry point and jumps to it in S-mode with
`a0 = hartid`, `a1 = pointer to the flattened device tree (FDT/DTB)`. This
lands in `_start` (`kernel/arch/riscv/entry.S`), which halts any hart other
than 0, sets up the stack pointer (`_stack_top`, defined in the linker
script), and calls `arch_entry(hartid, fdt)` (`kernel/arch/riscv/entry.c`) —
the calling convention lines up so no argument shuffling is needed between
`_start` and `arch_entry`. `arch_entry` packs those two raw values into a
`struct boot_info` (`kernel/kernel/boot_info.h`) and calls
`kmain(const struct boot_info*)` (`kernel/kernel/kmain.c`). This is the boundary
that keeps `kmain.c` arch-agnostic: it never sees a hartid/fdt calling
convention, only the portable struct, and it never calls anything
RISC-V-specific directly — all of that goes through the HAL below.

**Linker script** (`kernel/arch/riscv/link.ld`): loads the kernel at the fixed
address `0x80200000` (the address OpenSBI's `fw_dynamic` firmware expects for
an S-mode payload on `virt`). `.text.init` (the section `entry.S` is placed
in) is explicitly `KEEP`'d first inside the `.text` output section — this
matters because `kernel/Makefile` links C objects before assembly objects, so
without forcing the order, `_start` can end up somewhere other than byte 0 of
`.text`. The linker script also carves out a 16 KiB stack (`_stack_bottom`/
`_stack_top`) after `.bss`.

**HAL** (`kernel/hal/*.h`, implemented per-arch — currently
`kernel/arch/riscv/hal.c`): the only arch-facing surface generic code is
allowed to call. `hal_init()` sets up traps and probes the interrupt
controller (reading the global `info`, so it takes no argument);
`hal_irq_register_handler`/`hal_irq_enable` route a device's FDT-reported irq
number to a handler (no-ops if the arch has no interrupt controller — tracked
by a local `s_plic_present` flag in `hal.c`, so callers don't need to
condition on whether one was found); `hal_enable_interrupts` unmasks
interrupts globally (call only once every boot-time handler is registered);
`hal_wait_for_interrupt` idles (`wfi` on RISC-V); `hal_halt` loops on that
forever. Adding a second arch means adding `kernel/arch/<arch>/hal.c`
implementing this same interface — nothing in `kernel/kernel/`,
`kernel/hal/`, or `kernel/drivers/` should need to change.

**Boot sequence in `kmain`**: `drivers_early_probe()` → `hal_init()` →
`drivers_probe()` → `hal_enable_interrupts()` → `hal_halt()`. `kmain` copies
the caller's `boot_info` into a global (`info`, declared `extern` in
`boot_info.h`) that the HAL and probe tables read, rather than threading it
through every call.

**Two-tier driver probing** (`kernel/drivers/probe/`) — the ordering exists
because nothing can be logged until a console is up, but IRQs can't be
registered until the HAL is up:
- `early_probe.c` runs *before* `hal_init()`. It holds a table of
  console-capable drivers `{compatible, probe, putc}` in descending priority
  order; the first that probes successfully gets installed as the `printf`
  callback (`printf_set_callback`, see `libk/printf.h`). It must stay
  polled-output-only — no traps or interrupts exist yet.
- `probe.c` runs *after* `hal_init()`. It holds the main table of
  `{compatible, init}` and does a real FDT walk (`fdt_next_node` over every
  node, matching each against the table with `fdt_node_check_compatible`), so
  multiple instances of a device are all initialized. Adding a driver to the
  system means adding one line to this table.
Both tables degrade gracefully when there's no FDT.

**Device discovery**: drivers never hardcode MMIO addresses — they take the
node offset the probe walk found and read it. `kernel/fdt_util.c` provides
`fdt_get_reg`/`fdt_get_irq`, shared helpers that resolve a node's
`reg`/`interrupts` properties (handling `#address-cells`/`#size-cells` from
the parent bus node).

**Drivers** (`kernel/drivers/`): no registration framework beyond the probe
tables above.
- `drivers/ns16550a/`: the UART, and the reason a driver has *two* entry
  points. `ns16550a_probe_fdt(fdt)` is the early/minimal one: it searches for
  the first `"ns16550a"` node and sets up just enough for `ns16550a_putc` to
  work as the early console. `ns16550a_init(fdt, node)` is the full one called
  by the post-HAL walk on a specific node: it re-reads `reg`/`interrupts`,
  registers `ns16550a_irq_handler` with the HAL, enables the IRQ, and only
  *then* unmasks RX-data-available in the UART's `IER` — enabling that any
  earlier would arm a device interrupt with no handler behind it.

**PLIC** (`kernel/arch/riscv/plic.{c,h}`) — arch-local, not a portable driver,
since not every arch has a PLIC. `plic_probe` finds the `"sifive,plic-1.0.0"`
node for its base address; register offsets (priority/enable/threshold/claim)
are the standard SiFive PLIC layout. Interrupt routing is a small
`irq -> handler` table (`plic_register_handler`/`plic_handle_interrupt`) so
the trap dispatcher doesn't need to know about specific devices.
**Only one hart/S-mode context is supported**: the PLIC context index for
hart 0's S-mode is hardcoded (`CONTEXT_HART0_S` in `plic.c`) rather than
parsed out of `interrupts-extended`, since the kernel is single-hart,
S-mode-only by design. `hal.c` is the only caller of `plic.*` from outside
`trap.c` — generic code goes through the HAL functions above instead.

No CLINT driver: under QEMU's default OpenSBI, the CLINT MMIO range is
PMP-restricted to M-mode only (confirmed via the boot log's domain dump), so
an S-mode kernel can't touch it directly — timer/IPI would have to go
through SBI calls instead, and that's out of scope for now.

**Trap/exception handling** (`kernel/arch/riscv/`): `trap_entry.S` is the
`stvec` target — it saves all GPRs + `sepc` to a `struct trap_frame` (defined
in `trap.h`, must stay in sync with the asm offsets) on the current stack,
calls `trap_dispatch()`, restores, and `sret`s. `trap.c`'s `trap_dispatch`
reads `scause`: a supervisor-external-interrupt cause hands off to
`plic_handle_interrupt()`; anything else that's an interrupt is logged as
unexpected (no timer/software interrupt sources are ever enabled); a
non-interrupt cause is an unhandled exception and calls `panic()`
(`kernel/panic.c` — prints via `vprintf` and halts; there's no user mode or
fault recovery yet). `trap_init()` (called from `hal_init`, not directly from
`kmain.c`) points `stvec` at `trap_entry` and sets `sie.SEIE`. CSR access
helpers live in `csr.h`.

**Sub-libraries** (`libk/`, `libfdt/`): built as static archives
(`build/libk.a`, `build/libfdt.a`) and linked into `kernel.elf` (see
`kernel/Makefile`'s prerequisites for `kernel.elf`). `libk` is a minimal
freestanding libc subset (`string.c`, `memory.c`, a vendored `printf`
implementation in `printf.c`/`printf.h` with float support compiled out).
`libfdt` is the upstream devicetree library; `fdt_rw.c` and `fdt_wip.c` are
built alongside the obviously-needed `fdt.c`/`fdt_ro.c`/`fdt_addresses.c`/
`fdt_strerror.c` because `fdt_addresses.c` pulls in `fdt_appendprop` (from
`fdt_rw.c`), which in turn pulls in `fdt_node_end_offset_` (from
`fdt_wip.c`) — the linker only complains about missing symbols once
something actually gets linked, so pulling in one more libfdt source file
can cascade into needing another.

**Arch abstraction**: `config.mk` includes `scripts/$(ARCH).mk` (currently
only `scripts/riscv.mk`, setting `-march=rv64gc -mabi=lp64 -mcmodel=medany`)
and sets `ARCH := riscv`. Per-component Makefiles compile
`kernel/arch/$(ARCH)/` in addition to their common sources, so adding a new
architecture means adding `scripts/<arch>.mk` and a `kernel/arch/<arch>/`
directory with at least `entry.S`, `entry.c` (building `boot_info`), `link.ld`,
and a `hal.c` implementing the `kernel/hal/*.h` interface.
