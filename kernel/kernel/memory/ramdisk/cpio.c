#include "cpio.h"
#include <libfdt.h>
#include <memory.h>
#include <printf.h>
#include <hal/mmu.h>
#include <kernel/memory.h>
#include <kernel/memory/vmm/vmm.h>

typedef struct {
    char magic[6];      // "070701"
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];   // size of file in hex
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];   // name length with null terminator
    char check[8];      // 0
} cpio_header_t;

typedef struct {
    cpio_header_t* ptr;
    size_t size;
} cpio_t;

static cpio_t cpio;

void hexdump_field(const char* name, const char field[8]) {
    printf("[cpio] %s raw bytes: ", name);
    for (int i = 0; i < 8; i++) {
        char c = field[i];
        if (c >= 32 && c < 127) printf("%c", c);
        else printf("\\x%02x", (unsigned char)c);
    }
    printf("\n");
}

bool cpio_parse_fdt(void* fdt) {
    int chosen_off = fdt_path_offset(fdt, "/chosen");
    if (chosen_off < 0)
        return false;

    int len;
    size_t start = 0;
    size_t end = 0;
    const void* prop;

    // initrd-start
    prop = fdt_getprop(fdt, chosen_off, "linux,initrd-start", &len);
    if (!prop)
        return false;

    if (len == 4) {
        start = fdt32_to_cpu(*(const fdt32_t*)prop);
    } else if (len == 8) {
        start = fdt64_to_cpu(*(const fdt64_t*)prop);
    } else {
        return false;
    }

    // initrd-end
    prop = fdt_getprop(fdt, chosen_off, "linux,initrd-end", &len);
    if (!prop)
        return false;

    if (len == 4) {
        end = fdt32_to_cpu(*(const fdt32_t*)prop);
    } else if (len == 8) {
        end = fdt64_to_cpu(*(const fdt64_t*)prop);
    } else {
        return -1;
    }

    printf("[cpio] found ramdisk at 0x%llx\n", start);

    // map ramdisk in memory
    vmm_map(PA2VA(start), start, end - start, PERM_MMIO);

    cpio_t probably_cpio = {
        (cpio_header_t*)PA2VA(start),
        end - start
    };

    if (memcmp(probably_cpio.ptr->magic, "070701", 6) == 0) {
        memcpy(&cpio, &probably_cpio, sizeof(cpio_t));
        return true;
    }

    printf("[cpio] invalid header magic: %c%c%c%c%c%c\n",
        probably_cpio.ptr->magic[0],
        probably_cpio.ptr->magic[1],
        probably_cpio.ptr->magic[2],
        probably_cpio.ptr->magic[3],
        probably_cpio.ptr->magic[4],
        probably_cpio.ptr->magic[5]);

    return false;
}

static inline uint32_t hex8_to_u32(const char field[8]) {
    char buf[9];
    memcpy(buf, field, 8);
    buf[8] = '\0';
    return (uint32_t)strtoul(buf, NULL, 16);
}

static inline size_t align4(size_t x) {
    return (x + 3) & ~((size_t)3);
}

cpio_handle_t cpio_lookup(const char* path) {
    const uint8_t* p = (const uint8_t*)cpio.ptr;
    const uint8_t* end = p + cpio.size;

    cpio_handle_t handle = {NULL, 0};

    while (p + sizeof(cpio_header_t) <= end) {
        const cpio_header_t* hdr = (const cpio_header_t *)p;

        uint32_t namesize = hex8_to_u32(hdr->namesize);
        uint32_t filesize = hex8_to_u32(hdr->filesize);

        const char* name = (const char*)p + sizeof(cpio_header_t);

        // end TRAILER
        if (strcmp(name, "TRAILER!!!") == 0)
            return handle;

        size_t header_and_name = sizeof(cpio_header_t) + namesize;
        size_t data_offset = align4(header_and_name);

        const uint8_t* data = p + data_offset;

        if (strcmp(name, path) == 0) {
            handle.size = filesize;
            handle.ptr = (void*)data;
            return handle;
        }

        // next entry
        size_t entry_size = data_offset + align4(filesize);
        p += entry_size;
    }
    return handle;
}