/*
 * stubinit.S - low level startup code for the ROM-based R4600 gdb stub
 *
 *                   THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  The following software is offered for use in the public domain.
 *  There is no warranty with regard to this software or its performance
 *  and the user must accept the software "AS IS" with all faults.
 *
 *  THE CONTRIBUTORS DISCLAIM ANY WARRANTIES, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

	.nolist
#include "r4600.h"
#include "ioaddr.h"
#include "gdb_if.h"
	.list

#define	SERIAL_SPEED	19200		/* use 19200 bps serial link */
#define	FORCE_PC_TO_32_BITS		/* to work around a bug in gdb-4.16 */
#define	STACKSIZE	4096		/* allocate 4K bootstack */

	.sdata
	.globl	_gp			/* value to put in gp register */

	.bss
	.globl	_fbss			/* start of .bss area */
	.space	STACKSIZE		/* allocate the exception stack */
_stack:					/* stack top here (stack grows DOWN) */
	.globl	registers		/* register save area */
registers:
	.space	NUM_REGS*8
	.globl	_end			/* end of .bss area */

	.eject
	.text
	.globl	handle_exception	/* C exception handler */
	.set	noreorder
	.set	noat
	.globl	_reset_exception	/* entry point into the EPROM */
					/* it MUST reside at bfc00000 */
/*
 * Set the STATUS register initial state:  mark CP1 usable, MIPS-3
 * floating point registers enabled, boot exception vectors selected,
 * cache parity exceptions disabled, kernel TLB miss to XTLB refill
 * vector, machine in kernel mode, exception level cleared, error
 * level set, interrupt enable cleared, all interrupts masked.  Note
 * that the value written here is what will saved in the register
 * image;  this will put the machine in a reasonable default state
 * even if the debug exception handler does nothing to the status
 * register image.
 */
_reset_exception:
	li	k0,SR_CU1 + SR_FR + SR_BEV + SR_DE + SR_KX \
			+ SR_KSU_KERN + SR_ERL
	mtc0	k0,C0_STATUS

/*
 * Mark each TLB entry as "invalid" and fill the tag field with a VPN that
 * cannot occur and an ASID that will not match the value left in EntryHi.
 */
	li	k0,K1BASE+EH_ASID_MASK	/* EntryHi = unmapped VPN2, ASID 255 */
	dmtc0	zero,C0_ENTRYLO0	/* EntryLo0 = invalid */
	dmtc0	zero,C0_ENTRYLO1	/* EntryLo1 = invalid */
	mtc0	zero,C0_PAGEMASK	/* PageSize = 4K */
	li	k1,(NTLBENTRIES-1)	/* TLB index */
1:
	dmtc0	k0,C0_ENTRYHI
	mtc0	k1,C0_INDEX
	addu	k0,(1<<EH_VPN2_SHIFT)	/* incr VPN2 */
	tlbwi
	bnel	k1,zero,1b
	addiu	k1,k1,-1		/* decr INDEX */

	dmtc0	zero,C0_ENTRYHI		/* Clear EntryHi to select ASID = 0 */
	nop				/* wait for update to take effect */

	.eject
/*
 * Invalidate each line in both of the caches, then
 * set kseg0 config to cached/writeback/non-coherent.
 */
	mtc0	zero,C0_TAGLO		/* clear TAGLO and TAGHI - used below */
	mtc0	zero,C0_TAGHI		/* to set cache line state to invalid */
	nop

	mfc0	k0,C0_CONFIG		/* read config info */
	li	k1,(1<<12)		/* base size is 4K */
	and	k0,CFG_ICMASK		/* isolate I-cache shift count param */
	srl	k0,CFG_ICSHIFT		/* which determines the actual size */
	sll	k1,k0			/* shift left to get actual size */

	mfc0	k0,C0_CONFIG		/* read config info */
	nop				/* wait for CP0 read to take effect */
	and	k0,CFG_IBMASK		/* isolate line size config bit */
	bne	k0,zero,2f		/* jump if line size is 32 bytes */
	lui	k0,((K0BASE>>16)&0xffff)/* set lower limit to unmapped addr */

	addu	k1,k0			/* this prevents a TLB exception */
	addiu	k1,-16			/* set upper limit for 16 byte line */
1:
	cache	Index_Store_Tag_I,(k0)	/* write TAGLO/TAGHI to the cache */
	bnel	k0,k1,1b		/* do that by index so that a */
	addiu	k0,k0,16		/* cache hit is not required */
	b	4f
	nop
2:
	addu	k1,k0			/* come here for 32 byte line size */
	addiu	k1,-32
3:
	cache	Index_Store_Tag_I,(k0)
	bnel	k0,k1,3b
	addiu	k0,k0,32
4:
	.eject
	mfc0	k0,C0_CONFIG		/* repeat for D-cache */
	li	k1,(1<<12)
	and	k0,CFG_DCMASK
	srl	k0,CFG_DCSHIFT
	sll	k1,k0

	mfc0	k0,C0_CONFIG
	nop
	and	k0,CFG_DBMASK
	bne	k0,zero,6f
	lui	k0,((K0BASE>>16)&0xffff)

	addu	k1,k0
	addiu	k1,-16
5:
	cache	Index_Store_Tag_D,(k0)
	bnel	k0,k1,5b
	addiu	k0,k0,16
	b	8f
	nop
6:
	addu	k1,k0
	addiu	k1,-32
7:
	cache	Index_Store_Tag_D,(k0)
	bnel	k0,k1,7b
	addiu	k0,k0,32
8:
	mfc0	k0,C0_CONFIG		/* read config info one last time */
	li	k1,~CFG_K0C_MASK	/* clear K0 config field */
	and	k0,k1			/* in CONFIG image register image */
	or	k0,CFG_C_WRITEBACK	/* set config to cached/writeback */
	mtc0	k0,C0_CONFIG		/* write back to CP0 */
	nop				/* wait for update to take effect */

	.eject
/*
 * Initialize the serial port
 */
	li	k0,DIV_LATCH_EN + WORD_LEN_MASK
	la	k1,ISA_IO_BASE		/* set 8/bits char, no parity, and */
	sb	k0,LINE_CTL_COM1(k1)	/* enable access to divisor latch */

	li	k0,(1843200/(16*SERIAL_SPEED))
	sb	k0,DIV_LO_COM1(k1)	/* set clock divisor low byte */
	sra	k0,8
	sb	k0,DIV_HI_COM1(k1)	/* set clock divisor high byte */

	li	k0,WORD_LEN_MASK	/* set 8/bits char, no parity, and */
	sb	k0,LINE_CTL_COM1(k1)	/* disable access to divisor latch */

	lbu	k0,DATA_REG_COM1(k1)	/* read & discard any existing char */
	sb	zero,INT_ENA_COM1(k1)	/* disable all interrupt sources */

	li	k0,RTS + DTR		/* turn RTS and DTR on */
	sb	k0,MODEM_CTL_COM1(k1)

/*
 * Reset all pending ISA interrupts
 */
	la	k1,ISA_IRQ3_RESET
	sd	zero,(k1)
	la	k1,ISA_IRQ4_RESET
	sd	zero,(k1)
	la	k1,ISA_IRQ5_RESET
	sd	zero,(k1)
	la	k1,ISA_IRQ9_RESET
	sd	zero,(k1)

	.eject
/*
 * Clear the CAUSE register to indicate the
 * absence of software-initiated exceptions.
 */
	mtc0	zero,C0_CAUSE

/*
 * Clear floating point errors and configure
 * the FPU to flush denormals to zero.
 */
	li	k0,CSR_FS
	ctc1	k0,C1_CSR

/*
 * Clear the .bss area
 */
	la	k0,_fbss
	la	k1,_end
1:
	addiu	k1,k1,-8
	bne	k1,k0,1b
	sd	zero,(k1)

/*
 * Save the ErrorEPC register, in case of a pushbutton
 * reset, and join the general exception-handling path.
 */
	la	k0,registers
	dmfc0	k1,C0_ERROREPC
	j	_common_path_join
	sd	k1,8*PC(k0)

	.align	9
	.eject
_tlbmiss_exception:			/* bfc00200: tlbmiss exception */
	j	_general_exception
	nop

	.align	7
_xtlbmiss_exception:			/* bfc00280: xtlbmiss exception */
	j	_general_exception
	nop

	.align	7
_cache_parity_error:			/* bfc00300: cache parity error */
	j	_reset_exception
	nop

	.align	7
					/* bfc00380: general exception */
/*
 * Clear the register save area
 */
_general_exception:
	la	k0,registers
	addiu	k1,k0,8*(NUM_REGS-1)
1:
	sd	zero,(k1)
	bnel	k1,k0,1b
	addiu	k1,k1,-8

/*
 * Save the PC
 */
	dmfc0	k1,C0_EPC
	nop
	sd	k1,8*PC(k0)

	.eject
/*
 * Save the SR, CAUSE, and BAD_VA registers.
 */
_common_path_join:
	mfc0	k1,C0_STATUS
	nop
	sd	k1,8*SR(k0)
	mfc0	k1,C0_CAUSE
	nop
	sd	k1,8*CAUSE(k0)
	mfc0	k1,C0_BADVADDR
	nop
	sd	k1,8*BAD_VA(k0)

/*
 * Save LO, HI, and the general registers.
 * Note that zero, k0 & k1 are not saved.
 */
	mflo	k1
	sd	k1,8*LO(k0)
	mfhi	k1
	sd	k1,8*HI(k0)

	/* zero is hard-wired */
	sd	at,8*AT(k0)

	sd	v0,8*V0(k0)
	sd	v1,8*V1(k0)

	sd	a0,8*A0(k0)
	sd	a1,8*A1(k0)
	sd	a2,8*A2(k0)
	sd	a3,8*A3(k0)

	sd	t0,8*T0(k0)
	sd	t1,8*T1(k0)
	sd	t2,8*T2(k0)
	sd	t3,8*T3(k0)
	sd	t4,8*T4(k0)
	sd	t5,8*T5(k0)
	sd	t6,8*T6(k0)
	sd	t7,8*T7(k0)

	.eject
	sd	s0,8*S0(k0)
	sd	s1,8*S1(k0)
	sd	s2,8*S2(k0)
	sd	s3,8*S3(k0)
	sd	s4,8*S4(k0)
	sd	s5,8*S5(k0)
	sd	s6,8*S6(k0)
	sd	s7,8*S7(k0)

	sd	t8,8*T8(k0)
	sd	t9,8*T9(k0)

	/* k0 is not saved */
	/* k1 is not saved */

	sd	gp,8*GP(k0)
	sd	sp,8*SP(k0)
	sd	s8,8*S8(k0)
	sd	ra,8*RA(k0)

/*
 * Save the floating point control registers.
 */
	cfc1	k1,C1_CSR
	nop
	sd	k1,8*FCSR(k0)
	cfc1	k1,C1_IRR
	nop
	sd	k1,8*FIRR(k0)

/*
 * Save the even-numbered floating point registers.
 */
	sdc1	$0,8*F0(k0)
	sdc1	$2,8*F2(k0)
	sdc1	$4,8*F4(k0)
	sdc1	$6,8*F6(k0)
	sdc1	$8,8*F8(k0)
	sdc1	$10,8*F10(k0)
	sdc1	$12,8*F12(k0)
	sdc1	$14,8*F14(k0)
	sdc1	$16,8*F16(k0)
	sdc1	$18,8*F18(k0)
	sdc1	$20,8*F20(k0)
	sdc1	$22,8*F22(k0)
	sdc1	$24,8*F24(k0)
	sdc1	$26,8*F26(k0)
	sdc1	$28,8*F28(k0)
	sdc1	$30,8*F30(k0)

	.eject
/*
 * If they are enabled, save the odd-numbered
 * floating point registers as well.
 */
	mfc0	k1,C0_STATUS
	li	at,SR_FR
	and	k1,at
	beq	k1,zero,1f
	nop
	sdc1	$1,8*F1(k0)
	sdc1	$3,8*F3(k0)
	sdc1	$5,8*F5(k0)
	sdc1	$7,8*F7(k0)
	sdc1	$9,8*F9(k0)
	sdc1	$11,8*F11(k0)
	sdc1	$13,8*F13(k0)
	sdc1	$15,8*F15(k0)
	sdc1	$17,8*F17(k0)
	sdc1	$19,8*F19(k0)
	sdc1	$21,8*F21(k0)
	sdc1	$23,8*F23(k0)
	sdc1	$25,8*F25(k0)
	sdc1	$27,8*F27(k0)
	sdc1	$29,8*F29(k0)
	sdc1	$31,8*F31(k0)
1:

/*
 * Set up the global pointer and the stack
 * and invoke the exception handler.
 */
	la	gp,_gp
	la	sp,_stack
	jal	handle_exception
	addiu	fp,sp,0

	.eject
/*
 * On exit from the exception handler invalidate each line in the I-cache
 * and write back each dirty line in the D-cache.  This needs to be done
 * before the target program is resumed in order to ensure that software
 * breakpoints and downloaded code will actually take effect.
 */
	mfc0	t0,C0_CONFIG		/* read config info into t0 */
	nop				/* wait for CP0 read to take effect */

	and	t1,t0,CFG_ICMASK	/* isolate I-cache shift */
	srl	t1,t1,CFG_ICSHIFT	/* count param into t1 */
	li	t2,(1<<12)		/* put base size in t2 */
	sllv	t2,t2,t1		/* shift left to get actual size */

	and	t1,t0,CFG_IBMASK	/* isolate linesize config bit */
	bne	t1,zero,1f		/* guess line size is 32 bytes */
	addiu	t3,zero,32		/* skip next load if guessed right */
	addiu	t3,zero,16		/* else set size to 16 bytes */
1:
	la	t1,K0BASE		/* set lower & upper address limits */
	addu	t2,t2,t1		/* use kseg0 to avoid TLB exception */
	subu	t2,t2,t3
2:
	cache	Index_Invalidate_I,(t1)	/* invalidate each I-cache line */
	bnel	t1,t2,2b		/* do this by index so that */
	addu	t1,t1,t3		/* a hit is not required */

	and	t1,t0,CFG_DCMASK	/* repeat for D-cache with writeback */
	srl	t1,t1,CFG_DCSHIFT
	li	t2,(1<<12)
	sllv	t2,t2,t1

	and	t1,t0,CFG_DBMASK
	bne	t1,zero,3f
	addiu	t3,zero,32
	addiu	t3,zero,16
3:
	la	t1,K0BASE
	addu	t2,t2,t1
	subu	t2,t2,t3
4:
	cache	Index_Writeback_Inv_D,(t1)
	bnel	t1,t2,4b
	addu	t1,t1,t3

	.eject
/*
 * Now restore the registers.  Note that they may
 * have been modified while within the debugger.
 * Start with even-numbered floating point registers.
 */
	la	k0,registers
	ldc1	$0,8*F0(k0)
	ldc1	$2,8*F2(k0)
	ldc1	$4,8*F4(k0)
	ldc1	$6,8*F6(k0)
	ldc1	$8,8*F8(k0)
	ldc1	$10,8*F10(k0)
	ldc1	$12,8*F12(k0)
	ldc1	$14,8*F14(k0)
	ldc1	$16,8*F16(k0)
	ldc1	$18,8*F18(k0)
	ldc1	$20,8*F20(k0)
	ldc1	$22,8*F22(k0)
	ldc1	$24,8*F24(k0)
	ldc1	$26,8*F26(k0)
	ldc1	$28,8*F28(k0)
	ldc1	$30,8*F30(k0)

/*
 * If they are enabled, restore the odd-numbered
 * floating point registers as well.
 */
	mfc0	k1,C0_STATUS
	li	at,SR_FR
	and	k1,at
	beq	k1,zero,1f
	nop
	ldc1	$1,8*F1(k0)
	ldc1	$3,8*F3(k0)
	ldc1	$5,8*F5(k0)
	ldc1	$7,8*F7(k0)
	ldc1	$9,8*F9(k0)
	ldc1	$11,8*F11(k0)
	ldc1	$13,8*F13(k0)
	ldc1	$15,8*F15(k0)
	ldc1	$17,8*F17(k0)
	ldc1	$19,8*F19(k0)
	ldc1	$21,8*F21(k0)
	ldc1	$23,8*F23(k0)
	ldc1	$25,8*F25(k0)
	ldc1	$27,8*F27(k0)
	ldc1	$29,8*F29(k0)
	ldc1	$31,8*F31(k0)
1:
	.eject
/*
 * Restore the floating point control & status register.
 * FIRR is not restored because it is read-only.
 */
	ld	k1,8*FCSR(k0)
	ctc1	k1,C1_CSR

/*
 * Restore LO, HI, and the general registers.
 */
	ld	k1,8*LO(k0)
	mtlo	k1
	ld	k1,8*HI(k0)
	mthi	k1

	/* zero is hard-wired */
	ld	at,8*AT(k0)

	ld	v0,8*V0(k0)
	ld	v1,8*V1(k0)

	ld	a0,8*A0(k0)
	ld	a1,8*A1(k0)
	ld	a2,8*A2(k0)
	ld	a3,8*A3(k0)

	ld	t0,8*T0(k0)
	ld	t1,8*T1(k0)
	ld	t2,8*T2(k0)
	ld	t3,8*T3(k0)
	ld	t4,8*T4(k0)
	ld	t5,8*T5(k0)
	ld	t6,8*T6(k0)
	ld	t7,8*T7(k0)

	ld	s0,8*S0(k0)
	ld	s1,8*S1(k0)
	ld	s2,8*S2(k0)
	ld	s3,8*S3(k0)
	ld	s4,8*S4(k0)
	ld	s5,8*S5(k0)
	ld	s6,8*S6(k0)
	ld	s7,8*S7(k0)

	ld	t8,8*T8(k0)
	ld	t9,8*T9(k0)

	/* k0 is not restored */
	/* k1 is not restored */

	ld	gp,8*GP(k0)
	ld	sp,8*SP(k0)
	ld	s8,8*S8(k0)
	ld	ra,8*RA(k0)

	.eject
/*
 * Restore the cause register, the status register, and the PC.  Note
 * that the saved PC is loaded into the ErrorEPC if ERL is set in the
 * status register image and is loaded into the EPC otherwise.
 */
	ld	k1,8*CAUSE(k0)
	mtc0	k1,C0_CAUSE
	ld	k1,8*SR(k0)
	mtc0	k1,C0_STATUS
	andi	k1,k1,SR_ERL
	beq	k1,zero,1f
	ld	k1,8*PC(k0)
#if !defined(FORCE_PC_TO_32_BITS)
	b	2f
	dmtc0	k1,C0_ERROREPC
1:
	dmtc0	k1,C0_EPC
2:
#else
	addu	k1,k1,zero
	b	2f
	dmtc0	k1,C0_ERROREPC
1:
	addu	k1,k1,zero
	dmtc0	k1,C0_EPC
2:
#endif

/*
 * Flush the write buffer to memory.
 */
	sync

/*
 * Hide the contents of k0 & k1
 * and return to the user program.
 */
	move	k0,zero
	move	k1,zero
	eret

	.eject
/*
 * char getDebugChar (void);
 */
	.set	reorder
	.globl	getDebugChar
	.ent	getDebugChar
getDebugChar:
	la	t0,ISA_IO_BASE		/* point to ISA slot I/O */
1:
	lbu	t1,LINE_STS_COM1(t0)	/* read line status register */
	and	t1,RX_CHAR_AVA		/* isolate RX status bit */
	beqz	t1,1b			/* loop until char is available */

	lbu	v0,DATA_REG_COM1(t0)	/* read the character */
	j	ra			/* and return */
	.end	getDebugChar

/*
 * void putDebugChar (char);
 */
	.set	reorder
	.globl	putDebugChar
	.ent	putDebugChar
putDebugChar:
	la	t0,ISA_IO_BASE		/* point to ISA slot I/O */
1:
	lbu	t1,LINE_STS_COM1(t0)	/* read line status register */
	and	t1,TX_BUF_EMPTY		/* isolate TX status bit */
	beqz	t1,1b			/* loop while buffer is full */

	sb	a0,DATA_REG_COM1(t0)	/* write the outgoing character */
	sync				/* flush the write buffer to memory */
	j	ra			/* and return */
	.end	putDebugChar
