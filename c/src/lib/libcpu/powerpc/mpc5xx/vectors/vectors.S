/*
 * vectors.S
 *
 *  This file contains the assembly code for the PowerPC exception veneers
 *  for RTEMS.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/vectors/vectors.S,
 *
 *  (c) 1999, Eric Valette valette@crf.canon.fr
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/vectors.h>

#define SYNC \
	sync; \
	isync


/*
 * Hardware exception vector table.
 *
 * The MPC555 can be configured to use a compressed vector table with 8
 * bytes per entry, rather than the usual 0x100 bytes of other PowerPC
 * devices.  The following macro uses this feature to save the better part
 * of 8 kbytes of flash ROM.
 *
 * Each vector table entry has room for only a simple branch instruction
 * which branches to a prologue specific to that exception.  This
 * exception-specific prologue begins the context save, loads the exception
 * number into a register, and jumps to a common exception prologue, below.
 */

	.macro	vectors	num=0, total=NUM_EXCEPTIONS  /* create vector table */

/* vector table entry */
	.section .vectors, "ax"

	ba	specific_prologue\@		/* run specific prologue */
	.long	0				/* each entry is 8 bytes */

/* exception-specific prologue */
	.text

specific_prologue\@:
	stwu    r1, -EXCEPTION_FRAME_END(r1)	/* open stack frame */
	stw	r4, GPR4_OFFSET(r1)		/* preserve register */
	li	r4, \num			/* get exception number */
	b	common_prologue			/* run common prologue */

/* invoke macro recursively to create remainder of table */
	.if	\total - (\num + 1)
	vectors	"(\num + 1)", \total
	.endif

	.endm


/* invoke macro to create entire vector table */
	vectors


/*
 * Common exception prologue.
 *
 * Because the MPC555 vector table is in flash ROM, it's not possible to
 * change the exception handlers by overwriting them at run-time, so this
 * common exception prologue uses a table of exception handler pointers to
 * provide equivalent flexibility.
 *
 * When the actual exception handler is run, R1 points to the base of a new
 * exception stack frame, in which R3, R4 and LR have been saved.  R4 holds
 * the exception number.
 */
	.text

common_prologue:
	stw	r3, GPR3_OFFSET(r1)		/* preserve registers */
	mflr	r3
	stw	r3, EXC_LR_OFFSET(r1)

	slwi	r3, r4, 2				/* make table offset */
	addis	r3, r3, exception_handler_table@ha	/* point to entry */
	addi	r3, r3, exception_handler_table@l
	lwz	r3, 0(r3)				/* get entry */
	mtlr	r3					/* run it */
	blr


/*
 * Default exception handler.
 *
 * The function initialize_exceptions() initializes all of the entries in
 * the exception handler table with pointers to this routine, which saves
 * the remainder of the interrupted code's state, then calls
 * C_default_exception_handler() to dump registers.
 *
 * On entry, R1 points to a new exception stack frame in which R3, R4, and
 * LR have been saved.  R4 holds the exception number.
 */
	.text

PUBLIC_VAR(default_exception_handler)
SYM (default_exception_handler):
	/*
	 * Save the interrupted code's program counter and MSR.  Beyond this
	 * point, all exceptions are recoverable.  Use an RCPU-specific SPR
	 * to set the RI bit in the MSR to indicate the recoverable state.
	 */
	mfsrr0  r3
	stw	r3, SRR0_FRAME_OFFSET(r1)
	mfsrr1  r3
	stw	r3, SRR1_FRAME_OFFSET(r1)

	mtspr	eid, r3			/* set MSR[RI], clear MSR[EE] */
	SYNC

	/*
	 * Save the remainder of the general-purpose registers.
	 *
	 * Compute the value of R1 at exception entry before storing it in
	 * the frame.
	 *
	 * Note that R2 should never change (it's the EABI pointer to
	 * .sdata2), but we save it just in case.
	 *
	 * Recall that R3 and R4 were saved by the specific- and
	 * common-exception handlers before entry to this routine.
	 */
	stw	r0, GPR0_OFFSET(r1)
        addi    r0, r1, EXCEPTION_FRAME_END
        stw     r0, GPR1_OFFSET(r1)
	stw	r2, GPR2_OFFSET(r1)
	stmw	r5, GPR5_OFFSET(r1)		/* save R5 to R31 */

	/*
	 * Save the remainder of the UISA special-purpose registers.  Recall
	 * that LR was saved before entry.
	 */
	mfcr	r0
	stw	r0,  EXC_CR_OFFSET(r1)
	mfctr	r0
	stw	r0,  EXC_CTR_OFFSET(r1)
	mfxer	r0
	stw	r0,  EXC_XER_OFFSET(r1)

	/*
	 * Call C-language portion of the default exception handler, passing
	 * in the address of the frame.
	 *
	 * To simplify things a bit, we assume that the target routine is
	 * within +/- 32 Mbyte from here, which is a reasonable assumption
	 * on the MPC555.
	 */
	stw	r4, EXCEPTION_NUMBER_OFFSET(r1)	/* save exception number */
	addi	r3, r1, 0x8			/* get frame address */
	bl	C_default_exception_handler	/* call handler */

	/*
	 * Restore UISA special-purpose registers.
	 */
	lwz	r0,  EXC_XER_OFFSET(r1)
	mtxer	r0
	lwz	r0,  EXC_CTR_OFFSET(r1)
	mtctr	r0
	lwz	r0,  EXC_CR_OFFSET(r1)
	mtcr	r0
	lwz	r0,  EXC_LR_OFFSET(r1)
	mtlr	r0

	/*
	 * Restore most general-purpose registers.
	 */
	lmw	r2, GPR2_OFFSET(r1)

	/*
	 * Restore the interrupted code's program counter and MSR, but first
	 * use an RCPU-specific special-purpose register to clear the RI
	 * bit, indicating that exceptions are temporarily non-recoverable.
	 */
	mtspr	nri, r0			/* clear MSR[RI] */
	SYNC

	lwz	r0, SRR1_FRAME_OFFSET(r1)
	mtsrr1	r0
	lwz	r0, SRR0_FRAME_OFFSET(r1)
	mtsrr0	r0

	/*
	 * Restore the final GPR, close the stack frame, and return to the
	 * interrupted code.
	 */
	lwz	r0, GPR0_OFFSET(r1)
	addi 	r1, r1, EXCEPTION_FRAME_END
	SYNC
	rfi
