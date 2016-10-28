/*
 *  systrap.S
 *
 *  This file contains emulated system calls using software trap 0.
 *  The following calls are supported:
 *
 *    + SYS_exit	(halt)
 *    + SYS_irqdis	(disable interrupts)
 *    + SYS_irqset	(set interrupt level)
 *
 *  COPYRIGHT:
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  This terms of the RTEMS license apply to this file.
 *
 */

#include <rtems/asm.h>
#include "syscall.h"

	.section    ".text"
	/*
	 *  system call - halt
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    g1 = system call id (1)
	 *
	 *  System Call 1 (exit):
	 *    g2 = additional exit code 1
	 *    g3 = additional exit code 2
	 */

	PUBLIC(syscall)

SYM(syscall):
	ta	0			! syscall 1, halt with %g1,%g2,%g3 info

	PUBLIC(sparc_syscall_exit)

SYM(sparc_syscall_exit):

	mov	SYS_exit, %g1
	mov	%o0, %g2	! Additional exit code 1
	mov	%o1, %g3	! Additional exit code 2
	ta	SPARC_SWTRAP_SYSCALL

	/*
	 *  system call - Interrupt Disable
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr | SPARC_PSR_PIL_MASK
	 *
	 *  On exit:
	 *    g1 = old psr (to user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqdis)

SYM(syscall_irqdis):
	mov	%l3, %psr			! Set PSR. Write delay 3 instr
	or	%l0, SPARC_PSR_ET_MASK, %g1	! return old PSR with ET=1
	nop					! PSR write delay
	jmp	%l2				! Return to after TA 9.
	 rett	%l2 + 4

	/*
	 *  system call - Interrupt Enable
	 *
	 *  On entry:
	 *
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr & ~0x0f00
	 *    g1 = new PIL to write (from user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqen)

SYM(syscall_irqen):
	and	%g1, SPARC_PSR_PIL_MASK, %l4	! %l4 = (%g1 & 0xf00)
	wr	%l3, %l4, %psr			! PSR = (PSR & ~0xf00) ^ %l4
	nop; nop				! PSR write delay;
	jmp	%l2				! Return to after TA 10.
	 rett	%l2 + 4

#if SPARC_HAS_FPU == 1
	/*
	 *  system call - Interrupt disable and set PSR[EF] according to caller
	 *                specified %g1
	 *
	 *  On entry:
	 *
	 *    g1 = the desired PSR[EF] value (from caller)
	 *    l0 = psr (from trap table)
	 *    l1 = pc
	 *    l2 = npc
	 *    l3 = psr | SPARC_PSR_PIL_MASK
	 *
	 *  On exit:
	 *    g1 = old psr (to user)
	 */

.align 32				! Align to 32-byte cache-line
	PUBLIC(syscall_irqdis_fp)

SYM(syscall_irqdis_fp):
	/*
	 * We cannot use an intermediate value for operations with the PSR[EF]
	 * bit since they use a 13-bit sign extension and PSR[EF] is bit 12.
	 */
	sethi	%hi(SPARC_PSR_EF_MASK), %l4

	andn	%l3, %l4, %l3			! Clear PSR[EF]
	and	%g1, %l4, %g1			! Select PSR[EF] only from %g1
	or	%l3, %g1, %l3			! Set PSR[EF] according to %g1
	mov	%l3, %psr			! Set PSR. Write delay 3 instr
	or	%l0, SPARC_PSR_ET_MASK, %g1	! return old PSR with ET=1
	nop					! PSR write delay
	jmp	%l2				! Return to after TA 9.
	 rett	%l2 + 4
#endif

#if defined(RTEMS_PARAVIRT)

        PUBLIC(_SPARC_Get_PSR)

SYM(_SPARC_Get_PSR):

	retl
	 rd     %psr, %o0

        PUBLIC(_SPARC_Set_PSR)

SYM(_SPARC_Set_PSR):

	mov     %o0, %psr
	nop
	nop
	nop
	retl
	 nop

        PUBLIC(_SPARC_Get_TBR)

SYM(_SPARC_Get_TBR):

	retl
	 rd    %tbr, %o0

        PUBLIC(_SPARC_Set_TBR)

SYM(_SPARC_Set_TBR):

	retl
	 wr    %o0, 0, %tbr

#endif /* defined(RTEMS_PARAVIRT) */

/* end of file */
