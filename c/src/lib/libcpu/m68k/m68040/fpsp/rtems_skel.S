//
//
//	skeleton.sa 3.2 4/26/91
//
//	This file contains code that is system dependent and will
//	need to be modified to install the FPSP.
//
//	Each entry point for exception 'xxxx' begins with a 'jmp fpsp_xxxx'.
//	Put any target system specific handling that must be done immediately
//	before the jump instruction.  If there no handling necessary, then
//	the 'fpsp_xxxx' handler entry point should be placed in the exception
//	table so that the 'jmp' can be eliminated. If the FPSP determines that the
//	exception is one that must be reported then there will be a
//	return from the package by a 'jmp real_xxxx'.  At that point
//	the machine state will be identical to the state before
//	the FPSP was entered.  In particular, whatever condition
//	that caused the exception will still be pending when the FPSP
//	package returns.  Thus, there will be system specific code
//	to handle the exception.
//
//	If the exception was completely handled by the package, then
//	the return will be via a 'jmp fpsp_done'.  Unless there is
//	OS specific work to be done (such as handling a context switch or
//	interrupt) the user program can be resumed via 'rte'.
//
//	In the following skeleton code, some typical 'real_xxxx' handling
//	code is shown.  This code may need to be moved to an appropriate
//	place in the target system, or rewritten.
//

//		Copyright (C) Motorola, Inc. 1990
//			All Rights Reserved
//
//	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MOTOROLA
//	The copyright notice above does not evidence any
//	actual or intended publication of such source code.


//
//	Modified for Linux-1.3.x by Jes Sorensen (jds@kom.auc.dk)
//	Modified for RTEMS 4.0.0 by Eric Norum (eric@skatter.usask.ca)
//

#include <rtems/asm.h>

//SKELETON	idnt    2,1 | Motorola 040 Floating Point Software Package

#include "fpsp.defs"

//
//	Divide by Zero exception
//
//	All dz exceptions are 'real', hence no fpsp_dz entry point.
//
	.global	SYM(_fpspEntry_dz)
SYM(_fpspEntry_dz):
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E1,E_BYTE(a6)
	frestore	(sp)+
	unlk		a6
	jmp		([SYM(M68040FPSPUserExceptionHandlers)+3*4],%za0)

//
//	Inexact exception
//
//	All inexact exceptions are real, but the 'real' handler
//	will probably want to clear the pending exception.
//	The provided code will clear the E3 exception (if pending),
//	otherwise clear the E1 exception.  The frestore is not really
//	necessary for E1 exceptions.
//
// Code following the 'inex' label is to handle bug #1232.  In this
// bug, if an E1 snan, ovfl, or unfl occurred, and the process was
// swapped out before taking the exception, the exception taken on
// return was inex, rather than the correct exception.  The snan, ovfl,
// and unfl exception to be taken must not have been enabled.  The
// fix is to check for E1, and the existence of one of snan, ovfl,
// or unfl bits set in the fpsr.  If any of these are set, branch
// to the appropriate  handler for the exception in the fpsr.  Note
// that this fix is only for d43b parts, and is skipped if the
// version number is not $40.
//
//
	.global	SYM(_fpspEntry_inex)
	.global	real_inex
SYM(_fpspEntry_inex):
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	cmpib		#VER_40,(sp)		//test version number
	bnes		not_fmt40
	fmovel		fpsr,-(sp)
	btstb		#E1,E_BYTE(a6)		//test for E1 set
	beqs		not_b1232
	btstb		#snan_bit,2(sp) //test for snan
	beq		inex_ckofl
	addl		#4,sp
	frestore	(sp)+
	unlk		a6
	bra		snan
inex_ckofl:
	btstb		#ovfl_bit,2(sp) //test for ovfl
	beq		inex_ckufl
	addl		#4,sp
	frestore	(sp)+
	unlk		a6
	bra		SYM(_fpspEntry_ovfl)
inex_ckufl:
	btstb		#unfl_bit,2(sp) //test for unfl
	beq		not_b1232
	addl		#4,sp
	frestore	(sp)+
	unlk		a6
	bra		SYM(_fpspEntry_unfl)

//
// We do not have the bug 1232 case.  Clean up the stack and call
// real_inex.
//
not_b1232:
	addl		#4,sp
	frestore	(sp)+
	unlk		a6

real_inex:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
not_fmt40:
	bclrb		#E3,E_BYTE(a6)		//clear and test E3 flag
	beqs		inex_cke1
//
// Clear dirty bit on dest resister in the frame before branching
// to b1238_fix.
//
	moveml		d0/d1,USER_DA(a6)
	bfextu		CMDREG1B(a6){#6:#3},d0		//get dest reg no
	bclrb		d0,FPR_DIRTY_BITS(a6)	//clr dest dirty bit
	bsrl		b1238_fix		//test for bug1238 case
	moveml		USER_DA(a6),d0/d1
	bras		inex_done
inex_cke1:
	bclrb		#E1,E_BYTE(a6)
inex_done:
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+2*4],%za0)

//
//	Overflow exception
//
	.global	SYM(_fpspEntry_ovfl)
	.global	real_ovfl
SYM(_fpspEntry_ovfl):
	jmp	fpsp_ovfl
real_ovfl:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E3,E_BYTE(a6)		//clear and test E3 flag
	bnes		ovfl_done
	bclrb		#E1,E_BYTE(a6)
ovfl_done:
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+6*4],%za0)

//
//	Underflow exception
//
	.global	SYM(_fpspEntry_unfl)
	.global	real_unfl
SYM(_fpspEntry_unfl):
	jmp	fpsp_unfl
real_unfl:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E3,E_BYTE(a6)		//clear and test E3 flag
	bnes		unfl_done
	bclrb		#E1,E_BYTE(a6)
unfl_done:
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+4*4],%za0)

//
//	Signalling NAN exception
//
	.global	SYM(_fpspEntry_snan)
	.global	real_snan
SYM(_fpspEntry_snan):
snan:
	jmp	fpsp_snan
real_snan:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E1,E_BYTE(a6)	//snan is always an E1 exception
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+7*4],%za0)

//
//	Operand Error exception
//
	.global	SYM(_fpspEntry_operr)
	.global	real_operr
SYM(_fpspEntry_operr):
	jmp	fpsp_operr
real_operr:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E1,E_BYTE(a6)	//operr is always an E1 exception
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+5*4],%za0)

//
//	BSUN exception
//
//	This sample handler simply clears the nan bit in the FPSR.
//
	.global	SYM(_fpspEntry_bsun)
	.global	real_bsun
SYM(_fpspEntry_bsun):
	jmp	fpsp_bsun
real_bsun:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E1,E_BYTE(a6)	//bsun is always an E1 exception
	fmovel		fpsr,-(sp)
	bclrb		#nan_bit,(sp)
	fmovel		(sp)+,fpsr
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+1*4],%za0)

//
//	F-line exception
//
//	A 'real' F-line exception is one that the FPSP is not supposed to
//	handle. E.g. an instruction with a co-processor ID that is not 1.
//
	.global	SYM(_fpspEntry_fline)
	.global	real_fline
SYM(_fpspEntry_fline):
	jmp	fpsp_fline
real_fline:
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+0*4],%za0)

//
//	Unsupported data type exception
//
	.global	SYM(_fpspEntry_unsupp)
	.global	real_unsupp
SYM(_fpspEntry_unsupp):
	jmp	fpsp_unsupp
real_unsupp:
	link		a6,#-LOCAL_SIZE
	fsave		-(sp)
	bclrb		#E1,E_BYTE(a6)	//unsupp is always an E1 exception
	frestore	(sp)+
	unlk		a6
	jmp	([SYM(M68040FPSPUserExceptionHandlers)+8*4],%za0)

//
//	Trace exception
//
	.global	real_trace
real_trace:
	trap	#10

//
//	fpsp_fmt_error --- exit point for frame format error
//
//	The fpu stack frame does not match the frames existing
//	or planned at the time of this writing.  The fpsp is
//	unable to handle frame sizes not in the following
//	version:size pairs:
//
//	{4060, 4160} - busy frame
//	{4028, 4130} - unimp frame
//	{4000, 4100} - idle frame
//
	.global	fpsp_fmt_error
fpsp_fmt_error:
	trap	#11

//
//	fpsp_done --- FPSP exit point
//
//	The exception has been handled by the package and we are ready
//	to return to user mode, but there may be OS specific code
//	to execute before we do.  If there is, do it now.
//
// For now, the RTEMS does not bother looking at the
// possibility that it is time to reschedule....
//

	.global	fpsp_done
fpsp_done:
	rte

//
//	mem_write --- write to user or supervisor address space
//
// Writes to memory while in supervisor mode.
//
//	a0 - supervisor source address
//	a1 - user/supervisor destination address
//	d0 - number of bytes to write (maximum count is 12)
//
	.global	mem_write
mem_write:
	btstb	#5,EXC_SR(a6)	//check for supervisor state
	beqs	user_write
super_write:
	moveb	(a0)+,(a1)+
	subql	#1,d0
	bnes	super_write
	rts
user_write:
	movel	d1,-(sp)	//preserve d1 just in case
	movel	d0,-(sp)
	movel	a1,-(sp)
	movel	a0,-(sp)
	jsr		copyout
	addw	#12,sp
	movel	(sp)+,d1
	rts
//
//	mem_read --- read from user or supervisor address space
//
// Reads from memory while in supervisor mode.
//
// The FPSP calls mem_read to read the original F-line instruction in order
// to extract the data register number when the 'Dn' addressing mode is
// used.
//
//Input:
//	a0 - user/supervisor source address
//	a1 - supervisor destination address
//	d0 - number of bytes to read (maximum count is 12)
//
// Like mem_write, mem_read always reads with a supervisor
// destination address on the supervisor stack.  Also like mem_write,
// the EXC_SR is checked and a simple memory copy is done if reading
// from supervisor space is indicated.
//
	.global	mem_read
mem_read:
	btstb	#5,EXC_SR(a6)	//check for supervisor state
	beqs	user_read
super_read:
	moveb	(a0)+,(a1)+
	subql	#1,d0
	bnes	super_read
	rts
user_read:
	movel	d1,-(sp)	//preserve d1 just in case
	movel	d0,-(sp)
	movel	a1,-(sp)
	movel	a0,-(sp)
	jsr		copyin
	addw	#12,sp
	movel	(sp)+,d1
	rts

//
// Use these routines if your kernel does not have copyout/copyin equivalents.
// Assumes that D0/D1/A0/A1 are scratch registers. copyout overwrites DFC,
// and copyin overwrites SFC.
//
copyout:
	movel	4(sp),a0	// source
	movel	8(sp),a1	// destination
	movel	12(sp),d0	// count
	subl	#1,d0		// dec count by 1 for dbra
	movel	#1,d1
	movec	d1,dfc		// set dfc for user data space
moreout:
	moveb	(a0)+,d1	// fetch supervisor byte
	movesb	d1,(a1)+	// write user byte
	dbf	d0,moreout
	rts

copyin:
	movel	4(sp),a0	// source
	movel	8(sp),a1	// destination
	movel	12(sp),d0	// count
	subl	#1,d0		// dec count by 1 for dbra
	movel	#1,d1
	movec	d1,sfc		// set sfc for user space
morein:
	movesb	(a0)+,d1	// fetch user byte
	moveb	d1,(a1)+	// write supervisor byte
	dbf	d0,morein
	rts

	|end
