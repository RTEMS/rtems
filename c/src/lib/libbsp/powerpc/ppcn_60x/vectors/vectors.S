/*
 * (c) 1998, Radstone Technology plc.
 *
 *
 * This is an unpublished work the copyright  in  which   vests
 * in  Radstone Technology plc.  All rights reserved.
 *
 * The information contained herein is the property of Radstone
 * Technology  plc.  and  is  supplied  without  liability  for
 * errors or omissions and no part may be reproduced,  used  or
 * disclosed except as authorized by contract or  other written
 * permission.     The    copyright    and    the     foregoing
 * restriction   on  reproduction, use and disclosure extend to
 * all  the   media   in   which   this  information   may   be
 * embodied.
 *
 */
/*  vectors.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC 
 *  interrupt veneers for RTEMS.
 *
 */

/*
 *  The issue with this file is getting it loaded at the right place.
 *  The first vector MUST be at address 0x????0100.
 *  How this is achieved is dependant on the tool chain.
 *
 *  However the basic mechanism for ELF assemblers is to create a
 *  section called ".vectors", which will be loaded to an address
 *  between 0x????0000 and 0x????0100 (inclusive) via a link script.
 *
 *  The basic mechanism for XCOFF assemblers is to place it in the
 *  normal text section, and arrange for this file to be located
 *  at an appropriate position on the linker command line.
 *
 *  The variable 'PPC_VECTOR_FILE_BASE' must be defined to be the
 *  offset from 0x????0000 to the first location in the file.  This
 *  will usually be 0x0000 or 0x0100.
 *
 *  $Id$
 */

#include "asm.h"
#include "bsp.h"

#ifndef PPC_VECTOR_FILE_BASE
#error "PPC_VECTOR_FILE_BASE is not defined."
#endif

    .set    IP_LINK, 0
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
    .set    IP_0, (IP_LINK + 56)
#else
    .set    IP_0, (IP_LINK + 8)
#endif
    .set    IP_2, (IP_0 + 4)

    .set    IP_3, (IP_2 + 4)
    .set    IP_4, (IP_3 + 4)
    .set    IP_5, (IP_4 + 4)
    .set    IP_6, (IP_5 + 4)

    .set    IP_7, (IP_6 + 4)
    .set    IP_8, (IP_7 + 4)
    .set    IP_9, (IP_8 + 4)
    .set    IP_10, (IP_9 + 4)

    .set    IP_11, (IP_10 + 4)
    .set    IP_12, (IP_11 + 4)
    .set    IP_13, (IP_12 + 4)
    .set    IP_28, (IP_13 + 4)

    .set    IP_29, (IP_28 + 4)
    .set    IP_30, (IP_29 + 4)
    .set    IP_31, (IP_30 + 4)
    .set    IP_CR, (IP_31 + 4)

    .set    IP_CTR, (IP_CR + 4)
    .set    IP_XER, (IP_CTR + 4)
    .set    IP_LR, (IP_XER + 4)
    .set    IP_PC, (IP_LR + 4)

    .set    IP_MSR, (IP_PC + 4)

    .set    IP_END, (IP_MSR + 16)

	/* Where this file will be loaded */
	.set	file_base, PPC_VECTOR_FILE_BASE

	/* Vector offsets			 */
	.set    reset_vector,0x0100
	.set    mach_vector,0x0200
	.set    prot_vector,0x0300
	.set    isi_vector,0x0400
	.set    ext_vector,0x0500
	.set    align_vector,0x0600
	.set    prog_vector,0x0700
	.set    float_vector,0x0800
	.set    dec_vector,0x0900
	.set    sys_vector,0x00C00
	.set    trace_vector, 0x0d00
	.set    itm_vector,0x01000
	.set    dltm_vector,0x1100
	.set    dstm_vector,0x1200
	.set    addr_vector,0x1300
	.set    sysmgmt_vector,0x1400

/* Go to the right section */
#if PPC_ASM == PPC_ASM_ELF
	.section .vectors,"awx",@progbits
#elif PPC_ASM == PPC_ASM_XCOFF
	.csect	.text[PR]
#endif

	PUBLIC_VAR (__vectors)
SYM (__vectors):
	
#if PPCN_60X_USE_DINK
	.org    reset_vector - file_base    
	/* This is where the DINK soft reset handler is located */
	ba  0xfff00180  
	  
	.org    mach_vector - file_base  
	ba  0xfff00200
	
	.org    prot_vector - file_base  
	ba  0xfff00300
	
	.org    isi_vector - file_base  
	ba  0xfff00400
		
	.org    ext_vector - file_base    
	rfi
	
	.org    align_vector - file_base    
	ba  0xfff00600  
	
	.org    prog_vector - file_base    
	ba  0xfff00700  

	.org    float_vector - file_base  
	ba  0xfff00800

	.org    dec_vector - file_base  
	rfi
	
	.org    sys_vector - file_base    
	ba  0xfff00C00  
		
	.org    trace_vector - file_base  
	ba  0xfff00d00  
	
	.org    itm_vector - file_base    
	ba  0xfff01000  
	
	.org    dltm_vector - file_base    
	ba  0xfff01100  
	
	.org    dstm_vector - file_base    
	ba  0xfff01200  
	
	.org    addr_vector - file_base    
	ba  0xfff01300  
	
	.org    sysmgmt_vector - file_base    
	ba  0xfff01400  
#else
	.org    reset_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,1
display_exc:
	stw		r3,IP_3(r1)
	stw		r5,IP_5(r1)
	/*
	 * Enable data and instruction address translation
	 */
	li      r3,MSR_IR | MSR_DR
	mtmsr   r3
	lis		r3,0x8000
	stb		r4,0x860(r3)
	addi	r4,r4,0x30
waitfortx:
	lbz		r5,0x3fd(r3)
	andi.	r5,r5,0x20
	beq		waitfortx
	stb		r4,0x3f8(r3)
	li		r5,0
	stw		r4,0x00(r5)
	mfsrr0	r4
	stw		r4,0x04(r5)
	mfsrr1	r4
	stw		r4,0x08(r5)
	lwz		r4,IP_4(r1)
	lwz		r5,IP_5(r1)
	lwz		r3,IP_3(r1)
	addi	r1,r1,IP_END
	rfi
	  
	.org    mach_vector - file_base  
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	stw		r3,IP_3(r1)
	lis		r4,0
	mfspr	r3,srr0
	stw		r3,0x00(r4)
	mfspr	r3,srr1
	stw		r3,0x04(r4)
	stw		r5,0x08(r4)
	stw		r2,0x0c(r4)
	stw		r11,0x10(r4)
	stw		r12,0x14(r4)
	dcbst	0,r4
	li		r4,0x02
	b		display_exc
	
	.org    prot_vector - file_base  
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x03
	b		display_exc
	
	.org    isi_vector - file_base  
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x04
	b		display_exc
		
	.org    ext_vector - file_base    
	rfi
	
	.org    align_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x06
	b		display_exc
	
	.org    prog_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x07
	b		display_exc

	.org    float_vector - file_base  
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x08
	b		display_exc

	.org    dec_vector - file_base  
	rfi
	
	.org    sys_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0a
	b		display_exc
		
	.org    trace_vector - file_base  
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0b
	b		display_exc
	
	.org    itm_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0c
	b		display_exc
	
	.org    dltm_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0d
	b		display_exc
	
	.org    dstm_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0e
	b		display_exc
	
	.org    addr_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x0f
	b		display_exc
	
	.org    sysmgmt_vector - file_base    
	stwu    r1, -(IP_END)(r1)
	stw		r4,IP_4(r1)
	li		r4,0x00
	b		display_exc
#endif

