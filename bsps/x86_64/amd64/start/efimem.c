/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 Karel Gardas
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <efi.h>
#include <efilib.h>

#include <stdio.h>

extern Heap_Control *RTEMS_Malloc_Heap;

void bsp_memory_heap_extend(void);

extern EFI_BOOT_SERVICES        *BS;

static UINT32 total_pages = 0;
static UINT32 allocated_pages = 0;
static UINT32 usable_pages = 0;
static EFI_PHYSICAL_ADDRESS physBuf;

static int error = 0;
static int extension_steps = 0;

#ifdef BSP_EFI_MMAP_PRINTOUT
static const char*
efi_memory_type(EFI_MEMORY_TYPE type);
#endif

void
efi_memory_heap_extend( void );

static UINT64
heap_size(void)
{
    return RTEMS_Malloc_Heap->stats.size;
}

static UINT64
allocate_biggest_block( void )
{
    UINT64 sz = 0;
    EFI_MEMORY_DESCRIPTOR *map = 0, *p = 0;
    UINTN key = 0, dsz = 0;
    UINT32 dver = 0;
    EFI_STATUS status = 0;
    int i, ndesc = 0;

    UINT64 to_alloc_pages = 0;
    bool first_run = false;
    if (total_pages == 0)
        first_run = true;
    // let's see available RAM
    status = BS->GetMemoryMap(&sz, 0, &key, &dsz, &dver);
    if (status != EFI_BUFFER_TOO_SMALL) {
        printf("EFI: Can't determine memory map size\n");
        return 0;
    }
    map = malloc(sz);
    if (map == NULL) {
        printf("EFI: Can't allocate memory map backing\n");
        return 0;
    }
    status = BS->GetMemoryMap(&sz, map, &key, &dsz, &dver);
    if (EFI_ERROR(status)) {
        printf("EFI: Can't read memory map\n");
        free(map);
        return 0;
    }                  
    ndesc = sz / dsz;
#ifdef BSP_EFI_MMAP_PRINTOUT
    if (first_run)
        printf("%23s %12s %8s\n", "Type", "Physical", "#Pages");
#endif
    for (i = 0, p = map; i < ndesc;
         i++, p = NextMemoryDescriptor(p, dsz)) {
        if (first_run) {
#ifdef BSP_EFI_MMAP_PRINTOUT
            printf("%23s %012jx %08jx\n", efi_memory_type(p->Type),
                   (uintmax_t)p->PhysicalStart, (uintmax_t)p->NumberOfPages);
#endif
            if (p->Type != EfiReservedMemoryType)
                total_pages = total_pages + p->NumberOfPages;
            if (p->Type == EfiConventionalMemory) {
                usable_pages = usable_pages + p->NumberOfPages;
            }
        }
        if (p->Type == EfiConventionalMemory) {
            if (to_alloc_pages < p->NumberOfPages)
                to_alloc_pages = p->NumberOfPages;
        }
    }
    status = ST->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, to_alloc_pages, &physBuf );
    if (EFI_ERROR(status)) {
        /* on some UEFI implementations it is not possible to allocate biggest available block
           for whatever reasons. In that case, let's go wild and attempt to allocate
           half of it */
        error++;
        status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, (to_alloc_pages / 2), &physBuf );
        if (EFI_ERROR(status)) {
            printf("EFI can't allocate: %lu pages nor half of the amount.\n", to_alloc_pages);
            free(map);
            return 0;
        }
        else {
            to_alloc_pages = to_alloc_pages / 2;
        }
    }
    allocated_pages = allocated_pages + to_alloc_pages;
    sz = to_alloc_pages * 4096;
    uintptr_t es = 0;
    es = _Heap_Extend( RTEMS_Malloc_Heap, (void *)physBuf, sz, 0 );
    free(map);
    return es;
}

void efi_memory_heap_extend( void )
{
    int i;
    UINT64 asz = 0;
    UINT64 oldsz, newsz = 0;
    oldsz = heap_size();
    for (i = 0; i < 1024; i++) {
        /* let's try 1k alloc attempts */
        asz = allocate_biggest_block();
        if (asz == 0)
            break;
        extension_steps++;
    }
    newsz = heap_size();
    printf("EFI: Total memory: %u pages, %u megabytes\n", total_pages, (total_pages * 4 / 1024));
    printf("EFI: Usable memory: %u pages, %u megabytes\n", usable_pages, (usable_pages * 4 / 1024));
    printf("EFI: Allocated memory: %u pages, %u megabytes\n", allocated_pages, (allocated_pages * 4 / 1024));
    printf("RTEMS: Heap extended in %u steps with %u steps failed.\n", extension_steps, error);
    uint64_t s = newsz - oldsz;
    printf("RTEMS: Heap extended by %lu pages, %lu megabytes\n", (s / 4096), ((s / 1024) / 1024));
}

#ifdef BSP_EFI_MMAP_PRINTOUT
static const char*
efi_memory_type(EFI_MEMORY_TYPE type)
{
    switch (type) {
    case EfiReservedMemoryType:
        return "Reserved"; 
    case EfiLoaderCode:
        return "LoaderCode";
    case EfiLoaderData:
        return "LoaderData";
    case EfiBootServicesCode:
        return "BootServicesCode";
    case EfiBootServicesData:
        return "BootServicesData";
    case EfiRuntimeServicesCode:
        return "RuntimeServicesCode";
    case EfiRuntimeServicesData:
        return "RuntimeServicesData";
    case EfiConventionalMemory:
        return "ConventionalMemory";
    case EfiUnusableMemory:
        return "UnusableMemory";
    case EfiACPIReclaimMemory:
        return "ACPIReclaimMemory";
    case EfiACPIMemoryNVS:
        return "ACPIMemoryNVS";
    case EfiMemoryMappedIO:
        return "MemoryMappedIO";
    case EfiMemoryMappedIOPortSpace:
        return "MemoryMappedIOPortSpace";
    case EfiPalCode:
        return "PalCode";
    case EfiPersistentMemory:
        return "PersistentMemory";
    default:
        return "Unknown Type";
    }
}
#endif
