/*

Based upon IDT provided code with the following release:

This source code has been made available to you by IDT on an AS-IS
basis. Anyone receiving this source is licensed under IDT copyrights
to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or
without modifications.  No license under IDT patents or patent
applications is to be implied by the copyright license.

Any user of this software should understand that IDT cannot provide
technical support for this software and will not be responsible for
any consequences resulting from the use of this software.

Any person who transfers this source code or any derivative work must
include the IDT copyright notice, this paragraph, and the preceeding
two paragraphs in the transferred software.

COPYRIGHT IDT CORPORATION 1996
LICENSED MATERIAL - PROGRAM PROPERTY OF IDT


/*
** idtcpu.h -- cpu related defines
*/

#ifndef _RTEMS_MIPS_IDTCPU_H
#define _RTEMS_MIPS_IDTCPU_H

/*
 * 950313: Ketan added Register definition for XContext reg.
 *               added define for WAIT instruction.
 * 950421: Ketan added Register definition for Config reg (R3081)
 */

/*
** memory configuration and mapping
*/
#define K0BASE	0x80000000
#define K0SIZE	0x20000000
#define K1BASE	0xa0000000
#define K1SIZE	0x20000000
#define K2BASE	0xc0000000
#define K2SIZE	0x20000000
#if  __mips == 3
#define KSBASE	0xe0000000
#define KSSIZE	0x20000000
#endif

#define KUBASE	0
#define KUSIZE	0x80000000

/*
** Exception Vectors
*/
#if  __mips == 1
#define	UT_VEC	K0BASE			/* utlbmiss vector */
#define DB_VEC  (K0BASE+0x40)           /* debug vector */
#define E_VEC	(K0BASE+0x80)		/* exception vector */
#elif  __mips == 32
#define	T_VEC	(K0BASE+0x000)		/* tlbmiss vector */
#define X_VEC	(K0BASE+0x080)		/* xtlbmiss vector */
#define C_VEC	(K0BASE+0x100)		/* cache error vector */
#define E_VEC	(K0BASE+0x180)		/* exception vector */
#elif  __mips == 3
#define	T_VEC	(K0BASE+0x000)		/* tlbmiss vector */
#define X_VEC	(K0BASE+0x080)		/* xtlbmiss vector */
#define C_VEC	(K0BASE+0x100)		/* cache error vector */
#define E_VEC	(K0BASE+0x180)		/* exception vector */
#else
#error "EXCEPTION VECTORS: unknown ISA level"
#endif
#define	R_VEC	(K1BASE+0x1fc00000)	/* reset vector */

/*
** Address conversion macros
*/
#ifdef CLANGUAGE
#define CAST(as) (as)
#else
#define CAST(as)
#endif
#define	K0_TO_K1(x)	(CAST(unsigned)(x)|0xA0000000)	/* kseg0 to kseg1 */
#define	K1_TO_K0(x)	(CAST(unsigned)(x)&0x9FFFFFFF)	/* kseg1 to kseg0 */
#define	K0_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg0 to physical */
#define	K1_TO_PHYS(x)	(CAST(unsigned)(x)&0x1FFFFFFF)	/* kseg1 to physical */
#define	PHYS_TO_K0(x)	(CAST(unsigned)(x)|0x80000000)	/* physical to kseg0 */
#define	PHYS_TO_K1(x)	(CAST(unsigned)(x)|0xA0000000)	/* physical to kseg1 */

/*
**	Cache size constants
*/
#define	MINCACHE	0x200		/* 512       For 3041. */
#define	MAXCACHE	0x40000		/* 256*1024   256k */	

#if  __mips == 32
/* R4000 configuration register definitions */
#define CFG_CM		0x80000000	/* Master-Checker mode */
#define CFG_ECMASK	0x70000000	/* System Clock Ratio */
#define CFG_ECBY2	0x00000000 	/* divide by 2 */
#define CFG_ECBY3	0x10000000 	/* divide by 3 */
#define CFG_ECBY4	0x20000000 	/* divide by 4 */
#define CFG_EPMASK	0x0f000000	/* Transmit data pattern */
#define CFG_EPD		0x00000000	/* D */
#define CFG_EPDDX	0x01000000	/* DDX */
#define CFG_EPDDXX	0x02000000	/* DDXX */
#define CFG_EPDXDX	0x03000000	/* DXDX */
#define CFG_EPDDXXX	0x04000000	/* DDXXX */
#define CFG_EPDDXXXX	0x05000000	/* DDXXXX */
#define CFG_EPDXXDXX	0x06000000	/* DXXDXX */
#define CFG_EPDDXXXXX	0x07000000	/* DDXXXXX */
#define CFG_EPDXXXDXXX	0x08000000	/* DXXXDXXX */
#define CFG_SBMASK	0x00c00000	/* Secondary cache block size */
#define CFG_SBSHIFT	22
#define CFG_SB4		0x00000000	/* 4 words */
#define CFG_SB8		0x00400000	/* 8 words */
#define CFG_SB16	0x00800000	/* 16 words */
#define CFG_SB32	0x00c00000	/* 32 words */
#define CFG_SS		0x00200000	/* Split secondary cache */
#define CFG_SW		0x00100000	/* Secondary cache port width */
#define CFG_EWMASK	0x000c0000	/* System port width */
#define CFG_EWSHIFT	18
#define CFG_EW64	0x00000000	/* 64 bit */
#define CFG_EW32	0x00010000	/* 32 bit */
#define CFG_SC		0x00020000	/* Secondary cache absent */
#define CFG_SM		0x00010000	/* Dirty Shared mode disabled */
#define CFG_BE		0x00008000	/* Big Endian */
#define CFG_EM		0x00004000	/* ECC mode enable */
#define CFG_EB		0x00002000	/* Block ordering */
#define CFG_ICMASK	0x00000e00	/* Instruction cache size */
#define CFG_ICSHIFT	9
#define CFG_DCMASK	0x000001c0	/* Data cache size */
#define CFG_DCSHIFT	6
#define CFG_IB		0x00000020	/* Instruction cache block size */
#define CFG_DB		0x00000010	/* Data cache block size */
#define CFG_CU		0x00000008	/* Update on Store Conditional */
#define CFG_K0MASK	0x00000007	/* KSEG0 coherency algorithm */

/*
 * R4000 primary cache mode
 */
#define CFG_C_UNCACHED		2
#define CFG_C_NONCOHERENT	3
#define CFG_C_COHERENTXCL	4
#define CFG_C_COHERENTXCLW	5
#define CFG_C_COHERENTUPD	6

/*
 * R4000 cache operations (should be in assembler...?)
 */
#define Index_Invalidate_I               0x0         /* 0       0 */
#define Index_Writeback_Inv_D            0x1         /* 0       1 */
#define Index_Invalidate_SI              0x2         /* 0       2 */
#define Index_Writeback_Inv_SD           0x3         /* 0       3 */
#define Index_Load_Tag_I                 0x4         /* 1       0 */
#define Index_Load_Tag_D                 0x5         /* 1       1 */
#define Index_Load_Tag_SI                0x6         /* 1       2 */
#define Index_Load_Tag_SD                0x7         /* 1       3 */
#define Index_Store_Tag_I                0x8         /* 2       0 */
#define Index_Store_Tag_D                0x9         /* 2       1 */
#define Index_Store_Tag_SI               0xA         /* 2       2 */
#define Index_Store_Tag_SD               0xB         /* 2       3 */
#define Create_Dirty_Exc_D               0xD         /* 3       1 */
#define Create_Dirty_Exc_SD              0xF         /* 3       3 */
#define Hit_Invalidate_I                 0x10        /* 4       0 */
#define Hit_Invalidate_D                 0x11        /* 4       1 */
#define Hit_Invalidate_SI                0x12        /* 4       2 */
#define Hit_Invalidate_SD                0x13        /* 4       3 */
#define Hit_Writeback_Inv_D              0x15        /* 5       1 */
#define Hit_Writeback_Inv_SD             0x17        /* 5       3 */
#define Fill_I                           0x14        /* 5       0 */
#define Hit_Writeback_D                  0x19        /* 6       1 */
#define Hit_Writeback_SD                 0x1B        /* 6       3 */
#define Hit_Writeback_I                  0x18        /* 6       0 */
#define Hit_Set_Virtual_SI               0x1E        /* 7       2 */
#define Hit_Set_Virtual_SD               0x1F        /* 7       3 */

/* Disabled by chris -- horrible overload of common word.
#ifndef WAIT
#define WAIT .word 0x42000020
#endif
*/
/* Disabled by joel -- horrible overload of common word.
#ifndef wait
#define wait .word 0x42000020
#endif wait
*/

#endif

#if  __mips == 3
/* R4000 configuration register definitions */
#define CFG_CM		0x80000000	/* Master-Checker mode */
#define CFG_ECMASK	0x70000000	/* System Clock Ratio */
#define CFG_ECBY2	0x00000000 	/* divide by 2 */
#define CFG_ECBY3	0x10000000 	/* divide by 3 */
#define CFG_ECBY4	0x20000000 	/* divide by 4 */
#define CFG_EPMASK	0x0f000000	/* Transmit data pattern */
#define CFG_EPD		0x00000000	/* D */
#define CFG_EPDDX	0x01000000	/* DDX */
#define CFG_EPDDXX	0x02000000	/* DDXX */
#define CFG_EPDXDX	0x03000000	/* DXDX */
#define CFG_EPDDXXX	0x04000000	/* DDXXX */
#define CFG_EPDDXXXX	0x05000000	/* DDXXXX */
#define CFG_EPDXXDXX	0x06000000	/* DXXDXX */
#define CFG_EPDDXXXXX	0x07000000	/* DDXXXXX */
#define CFG_EPDXXXDXXX	0x08000000	/* DXXXDXXX */
#define CFG_SBMASK	0x00c00000	/* Secondary cache block size */
#define CFG_SBSHIFT	22
#define CFG_SB4		0x00000000	/* 4 words */
#define CFG_SB8		0x00400000	/* 8 words */
#define CFG_SB16	0x00800000	/* 16 words */
#define CFG_SB32	0x00c00000	/* 32 words */
#define CFG_SS		0x00200000	/* Split secondary cache */
#define CFG_SW		0x00100000	/* Secondary cache port width */
#define CFG_EWMASK	0x000c0000	/* System port width */
#define CFG_EWSHIFT	18
#define CFG_EW64	0x00000000	/* 64 bit */
#define CFG_EW32	0x00010000	/* 32 bit */
#define CFG_SC		0x00020000	/* Secondary cache absent */
#define CFG_SM		0x00010000	/* Dirty Shared mode disabled */
#define CFG_BE		0x00008000	/* Big Endian */
#define CFG_EM		0x00004000	/* ECC mode enable */
#define CFG_EB		0x00002000	/* Block ordering */
#define CFG_ICMASK	0x00000e00	/* Instruction cache size */
#define CFG_ICSHIFT	9
#define CFG_DCMASK	0x000001c0	/* Data cache size */
#define CFG_DCSHIFT	6
#define CFG_IB		0x00000020	/* Instruction cache block size */
#define CFG_DB		0x00000010	/* Data cache block size */
#define CFG_CU		0x00000008	/* Update on Store Conditional */
#define CFG_K0MASK	0x00000007	/* KSEG0 coherency algorithm */

/*
 * R4000 primary cache mode
 */
#define CFG_C_UNCACHED		2
#define CFG_C_NONCOHERENT	3
#define CFG_C_COHERENTXCL	4
#define CFG_C_COHERENTXCLW	5
#define CFG_C_COHERENTUPD	6

/*
 * R4000 cache operations (should be in assembler...?)
 */
#define Index_Invalidate_I               0x0         /* 0       0 */
#define Index_Writeback_Inv_D            0x1         /* 0       1 */
#define Index_Invalidate_SI              0x2         /* 0       2 */
#define Index_Writeback_Inv_SD           0x3         /* 0       3 */
#define Index_Load_Tag_I                 0x4         /* 1       0 */
#define Index_Load_Tag_D                 0x5         /* 1       1 */
#define Index_Load_Tag_SI                0x6         /* 1       2 */
#define Index_Load_Tag_SD                0x7         /* 1       3 */
#define Index_Store_Tag_I                0x8         /* 2       0 */
#define Index_Store_Tag_D                0x9         /* 2       1 */
#define Index_Store_Tag_SI               0xA         /* 2       2 */
#define Index_Store_Tag_SD               0xB         /* 2       3 */
#define Create_Dirty_Exc_D               0xD         /* 3       1 */
#define Create_Dirty_Exc_SD              0xF         /* 3       3 */
#define Hit_Invalidate_I                 0x10        /* 4       0 */
#define Hit_Invalidate_D                 0x11        /* 4       1 */
#define Hit_Invalidate_SI                0x12        /* 4       2 */
#define Hit_Invalidate_SD                0x13        /* 4       3 */
#define Hit_Writeback_Inv_D              0x15        /* 5       1 */
#define Hit_Writeback_Inv_SD             0x17        /* 5       3 */
#define Fill_I                           0x14        /* 5       0 */
#define Hit_Writeback_D                  0x19        /* 6       1 */
#define Hit_Writeback_SD                 0x1B        /* 6       3 */
#define Hit_Writeback_I                  0x18        /* 6       0 */
#define Hit_Set_Virtual_SI               0x1E        /* 7       2 */
#define Hit_Set_Virtual_SD               0x1F        /* 7       3 */

/* Disabled by chris -- horrible overload of common word.
#ifndef WAIT
#define WAIT .word 0x42000020
#endif
*/
/* Disabled by joel -- horrible overload of common word.
#ifndef wait
#define wait .word 0x42000020
#endif wait
*/

#endif

/*
** TLB resource defines
*/
#if  __mips == 1
#define	N_TLB_ENTRIES	64
#define TLB_PGSIZE	0x1000
#define RANDBASE	8
#define	TLBLO_PFNMASK	0xfffff000
#define	TLBLO_PFNSHIFT	12
#define	TLBLO_N		0x800	/* non-cacheable */
#define	TLBLO_D		0x400	/* writeable */
#define	TLBLO_V		0x200	/* valid bit */
#define	TLBLO_G		0x100	/* global access bit */

#define	TLBHI_VPNMASK	0xfffff000
#define	TLBHI_VPNSHIFT	12
#define	TLBHI_PIDMASK	0xfc0
#define	TLBHI_PIDSHIFT	6
#define	TLBHI_NPID	64

#define	TLBINX_PROBE	0x80000000
#define	TLBINX_INXMASK	0x00003f00
#define	TLBINX_INXSHIFT	8

#define	TLBRAND_RANDMASK	0x00003f00
#define	TLBRAND_RANDSHIFT	8

#define	TLBCTXT_BASEMASK	0xffe00000
#define	TLBCTXT_BASESHIFT	21

#define	TLBCTXT_VPNMASK		0x001ffffc
#define	TLBCTXT_VPNSHIFT	2
#endif
#if  __mips == 3
#define	N_TLB_ENTRIES	48

#define	TLBHI_VPN2MASK	0xffffe000
#define	TLBHI_PIDMASK	0x000000ff
#define	TLBHI_NPID	256

#define	TLBLO_PFNMASK	0x3fffffc0
#define TLBLO_PFNSHIFT	6
#define	TLBLO_D		0x00000004	/* writeable */
#define	TLBLO_V		0x00000002	/* valid bit */
#define	TLBLO_G		0x00000001	/* global access bit */
#define TLBLO_CMASK	0x00000038	/* cache algorithm mask */
#define TLBLO_CSHIFT	3

#define TLBLO_UNCACHED		(CFG_C_UNCACHED<<TLBLO_CSHIFT)
#define TLBLO_NONCOHERENT	(CFG_C_NONCOHERENT<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCL	(CFG_C_COHERENTXCL<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCLW	(CFG_C_COHERENTXCLW<<TLBLO_CSHIFT)
#define TLBLO_COHERENTUPD	(CFG_C_COHERENTUPD<<TLBLO_CSHIFT)

#define	TLBINX_PROBE	0x80000000
#define	TLBINX_INXMASK	0x0000003f

#define	TLBRAND_RANDMASK	0x0000003f

#define	TLBCTXT_BASEMASK	0xff800000
#define	TLBCTXT_BASESHIFT	23

#define	TLBCTXT_VPN2MASK	0x007ffff0
#define	TLBCTXT_VPN2SHIFT	4

#define TLBPGMASK_MASK		0x01ffe000
#endif

#if  __mips == 32
#define	N_TLB_ENTRIES	16

#define	TLBHI_VPN2MASK	0xffffe000
#define	TLBHI_PIDMASK	0x000000ff
#define	TLBHI_NPID	256

#define	TLBLO_PFNMASK	0x3fffffc0
#define TLBLO_PFNSHIFT	6
#define	TLBLO_D		0x00000004	/* writeable */
#define	TLBLO_V		0x00000002	/* valid bit */
#define	TLBLO_G		0x00000001	/* global access bit */
#define TLBLO_CMASK	0x00000038	/* cache algorithm mask */
#define TLBLO_CSHIFT	3

#define TLBLO_UNCACHED		(CFG_C_UNCACHED<<TLBLO_CSHIFT)
#define TLBLO_NONCOHERENT	(CFG_C_NONCOHERENT<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCL	(CFG_C_COHERENTXCL<<TLBLO_CSHIFT)
#define TLBLO_COHERENTXCLW	(CFG_C_COHERENTXCLW<<TLBLO_CSHIFT)
#define TLBLO_COHERENTUPD	(CFG_C_COHERENTUPD<<TLBLO_CSHIFT)

#define	TLBINX_PROBE	0x80000000
#define	TLBINX_INXMASK	0x0000003f

#define	TLBRAND_RANDMASK	0x0000003f

#define	TLBCTXT_BASEMASK	0xff800000
#define	TLBCTXT_BASESHIFT	23

#define	TLBCTXT_VPN2MASK	0x007ffff0
#define	TLBCTXT_VPN2SHIFT	4

#define TLBPGMASK_MASK		0x01ffe000
#endif

#if  __mips == 1


/* definitions for Debug and Cache Invalidate control (DCIC) register bits */
#define DCIC_TR		0x80000000	/* Trap enable */
#define DCIC_UD		0x40000000	/* User debug enable */
#define DCIC_KD		0x20000000	/* Kernel debug enable */
#define DCIC_TE		0x10000000	/* Trace enable */
#define DCIC_DW		0x08000000	/* Enable data breakpoints on write */
#define DCIC_DR		0x04000000	/* Enable data breakpoints on read */
#define DCIC_DAE	0x02000000	/* Enable data addresss breakpoints */
#define DCIC_PCE	0x01000000	/* Enable instruction breakpoints */
#define DCIC_DE		0x00800000	/* Debug enable */
#define DCIC_DL		0x00008000	/* Data cache line invalidate */
#define DCIC_IL		0x00004000	/* Instruction cache line invalidate */
#define DCIC_D		0x00002000	/* Data cache invalidate enable */
#define DCIC_I		0x00001000	/* Instr. cache invalidate enable */
#define DCIC_T		0x00000020	/* Trace, set by CPU */
#define DCIC_W		0x00000010	/* Write reference, set by CPU */
#define DCIC_R		0x00000008	/* Read reference, set by CPU */
#define DCIC_DA		0x00000004	/* Data address, set by CPU */
#define DCIC_PC		0x00000002	/* Program counter, set by CPU */
#define DCIC_DB		0x00000001	/* Debug, set by CPU */




#define	SR_CUMASK	0xf0000000	/* coproc usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

#define	SR_BEV		0x00400000	/* use boot exception vectors */

/* Cache control bits */
#define	SR_TS		0x00200000	/* TLB shutdown */
#define	SR_PE		0x00100000	/* cache parity error */
#define	SR_CM		0x00080000	/* cache miss */
#define	SR_PZ		0x00040000	/* cache parity zero */
#define	SR_SWC		0x00020000	/* swap cache */
#define	SR_ISC		0x00010000	/* Isolate data cache */

/*
**	status register interrupt masks and bits
*/

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

#define	SR_IMASKSHIFT	8

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
#endif

#if  __mips == 3
#define	SR_CUMASK	0xf0000000	/* coproc usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

#define SR_RP		0x08000000      /* Reduced power operation */
#define SR_FR		0x04000000	/* Additional floating point registers */
#define SR_RE		0x02000000	/* Reverse endian in user mode */

#define SR_BEV		0x00400000	/* Use boot exception vectors */
#define SR_TS		0x00200000	/* TLB shutdown */
#define SR_SR		0x00100000	/* Soft reset */
#define SR_CH		0x00040000	/* Cache hit */
#define SR_CE		0x00020000	/* Use cache ECC  */
#define SR_DE		0x00010000	/* Disable cache exceptions */

/*
**	status register interrupt masks and bits
*/

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

#define	SR_IMASKSHIFT	8

#define	SR_IBIT8	0x00008000	/* bit level 8 */
#define	SR_IBIT7	0x00004000	/* bit level 7 */
#define	SR_IBIT6	0x00002000	/* bit level 6 */
#define	SR_IBIT5	0x00001000	/* bit level 5 */
#define	SR_IBIT4	0x00000800	/* bit level 4 */
#define	SR_IBIT3	0x00000400	/* bit level 3 */
#define	SR_IBIT2	0x00000200	/* bit level 2 */
#define	SR_IBIT1	0x00000100	/* bit level 1 */

#define SR_KSMASK	0x00000018	/* Kernel mode mask */
#define SR_KSUSER	0x00000010	/* User mode */
#define SR_KSSUPER	0x00000008	/* Supervisor mode */
#define SR_KSKERNEL	0x00000000	/* Kernel mode */
#define SR_ERL		0x00000004	/* Error level */
#define SR_EXL		0x00000002	/* Exception level */
#define SR_IE		0x00000001	/* Interrupts enabled */
#endif

#if  __mips == 32
#define	SR_CUMASK	0xf0000000	/* coproc usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* Coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

#define SR_RP		0x08000000      /* Reduced power operation */
#define SR_FR		0x04000000	/* Additional floating point registers */
#define SR_RE		0x02000000	/* Reverse endian in user mode */

#define SR_BEV		0x00400000	/* Use boot exception vectors */
#define SR_TS		0x00200000	/* TLB shutdown */
#define SR_SR		0x00100000	/* Soft reset */
#define SR_CH		0x00040000	/* Cache hit */
#define SR_CE		0x00020000	/* Use cache ECC  */
#define SR_DE		0x00010000	/* Disable cache exceptions */

/*
**	status register interrupt masks and bits
*/

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

#define	SR_IMASKSHIFT	8

#define	SR_IBIT8	0x00008000	/* bit level 8 */
#define	SR_IBIT7	0x00004000	/* bit level 7 */
#define	SR_IBIT6	0x00002000	/* bit level 6 */
#define	SR_IBIT5	0x00001000	/* bit level 5 */
#define	SR_IBIT4	0x00000800	/* bit level 4 */
#define	SR_IBIT3	0x00000400	/* bit level 3 */
#define	SR_IBIT2	0x00000200	/* bit level 2 */
#define	SR_IBIT1	0x00000100	/* bit level 1 */

#define SR_KSMASK	0x00000018	/* Kernel mode mask */
#define SR_KSUSER	0x00000010	/* User mode */
#define SR_KSSUPER	0x00000008	/* Supervisor mode */
#define SR_KSKERNEL	0x00000000	/* Kernel mode */
#define SR_ERL		0x00000004	/* Error level */
#define SR_EXL		0x00000002	/* Exception level */
#define SR_IE		0x00000001	/* Interrupts enabled */
#endif

/*
 * Cause Register
 */
#define	CAUSE_BD	0x80000000	/* Branch delay slot */
#define	CAUSE_BT	0x40000000	/* Branch Taken */
#define	CAUSE_CEMASK	0x30000000	/* coprocessor error */
#define	CAUSE_CESHIFT	28


#define	CAUSE_IPMASK	0x0000FF00	/* Pending interrupt mask */
#define	CAUSE_IPSHIFT	8

#define	CAUSE_EXCMASK	0x0000003C	/* Cause code bits */
#define	CAUSE_EXCSHIFT	2

#ifndef XDS
/*
**  Coprocessor 0 registers
*/
#define	C0_INX		$0		/* tlb index */
#define	C0_RAND		$1		/* tlb random */
#if  __mips == 1
#define	C0_TLBLO	$2		/* tlb entry low */
#endif
#if  __mips == 3
#define	C0_TLBLO0	$2		/* tlb entry low 0 */
#define	C0_TLBLO1	$3		/* tlb entry low 1 */
#endif

#if  __mips == 32
#define	C0_TLBLO0	$2		/* tlb entry low 0 */
#define	C0_TLBLO1	$3		/* tlb entry low 1 */
#endif


#define	C0_CTXT		$4		/* tlb context */

#if  __mips == 3
#define C0_PAGEMASK	$5		/* tlb page mask */
#define C0_WIRED	$6		/* number of wired tlb entries */
#endif

#if  __mips == 32
#define C0_PAGEMASK	$5		/* tlb page mask */
#define C0_WIRED	$6		/* number of wired tlb entries */
#endif

#if  __mips == 1
#define C0_TAR          $6
#endif

#define	C0_BADVADDR	$8		/* bad virtual address */

#if  __mips == 3
#define C0_COUNT	$9		/* cycle count */
#endif
#if  __mips == 32
#define C0_COUNT	$9		/* cycle count */
#endif

#define	C0_TLBHI	$10		/* tlb entry hi */

#if  __mips == 3
#define C0_COMPARE	$11		/* cyccle count comparator  */
#endif

#if  __mips == 32
#define C0_COMPARE	$11		/* cyccle count comparator  */
#endif

#define	C0_SR		$12		/* status register */
#define	C0_CAUSE	$13		/* exception cause */
#define	C0_EPC		$14		/* exception pc */
#define	C0_PRID		$15		/* revision identifier */

#if  __mips == 1
#define C0_CONFIG   $3     /* configuration register R3081*/
#endif

#if  __mips == 3
#define C0_CONFIG	$16		/* configuration register */
#define C0_LLADDR	$17		/* linked load address */
#define C0_WATCHLO	$18		/* watchpoint trap register */
#define C0_WATCHHI	$19		/* watchpoint trap register */
#define C0_XCTXT    $20     /* extended tlb context */
#define C0_ECC		$26		/* secondary cache ECC control */
#define C0_CACHEERR	$27		/* cache error status */
#define C0_TAGLO	$28		/* cache tag lo */
#define C0_TAGHI	$29		/* cache tag hi */
#define C0_ERRPC	$30		/* cache error pc */
#endif

#if  __mips == 32
#define C0_CONFIG	$16		/* configuration register */
#define C0_LLADDR	$17		/* linked load address */
#define C0_WATCHLO	$18		/* watchpoint trap register */
#define C0_WATCHHI	$19		/* watchpoint trap register */
#define C0_XCTXT    $20     /* extended tlb context */
#define C0_ECC		$26		/* secondary cache ECC control */
#define C0_CACHEERR	$27		/* cache error status */
#define C0_TAGLO	$28		/* cache tag lo */
#define C0_TAGHI	$29		/* cache tag hi */
#define C0_ERRPC	$30		/* cache error pc */
#endif


#define C1_REVISION     $0
#define C1_STATUS       $31

#endif /* XDS */

#ifdef R4650
#define IWATCH $18
#define DWATCH $19
#define IBASE  $0
#define IBOUND $1
#define DBASE  $2
#define DBOUND $3
#define CALG	$17
#endif

#endif /* _RTEMS_MIPS_IDTCPU_H */

