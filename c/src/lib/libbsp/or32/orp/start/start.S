/* start.S -- bootup code for the Bender board using the Or1k
 *            architecture.
 *	
 *  Copyright (C) 2001 Chris Ziomkowski, chris@asics.ws
 *
 *  This file is distributed as part of the RTEMS package from
 *  OAR Corporation, and follows the licensing and distribution
 *  terms as stated for RTEMS. 
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */
	
#include "asm.h"


/* Since we don't yet have a memory map for Bender, I am
   assuming the following. Hopefully, this will be easily
   modified once we get the real values.

   0x00000000 - 0x00200000:    Flash/ROM (boot code / 2 MB)
   0x01000000 - 0x010FFFFF:    Synchronous SRAM (area 2 / 1 MB)
   0x10000000 - 0x1FFFFFFF:    External SDRAM (area 3 / 256 MB)
   0x20000000 - 0x2FFFFFFF:    External SDRAM (area 4 / 256 MB)

   0x80000000 - 0x8000001F:    4 16550 UART controllers
   0x80010000 - 0x80017FFF:    Internal Bender RAM
   0x80020000 - 0xFFFFFFFF:    Memory mapped Bender Peripherals

   For this version, I assume that only the flash and 32 MB
   of RAM in area 3 are populated. Everything else should
   return a bus error when accessed.
*/
.file	"start.S"
	
.data
	PUBLIC(Or1k_Interrupt_Vectors)
SYM (Or1k_Interrupt_Vectors):	
	.word	0x00000000			# No Vector
	.word	_start				# Reset Vector (Ignored)
	.word	__Internal_error_Occurred	# Bus Error
	.word	__Internal_error_Occurred	# Data Page Fault
	.word	__Internal_error_Occurred	# Instruction Page Fault
	.word	__int_reenable			# Low Priority Interrupt
	.word	__Internal_error_Occurred	# Alignment Exception
	.word	__Internal_error_Occurred	# Illegal Instruction Exception
	.word	__int_reenable			# High Priority Interrupt
	.word	__Internal_error_Occurred	# ITBL Miss
	.word	__Internal_error_Occurred	# DTBL Miss
	.word	0x00000000			# Range Exception
	.word	0x00000000			# System Call
	.word	0x00000000			# Breakpoint
	.word	0x00000000			# Trap

	PUBLIC(BOTTOM_OF_MEMORY)
SYM (BOTTOM_OF_MEMORY):	
	.word	0x00000000       # Assume RAM @ 0 for the sim

	PUBLIC(TOP_OF_MEMORY)
SYM (TOP_OF_MEMORY):	
	.word	0x800000	# Assume RAM @ 0 for the sim
	
	PUBLIC(_mem_end)
SYM (_mem_end):
	.word	0x00000000
	
        BEGIN_CODE
	.org	0x0
	/**************/
	/*   _panic   */
	/**************/

	/* Place the panic vector at 0 */
	
.proc	__panic
	.def	__panic
	.val	__panic
	.scl	2
	.type	044
	.endef
	.global	__panic
__panic:
	
	l.jal		__exit
	l.nop
	
.endproc __panic
	.def	__panic
	.val	.
	.scl	-1
	.endef

	/* Exception processing...first, we will save the
	   16 non callee saved registers which could be
	   corrupted by calling a C function. We have no
	   way of knowing which of these will be used, so
	   we have to save all of them. We will then save
	   the EPCR and ESR, in case a nested exception is
	   called. Next, we call the user function. We then
	   restore all the registers to their original
	   values, and finally disable exceptions, restore
	   EPCR and ESR (EEAR is not essential to restore)
	   and then return from the interrupt. */
	   
	/******************************************/
	/*       Normal exception handling        */
        /* Called with 80 bytes allocated on the  */
	/* stack, the vector function in r11, and */
	/* the vector number in r3. Original      */
	/* values at 28(r1) and 0(r1).            */
	/******************************************/
.proc	___standard_exception
	.def	___standard_exception
	.val	___standard_exception
	.scl	2
	.type	044
	.endef
	.global	___standard_exception
___standard_exception:		
	l.sfeqi	r11,0			/* Ignore it if it is zero */
	l.bf	L2_2
	l.sw	4(r1),r4		/* Save r4 */

	/* Ignore fast context switching in this release. */
	/* It's poorly conceived, and will probably never */
	/* be implemented...				  */

	l.sw	8(r1),r5
	l.sw	12(r1),r6
	l.sw	16(r1),r7
	
	l.mfspr	r4,r0,0x20		/* Save EPCR */
	l.mfspr	r5,r0,0x30		/* Save EEAR */
	l.mfspr	r6,r0,0x40		/* Save ESR */
	
	l.mfspr	r7,r0,17
	l.ori	r7,r7,2
	l.mtspr	r0,r7,17		/* Reenable exceptions */

	l.sw	20(r1),r8
	l.sw	24(r1),r9
	l.sw	32(r1),r12
	l.sw	36(r1),r14
	l.sw	40(r1),r16
	l.sw	44(r1),r18
	l.sw	48(r1),r20
	l.sw	52(r1),r22
	l.sw	56(r1),r24
	l.sw	60(r1),r26
	l.sw	64(r1),r28
	l.sw	68(r1),r30
	l.sw	72(r1),r4		/* Save EPCR. User could change r4 */

	/* Now, call the installed handler with the arguments:
		r3 ==> vector # (1-14)
		r4 ==> EPCR
		r5 ==> EEAR
		r6 ==> ESR
		r11 ==> User function
	*/

	l.jal	___user_function	/* Call the user routine */
	l.sw	76(r1),r6		/* Save ESR. User could change r6 */
					/* Ignore r5 (EEAR). It is not critical for state */

	l.lwz	r30,68(r1)
	l.lwz	r28,64(r1)
	l.lwz	r26,60(r1)
	l.lwz	r24,56(r1)
	l.lwz	r22,52(r1)
	l.lwz	r20,48(r1)
	l.lwz	r18,44(r1)
	l.lwz	r16,40(r1)
	l.lwz	r14,36(r1)
	l.lwz	r12,32(r1)
	l.lwz	r9,24(r1)
	l.lwz	r8,20(r1)
	l.lwz	r7,16(r1)
	l.lwz	r5,8(r1)

	l.addi	r6,r0,-3		/* Set r6 to 0xFFFFFFFD */
	l.mfspr	r3,r0,17		/* Get SR value */
	l.and	r3,r3,r6		/* Clear exception bit */
	l.mfspr	r0,r3,17		/* Disable exceptions */
	
	l.lwz	r6,76(r1)		/* Recover ESR */
	l.lwz	r4,72(r1)		/* Recover EPCR */
	l.mtspr	r0,r4,0x20		/* Restore ESR */
	l.mtspr	r0,r6,0x40		/* Restore EPCR */
	l.lwz	r6,12(r1)
	l.lwz	r4,4(r1)

L2_2:
	l.lwz	r11,28(r1)
	l.lwz	r3,0(r1)
	l.addi	r1,r1,80
	l.rfe
	l.nop				/* The document doesn't say this is
					   a delay slot instruction, but the
					   simulator doesn't work without this. */
	
.endproc ___standard_exception
	.def	___standard_exception
	.val	.
	.scl	-1
	.endef
	

	/** Currently, about 57 of the 64 valid address locations
	    are being used here. If you add code to the above
	    routine, make sure it isn't more than 7 instructions
	    or you will overflow into the reset vector. **/
		
	/****************************/
	/* Reset vector static code */
	/****************************/
	.org	0x100
.proc	___rst
	.global	___rst
___rst:
	/* Set the stack pointer */
	l.movhi		r1,hi(_TOP_OF_MEMORY)
	l.ori		r1,r1,lo(_TOP_OF_MEMORY)
	l.lwz		r1,0(r1)   /* Dereference it */
	
	/* Set the frame pointer */
	l.add		r2,r0,r1

        l.mfspr         r3,r0,17	/* Get SR value */
        l.ori           r3,r3,2         /* Set exception enable bit */
        l.j             _start          /* Jump to main routine */
        l.mtspr         r0,r3,17        /* Enable exceptions (DELAY) */
.endproc ___rst

	/***********************************************************/
	/* Note:  right after the reset vector, we are going to    */
	/* place a table with the necessary values to initialize   */
	/* the memory controller. This pointer will be set and     */
	/* passed to the _start routine in r4. The first thing the */
	/* the _start routine will do is to initialize the memory  */
	/* controller. The code to initialze the memory controller */
	/* is expected to be larger than the 50 some odd           */
	/* instructions that are remaining here before the bus     */
	/* error vector, which is why it is left to the _start     */
	/* routine.                                                */
	/***********************************************************/
	
	/********************************/
	/* Bus Error vector static code */
	/********************************/
	.org	0x200
.proc	___bus_error
	.global	___bus_error
___bus_error:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,8(r11)
	l.j	___standard_exception
	l.addi	r3,r0,2
	
.endproc ___bus_error

	/* Put _Internal_error_Occurred and _int_reenable here */
	/* No reason to waste space...it'll be filled with 0 if */
	/* we don't... */
	
	/********************************/
	/*   _Internal_error_Occurred   */
	/********************************/
		
.proc	__Internal_error_Occurred
	.def	__Internal_error_Occurred
	.val	__Internal_error_Occurred
	.scl	2
	.type	044
	.endef
	.global	__Internal_error_Occurred
__Internal_error_Occurred:
	
	l.jal		__panic
	l.nop
	
.endproc __Internal_error_Occurred
	.def	__Internal_error_Occurred
	.val	.
	.scl	-1
	.endef

	
	/*********************/
	/*   _int_reenable   */
	/*********************/
		
.proc	__int_reenable
	.def	__int_reenable
	.val	__int_reenable
	.scl	2
	.type	044
	.endef
	.global	__int_reenable
__int_reenable:
	
	l.mfspr	r11,r0,17
	l.ori	r11,r11,0x04
	l.jr	r9
	l.mtspr r0,r11,17
	
.endproc __int_reenable
	.def	__int_reenable
	.val	.
	.scl	-1
	.endef

	/*********************&**/
	/*   ___user_function   */
	/************************/
		
.proc	___user_function
	.def	___user_function
	.val	___user_function
	.scl	2
	.type	044
	.endef
	.global	___user_function
___user_function:

	/* r11 contains the address to call. We can
	   modify r7, r8, r12, and r14 at will */
	
	l.movhi	r7,hi(__Thread_Dispatch_disable_level)
	l.ori	r7,r7,lo(__Thread_Dispatch_disable_level)
	l.lwz	r8,0(r7)
	
	l.addi	r1,r1,-8	#  Stack must be DWORD aligned
	l.sw	0(r1),r9	#  Save the return address

	l.addi	r8,r8,1		# Increment __Thread_Dispatch...
	l.jalr	r11
	l.sw	0(r7),r8	# Disable thread dispatching
	
	/* Now, we need to determine if we need to
	   service the RTEMS environment. RTEMS tries
	   to draw a distinction between a RAW handler
	   (where this isn't necessary) and an RTEMS
	   handler. However, it appears almost all ISR's
	   will not be RAW under this definition, and
	   those that are will not honestly be hurt by
	   the 20 or so extra cycles it will take to do
	   the following code. If there is a very frequent
	   interrupt, then it should probably be hard
	   coded into the static routine anyway, rather
	   than suffer the hit of calling it indirectly */

	/* Note:  RTEMS recommends incrementing and
	   decrementing the _ISR_Nest_Level as well.
	   We are specifically not doing this because
	   in the Or1k architecture it is impossible
	   to nest interrupts. Interrupts must run to
	   completion before reenabling. If there is a
	   significant task to be done, then it should
	   run in a bottom half handler, similar to the
	   way Linux works. In theory though, even if
	   we do allow nested interrupts, there is no
	   reason for this flag, as it seems to be for
	   the purpose of restoring the normal stack in
	   place of the interrupt stack. We don't use a
	   separate exception stack, so this should not
	   be an issue for us. */
		
	l.movhi	r7,hi(__Thread_Dispatch_disable_level)
	l.ori	r7,r7,lo(__Thread_Dispatch_disable_level)
	l.lwz	r8,0(r7)
	l.addi	r8,r8,-1	# Decrement __Thread_Dispatch...
	l.sw	0(r7),r8	# Memory stall likely here...		

	l.sfeqi	r8,0		# Skip if _Thread_Dispatch != 0
	l.bnf	L4_2
	l.movhi	r7,hi(__Context_Switch_necessary)
	
	l.ori	r7,r7,lo(__Context_Switch_necessary)
	l.lwz	r8,0(r7)
		
	l.movhi	r7,hi(__ISR_Signals_to_thread_executing)
	l.ori	r7,r7,lo(__ISR_Signals_to_thread_executing)
	l.lwz	r12,0(r7)

	l.sfeqi	r8,0		# Skip if __Context... is false
	l.bf	L4_2
	l.movhi	r14,hi(__Thread_Dispatch)
	
	l.sfeqi	r12,0		# Skip if __ISR... is true
	l.bnf	L4_2
	l.ori	r14,r14,lo(__Thread_Dispatch)
	
	l.jalr	r14
	l.sw	0(r7),r0	# Set __ISR... to false
		
L4_2:			
	l.lwz	r9,0(r1)	#  Recover the return address
	l.jr	r9
	l.addi	r1,r1,8		#  Reset the stack
	
.endproc ___user_function
	.def	___user_function
	.val	.
	.scl	-1
	.endef

	
	/* Code wasted between here and 0x300 */
	
	/**************************************/
	/* Data Page Fault vector static code */
	/**************************************/
	.org	0x300
.proc	___data_page_fault
	.global	___data_page_fault
___data_page_fault:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,12(r11)
	l.j	___standard_exception
	l.addi	r3,r0,3
.endproc ___data_page_fault

	/* Code wasted between here and 0x400 */
		
	/*********************************************/
	/* Instruction Page Fault vector static code */
	/*********************************************/
	.org	0x400
.proc	___insn_page_fault
	.global	___insn_page_fault
___insn_page_fault:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,16(r11)
	l.j	___standard_exception
	l.addi	r3,r0,4
.endproc ___insn_page_fault

	/* Code wasted between here and 0x500 */
	
	/**************************************/
	/* Low Priority Interrupt static code */
	/**************************************/
	.org	0x500
.proc	___low_priority_int
	.global	___low_priority_int
___low_priority_int:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.mfspr	r3,r0,17			# Get the SR
	l.addi	r11,r0,-5			# r11 = 0xFFFFFFFB
	l.and	r11,r11,r3			# Clear the EIR bit
	l.mtspr	r0,r11,17			# Set the SR w/o INT
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,20(r11)
	l.j	___standard_exception
	l.addi	r3,r0,5
.endproc ___low_priority_int

	/* Code wasted between here and 0x600 */
	
	/******************************************/
	/* Alignment Exception vector static code */
	/******************************************/
	.org	0x600
.proc	___alignment_exception
	.global	___alignment_exception
___alignment_exception:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,24(r11)
	l.j	___standard_exception
	l.addi	r3,r0,6
.endproc ___alignment_exception

	/* Code wasted between here and 0x700 */
	
	/******************************************/
	/* Illegal Instruction vector static code */
	/******************************************/
	.org	0x700
.proc	___illegal_instruction
	.global	___illegal_instruction
___illegal_instruction:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,28(r11)
	l.j	___standard_exception
	l.addi	r3,r0,7
.endproc ___illegal_instruction

	/* Code wasted between here and 0x800 */
	
	/***************************************/
	/* High Priority Interrupt static code */
	/***************************************/
	.org	0x800
.proc	___high_priority_int
	.global	___high_priority_int
___high_priority_int:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.mfspr	r3,r0,17			# Get the SR
	l.addi	r11,r0,-5			# r11 = 0xFFFFFFFB
	l.and	r11,r11,r3			# Clear the EIR bit
	l.mtspr	r0,r11,17			# Set the SR w/o INT
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,32(r11)
	l.j	___standard_exception
	l.addi	r3,r0,8
.endproc ___high_priority_int

	/* Code wasted between here and 0x900 */
	
	/********************************/
	/* ITBL Miss vector static code */
	/********************************/
	.org	0x900
.proc	___ITBL_miss_exception
	.global	___ITBL_miss_exception
___ITBL_miss_exception:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,36(r11)
	l.j	___standard_exception
	l.addi	r3,r0,9
.endproc ___ITBL_miss_exception

	/* Code wasted between here and 0xA00 */
	
	/********************************/
	/* DTBL Miss vector static code */
	/********************************/
	.org	0xA00
.proc	___DTBL_miss_exception
	.global	___DTBL_miss_exception
___DTBL_miss_exception:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,40(r11)
	l.j	___standard_exception
	l.addi	r3,r0,10
.endproc ___DTBL_miss_exception

	/* Code wasted between here and 0xB00 */
	
	/**************************************/
	/* Range Exception vector static code */
	/**************************************/
	.org	0xB00
.proc	___range_exception
	.global	___range_exception
___range_exception:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,44(r11)
	l.j	___standard_exception
	l.addi	r3,r0,11
.endproc ___range_exception

	/* Code wasted between here and 0xC00 */
	
	/**********************************/
	/* System Call vector static code */
	/**********************************/
	.org	0xC00
.proc	___system_call
	.global	___system_call
___system_call:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,48(r11)
	l.j	___standard_exception
	l.addi	r3,r0,12
.endproc ___system_call

	/* Code wasted between here and 0xD00 */
	
	/**********************************/
	/* Breakpoint vector static code */
	/**********************************/
	.org	0xD00
.proc	___breakpoint
	.global	___breakpoint
___breakpoint:

	/* In keeping with the necessary requirements for
	   gdb to work, we are limiting this vector to
	   only 2 statements, which effect an immediate
	   return. At a later date, we may insert a debug
	   monitor here that will do even more, but for
	   now, this is all we want. */
	l.rfe
	l.nop

.endproc ___breakpoint

	/* Code wasted between here and 0xE00 */
	
	/*************************************/
	/* Trap Exception vector static code */
	/*************************************/
	.org	0xE00
.proc	___trap_exception
	.global	___trap_exception
___trap_exception:
	l.addi	r1,r1,-80
	l.sw	0(r1),r3
	l.sw	28(r1),r11
	l.movhi	r11,hi(_Or1k_Interrupt_Vectors)
	l.ori	r11,r11,lo(_Or1k_Interrupt_Vectors)
	l.lwz	r11,56(r11)
	l.j	___standard_exception
	l.addi	r3,r0,14
.endproc ___trap_exception

	/* Code wasted between here and 0x2000 */
	
	/* Exceptions from 0xF00 to 0x1F00 are not defined */
	/* in the Or1k architecture. They should be filled */
	/* in here for other implementations.              */

	.org	0x2000		/* Start after exception vector table */
		
	/*********************/
	/*       start       */
	/*********************/

	/* This is where we jump to right after the reset exception
	   handler. The system configuration information should
	   be passed to us in a pointer in r4. Generally, the
	   reset vector will call this routine directly, and
	   the memory configuration information will be stored
	   in the ROM/Flash image. It was decided no attempt
	   would be made to automatically determine this
	   information by probing, as the scheme would be too
	   complex and inherently unreliable. */

	/* Initialize strings and structures here */	
L_program:	
	.ascii	"RTEMS_or1k\000"
	.align	4
L_argv:	
	.word	L_program
				
.proc _start
	.def	_start
	.val	_start
	.scl	2
	.type	044
	.endef
	.global	_start
_start:	

	/* Initialize the memory controller here!
	   Discussions with Rudi have stated that
	   the first few bytes of the ROM image should 
	   contain a RAM map as opposed to trying to
	   figure out what to do based on probing. This
	   means a separate build of the OS for every
	   possible board configuration, but there
	   doesn't seem to be a better alternative. */

	/*** FIX ME! Initialize the external memory controller! ***/
	
	/* Move the data segment to RAM. Alternatively, we may
	   copy the text segment as well. For now, we'll assume
	   that the cache gives us sufficient performance that this
	   is not necessary. It will be very easy to add this later.
	 */	
	l.movhi		r4,hi(_data_start)
	l.ori		r4,r4,lo(_data_start)
	l.movhi		r5,hi(_BOTTOM_OF_MEMORY)
	l.ori		r5,r5,lo(_BOTTOM_OF_MEMORY)
	l.lwz		r5,0(r5)            # Dereference it
	l.add		r5,r5,r4	    # Place it in memory above the text segment
	l.movhi		r3,hi(_edata)
	l.ori		r3,r3,lo(_edata)
	
L3_0:	
	l.lwz		r6,0(r4)
	l.addi		r5,r5,4
	l.addi		r4,r4,4
	l.sfeq		r3,r4
	l.bnf		L3_0
	l.sw		-4(r5),r6           # Minimize write after read stalls

	/* Initialize the BSS segment */
	l.movhi		r3,hi(_bss_start)
	l.ori		r3,r3,lo(_bss_start)
	l.sub		r3,r3,r4
	l.add		r3,r3,r5
	l.sfleu		r3,r5
	l.bf		L3_2		   # Check for no BSS segment!
	l.nop
		
L3_1:
	l.sw		0(r5),r0
	l.sfeq		r5,r3
	l.bnf		L3_1
	l.addi		r5,r5,4	

L3_2:
	/* Tell everyone where the heap begins */
	l.movhi		r4,hi(__mem_end)
	l.ori		r4,r4,lo(__mem_end)
	l.sw		0(r4),r5
			
	/* Due to what I consider a bug in RTEMS, the entire
	   heap must be zeroed. I think this is the dumbest thing
	   I've ever heard, but whatever turns them on. I'd rather
	   see the code which depends on this behavior fixed. I
	   myself have never written code which assumes zeroes
	   will be returned from memory allocated from the heap. 
	   Anyway, if I don't do it here, I have to set a flag in
	   the CPU structure which then will do it anyway, but
	   from less efficient C code! Zero from here to the
	   stack pointer... One day when I'm old and gray maybe
	   I'll set this to random values instead and fix
	   whatever breaks. */
	
	l.sw		0(r5),r0
	l.sfeq		r5,r1
	l.bnf		L3_3
	l.addi		r5,r5,4	
	
L3_3:
	l.addi		r3,r0,1		/* Set argc to 1 */
	l.movhi		r4,hi(L_argv)	/* Initialize argv */
	l.ori		r4,r4,lo(L_argv)
	l.addi		r5,r5,0		/* Set envp to NULL */
		
        l.mfspr         r11,r0,17	/* Get SR value */
        l.ori           r11,r11,0x4     /* Set interrupt enable bit */
        l.jal           _boot_card      /* Boot up the card...run the OS */
        l.mtspr         r0,r11,17       /* Enable exceptions (DELAY) */
	
	/* We're done. We exited normally. Shut down. */
	l.jal		__exit
	l.nop
	
.endproc _start
	.def	_start
	.val	.
	.scl	-1
	.endef
	
	END_CODE

