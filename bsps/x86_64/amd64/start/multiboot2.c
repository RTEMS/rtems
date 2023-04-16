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
#include <inttypes.h>
#include <multiboot2.h>
#include <multiboot2impl.h>

#include <rtems/score/heap.h>

#include <efi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BSP_MULTIBOOT_SUPPORT
#ifdef BSP_USE_EFI_BOOT_SERVICES

extern void
efi_console_initialize( void );

extern void
efi_memory_heap_extend(void);


extern EFI_HANDLE               IH;
extern EFI_SYSTEM_TABLE         *ST;
extern EFI_BOOT_SERVICES        *BS;
extern EFI_RUNTIME_SERVICES     *RS;
#endif
extern int _multiboot2_magic;
extern void* _multiboot2_info_ptr;
static int already_processed = 0;
#endif

static int bootservices_running = 0;

static char multiboot_boot_args[256];

extern Heap_Control *RTEMS_Malloc_Heap;

#ifdef BSP_MULTIBOOT_SUPPORT

void
process_multiboot2_info()
{
    struct multiboot_tag *tag;
    unsigned size;

#ifdef BSP_USE_EFI_BOOT_SERVICES
    ST = 0;
    RS = 0;
    BS = 0;
#endif
    if (already_processed)
        return;
    if (_multiboot2_magic == MULTIBOOT2_BOOTLOADER_MAGIC) {
        if ((*(unsigned*)_multiboot2_info_ptr) & 7) {
            printf("Multiboot2 info @ %p, unaligned mbi: 0x%x\n", _multiboot2_info_ptr, *(unsigned*)_multiboot2_info_ptr);
        }
        else {
            size = *(unsigned*)_multiboot2_info_ptr;
            printf("Multiboot2 info @ %p, size 0x%x\n", _multiboot2_info_ptr, size);
            for (tag = (struct multiboot_tag *) (_multiboot2_info_ptr + 8);
                 tag->type != MULTIBOOT_TAG_TYPE_END;
                 tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
                switch (tag->type) {
                case MULTIBOOT_TAG_TYPE_CMDLINE:
                    printf("Multiboot2 booting arguments: `%s', args len: %d\n",
                      ((struct multiboot_tag_string *) tag)->string,
                      ((struct multiboot_tag_string *) tag)->size);
                    strncpy(multiboot_boot_args, ((struct multiboot_tag_string*)tag)->string, ((struct multiboot_tag_string*)tag)->size);
                    break;
                case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                    printf("Multiboot2 loader name: `%s'\n", ((struct multiboot_tag_string *) tag)->string);
                    break;
                case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                    struct multiboot_tag_framebuffer* fbtag = (struct multiboot_tag_framebuffer*)tag;
                    uint64_t fbbase = fbtag->common.framebuffer_addr;
		    printf("Multiboot2 framebuffer @ %p, resolution: %dx%d, pitch/bpp: %d/%d, type: %d\n",
                           (void*)fbbase, fbtag->common.framebuffer_width, fbtag->common.framebuffer_height,
				    fbtag->common.framebuffer_pitch, fbtag->common.framebuffer_bpp,
				    fbtag->common.framebuffer_type);
                    break;
#ifdef BSP_USE_EFI_BOOT_SERVICES
                case MULTIBOOT_TAG_TYPE_EFI64:
                    printf("EFI64 system table @ 0x%llx\n", ((struct multiboot_tag_efi64 *) tag)->pointer);
                    ST = ((EFI_SYSTEM_TABLE*)((struct multiboot_tag_efi64 *) tag)->pointer);
                    BS = ST->BootServices;
                    RS = ST->RuntimeServices;
                    efi_console_initialize();
		    efi_memory_heap_extend();
                    break;
                case MULTIBOOT_TAG_TYPE_EFI_BS:
                    printf("GRUB: EFI boot services running.\n");
                    bootservices_running = 1;
                    break;
                case MULTIBOOT_TAG_TYPE_EFI64_IH:
                    printf("EFI: 64bit image handle: 0x%llx\n", ((struct multiboot_tag_efi64_ih *) tag)->pointer);
                    IH = (EFI_HANDLE)((struct multiboot_tag_efi64_ih *) tag)->pointer;
		    break;
#endif
                }
            }
        }
    }
    else {
        printf("So while not being booted by multiboot2, let's see what's in its magic then?: %dx\n", _multiboot2_magic);
    }
}

#endif /* BSP_MULTIBOOT_SUPPORT */

int
uefi_bootservices_running()
{
    return bootservices_running;
}

const char*
boot_args()
{
    return multiboot_boot_args;
}

int
get_boot_arg_int_value(const char* boot_args, const char* arg, int* val)
{
    int len_arg = strlen(arg);
    int len_try = strlen(boot_args) - len_arg;
    int len_bootargs = strlen(boot_args);
    char num[10];
    for (int i = 0; i < len_try; i++) {
        if (strncmp(&(boot_args[i]), arg, len_arg) == 0) {
            i = i + len_arg + 1;
            for (int j = 0; j < 10; j++) {
                if (boot_args[i + j] != ' ' && i + j < len_bootargs)
                    num[j] = boot_args[i + j];
                else {
                    num[j] = 0;
                    *val = atoi(num);
                    return 0;
                }
            }
        }
    }
    return -1;
}

