/*  cpu_asm.S
 *
 *  This file contains all assembly code for the Intel i386 IDT
 *  manipulation.
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
				     unsigned* limit);
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
	movl	eax, (edx)

	addl	$6, esp		    /* restore %esp */
	ret

/*
 * C callable function enabling to change the value of GDT register.
 * Must be called with interrupts masked at processor level!!!.
 *   extern void i386_set_GDTR (segment_descriptors*, unsigned limit);
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
