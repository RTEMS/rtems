/*  rdbg_cpu_asm.s
 *
 *  This file contains all assembly code for the Motorola m68k implementation
 *  of RDBG.
 *
 * $Id$
 *
 */

#include <asm.h>
#include <rtems/score/cpu.h>    

        BEGIN_CODE

/*
 *  void copyback_data_cache_and_invalidate_instr_cache(addr, size)
 *
 *  This routine performs a copy of the data cache 
 *  and invalidate the instruction cache
 */

        .align  2
        PUBLIC (copyback_data_cache_and_invalidate_instr_cache)

SYM (copyback_data_cache_and_invalidate_instr_cache):
	nop	| how?
        rts


	
/*
 * void enterRdbg(void)
 *	
 * This function performs trap #4
 * It is used :
 *   1 - in the user code, to simulate a Breakpoint.
 *       (with justSaveContext = 0)
 *   2 - in the RDBG code, to push a ctx in the list.
 *       (with justSaveContext = 1)
 *
 * In most of case, it will be use as described in 1.
 * The 2nd possibility will be used by RDBG to obtain
 * its own ctx
 */

        PUBLIC (enterRdbg)

SYM (enterRdbg):
        trap	#4
        rts


/*
 * void excHandler(void)
 *	
 * lotsa copypaste from cpu_asm.S
 *
 */

/* 
 * The RTEMS jump table pushes vector
 *
 * The two types of exception frames on m68000 are
 *
 * unsigned16 sr <- sp
 * unsigned32 pc
 *
 * unsigned16 fc <- sp
 * unsigned32 addr
 * unsigned16 instr
 * unsigned16 sr
 * unsigned32 pc
 *
 * after real frame we push d0-d1/a0-a1
 *
 * after that we push CPU_Exception_frame
 *
 * exframe      <- sp
 * d0-d1/a0-a1
 * frame
 */
#if ( M68K_COLDFIRE_ARCH == 1 )
.set SR_OFFSET,    2                     | Status register offset
.set PC_OFFSET,    4                     | Program Counter offset
.set FVO_OFFSET,   0                     | Format/vector offset
#elif ( M68K_HAS_VBR == 1 )
.set SR_OFFSET,    0                     | Status register offset
.set PC_OFFSET,    2                     | Program Counter offset
.set FVO_OFFSET,   6                     | Format/vector offset
#else
.set SR_OFFSET,    2                     | Status register offset
.set PC_OFFSET,    4                     | Program Counter offset
.set FVO_OFFSET,   0                     | Format/vector offset placed in the stack
#endif /* M68K_HAS_VBR */
 
	.align 4
	
        PUBLIC (excHandler)
        PUBLIC (BreakPointExcHdl)

SYM (excHandler):	
#if ( M68K_COLDFIRE_ARCH == 0 )
        moveml  d0-d1/a0-a1,a7@-         | save d0-d1,a0-a1
        movew   a7@(16+FVO_OFFSET),d0	 | d0 = F/VO
        andl    #0x0fff,d0               | d0 = vector offset in vbr
#else
	lea	a7@(-16),a7
	movm.l  d0-d1/a0-a1,a7@		 | save d0-d1,a0-a1
	movew   a7@(16+FVO_OFFSET),d0	 | d0 = F/VO
	andl    #0x0ffc,d0               | d0 = vector offset in vbr
#endif
        lsrl    #2,d0                    | d0 = vector number
	lea	a7@(16),a1		 | address of orig frame
	lea	a7@(16),a0		 | address of orig frame
#if ( M68K_HAS_VBR == 0 )
	lea	a0@(2),a0		 | skip vector
#endif
	lea	a0@(6),a0		 | a0 = orig sp
#if ( M68K_COLDFIRE_ARCH == 0 )
        cmpiw   #3,d0			 | bus error or address error ?
#else
        cmpi.l  #3,d0
#endif
        bgt     1f			 | if >3 then normal exc
#if ( M68K_HAS_VBR == 1 )
	lea	a1@(2*29),a0
#else
	lea	a1@(8),a0		 | skip extra stuff
	lea	a0@(6),a0		 | a0 = orig sp
#endif
1:
	movew	a1@(SR_OFFSET),d1	 | d1 = sr
	andl	#0xffff,d1
	lea	a7@(-76),a7		 | reserve room for exception frame
| build CPU_Exception_frame
	movel	d0,a7@			 | vecnum
	movel	d1,a7@(4)		 | sr
	movel	a1@(PC_OFFSET),d1	 | d1 = pc
#if ( M68K_COLDFIRE_ARCH == 0 )
	cmpiw	#47,d0			 | trap #15, breakpoint?
#else
	cmpil	#47,d0			 | trap #15, breakpoint?
#endif
	bne	2f
	subql	#2,d1			 | real PC is at trap insn
2:	movel	d1,a7@(8)		 | store pc to exframe struct
	movel	a7@(76),a7@(12)		 | the orig d0 pushed at beginning
	movel	a7@(76+4),a7@(16)	 | the orig d1 pushed at beginning
	movel	d2,a7@(20)
	movel	d3,a7@(24)
	movel	d4,a7@(28)
	movel	d5,a7@(32)
	movel	d6,a7@(36)
	movel	d7,a7@(40)
	movel	a7@(76+8),a7@(44)	 | the orig a0 pushed at beginning
	movel	a7@(76+12),a7@(48)	 | the orig a1 pushed at beginning
	movel	a2,a7@(52)
	movel	a3,a7@(56)
	movel	a4,a7@(60)
	movel	a5,a7@(64)
	movel	a6,a7@(68)
	movel	a0,a7@(72)		 | stack pointer before exception
	lea	a7@,a0			 | exframe address
	movel	a1,a7@-			 | save top of orig frame
	movel	a0,a7@-			 | push exframe address
        jsr	SYM(BreakPointExcHdl)
	addql	#4,a7			 | pop exframe address
	movel	a7@+,a1			 | restore orig frame address
| XXX what should be restored from exframe??
	movel	a7@(4),d1		 | sr
	movew	d1,a1@(SR_OFFSET)	 | store sr to frame XXX ???
	movel	a7@(8),a1@(PC_OFFSET)	 | store pc to frame XXX ???
	movel	a7@(12),a7@(76)		 | d0 to be restored from stack
	movel	a7@(16),a7@(76+4)	 | d1 to be restored from stack
	movel	a7@(20),d2
	movel	a7@(24),d3
	movel	a7@(28),d4
	movel	a7@(32),d5
	movel	a7@(36),d6
	movel	a7@(40),d7
	movel	a7@(44),a7@(76+8)	 | a0 to be restored from stack
	movel	a7@(48),a7@(76+12)	 | a1 to be restored from stack
	movel	a7@(52),a2
	movel	a7@(56),a3
	movel	a7@(60),a4
	movel	a7@(64),a5
	movel	a7@(68),a6
	addl	#76,a7			 | pop exframe
	
#if ( M68K_COLDFIRE_ARCH == 0 )
	moveml  a7@+,d0-d1/a0-a1	 | restore d0-d1,a0-a1
#else
	moveml	a7@,d0-d1/a0-a1		 | restore d0-d1,a0-a1
	lea     a7@(16),a7
#endif

#if ( M68K_HAS_VBR == 0 )
        addql   #2,a7                    | pop format/id
#endif /* M68K_HAS_VBR */

| XXX bus err cleanup

	rte

END_CODE

END
