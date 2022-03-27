#include <elf.h>

#include <string.h>
#include <debug.h>
#include <vfs.h>

typedef struct elf_ehdr {
    char ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} elf_ehdr_t;

typedef struct elf_phdr {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t align;
} elf_phdr_t;

static int check_elf_header(elf_ehdr_t* hdr) {
    if (hdr->ident[0] != 0x7F) {
        return 0;
    }
    if (hdr->ident[1] != 'E') {
        return 0;
    }
    if (hdr->ident[2] != 'L') {
        return 0;
    }
    if (hdr->ident[3] != 'F') {
        return 0;
    }
    if (hdr->ident[4] != 1) {
        return 0;
    }
    if (hdr->ident[5] != 1) {
        return 0;
    }
    if (hdr->ident[6] != 1) {
        return 0;
    }
    if (hdr->machine != 3) {
        return 0;
    }
    if (hdr->type != 1 && hdr->type != 2) {
        return 0;
    }
    if (hdr->entry < 0xA0000000) {
        return 0;
    }
    return 1;
}

int elf_load(process_t* proc, char* path, int argc, char** argv, char** envp) {
    fs_node_t* file = vfs_lookup(path);
    if (!file) {
        ERROR("elf_load: failed to find file %s\n", path);
        return 0;
    }

    vfs_open(file, 0);

    elf_ehdr_t* ehdr = (elf_ehdr_t*)malloc(sizeof(elf_ehdr_t));
    vfs_read(file, 0, sizeof(elf_ehdr_t), (uint8_t*)ehdr);
    if (!check_elf_header(ehdr)) {
        WARN("elf_load: invalid elf executable\n");
        free(file);
        free(ehdr);
        return 0;
    }

    elf_phdr_t* phdr = (elf_phdr_t*)malloc(sizeof(elf_phdr_t));
    for (uint32_t i = 0; i < ehdr->phnum; i++) {
        vfs_read(file, ehdr->phoff + i * sizeof(elf_phdr_t), sizeof(elf_phdr_t), (uint8_t*)phdr);
        if (phdr->type == 1) {
            while (proc->brk < phdr->vaddr + phdr->memsz) {
                if (NO_MEMORY) {
                    ERROR("elf_load: cannot load elf executable... no memory left\n");
                    return 0;
                }
                map_page(proc->pd, proc->brk, alloc_frame(), PAGE_USER);
                proc->brk += PAGE_SIZE;
            }
            vfs_read(file, phdr->offset, phdr->memsz, (uint8_t*)phdr->vaddr);
        }
    }

    /* do something with argv and envp */

    proc_init_stack(proc, (void*)ehdr->entry, (void*)0xFFFFFFFF);

    free(file);
    free(ehdr);
    free(phdr);
    
    return 1;
}
