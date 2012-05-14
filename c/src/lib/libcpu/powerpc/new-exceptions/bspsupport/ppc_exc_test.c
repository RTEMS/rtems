/*
 * Test low-level exception handling code:
 *
 *  - hook an exception handler
 *  - clobber (almost) all registers with a known value
 *  - raise exception
 *  - from exception handler, increment all saved register
 *    contents by one (to ensure registers are not only
 *    saved properly but also restored properly).
 *  - resume execution
 *  - verify registers are now 'clobber_value + 1'
 *
 * NOTE: cannot be used on PSIM because SYS exception is used
 *       internally by simulator (but we could use a trap or
 *       something else).
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 2007
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vectors.h"


typedef struct regs_ {
	uint32_t	cr, xer, lr, ctr;
	uint32_t	gpr0;
	uint32_t	gpr1;
	uint32_t	gpr2;
	uint32_t	gpr3;
	uint32_t	gpr4;
	uint32_t	gpr5;
	uint32_t	gpr6;
	uint32_t	gpr7;
	uint32_t	gpr8;
	uint32_t	gpr9;
	uint32_t	gpr10;
	uint32_t	gpr11;
	uint32_t	gpr12;
	uint32_t	gpr13;
	uint32_t	gpr14;
	uint32_t	gpr15;
	uint32_t	gpr16;
	uint32_t	gpr17;
	uint32_t	gpr18;
	uint32_t	gpr19;
	uint32_t	gpr20;
	uint32_t	gpr21;
	uint32_t	gpr22;
	uint32_t	gpr23;
	uint32_t	gpr24;
	uint32_t	gpr25;
	uint32_t	gpr26;
	uint32_t	gpr27;
	uint32_t	gpr28;
	uint32_t	gpr29;
	uint32_t	gpr30;
	uint32_t	gpr31;
} ppc_exc_int_regs;

#define OFF(x)	(uintptr_t)(&((ppc_exc_int_regs*)0)->x)

void
storegs(ppc_exc_int_regs *p0, ppc_exc_int_regs *p1)
{
	asm volatile(
		"	stmw 0, %6(%0)   ;"
		"	mfcr	0		 ;"
		"	stw	 0, %2(%0)   ;"
		"	mflr	0		 ;"
		"	stw	 0, %3(%0)   ;"
		"	mfxer	0		 ;"
		"	stw	 0, %4(%0)   ;"
		"	mfctr	0		 ;"
		"	stw	 0, %5(%0)   ;"
		"	lwz  0, %6(%0)   ;"
		"	trap             ;"
		"	stmw 0, %6(%1)   ;"
		"	mfcr	0		 ;"
		"	stw	 0, %2(%1)   ;"
		"	mflr	0		 ;"
		"	stw	 0, %3(%1)   ;"
		"	mfxer	0		 ;"
		"	stw	 0, %4(%1)   ;"
		"	mfctr	0		 ;"
		"	stw	 0, %5(%1)   ;"
		:
		:"b"(p0),"b"(p1),
		"i"(OFF(cr)), "i"(OFF(lr)), "i"(OFF(xer)), "i"(OFF(ctr)),
		"i"(OFF(gpr0))
		:"r0");
}

/* Load up all registers from 'pre' issue system call and store
 * registers in 'post'
 */

ppc_exc_int_regs pre;
ppc_exc_int_regs pst;

void
clobber()
{
	asm volatile(
		"	lis 2,  pre@h  ;"
		"   ori 2, 2, pre@l  ;"
		"	lwz	3,	%0(2) ;"
		"	mtcr	3      ;"
		"	lwz	3,	%1(2) ;"
		"	mtlr	3      ;"
		"	lwz	3,	%2(2) ;"
		"	mtxer   3      ;"
		/* don't know which ones stick */
		"	mfxer   3      ;"
		"   stw 3,  %2(2)  ;"
		"	lwz	3,	%3(2) ;"
		"	mtctr	3      ;"
		"	lwz	0,	%4(2) ;"
		/* must not clobber R13, R1, R2 */
		"	stw 13, %6(2) ;"
		"   lmw 3,  %5(2)  ;"
		"   trap           ;"
		"   stmw 0, %4(2) ;"
		"	mfcr	0	   ;"
		"	stw	 0, %0(2) ;"
		"	mflr	0	   ;"
		"	stw	 0, %1(2) ;"
		"	mfxer	0	   ;"
		"	stw	 0, %2(2) ;"
		"	mfctr	0	   ;"
		"	stw	 0, %3(2) ;"
		:
		:"i"(OFF(cr)), "i"(OFF(lr)), "i"(OFF(xer)), "i"(OFF(ctr)),
		 "i"(OFF(gpr0)), "i"(OFF(gpr3)), "i"(OFF(gpr13))
		:"r0", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9",
		 "r10", "r11", "r12", "r14", "r15", "r16",
		 "r17", "r18", "r19", "r20", "r21", "r22", "r23",
		 "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
		 "xer","lr","ctr",
		 "cr0","cr1","cr2","cr3","cr4","cr5","cr6","cr7",
		 "memory");
}

typedef union { uint32_t u; uint8_t c[4]; } u32_a_t;

/* exception handler; adds 1 to all register contents (except r1,r2,r13) */
int
handle_clobber_exc(BSP_Exception_frame *f, unsigned vector)
{
int i;
u32_a_t *p = (u32_a_t*)&f->GPR0;
	for ( i=0; i<32; i++ ) {
		switch (i) {
			case 1: case 2: case 13: break;
			default:
				p[i].u++;
			break;
		}
	}
  f->GPR2 = (uint32_t)&pst;
  f->EXC_CR++;
  f->EXC_CTR++;
  f->EXC_XER++;
  f->EXC_LR++;
  f->EXC_SRR0 += 4;
  return 0;
}


/* This routine tests the raw exception code;
 *  - hook 'handle_clobber_exc' to SYS exception handler
 *  - clobber all registers with 0xaffe0000 + <index>
 *    (except: r1, r2, r13, non-sticky bits in xer)
 *    R2 is clobbered with the address of the pre area.
 *  - issue 'trap' -> PROG exception
 *  - exception handler increments all reg. contents by 1,
 *    stores address of 'pst' area in R2 and returns control
 *    to ppc_exc_clobber().
 *  - save all register contents to *R2 (should be &pst).
 *  - test for mismatches (except R1, R2, R13 and parts of xer)
 */
void
ppc_exc_clobber()
{
u32_a_t *a, *b;
	int i;
	a = (u32_a_t*)&pre;
	b = (u32_a_t*)&pst;
	for ( i=0; i< sizeof(pre)/sizeof(uint32_t); i++ ) {
		a[i].u = 0xaffe0000 + i;
	}
	ppc_exc_set_handler(ASM_PROG_VECTOR, handle_clobber_exc);
	clobber();
	ppc_exc_set_handler(ASM_PROG_VECTOR, 0);
	for ( i=0; i< sizeof(pre)/sizeof(uint32_t); i++ ) {
		switch (i) {
			case OFF(gpr1)/sizeof(uint32_t):
			case OFF(gpr2)/sizeof(uint32_t):
			case OFF(gpr13)/sizeof(uint32_t):
			break;

			default:
				if ( a[i].u != b[i].u - 1  ) {
					 printf("MISMATCH at %i: 0x%08"PRIx32" -- 0x%08"PRIx32"\n",
					 	i, a[i].u, b[i].u);
				}
		}
	}
}

#if 0
void
ppc_exc_test()
{
ppc_exc_int_regs a, b;
int              i;
	memset(&a, 0xaa, sizeof(a));
	memset(&b, 0x55, sizeof(b));
	storegs(&a, &b);
	if ( memcmp(&a, &b, sizeof(a)) ) {
		printf("FAILURE: context prior and after exception don't match!\n");
	}
	for ( i=0; i< sizeof(a)/sizeof(uint32_t); i++ ) {
			printf("0x%08"PRIx32" -- 0x%08"PRIx32"\n",
				((uint32_t __attribute__((may_alias)) *)&a)[i],
				((uint32_t __attribute__((may_alias)) *)&b)[i]);
	}
}
#endif
