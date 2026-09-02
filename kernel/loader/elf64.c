#include "elf64.h"

#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <printf.h>
#include <util/math.h>
#include <kernel/memory.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

#define ELFCLASS64  2
#define ELFDATA2LSB 1

#define ET_EXEC     2
#define ET_DYN      3

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_PHDR     6
#define PT_GNU_STACK 0x6474e551

#define PF_X        0x1
#define PF_W        0x2
#define PF_R        0x4

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    // ...
} elf64_ehdr_t;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} elf64_phdr_t;

static const char* machine_to_name(uint16_t machine) {
    switch (machine) {
        case 0x3E:
            return "x86_64";
        case 0xB7:
            return "ARM64";
        case 0xF3:
            return "RISC-V";
        case 0x15:
            return "PowerPC";
        case 0x02:
            return "SPARC";
        case 0x03:
            return "x86/i386";
        case 0x28:
            return "ARM";
        default:
            return "unknown";
    }
}

elf64_exec_handle_t elf64_load_exec(void* ptr, size_t size) {
    elf64_ehdr_t* ehdr = (elf64_ehdr_t*)ptr;

    elf64_exec_handle_t handle = {};

    if (memcmp(ehdr->e_ident, "\x7f""ELF", 4) != 0) {
        printf("[elf64] invalid ELF header\n");
        handle.status = ELF64_ERR_MAGIC;
        return handle;
    }

    if (ehdr->e_ident[4] != 2 /* ELFCLASS64 */) {
        printf("[elf64] invalid/non-64-bit ELF class\n");
        handle.status = ELF64_ERR_CLASS;
        return handle;
    }

    if (ehdr->e_ident[5] != ELFDATA2LSB) {
        printf("[elf64] invalid ELF byte order\n");
        handle.status = ELF64_ERR_DATA;
        return handle;
    }

#if defined(__riscv)
    uint16_t machine = 0xF3;
#else
    uint16_t machine = 0;
#endif

    if (ehdr->e_machine != 0xF3 /* EM_RISCV */) {
        printf("[elf64] incompatible machine: expected %s, got %s\n", machine_to_name(machine), machine_to_name(ehdr->e_machine));
        return handle;
    }

    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN) {
        printf("[elf64] invalid ELF type\n");
        handle.status = ELF64_ERR_TYPE;
        return handle;
    }

    if (ehdr->e_phoff == 0 || ehdr->e_phnum == 0) {
        printf("[elf64] no load sections found\n");
        handle.status = ELF64_ERR_NO_LOAD;
        return handle;
    }

    elf64_phdr_t* phdrs = (elf64_phdr_t*)(ptr + ehdr->e_phoff);

    uint64_t phdrs_end = ehdr->e_phoff +
                          (uint64_t)ehdr->e_phnum * ehdr->e_phentsize;
    if (phdrs_end > size) {
        printf("[elf64] invalid phdr bounds\n");
        handle.status = ELF64_ERR_BOUNDS;
        return handle;
    }
    uint16_t i;
    for (i = 0; i < ehdr->e_phnum; i++) {
        elf64_phdr_t* phdr = &phdrs[i];

        if (phdr->p_type != PT_LOAD) continue;

        if (phdr->p_filesz > phdr->p_memsz ||
            phdr->p_offset + phdr->p_filesz > size) {
            printf("[elf64] invalid segment bounds\n");
            handle.status = ELF64_ERR_BOUNDS;
            goto fail;
        }

        uint64_t seg_start = PAGE_ALIGN_DOWN(phdr->p_vaddr);
        uint64_t seg_end   = PAGE_ALIGN_UP(phdr->p_vaddr + phdr->p_memsz);
        uint64_t seg_size  = seg_end - seg_start;

        void* phys = pmm_alloc_pages(seg_size);
        if (!phys) {
            printf("[elf64] out of memory loading segment\n");
            handle.status = ELF64_ERR_NOMEM;
            goto fail;
        }

        void* seg_kva = (void*)(uintptr_t)PA2VA((uint64_t)phys);
        memset(seg_kva, 0, seg_size);
        memcpy((uint8_t*)seg_kva + (phdr->p_vaddr - seg_start),
               (uint8_t*)ptr + phdr->p_offset, phdr->p_filesz);

        hal_mmu_perm_t perm = (phdr->p_flags & PF_X) ? PERM_USER_CODE : PERM_USER_DATA;

        if (!vmm_map(seg_start, (uint64_t)phys, seg_size, perm)) {
            printf("[elf64] failed to map segment\n");
            pmm_free_pages(phys, seg_size);
            handle.status = ELF64_ERR_MAP;
            goto fail;
        }
    }

    handle.status = ELF64_OK;
    handle.ptr = (void*)(uintptr_t)ehdr->e_entry;
    return handle;

fail:
    for (uint16_t j = 0; j < i; j++) {
        elf64_phdr_t* prev = &phdrs[j];
        if (prev->p_type != PT_LOAD) continue;

        uint64_t pstart = PAGE_ALIGN_DOWN(prev->p_vaddr);
        uint64_t pend   = PAGE_ALIGN_UP(prev->p_vaddr + prev->p_memsz);
        uint64_t psize  = pend - pstart;
        uint64_t pphys  = vmm_virt_to_phys(pstart);

        vmm_unmap(pstart, psize);
        pmm_free_pages((void*)pphys, psize);
    }
    return handle;
}