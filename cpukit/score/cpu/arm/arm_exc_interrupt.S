/**
 * @file
 *
 * @brief ARM interrupt exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

/*
 * These two non-volatile registers contain the program status for INT and SVC
 * mode.  It is important that they are directly accessible in THUMB
 * instruction mode.
 */
#define MODE_INT r4
#define MODE_SVC r5

/*
 * These three non-volatile registers are used to exchange information between
 * INT and SVC mode.
 */
#define SCRATCH_0 r6
#define SCRATCH_1 r7
#define SCRATCH_2 r8

/* List of scratch registers.  They will be saved and restored in INT mode. */
#define SCRATCH_LIST {SCRATCH_0, SCRATCH_1, SCRATCH_2}

/*
 * List of all volatile registers (r0, r1, r2, r3, r12 and lr), registers
 * containing the interrupt context (return address in SCRATCH_0 and saved
 * program status in SCRATCH_1) and non-volatile registers used for modes
 * (MODE_INT and MODE_SVC).  They will be saved and restored in SVC mode.
 */
#define TASK_CONTEXT_LIST \
	{r0, r1, r2, r3, MODE_INT, MODE_SVC, SCRATCH_0, SCRATCH_1, r12, lr}

/*
 * List of all volatile registers (r0, r1, r2, r3, r12 and lr) and the saved
 * program status (SCRATCH_0 register).  They will be saved and restored in INT
 * mode.
 */
#define INTERRUPT_CONTEXT_LIST \
	{r0, r1, r2, r3, SCRATCH_0, r12, lr}

.extern _ISR_Thread_dispatch
.extern _ISR_Nest_level
.extern _Thread_Dispatch_disable_level
.extern bsp_interrupt_dispatch

.arm
.globl arm_exc_interrupt
arm_exc_interrupt:

	/* Save scratch registers on INT stack */
	stmdb	sp!, SCRATCH_LIST

	/* Increment interrupt nest level */
	ldr	SCRATCH_0, =_ISR_Nest_level
	ldr	SCRATCH_1, [SCRATCH_0]
	add	SCRATCH_2, SCRATCH_1, #1
	str	SCRATCH_2, [SCRATCH_0]

	/* Branch for nested interrupts */
	cmp	SCRATCH_1, #0
	bne	nested_interrupt_context_save

	/* Move interrupt context and CPSR to scratch registers */
	mov	SCRATCH_0, lr
	mrs	SCRATCH_1, spsr
	mrs	SCRATCH_2, cpsr

	/* Switch to SVC mode */
	orr	SCRATCH_2, SCRATCH_2, #0x1
	msr	cpsr_c, SCRATCH_2

	/* Save context on SVC stack */
	stmdb	sp!, TASK_CONTEXT_LIST

	/* Save SVC mode program status to non-volatile register */
	mov	MODE_SVC, SCRATCH_2

	/* Save INT mode program status to non-volatile register */
	bic	MODE_INT, MODE_SVC, #0x1
	
	/* Switch to INT mode */
	msr	cpsr_c, MODE_INT

	/* Restore scratch registers from INT stack */
	ldmia	sp!, SCRATCH_LIST

	/*
	 * At this point the INT stack is in the exception entry state and
	 * contains no data for us.  The context is saved on the SVC stack.  We
	 * can easily switch modes via the registers MODE_INT and MODE_SVC
	 * which are valid through subroutine calls.  We can use all volatile
	 * registers in both modes.  Note that this comment describes the non
	 * nested interrupt entry.  For a nested interrupt things are
	 * different, since we can save everything on the INT stack and there
	 * is no need to switch modes.
	 */

task_context_save_done:

	/* Switch to THUMB instructions if necessary */
#ifdef __thumb__
	add	r0, pc, #1
	bx	r0
.thumb
#endif /* __thumb__ */

	/* Increment thread dispatch disable level */
	ldr	r1, =_Thread_Dispatch_disable_level
	ldr	r3, [r1]
	add	r3, r3, #1
	str	r3, [r1]

	/* Call BSP dependent interrrupt dispatcher */
	bl	bsp_interrupt_dispatch

	/* Decrement interrupt nest and thread dispatch disable level */
	ldr	r0, =_ISR_Nest_level
	ldr	r1, =_Thread_Dispatch_disable_level
	ldr	r2, [r0]
	ldr	r3, [r1]
	sub	r2, r2, #1
	sub	r3, r3, #1
	str	r2, [r0]
	str	r3, [r1]

	/* Switch to ARM instructions if necessary */
#ifdef __thumb__
.align 2
	bx	pc
.arm
#endif /* __thumb__ */

	/* Branch if we have a nested interrupt */
	cmp	r2, #0
	bne	nested_interrupt_return

	/* Branch if thread dispatching is disabled */
	cmp	r3, #0
	bne	thread_dispatch_done
	
	/*
	 * Switch to SVC mode.  It is important to call the thread dispatcher
	 * in SVC mode since overwise the INT stack may need to store an
	 * arbitrary number of contexts and it may lead to an invalid order of
	 * stack operations.
	 */
	msr	cpsr_c, MODE_SVC

	/* Call thread dispatcher */
#ifdef __thumb__
	ldr	r0, =_ISR_Thread_dispatch
	mov	lr, pc
	bx	r0
.thumb
	bx	pc
	nop
.arm
#else
	bl	_ISR_Thread_dispatch
#endif

	/* Switch to INT mode */
	msr	cpsr_c, MODE_INT

thread_dispatch_done:

	/* Save scratch registers on INT stack */
	stmdb	sp!, SCRATCH_LIST

	/* Switch to SVC mode */
	msr	cpsr_c, MODE_SVC

	/* Move INT mode program status to scratch register */
	mov	SCRATCH_2, MODE_INT

	/* Restore context from SVC stack */
	ldmia	sp!, TASK_CONTEXT_LIST

	/* Switch to INT mode */
	msr	cpsr_c, SCRATCH_2

	/* Restore interrupt context */
	mov	lr, SCRATCH_0
	msr	spsr, SCRATCH_1

	/* Restore scratch registers from INT stack */
	ldmia	sp!, SCRATCH_LIST

	/* Return from interrupt */
	subs	pc, lr, #4

nested_interrupt_context_save:

	/* Move saved program status register to scratch register */
	mrs	SCRATCH_0, spsr

	/* Save context on INT stack */
	stmdb	sp!, INTERRUPT_CONTEXT_LIST

	b	task_context_save_done

nested_interrupt_return:

	/* Restore context from INT stack */
	ldmia	sp!, INTERRUPT_CONTEXT_LIST

	/* Restore saved program status register */
	msr	spsr, SCRATCH_0

	/* Restore scratch registers from INT stack */
	ldmia	sp!, SCRATCH_LIST

	/* Return from interrupt */
	subs	pc, lr, #4
