/* SPDX-License-Identifier: BSD-3-Clause */

/* Copyright (c) 2001, 2009 Xilinx, Inc.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1.  Redistributions source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2.  Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3.  Neither the name of Xilinx nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <bspopts.h>

	.globl _crtinit
	.align 2
	.ent _crtinit
	.type _crtinit, @function
_crtinit:
	addi	r1, r1, -20			   	/* Save Link register	 */
	swi	r15, r1, 0

#ifndef __rtems__
	addi	r6, r0, __sbss_start		  	/* clear SBSS */
	addi	r7, r0, __sbss_end
	rsub	r18, r6, r7
	blei	r18, .Lendsbss

.Lloopsbss:
	swi	r0, r6, 0
	addi	r6, r6, 4
	rsub	r18, r6, r7
	bgti	r18, .Lloopsbss
.Lendsbss:
#endif /* __rtems__ */

#ifndef __rtems__
	addi	r6, r0, __bss_start			 /* clear BSS */
	addi	r7, r0, __bss_end
#else
	addi  r6, r0, bsp_section_bss_begin
	addi  r7, r0, bsp_section_bss_end
#endif	/* __rtems__ */
	rsub	r18, r6, r7
	blei	r18, .Lendbss
.Lloopbss:
	swi	r0, r6, 0
	addi	r6, r6, 4
	rsub	r18, r6, r7
	bgti	r18, .Lloopbss
.Lendbss:

#ifndef __rtems__
	brlid	r15, _program_init		 	 /* Initialize the program  */
	nop

	brlid   r15, __init					 /* Invoke language initialization functions */
	nop
#endif /* __rtems__ */
#ifdef BSP_START_COPY_FDT_FROM_U_BOOT      /* Boot loaders may pass the device tree in r5 */
	brlid	r15, bsp_fdt_copy              /* Do not touch r5 until bsp_fdt_copy() is called */
#endif /* BSP_START_COPY_FDT_FROM_U_BOOT */
	addi	r6, r0, 0					   /* Initialize argc = 1 and argv = NULL and envp = NULL  */
	addi	r7, r0, 0
#ifndef __rtems__
	brlid	r15, main					   /* Execute the program */
#else
	mfs r3, rmsr
	ori r3, r3, 0x100					   /* Set Exception Enable MSR flag */
	mts rmsr, r3
	brlid	r15, boot_card
#endif /* __rtems__ */
	addi	r5, r0, 0

	addik   r19, r3, 0					  /* Save return value */

#ifndef __rtems__
	brlid   r15, __fini					 /* Invoke language cleanup functions */
	nop

	brlid	r15, _program_clean			 /* Cleanup the program  */
	nop
#endif /* __rtems__ */

	lw r15, r1, r0					 /* Return back to CRT   */

	addik r3, r19, 0					  /* Restore return value */
	rtsd r15, 8
	addi r1, r1, 20
	.end _crtinit
