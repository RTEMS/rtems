/* r3000.h - mips R3k architecture header file */

/*
* Copyright 1985-1997 by MIPS Computer Systems, Inc.
*/

/*
modification history
--------------------
01k,17jan96,kkk  made MINCACHE 512 (spr# 7556)
01j,13jan96,kkk  undid 01i.
01i,18dec96,tam  enabled FP exceptions via FP_TASK_STATUS (spr #7665).
01h,13sep93,caf  fixed K2SIZE (SPR #1880).
01g,22sep92,rrr  added support for c++
01f,02jun92,ajm  the 5.0.5 merge
01e,26may92,rrr  the tree shuffle
01d,04oct91,rrr  passed through the ansification filter
		  -changed copyright notice
01c,23jul91,ajm  changed default fp status register to all exceptions
		  off forcing the user to enable exceptions for signals
		  Enable FPA interrupt to tasks
01b,08jul91,ajm  added SR_KUMSK for exception handling
01a,21feb91,ajm  written.
*/

#ifndef __INCr3000h
#define __INCr3000h

#ifdef __cplusplus
extern "C" {
#endif

/*
* Segment base addresses and sizes
*/

#define	K0BASE		0x80000000
#define	K0SIZE		0x20000000
#define	K1BASE		0xA0000000
#define	K1SIZE		0x20000000
#define	K2BASE		0xC0000000
#define	K2SIZE		0x40000000

/*
* Exception vectors
*/

#define	UT_VEC		K0BASE			/* utlbmiss vector */
#define	E_VEC		(K0BASE+0x80)		/* exception vector */
#define	R_VEC		(K1BASE+0x1fc00000)	/* reset vector */

/*
 * Address conversion macros
 */

#define	K0_TO_K1(x)	((unsigned)(x)|0xA0000000)	/* kseg0 to kseg1 */
#define	K1_TO_K0(x)	((unsigned)(x)&0x9FFFFFFF)	/* kseg1 to kseg0 */
#define	K0_TO_PHYS(x)	((unsigned)(x)&0x1FFFFFFF)	/* kseg0 to physical */
#define	K1_TO_PHYS(x)	((unsigned)(x)&0x1FFFFFFF)	/* kseg1 to physical */
#define	PHYS_TO_K0(x)	((unsigned)(x)|0x80000000)	/* physical to kseg0 */
#define	PHYS_TO_K1(x)	((unsigned)(x)|0xA0000000)	/* physical to kseg1 */

/*
* Address predicates
*/

#define	IS_KSEG0(x)	((unsigned)(x) >= K0BASE && (unsigned)(x) < K1BASE)
#define	IS_KSEG1(x)	((unsigned)(x) >= K1BASE && (unsigned)(x) < K2BASE)
#define	IS_KUSEG(x)	((unsigned)(x) < K0BASE)

/*
* Cache size constants
*/

#define	MINCACHE	+(1*512)	/* leading plus for mas's benefit */
#define	MAXCACHE	+(256*1024)	/* leading plus for mas's benefit */

/*
* Cause bit definitions
*/

#define	CAUSE_BD	0x80000000	/* Branch delay slot */
#define	CAUSE_CEMASK	0x30000000	/* coprocessor error */
#define	CAUSE_CESHIFT	28

#define	CAUSE_IP8	0x00008000	/* External level 8 pending */
#define	CAUSE_IP7	0x00004000	/* External level 7 pending */
#define	CAUSE_IP6	0x00002000	/* External level 6 pending */
#define	CAUSE_IP5	0x00001000	/* External level 5 pending */
#define	CAUSE_IP4	0x00000800	/* External level 4 pending */
#define	CAUSE_IP3	0x00000400	/* External level 3 pending */
#define	CAUSE_SW2	0x00000200	/* Software level 2 pending */
#define	CAUSE_SW1	0x00000100	/* Software level 1 pending */

#define	CAUSE_IPMASK	0x0000FF00	/* Pending interrupt mask */
#define	CAUSE_IPSHIFT	8

#define	CAUSE_EXCMASK	0x0000003C	/* Cause code bits */
#define	CAUSE_EXCSHIFT	2

/*
* Status definition bits
*/

#define	SR_CUMASK	0xf0000000	/* coproc usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */
#define	SR_BEV		0x00400000	/* use boot exception vectors */
#define	SR_TS		0x00200000	/* TLB shutdown */
#define	SR_PE		0x00100000	/* cache parity error */
#define	SR_CM		0x00080000	/* cache miss */
#define	SR_PZ		0x00040000	/* cache parity zero */
#define	SR_SWC		0x00020000	/* swap cache */
#define	SR_ISC		0x00010000	/* Isolate data cache */
#define	SR_IMASK	0x0000ff00	/* Interrupt mask */
#define	SR_IMASK8	0x00000000	/* mask level 8 */
#define	SR_IMASK7	0x00008000	/* mask level 7 */
#define	SR_IMASK6	0x0000c000	/* mask level 6 */
#define	SR_IMASK5	0x0000e000	/* mask level 5 */
#define	SR_IMASK4	0x0000f000	/* mask level 4 */
#define	SR_IMASK3	0x0000f800	/* mask level 3 */
#define	SR_IMASK2	0x0000fc00	/* mask level 2 */
#define	SR_IMASK1	0x0000fe00	/* mask level 1 */
#define	SR_IMASK0	0x0000ff00	/* mask level 0 */

#define	SR_IBIT8	0x00008000	/* bit level 8 */
#define	SR_IBIT7	0x00004000	/* bit level 7 */
#define	SR_IBIT6	0x00002000	/* bit level 6 */
#define	SR_IBIT5	0x00001000	/* bit level 5 */
#define	SR_IBIT4	0x00000800	/* bit level 4 */
#define	SR_IBIT3	0x00000400	/* bit level 3 */
#define	SR_IBIT2	0x00000200	/* bit level 2 */
#define	SR_IBIT1	0x00000100	/* bit level 1 */

#define	SR_KUO		0x00000020	/* old kernel/user, 0 => k, 1 => u */
#define	SR_IEO		0x00000010	/* old interrupt enable, 1 => enable */
#define	SR_KUP		0x00000008	/* prev kernel/user, 0 => k, 1 => u */
#define	SR_IEP		0x00000004	/* prev interrupt enable, 1 => enable */
#define	SR_KUC		0x00000002	/* cur kernel/user, 0 => k, 1 => u */
#define	SR_IEC		0x00000001	/* cur interrupt enable, 1 => enable */
#define SR_KUMSK	(SR_KUO|SR_IEO|SR_KUP|SR_IEP|SR_KUC|SR_IEC)

#define	SR_IMASKSHIFT	8

/*
* fpa definitions
*/

#define FP_ROUND	0x3		/* r3010 round mode mask */
#define FP_STICKY 	0x7c		/* r3010 sticky bits mask */
#define FP_ENABLE 	0xf80		/* r3010 enable mode mask */
#define FP_EXC		0x3f000		/* r3010 exception mask */

#define FP_ROUND_N	0x0		/* round to nearest */
#define FP_ROUND_Z 	0x1		/* round to zero */
#define FP_ROUND_P 	0x2		/* round to + infinity */
#define FP_ROUND_M 	0x3		/* round to - infinity */

#define FP_STICKY_I	0x4		/* sticky inexact operation */
#define FP_STICKY_U	0x8		/* sticky underflow */
#define FP_STICKY_O	0x10		/* sticky overflow */
#define FP_STICKY_Z	0x20		/* sticky divide by zero */
#define FP_STICKY_V	0x40		/* sticky invalid operation */

#define FP_ENABLE_I	0x80		/* enable inexact operation */
#define FP_ENABLE_U	0x100		/* enable underflow exc  */
#define FP_ENABLE_O	0x200		/* enable overflow exc  */
#define FP_ENABLE_Z	0x400		/* enable divide by zero exc  */
#define FP_ENABLE_V	0x800		/* enable invalid operation exc  */

#define FP_EXC_I	0x1000		/* inexact operation */
#define FP_EXC_U	0x2000		/* underflow */
#define FP_EXC_O	0x4000		/* overflow */
#define FP_EXC_Z	0x8000		/* divide by zero */
#define FP_EXC_V	0x10000		/* invalid operation */
#define FP_EXC_E	0x20000		/* unimplemented operation */

#define FP_COND		0x800000	/* condition bit */

#define FP_EXC_SHIFT	12
#define FP_ENABLE_SHIFT	7
#define FP_EXC_MASK	(FP_EXC_I|FP_EXC_U|FP_EXC_O|FP_EXC_Z|FP_EXC_V|FP_EXC_E)
#define FP_ENABLE_MASK	(FP_ENABLE_I|FP_ENABLE_U|FP_ENABLE_O|FP_ENABLE_Z| \
			 FP_ENABLE_V)
#define FP_TASK_STATUS	0x0 		/* all FP exceptions  are disabled
					   (see fppAlib.s and spr #7665) */

/*
* tlb definitions
*/

#define	TLB_ENTRIES		64
#define	TLBLO_PFNMASK		0xfffff000
#define	TLBLO_PFNSHIFT		12
#define	TLBLO_N			0x800		/* non-cacheable */
#define	TLBLO_D			0x400		/* writeable */
#define	TLBLO_V			0x200		/* valid bit */
#define	TLBHI_VPNMASK		0xfffff000
#define	TLBHI_VPNSHIFT		12
#define	TLBHI_PIDMASK		0xfc0
#define	TLBHI_PIDSHIFT		6
#define	TLBHI_NPID		64
#define	TLBINX_PROBE		0x80000000
#define	TLBINX_INXMASK		0x00003f00
#define	TLBINX_INXSHIFT		8
#define	TLBRAND_RANDMASK	0x00003f00
#define	TLBRAND_RANDSHIFT	8
#define	TLBCTXT_BASEMASK	0xffe00000
#define	TLBCTXT_BASESHIFT	21
#define	TLBCTXT_VPNMASK		0x001ffffc
#define	TLBCTXT_VPNSHIFT	2

/*
* Coprocessor 0 operations
*/

#define	C0_READI  0x1		/* read ITLB entry addressed by C0_INDEX */
#define	C0_WRITEI 0x2		/* write ITLB entry addressed by C0_INDEX */
#define	C0_WRITER 0x6		/* write ITLB entry addressed by C0_RAND */
#define	C0_PROBE  0x8		/* probe for ITLB entry addressed by TLBHI */
#define	C0_RFE	  0x10		/* restore for exception */

#ifdef __cplusplus
}
#endif

#endif /* __INCr3000h */
