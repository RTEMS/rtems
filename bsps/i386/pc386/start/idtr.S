/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Intel i386 IDT Helpers
 *
 * This file contains all assembly code for the Intel i386 IDT manipulation.
 */

/*
 * Copyright (c) 1998 Eric Valette <eric.valette@free.fr>
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

#include <rtems/asm.h>

BEGIN_CODE
/*
 * C callable function enabling to get easilly usable info from
 * the actual value of IDT register.
 *
extern void i386_get_info_from_IDTR (interrupt_gate_descriptor** table,
				     unsigned* limit);
 */
PUBLIC (i386_get_info_from_IDTR)
PUBLIC (i386_set_IDTR)
PUBLIC (i386_get_info_from_GDTR)
PUBLIC (i386_set_GDTR)

SYM (i386_get_info_from_IDTR):
	movl	4(esp), ecx	    /* get location where table address */
                                    /*    must be stored */
	movl	8(esp), edx	    /* get location table size must be stored */

	subl	$6, esp		    /* let room to prepare 48 bit IDTR */

	sidt	(esp)		    /* get 48 bit IDTR value */

	movl	2(esp), eax	    /* get base */
	movl	eax, (ecx)

	movzwl	(esp), eax	    /* get limit */
	movl	eax, (edx)

	addl	$6, esp		    /* restore %esp */
	ret

/*
 * C callable function enabling to change the value of IDT register. Must be called
 * with inmterrupt masked at processor level!!!.
 *
extern void i386_set_IDTR (interrupt_gate_descriptor* table,
		           unsigned limit);
 */
SYM (i386_set_IDTR):

	leal	4(esp), edx	    /* load in edx address of input */
                                    /*    parameter "table" */

	movl	(edx), eax	    /* load base into eax */
	movl	4(edx), ecx	    /* load limit into ecx */

	movw	cx, (edx)	    /* prepare 48 bit pointer */
	movl	eax, 2(edx)

	lidt	(edx)

	ret
/*
 *
 * C callable function enabling to get easilly usable info from
 * the actual value of GDT register.
extern void i386_get_info_from_GDTR (segment_descriptors** table,
                                     uint16_t* limit);
 */

SYM (i386_get_info_from_GDTR):
	movl	4(esp), ecx	    /* get location where table address */
                                    /*    must be stored */
	movl	8(esp), edx	    /* get location table size must be stored */

	subl	$6, esp		    /* let room to prepare 48 bit GDTR */

	sgdt	(esp)		    /* get 48 bit GDTR value */

	movl	2(esp), eax	    /* get base */
	movl	eax, (ecx)

	movzwl	(esp), eax	    /* get limit */
	movw	ax, (edx)

	addl	$6, esp		    /* restore %esp */
	ret

/*
 * C callable function enabling to change the value of GDT register.
 * Must be called with interrupts masked at processor level!!!.
 *   extern void i386_set_GDTR (segment_descriptors*, uint16_t limit);
 */
SYM (i386_set_GDTR):

	leal	4(esp), edx	    /* load in edx address of input */
                                    /*   parameter "table" */

	movl	(edx), eax	    /* load base into eax */
	movl	4(edx), ecx	    /* load limit into ecx */

	movw	cx, (edx)	    /* prepare 48 bit pointer */
	movl	eax, 2(edx)

	lgdt	(edx)

	ret

END_CODE

END
