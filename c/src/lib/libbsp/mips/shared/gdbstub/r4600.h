/*
 * r4600.h - register and address space definitions for the R4600 processor
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

#ifndef _R4600_H
#define	_R4600_H

/*
 * R4600 general registers
 */
#define	zero		$0
#define	at		$1		/* assembler temporary */
#define	v0		$2		/* value holders */
#define	v1		$3
#define	a0		$4		/* arguments */
#define	a1		$5
#define	a2		$6
#define	a3		$7
#define	t0		$8		/* temporaries */
#define	t1		$9
#define	t2		$10
#define	t3		$11
#define	t4		$12
#define	t5		$13
#define	t6		$14
#define	t7		$15
#define	s0		$16		/* saved registers */
#define	s1		$17
#define	s2		$18
#define	s3		$19
#define	s4		$20
#define	s5		$21
#define	s6		$22
#define	s7		$23
#define	t8		$24		/* temporaries */
#define	t9		$25
#define	k0		$26		/* kernel registers */
#define	k1		$27
#define	gp		$28		/* global pointer */
#define	sp		$29		/* stack pointer */
#define	s8		$30		/* saved register */
#define	fp		$30		/* frame pointer (obsolete usage) */
#define	ra		$31		/* return address */


/*
 * Kernel address space definitions (32 bit/64 bit compatibility spaces)
 */
#define	K0BASE 		0x80000000
#define	K0SIZE 		0x20000000
#define	K1BASE 		0xa0000000
#define	K1SIZE 		0x20000000

#define	PHYS_TO_K0(pa)	((pa)|K0BASE)
#define	PHYS_TO_K1(pa)	((pa)|K1BASE)
#define	K0_TO_PHYS(va)	((va)&(K0SIZE-1))
#define	K1_TO_PHYS(va)	((va)&(K1SIZE-1))
#define	K0_TO_K1(va)	((va)|K1SIZE)
#define	K1_TO_K0(va)	((va)&~K1SIZE)


/*
 * System Control Coprocessor (CP0) memory-management registers
 */
#define	C0_INDEX	$0		/* TLB Index */
#define	C0_RANDOM	$1		/* TLB Random */
#define	C0_ENTRYLO0	$2		/* TLB EntryLo0 */
#define	C0_ENTRYLO1	$3		/* TLB EntryLo1 */
#define	C0_PAGEMASK	$5		/* TLB PageMask */
#define	C0_WIRED	$6		/* TLB Wired */
#define	C0_ENTRYHI	$10		/* TLB EntryHi */
#define	C0_PRID		$15		/* Processor Revision Indentifier */
#define	C0_CONFIG	$16		/* Config */
#define	C0_LLADDR	$17		/* LLAddr */
#define	C0_TAGLO	$28		/* TagLo */
#define	C0_TAGHI	$29		/* TagHi (always zero on the R4600) */

/* EntryHi register fields */
#define	EH_REGION_MASK	0xc000000000000000	/* 11=krnl, 01=supv, 00=user */
#define	EH_REGION_SHIFT	62
#define	EH_FILL_MASK	0x3fffff0000000000	/* (holds replica of bit 63) */
#define	EH_FILL_SHIFT	40
#define	EH_VPN2_MASK	0x000000ffffffe000	/* Virtual pageno div 2 */
#define	EH_VPN2_SHIFT	13
#define	EH_ASID_MASK	0x00000000000000ff	/* Address space ID */
#define	EH_ASID_SHIFT	0

/* EntryLo register fields */
#define	EL_PFN_MASK	0x000000003fffffc0	/* Page Frame Number */
#define	EL_PFN_SHIFT	6
#define	EL_C_MASK	0x0000000000000038	/* Cacheability attributes */
#define	EL_C_SHIFT	3
#define	EL_D_MASK	0x0000000000000004	/* Dirty bit */
#define	EL_D_SHIFT	2
#define	EL_V_MASK	0x0000000000000002	/* Valid bit */
#define	EL_V_SHIFT	1
#define	EL_G_MASK	0x0000000000000001	/* Global bit */
#define	EL_G_SHIFT	0

/* PageMask register fields */
#define	PM_MASK		0x01ffe000	/* Page size mask: */
#define	PM_M_4K		0x00000000	/*  4K bytes */
#define	PM_M_16K	0x00006000	/*  16K bytes */
#define	PM_M_64K	0x0001e000	/*  64K bytes */
#define	PM_M_256K	0x0007e000	/*  256K bytes */
#define	PM_M_1M		0x001fe000	/*  1M bytes */
#define	PM_M_4M		0x007fe000	/*  4M bytes */
#define	PM_M_16M	0x01ffe000	/*  16M bytes */

/* Index register fields */
#define	IR_P_MASK	0x80000000	/* TLB Probe (TLBP) failure */
#define	IR_INDEX_MASK	0x0000003f	/* Index of TLB entry for TLBR/TLBWI */

/* Random register */
#define	RR_INDEX_MASK	0x0000003f	/* Index of TLB entry for TLBWR */
#define	NTLBENTRIES	48		/* Max TLB index is one less */

/* Wired register */
#define	WR_INDEX_MASK	0x0000003f	/* Number of wired TLB entries */

/* PrID register fields */
#define	PRID_IMP_MASK	(0xff<<8)	/* Implementation number */
#define	PRID_REV_MASK	(0xff<<0)	/* Revision number */

/* Config register fields (read only except for K0 cacheability attributes) */
#define	CFG_ECMASK	0x70000000	/* System Clock Ratio: */
#define	CFG_ECBY2	0x00000000 	/*  processor clock divided by 2 */
#define	CFG_ECBY3	0x10000000 	/*  processor clock divided by 3 */
#define	CFG_ECBY4	0x20000000 	/*  processor clock divided by 4 */
#define	CFG_ECBY5	0x30000000 	/*  processor clock divided by 5 */
#define	CFG_ECBY6	0x40000000 	/*  processor clock divided by 6 */
#define	CFG_ECBY7	0x50000000 	/*  processor clock divided by 7 */
#define	CFG_ECBY8	0x60000000 	/*  processor clock divided by 8 */
#define	CFG_EC_RESERVED	0x70000000 	/*  (reserved) */
#define	CFG_EPMASK	0x0f000000	/* Writeback pattern: */
#define	CFG_EPD		0x00000000	/*  DDDD (one dword every cycle) */
#define	CFG_EPDDx	0x01000000	/*  DDxDDx (2 dword/3 cyc) */
#define	CFG_EPDDxx	0x02000000	/*  DDxDDx (2 dword/4 cyc) */
#define	CFG_EPDxDx	0x03000000	/*  DxDxDxDx (2 dword/4 cyc) */
#define	CFG_EPDDxxx	0x04000000	/*  DDxxxDDxxx (2 dword/5 cyc) */
#define	CFG_EPDDxxxx	0x05000000	/*  DDxxxxDDxxxx (2 dword/6 cyc) */
#define	CFG_EPDxxDxx	0x06000000	/*  DxxDxxDxxDxx (2 dword/6 cyc) */
#define	CFG_EPDDxxxxx	0x07000000	/*  DDxxxxxDDxxxxx (2 dword/7 cyc) */
#define	CFG_EPDDxxxxxx	0x08000000	/*  DDxxxxxxDDxxxxxx (2 dword/8 cyc) */
#define	CFG_BEMASK	0x00008000	/* Big Endian */
#define	CFG_EMMASK	0x00004000	/* set to 1 => Parity mode enabled */
#define	CFG_EBMASK	0x00002000	/* set to 1 => Sub-block ordering */
#define	CFG_ICMASK	0x00000e00	/* I-cache size = 2**(12+IC) bytes */
#define	CFG_ICSHIFT	9
#define	CFG_DCMASK	0x000001c0	/* D-cache size = 2**(12+DC) bytes */
#define	CFG_DCSHIFT	6
#define	CFG_IBMASK	0x00000020	/* set to 1 => 32 byte I-cache line */
#define	CFG_DBMASK	0x00000010	/* set to 1 => 32 byte D-cache line */
#define	CFG_K0C_MASK	0x00000007	/* KSEG0 cacheability attributes: */
#define	CFG_C_WTNOALLOC	0		/*  write thru, no write allocate */
#define	CFG_C_WTALLOC	1		/*  write thru, write allocate */
#define	CFG_C_UNCACHED	2		/*  uncached */
#define	CFG_C_WRITEBACK	3		/*  writeback, non-coherent */


/* Primary Cache TagLo */
#define	TAG_PTAG_MASK		0xffffff00      /* P-Cache Tag (Addr 35:12) */
#define	TAG_PTAG_SHIFT          8
#define	TAG_PSTATE_MASK         0x000000c0      /* P-Cache State */
#define	TAG_PSTATE_SHIFT        6
#define	TAG_FIFO_BIT_MASK	0x00000002      /* P-Cache FIFO bit */
#define	TAG_FIFO_BIT_SHIFT	1
#define	TAG_PARITY_MASK		0x00000001      /* P-Cache Tag Parity */
#define	TAG_PARITY_SHIFT        0

#define	PSTATE_INVAL		0		/* Invalid */
#define	PSTATE_SHARED		1		/* Should not occur */
#define	PSTATE_CLEAN_EXCL	2		/* Should not occur */
#define	PSTATE_DIRTY_EXCL	3		/* Dirty exclusive */


/*
 * System Control Coprocessor (CP0) exception processing registers
 */
#define	C0_CONTEXT	$4		/* Context */
#define	C0_BADVADDR	$8		/* Bad Virtual Address */
#define	C0_COUNT 	$9		/* Count */
#define	C0_COMPARE	$11		/* Compare */
#define	C0_STATUS	$12		/* Processor Status */
#define	C0_CAUSE	$13		/* Exception Cause */
#define	C0_EPC		$14		/* Exception PC */
#define	C0_XCONTEXT	$20		/* XContext */
#define	C0_ECC		$26		/* ECC */
#define	C0_CACHEERR	$27		/* CacheErr */
#define	C0_ERROREPC	$30		/* ErrorEPC */


/* Status register fields */
#define	SR_CUMASK	0xf0000000	/* Coprocessor usable bits */
#define	SR_CU3		0x80000000	/* Coprocessor 3 usable */
#define	SR_CU2		0x40000000	/* coprocessor 2 usable */
#define	SR_CU1		0x20000000	/* Coprocessor 1 usable */
#define	SR_CU0		0x10000000	/* Coprocessor 0 usable */

#define	SR_FR		0x04000000	/* Enable 32 floating-point registers */
#define	SR_RE		0x02000000	/* Reverse Endian in user mode */

#define	SR_BEV		0x00400000	/* Bootstrap Exception Vector */
#define	SR_TS		0x00200000	/* TLB shutdown (reserved on R4600) */
#define	SR_SR		0x00100000	/* Soft Reset */

#define	SR_CH		0x00040000	/* Cache Hit */
#define	SR_CE		0x00020000	/* ECC register modifies check bits */
#define	SR_DE		0x00010000	/* Disable cache errors */

#define	SR_IMASK	0x0000ff00	/* Interrupt Mask */
#define	SR_IMASK8	0x00000000	/* Interrupt Mask level=8 */
#define	SR_IMASK7	0x00008000	/* Interrupt Mask level=7 */
#define	SR_IMASK6	0x0000c000	/* Interrupt Mask level=6 */
#define	SR_IMASK5	0x0000e000	/* Interrupt Mask level=5 */
#define	SR_IMASK4	0x0000f000	/* Interrupt Mask level=4 */
#define	SR_IMASK3	0x0000f800	/* Interrupt Mask level=3 */
#define	SR_IMASK2	0x0000fc00	/* Interrupt Mask level=2 */
#define	SR_IMASK1	0x0000fe00	/* Interrupt Mask level=1 */
#define	SR_IMASK0	0x0000ff00	/* Interrupt Mask level=0 */

#define	SR_IBIT8	0x00008000	/*  (Intr5) */
#define	SR_IBIT7	0x00004000	/*  (Intr4) */
#define	SR_IBIT6	0x00002000	/*  (Intr3) */
#define	SR_IBIT5	0x00001000	/*  (Intr2) */
#define	SR_IBIT4	0x00000800	/*  (Intr1) */
#define	SR_IBIT3	0x00000400	/*  (Intr0) */
#define	SR_IBIT2	0x00000200	/*  (Software Interrupt 1) */
#define	SR_IBIT1	0x00000100	/*  (Software Interrupt 0) */

#define	SR_KX		0x00000080	/* xtlb in kernel mode */
#define	SR_SX		0x00000040	/* mips3 & xtlb in supervisor mode */
#define	SR_UX		0x00000020	/* mips3 & xtlb in user mode */

#define	SR_KSU_MASK	0x00000018	/* ksu mode mask */
#define	SR_KSU_USER	0x00000010	/* user mode */
#define	SR_KSU_SUPV	0x00000008	/* supervisor mode */
#define	SR_KSU_KERN	0x00000000	/* kernel mode */

#define	SR_ERL		0x00000004	/* error level */
#define	SR_EXL		0x00000002	/* exception level */
#define	SR_IE		0x00000001 	/* interrupt enable */

/* Cause register fields */
#define	CAUSE_BD	0x80000000	/* Branch Delay */
#define	CAUSE_CEMASK	0x30000000	/* Coprocessor Error */
#define	CAUSE_CESHIFT	28		/* Right justify CE  */
#define	CAUSE_IPMASK	0x0000ff00	/* Interrupt Pending */
#define	CAUSE_IPSHIFT	8		/* Right justify IP  */
#define	CAUSE_IP8	0x00008000	/*  (Intr5) */
#define	CAUSE_IP7	0x00004000	/*  (Intr4) */
#define	CAUSE_IP6	0x00002000	/*  (Intr3) */
#define	CAUSE_IP5	0x00001000	/*  (Intr2) */
#define	CAUSE_IP4	0x00000800	/*  (Intr1) */
#define	CAUSE_IP3	0x00000400	/*  (Intr0) */
#define	CAUSE_SW2	0x00000200	/*  (Software Interrupt 1) */
#define	CAUSE_SW1	0x00000100	/*  (Software Interrupt 0) */
#define	CAUSE_EXCMASK	0x0000007c	/* Exception Code */
#define	CAUSE_EXCSHIFT	2		/* Right justify EXC */

/* Exception Codes */
#define	EXC_INT		0		/* External interrupt */
#define	EXC_MOD		1		/* TLB modification exception */
#define	EXC_TLBL	2		/* TLB miss (Load or Ifetch) */
#define	EXC_TLBS	3		/* TLB miss (Store) */
#define	EXC_ADEL	4		/* Address error (Load or Ifetch) */
#define	EXC_ADES	5		/* Address error (Store) */
#define	EXC_IBE		6		/* Bus error (Ifetch) */
#define	EXC_DBE		7		/* Bus error (data load or store) */
#define	EXC_SYS		8		/* System call */
#define	EXC_BP		9		/* Break point */
#define	EXC_RI		10		/* Reserved instruction */
#define	EXC_CPU		11		/* Coprocessor unusable */
#define	EXC_OVF		12		/* Arithmetic overflow */
#define	EXC_TRAP	13		/* Trap exception */
#define	EXC_FPE		15		/* Floating Point Exception */


/* CacheErr register */
#define	CACHEERR_TYPE		0x80000000	/* reference type:
						   0=Instr, 1=Data */
#define	CACHEERR_LEVEL		0x40000000	/* cache level:
						   0=Primary, 1=Secondary */
#define	CACHEERR_DATA		0x20000000	/* data field:
						   0=No error, 1=Error */
#define	CACHEERR_TAG		0x10000000	/* tag field:
						   0=No error, 1=Error */
#define	CACHEERR_REQ		0x08000000	/* request type:
						   0=Internal, 1=External */
#define	CACHEERR_BUS		0x04000000	/* error on bus:
						   0=No, 1=Yes */
#define	CACHEERR_BOTH		0x02000000	/* Data & Instruction error:
						   0=No, 1=Yes */
#define	CACHEERR_REFILL		0x01000000	/* Error on Refill:
						   0=No, 1=Yes */
#define	CACHEERR_SIDX_MASK	0x003ffff8	/* PADDR(21..3) */
#define	CACHEERR_SIDX_SHIFT		 0
#define	CACHEERR_PIDX_MASK	0x00000007	/* VADDR(14..12) */
#define	CACHEERR_PIDX_SHIFT		12


/*
 * R4600 Cache operations
 */
#define	Index_Invalidate_I      0x0         /* 0       0 */
#define	Index_Writeback_Inv_D   0x1         /* 0       1 */
#define	Index_Load_Tag_I        0x4         /* 1       0 */
#define	Index_Load_Tag_D        0x5         /* 1       1 */
#define	Index_Store_Tag_I       0x8         /* 2       0 */
#define	Index_Store_Tag_D       0x9         /* 2       1 */
#define	Create_Dirty_Exc_D      0xD         /* 3       1 */
#define	Hit_Invalidate_I        0x10        /* 4       0 */
#define	Hit_Invalidate_D        0x11        /* 4       1 */
#define	Fill_I                  0x14        /* 5       0 */
#define	Hit_Writeback_Inv_D     0x15        /* 5       1 */
#define	Hit_Writeback_I         0x18        /* 6       0 */
#define	Hit_Writeback_D         0x19        /* 6       1 */


/*
 * Floating Point Coprocessor (CP1) registers
 */
#define	C1_IRR		$0		/* Implementation/Revision register */
#define	C1_CSR		$31		/* FPU Control/Status register */

/* Implementation/Revision reg fields */
#define	IRR_IMP_MASK	(0xff<<8)	/* Implementation number */
#define	IRR_REV_MASK	(0xff<<0)	/* Revision number */

/* FPU Control/Status register fields */
#define	CSR_FS		0x01000000	/* Set to flush denormals to zero */
#define	CSR_C		0x00800000	/* Condition bit (set by FP compare) */

#define	CSR_CMASK	(0x3f<<12)
#define	CSR_CE		0x00020000
#define	CSR_CV		0x00010000
#define	CSR_CZ		0x00008000
#define	CSR_CO		0x00004000
#define	CSR_CU		0x00002000
#define	CSR_CI		0x00001000

#define	CSR_EMASK	(0x1f<<7)
#define	CSR_EV		0x00000800
#define	CSR_EZ		0x00000400
#define	CSR_EO		0x00000200
#define	CSR_EU		0x00000100
#define	CSR_EI		0x00000080

#define	CSR_FMASK	(0x1f<<2)
#define	CSR_FV		0x00000040
#define	CSR_FZ		0x00000020
#define	CSR_FO		0x00000010
#define	CSR_FU		0x00000008
#define	CSR_FI		0x00000004

#define	CSR_RMODE_MASK	(0x3<<0)
#define	CSR_RM		0x00000003
#define	CSR_RP		0x00000002
#define	CSR_RZ		0x00000001
#define	CSR_RN		0x00000000

#endif /* _R4600_H */
