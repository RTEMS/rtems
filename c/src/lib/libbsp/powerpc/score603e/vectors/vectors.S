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

#include <bsp.h>
#include "asm.h"

#ifndef PPC_VECTOR_FILE_BASE
#error "PPC_VECTOR_FILE_BASE is not defined."
#endif

#if (SCORE603E_USE_NONE)

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
	
  /* Decrementer interrupt */
	.org    reset_vector - file_base    
	ba  0x00100  
	ba  0xfff00100  
	ba  0xfff00100  
	ba  0xfff00100
	  
	.org    mach_vector - file_base  
	ba  0x00200
	ba  0xfff00200
	ba  0xfff00200
	ba  0xfff00200
	
	.org    prot_vector - file_base  
	ba  0x00300
	ba  0xfff00300
	ba  0xfff00300
	ba  0xfff00300
	
	.org    isi_vector - file_base  
	ba  0x00400
	ba  0xfff00400
	ba  0xfff00400
	ba  0xfff00400
		
	.org    ext_vector - file_base    
	ba  0x0500  
	ba  0xfff00500  
	ba  0xfff00500  
	ba  0xfff00500  
	
	.org    align_vector - file_base    
	ba  0x00600  
	ba  0xfff00600  
	ba  0xfff00600  
	ba  0xfff00600  
	
	.org    prog_vector - file_base    
	ba  0x00700  
	ba  0xfff00700  
	ba  0xfff00700  
	ba  0xfff00700  

	.org    float_vector - file_base  
	ba  0x00800
	ba  0xfff00800
	ba  0xfff00800
	ba  0xfff00800

	.org    dec_vector - file_base  
	rfi
	ba  0xfff00900
	ba  0xfff00900
	ba  0xfff00900
	
	.org    sys_vector - file_base    
	ba  0x0c00  
	ba  0xfff00C00  
	ba  0xfff00C00  
	ba  0xfff00C00  
		
	.org    trace_vector - file_base  
	ba  0x0d00  
	ba  0xfff00d00  
	ba  0xfff00d00  
	ba  0xfff00d00  
	
	.org    itm_vector - file_base    
	ba  0x01000  
	ba  0xfff01000  
	ba  0xfff01000  
	ba  0xfff01000  
	
	.org    dltm_vector - file_base    
	ba  0x01100  
	ba  0xfff01100  
	ba  0xfff01100  
	ba  0xfff01100  
	
	.org    dstm_vector - file_base    
	ba  0x1200  
	ba  0xfff01200  
	ba  0xfff01200  
	ba  0xfff01200  
	
	.org    addr_vector - file_base    
	ba  0x1300  
	ba  0xfff01300  
	ba  0xfff01300  
	ba  0xfff01300  
	
	.org    sysmgmt_vector - file_base    
	ba  0x1400  
	ba  0xfff01400  
	ba  0xfff01400  
	ba  0xfff01400  
#endif














