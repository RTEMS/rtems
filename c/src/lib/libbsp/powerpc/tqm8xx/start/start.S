/*===============================================================*\
| Project: RTEMS generic TQM8xx BSP                               |
+-----------------------------------------------------------------+
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the startup assembly code                    |
| it is based on the gen83xx BSP                                  |
\*===============================================================*/

#include <libcpu/powerpc-utility.h>
#include <rtems/powerpc/cache.h>
#include <bsp.h>
#include <mpc8xx.h>

.extern boot_card

.section ".entry"
PUBLIC_VAR (start)
start:

	/*
	 * basic CPU setup:
	 * init MSR
	 */
	mfmsr	r30
	SETBITS r30, r29, MSR_ME|MSR_RI
	CLRBITS r30, r29, MSR_IP|MSR_EE
	mtmsr	r30 				/* Set RI/ME, Clr EE in MSR */
	/*
	 * init IMMR
	 */
	LA  r30,m8xx
	mtspr immr,r30
	/*
	 * determine current execution address offset
	 */
	bl start_1
start_1:
	mflr r20
	LA   r30,start_1
	sub. r20,r20,r30
	/*
	 * execution address offset == 0?
	 * then do not relocate code and data
	 */
	beq  start_code_in_ram
	/*
	 * ROM or relocatable startup: copy startup code to SDRAM
	 */
	/* get start address of text section in RAM */
	LA	r29, bsp_section_text_start
	/* get start address of text section in ROM (add reloc offset) */
	add	r30, r20, r29
	/* get size of startup code */
	LA	r28, end_reloc_startup
	LA	r31, bsp_section_text_start
	sub	28,r28,r31
	/* copy startup code from ROM to RAM location */
	bl	copy_image

	/*
	 * jump to code copy in  SDRAM
	 */
	/* get compile time address of label */
	LA	r29, copy_rest_of_text
	mtlr	r29
	blr                     /* now further execution RAM */
copy_rest_of_text:
	/*
	 * ROM or relocatable startup: copy rest of code to SDRAM
	 */
	/* get start address of rest of code in RAM */
	LA	r29, end_reloc_startup
	/* get start address of text section in ROM (add reloc offset) */
	add	r30, r20, r29
	/* get size of rest of code */
	LA	r28, bsp_section_text_start
	LA	r31, bsp_section_text_size
	add	r28,r28,r31
	sub	r28,r28,r29
	bl	copy_image      /* copy text section from ROM to RAM location */

	/*
	 * ROM or relocatable startup: copy data to SDRAM
	 */
	/* get start address of data section in RAM */
	LA	r29, bsp_section_data_start
	/* get start address of data section in ROM (add reloc offset) */
	add	r30, r20, r29
	/* get size of RAM image */
	LA	r28, bsp_section_data_size
	/* copy initialized data section from ROM to RAM location */
	bl	copy_image

start_code_in_ram:

	/*
	 * ROM/RAM startup: clear bss in SDRAM
	 */
	LA	r3, bsp_section_bss_start  /* get start address of bss section */
	LWI	r4, bsp_section_bss_size   /* get size of bss section */
	bl	mpc8xx_zero_4          /* Clear the bss section */
	/*
	 * call boot_card
	 */

	/* Set stack pointer (common for RAM/ROM startup) */
	LA	r1, bsp_section_text_start
        addi    r1, r1, -0x10 /* Set up stack pointer = beginning of text section - 0x10 */

	/* Create NULL */
	li r0, 0

	/* Return address */
	stw r0, 4(r1)

	/* Back chain */
	stw r0, 0(r1)

	/* Read-only small data */
	LA r2, _SDA2_BASE_

	/* Read-write small data */
	LA r13, _SDA_BASE_

	/*
	 * init some CPU stuff
	 */
	bl SYM (_InitTQM8xx)

/* clear arguments and do further init. in C (common for RAM/ROM startup) */

        /* Clear cmdline */
        xor r3, r3, r3

        bl      SYM (boot_card)  /* Call the first C routine */

twiddle:
	/* We don't expect to return from boot_card but if we do */
	/* wait here for watchdog to kick us into hard reset     */
	b	twiddle

copy_with_watchdog:
     addi    r5,r5,16
     rlwinm. r5,r5,28,4,31
     mtctr   r5

copy_loop:
     lwz     r6,0(r3)
     lwz     r7,4(r3)
     lwz     r8,8(r3)
     lwz     r9,12(r3)
     stw     r6,0(r4)
     stw     r7,4(r4)
     stw     r8,8(r4)
     stw     r9,12(r4)
     addi    r3,r3,16
     addi    r4,r4,16
     sth     r28,14(r30)
     sth     r29,14(r30)
     bdnz+   copy_loop
     blr

copy_image:
	/*
	 * watchdog:
	 * r26 = immr
	 * r25 = watchdog magic 1
	 * r24 = watchdog magic 2
	 */
	mfimmr  r26
	rlwinm. r26,r26,0,0,15
	li      r25,0x556c
	li      r24,0xffffaa39

	mr	r27, r28		/* determine number of 4word chunks */
	srwi	r28, r28, 4
	mtctr	r28

	slwi	r28, r28, 4
	sub     r27, r27, r28		/* determine residual bytes */
copy_image_4word:
	lwz	r20, 0(r30)		/* fetch data */
	lwz	r21, 4(r30)
	lwz	r22, 8(r30)
	lwz	r23,12(r30)
	stw	r20, 0(r29)		/* store data */
	stw	r21, 4(r29)
	stw	r22, 8(r29)
	stw	r23,12(r29)

	addi    r30, r30, 0x10		/* increment source pointer */
	addi    r29, r29, 0x10		/* increment destination pointer */
	/*
	 * trigger watchdog
	 */
	sth     r25,14(r26)
	sth     r24,14(r26)

	bdnz	copy_image_4word	/* decrement ctr and branch if not 0 */

	cmpwi	r27, 0x00		/* copy image finished ? */
	beq	copy_image_end;
	mtctr   r27			/* reload counter for residual bytes */
copy_image_byte:
	lswi	r28, r30, 0x01

	stswi   r28, r29, 0x01		/* do byte copy ROM -> RAM */


	addi    r30, r30, 0x01		/* increment source pointer */
	addi    r29, r29, 0x01		/* increment destination pointer */

	bdnz	copy_image_byte		/* decrement ctr and branch if not 0 */

copy_image_end:
	blr


/**
 * @fn int mpc8xx_zero_4( void *dest, size_t n)
 *
 * @brief Zero all @a n bytes starting at @a dest with 4 byte writes.
 *
 * The address @a dest has to be aligned on 4 byte boundaries.  The size @a n
 * must be evenly divisible by 4.
 */
GLOBAL_FUNCTION mpc8xx_zero_4
	/* Create zero */
	xor	r0, r0, r0

	/* Set offset */
	xor	r5, r5, r5

	/* Loop counter for the first bytes up to 16 bytes */
	rlwinm.	r9, r4, 30, 30, 31
	beq	mpc8xx_zero_4_more
	mtctr	r9

mpc8xx_zero_4_head:

	stwx	r0, r3, r5
	addi	r5, r5, 4
	bdnz	mpc8xx_zero_4_head

mpc8xx_zero_4_more:

	/* More than 16 bytes? */
	srwi.	r9, r4, 4
	beqlr
	mtctr	r9

	/* Set offsets */
	addi	r6, r5, 4
	addi	r7, r5, 8
	addi	r8, r5, 12

mpc8xx_zero_4_tail:

	stwx	r0, r3, r5
	addi	r5, r5, 16
	stwx	r0, r3, r6
	addi	r6, r6, 16
	stwx	r0, r3, r7
	addi	r7, r7, 16
	stwx	r0, r3, r8
	addi	r8, r8, 16
	bdnz	mpc8xx_zero_4_tail

	/* Return */
	blr

end_reloc_startup:
