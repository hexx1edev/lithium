#include "early_probe.h"

#include <stddef.h>

#include <printf.h>

#include <drivers/ns16550a/ns16550a.h>
#include <kernel/boot_info.h>

typedef struct {
    const char* compatible;
    bool (*probe)(const void* fdt);
    void (*putc)(char c);
} console_driver;

// list of console drivers
static const console_driver consoles[] = {
    { "ns16550a", ns16550a_probe_fdt, ns16550a_putc },
};

bool drivers_early_probe() {
    if (!info || !info->fdt)
        return false;

    for (size_t i = 0; i < sizeof(consoles) / sizeof(consoles[0]); i++) {
        if (!consoles[i].probe(info->fdt))
            continue;

        printf_set_callback(consoles[i].putc);
        printf("[probe] early console: %s\n", consoles[i].compatible);

        return true;
    }

    return false;
}
