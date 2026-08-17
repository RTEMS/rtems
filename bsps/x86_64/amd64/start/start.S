/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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
#include <gdt.h>

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
        movq	%rsp, %rbp
	/*
	 * _ISR_Stack_size is aligned to CPU_INTERRUPT_STACK_ALIGNMENT (64 bits)
	 * by compiler directive at /cpukit/include/rtems/confdefs/percpu.h.
	 * No reference will occur beyond the stack region since the call
	 * instruction will decrement the %rsp and then save the value of %rip.
	 */
	movabsq	$_ISR_Stack_area_begin, %rsp
        addq $_ISR_Stack_size, %rsp

#ifndef BSP_USE_EFI_BOOT_SERVICES
        /* Use our own GDT instead of the one set up by UEFI */
        lgdt amd64_gdt_descriptor

        /* Load data segment registers */
        movw $GDT_DATA_SEG_OFFSET, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %ss
        movw %ax, %fs

        /* Load code segment register */
        pushq $GDT_CODE_SEG_OFFSET
        pushq $after_load_gdt
        retfq

after_load_gdt:
#endif

#ifndef BSP_MULTIBOOT_SUPPORT
        /**
         * The FreeBSD bootloader gives us control with the following stack:
         * 0(%rsp) = 32 bit return address (cannot be used)
         * 4(%rsp) = 32 bit modulep
         * 8(%rsp) = 32 bit kernend
         *
         * We will extract the necessary info set up by the FreeBSD bootloader
         * before dynamic memory is set up since it is stored in memory after
         * the kernel
         */
        movl	4(%rbp), %edi
        call	retrieve_info_from_freebsd_loader
#endif

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
        .long	MULTIBOOT_TAG_TYPE_ACPI_OLD
        .long	MULTIBOOT_TAG_TYPE_ACPI_NEW
        .long	0
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

#else /* !BSP_MULTIBOOT_SUPPORT */

/*
 * Virtual machine monitors such as Qemu start a kernel through the x86/HVM
 * direct boot ABI.  The entry point of this protocol is published by an ELF
 * note.  The processor is in 32-bit protected mode with paging disabled and
 * %ebx holds the physical address of the boot information.  Long mode must be
 * established before the common start-up code can run.
 */

#define XEN_ELFNOTE_PHYS32_ENTRY 18

#define PVH_PAGE_SIZE 4096

/* One first level, one second level and four third level tables */
#define PVH_PAGE_TABLE_SIZE 24576
#define PVH_PAGE_TABLE_DWORDS 6144
#define PVH_SECOND_LEVEL_OFFSET 4096
#define PVH_THIRD_LEVEL_OFFSET 8192
#define PVH_THIRD_LEVEL_COUNT 4

#define PVH_PAGE_PRESENT_WRITE 0x3
#define PVH_PAGE_PRESENT_WRITE_LARGE 0x83
#define PVH_LARGE_PAGE_SIZE 0x200000

/* Four third level tables identity map the first 4 GiB */
#define PVH_LARGE_PAGE_COUNT 2048

/* Paging and the monitor coprocessor bit required by SSE */
#define PVH_CR0_PG_MP 0x80000002

/* Physical address extension and the SSE support of the operating system */
#define PVH_CR4_PAE_OSFXSR_OSXMMEXCPT 0x620

#define PVH_MSR_EFER 0xc0000080
#define PVH_EFER_LME 0x100

	.section	.note.Xen,"a",@note
	.p2align	2
	.long		4
	.long		8
	.long		XEN_ELFNOTE_PHYS32_ENTRY
	.asciz		"Xen"
	.p2align	2
	.quad		_pvh_start
	.p2align	2

	.section	.text._pvh_start,"ax",@progbits
	.code32
	.p2align 4,,15
	.globl	_pvh_start
	.type	_pvh_start, @function
_pvh_start:
	cli
	cld

	/* The boot information is evaluated once long mode is active */
	movl	%ebx, pvh_start_info_address

	/* Clear the temporary page tables */
	movl	$pvh_page_tables, %edi
	movl	$PVH_PAGE_TABLE_DWORDS, %ecx
	xorl	%eax, %eax
	rep	stosl

	/* The first level table maps the single second level table */
	movl	$pvh_page_tables, %edi
	leal	PVH_SECOND_LEVEL_OFFSET(%edi), %eax
	orl	$PVH_PAGE_PRESENT_WRITE, %eax
	movl	%eax, (%edi)

	/* The second level table maps the four third level tables */
	leal	PVH_SECOND_LEVEL_OFFSET(%edi), %esi
	leal	PVH_THIRD_LEVEL_OFFSET(%edi), %eax
	orl	$PVH_PAGE_PRESENT_WRITE, %eax
	movl	$PVH_THIRD_LEVEL_COUNT, %ecx
1:
	movl	%eax, (%esi)
	addl	$PVH_PAGE_SIZE, %eax
	addl	$8, %esi
	loop	1b

	/* The third level tables identity map memory with large pages */
	leal	PVH_THIRD_LEVEL_OFFSET(%edi), %esi
	movl	$PVH_PAGE_PRESENT_WRITE_LARGE, %eax
	movl	$PVH_LARGE_PAGE_COUNT, %ecx
2:
	movl	%eax, (%esi)
	addl	$PVH_LARGE_PAGE_SIZE, %eax
	addl	$8, %esi
	loop	2b

	movl	%edi, %cr3

	/*
	 * The physical address extension is mandatory for long mode.  The
	 * compiler uses SSE instructions, so enable them here since no boot
	 * loader did it for us.
	 */
	movl	%cr4, %eax
	orl	$PVH_CR4_PAE_OSFXSR_OSXMMEXCPT, %eax
	movl	%eax, %cr4

	/* Enable long mode */
	movl	$PVH_MSR_EFER, %ecx
	rdmsr
	orl	$PVH_EFER_LME, %eax
	wrmsr

	/* Paging activates long mode */
	movl	%cr0, %eax
	orl	$PVH_CR0_PG_MP, %eax
	movl	%eax, %cr0

	/* Only our own descriptor table provides a 64-bit code segment */
	lgdt	amd64_gdt_descriptor
	ljmpl	$GDT_CODE_SEG_OFFSET, $_pvh_start_64

	.code64
_pvh_start_64:
	movw	$GDT_DATA_SEG_OFFSET, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs

	movabsq	$_ISR_Stack_area_begin, %rsp
	addq	$_ISR_Stack_size, %rsp

	xorl	%edi, %edi
	movabsq	$boot_card, %rax
	call	*%rax

	.data
	.p2align	3
	.globl	pvh_start_info_address
pvh_start_info_address:
	.quad	0

	.section	.bss.pvh_page_tables,"aw",@nobits
	.p2align	12
pvh_page_tables:
	.space	PVH_PAGE_TABLE_SIZE

#endif
