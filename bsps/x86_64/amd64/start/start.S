/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2023 Karel Gardas
 * Copyright (C) 2018 embedded brains GmbH & Co. KG
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

#include <bspopts.h>

#ifdef BSP_MULTIBOOT_SUPPORT
#include <multiboot2.h>
#endif

	.text
	.section	.text._start,"ax",@progbits
	.p2align 4,,15
	.globl	_start
	.type	_start, @function
_start:
	.cfi_startproc
	movabsq	$_ISR_Stack_area_end, %rsp
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	xorl	%edi, %edi
#ifdef BSP_MULTIBOOT_SUPPORT
        mov    %eax, _multiboot2_magic
        mov    %rbx, _multiboot2_info_ptr
#endif
	movabsq	$boot_card, %rax
	call	*%rax
	.cfi_endproc

#ifdef BSP_MULTIBOOT_SUPPORT

multiboot2:
        ret

_multiboot2_start:
	jmp	_start

	.text
	.section	.multiboot2_header
	.p2align	4,,15
multiboot2_header_start:
        .long	MULTIBOOT2_HEADER_MAGIC
        .long 	MULTIBOOT_ARCHITECTURE_I386
	.long 	multiboot2_header_end - multiboot2_header_start
        .long   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot2_header_end - multiboot2_header_start))
efi_bootservices_start:
        .short  MULTIBOOT_HEADER_TAG_EFI_BS
        .short  0
        .long   efi_bootservices_end - efi_bootservices_start
efi_bootservices_end:
efi64_entry_start:
        .short  MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64
        .short  0
        .long   efi64_entry_end - efi64_entry_start
        .long   _start /* directly copied from resulting ELF */
        /* padding to 8 byte tags allignment */
        .long   0
efi64_entry_end:
info_requests_start:
        .short  MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST
        .short  0
        .long   info_requests_end - info_requests_start
        .long   MULTIBOOT_TAG_TYPE_EFI64
        .long   MULTIBOOT_TAG_TYPE_CMDLINE
#ifdef BSP_USE_EFI_BOOT_SERVICES
        .long   MULTIBOOT_TAG_TYPE_EFI_BS
#else
	.long	0
#endif
        /* padding to 8 byte tags allignment */
        .long   0
info_requests_end:
        /* header end*/
        .short  MULTIBOOT_HEADER_TAG_END
        .short  0
        .long   8
multiboot2_header_end:

        .data
        .global  _multiboot2_magic
_multiboot2_magic:
        .long   0

        .data
        .global  _multiboot2_info_ptr
_multiboot2_info_ptr:
        .quad   0

#endif
