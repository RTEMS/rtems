/*
 * This file contains some powerpc MSR and registers access definitions.
 *
 * COPYRIGHT (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  Added MPC8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POWERPC_REGISTERS_H
#define _RTEMS_POWERPC_REGISTERS_H

/* Bit encodings for Machine State Register (MSR) */
#define MSR_UCLE	(1<<26)		/* User-mode cache lock enable (e500) */
#define MSR_VE		(1<<25)		/* Alti-Vec enable (7400+) */
#define MSR_SPE		(1<<25)		/* SPE enable (e500) */
#define MSR_POW		(1<<18)		/* Enable Power Management */
#define MSR_WE		(1<<18)		/* Wait state enable (e500) */
#define MSR_TGPR	(1<<17)		/* TLB Update registers in use */
#define MSR_CE		(1<<17)		/* BookE critical interrupt */
#define MSR_ILE		(1<<16)		/* Interrupt Little-Endian enable */
#define MSR_EE		(1<<15)		/* External Interrupt enable */
#define MSR_PR		(1<<14)		/* Supervisor/User privilege */
#define MSR_FP		(1<<13)		/* Floating Point enable */
#define MSR_ME		(1<<12)		/* Machine Check enable */
#define MSR_FE0		(1<<11)		/* Floating Exception mode 0 */
#define MSR_SE		(1<<10)		/* Single Step */
#define MSR_UBLE	(1<<10)		/* User-mode BTB lock enable (e500) */
#define MSR_BE		(1<<9)		/* Branch Trace */
#define MSR_DE		(1<<9)		/* BookE debug exception */
#define MSR_FE1		(1<<8)		/* Floating Exception mode 1 */
#define MSR_E300_CE	(1<<7)		/* e300 critical interrupt */
#define MSR_IP		(1<<6)		/* Exception prefix 0x000/0xFFF */
#define MSR_IR		(1<<5)		/* Instruction MMU enable */
#define MSR_DR		(1<<4)		/* Data MMU enable */
#define MSR_IS		(1<<5)		/* Instruction address space */
#define MSR_DS		(1<<4)		/* Data address space */
#define MSR_PMM		(1<<2)		/* Performance monitor mark */
#define MSR_RI		(1<<1)		/* Recoverable Exception */
#define MSR_LE		(1<<0)		/* Little-Endian enable */

/* Bit encodings for Hardware Implementation Register (HID0)
   on PowerPC 603, 604, etc. processors (not 601). */

/* WARNING: HID0/HID1 are *truely* implementation dependent!
 *          you *cannot* rely on the same bits to be present,
 *          at the same place or even in the same register
 *          on different CPU familys.
 *          E.g., EMCP isHID0_DOZE is HID0_HI_BAT_EN on the
 *          on the 7450s. IFFT is XBSEN on 7450 and so on...
 */
#define HID0_EMCP	(1<<31)		/* Enable Machine Check pin */
#define HID0_EBA	(1<<29)		/* Enable Bus Address Parity */
#define HID0_EBD	(1<<28)		/* Enable Bus Data Parity */
#define HID0_SBCLK	(1<<27)
#define HID0_TBEN       (1<<26)         /* 7455:this bit must be set
					 * and TBEN signal must be asserted
					 * to enable the time base and
					 * decrementer.
					 */
#define HID0_EICE	(1<<26)
#define HID0_ECLK	(1<<25)
#define HID0_PAR	(1<<24)
#define HID0_DOZE	(1<<23)
/* this HI_BAT_EN only on 7445, 7447, 7448, 7455 & 7457 !!          */
#define HID0_7455_HIGH_BAT_EN (1<<23)

#define HID0_NAP	(1<<22)
#define HID0_SLEEP	(1<<21)
#define HID0_DPM	(1<<20)
#define HID0_ICE	(1<<15)		/* Instruction Cache Enable */
#define HID0_DCE	(1<<14)		/* Data Cache Enable */
#define HID0_ILOCK	(1<<13)		/* Instruction Cache Lock */
#define HID0_DLOCK	(1<<12)		/* Data Cache Lock */
#define HID0_ICFI	(1<<11)		/* Instruction Cache Flash Invalidate */
#define HID0_DCI	(1<<10)		/* Data Cache Invalidate */
/* this bit is XSBSEN (xtended block size enable) on 7447, 7448, 7455 and 7457 only */
#define HID0_7455_XBSEN       (1<<8)
#define HID0_SIED	(1<<7)		/* Serial Instruction Execution [Disable] */
#define HID0_BTIC	(1<<5)          /* Branch Target Instruction Cache [Enable] */
/* S.K. Feng 10/03, added for MPC7455 */
#define HID0_LRSTK	(1<<4)          /* Link register stack enable (7455) */
#define HID0_FOLD	(1<<3)          /* Branch folding enable (7455) */

#define HID0_BHTE	(1<<2)		/* Branch History Table Enable */
#define HID0_BTCD	(1<<1)		/* Branch target cache disable */

/* fpscr settings */
#define FPSCR_FX        (1<<31)
#define FPSCR_FEX       (1<<30)

#define _MACH_prep     1
#define _MACH_Pmac     2 /* pmac or pmac clone (non-chrp) */
#define _MACH_chrp     4 /* chrp machine */
#define _MACH_mbx      8 /* Motorola MBX board */
#define _MACH_apus    16 /* amiga with phase5 powerup */
#define _MACH_fads    32 /* Motorola FADS board */

/* see residual.h for these */
#define _PREP_Motorola 0x01  /* motorola prep */
#define _PREP_Firm     0x02  /* firmworks prep */
#define _PREP_IBM      0x00  /* ibm prep */
#define _PREP_Bull     0x03  /* bull prep */

/* these are arbitrary */
#define _CHRP_Motorola 0x04  /* motorola chrp, the cobra */
#define _CHRP_IBM     0x05   /* IBM chrp, the longtrail and longtrail 2 */

#define _GLOBAL(n)\
	.globl n;\
n:

#define	TBRU	269	/* Time base Upper/Lower (Reading) */
#define	TBRL	268
#define TBWU	285	/* Time base Upper/Lower (Writing) */
#define TBWL	284
#define	PPC_XER	1
#define PPC_LR	8
#define PPC_CTR	9
#define HID0	1008	/* Hardware Implementation 0 */
#define HID1	1009	/* Hardware Implementation 1 */
#define HID2	1011	/* Hardware Implementation 2 */
#define DABR	1013	/* Data Access Breakpoint  */
#define PPC_PVR	287	/* Processor Version */
#define IBAT0U	528	/* Instruction BAT #0 Upper/Lower */
#define IBAT0L	529
#define IBAT1U	530	/* Instruction BAT #1 Upper/Lower */
#define IBAT1L	531
#define IBAT2U	532	/* Instruction BAT #2 Upper/Lower */
#define IBAT2L	533
#define IBAT3U	534	/* Instruction BAT #3 Upper/Lower */
#define IBAT3L	535

/* Only present on 7445, 7447, 7448, 7455 and 7457 (if HID0[HIGH_BAT_EN]) */
#define IBAT4U	560	/* Instruction BAT #4 Upper/Lower */
#define IBAT4L	561
#define IBAT5U	562	/* Instruction BAT #5 Upper/Lower */
#define IBAT5L	563
#define IBAT6U	564	/* Instruction BAT #6 Upper/Lower */
#define IBAT6L	565
#define IBAT7U	566	/* Instruction BAT #7 Upper/Lower */
#define IBAT7L	567

#define DBAT0U	536	/* Data BAT #0 Upper/Lower */
#define DBAT0L	537
#define DBAT1U	538	/* Data BAT #1 Upper/Lower */
#define DBAT1L	539
#define DBAT2U	540	/* Data BAT #2 Upper/Lower */
#define DBAT2L	541
#define DBAT3U	542	/* Data BAT #3 Upper/Lower */
#define DBAT3L	543

/* Only present on 7445, 7447, 7448, 7455 and 7457 (if HID0[HIGH_BAT_EN]) */
#define DBAT4U	568	/* Instruction BAT #4 Upper/Lower */
#define DBAT4L	569
#define DBAT5U	570	/* Instruction BAT #5 Upper/Lower */
#define DBAT5L	571
#define DBAT6U	572	/* Instruction BAT #6 Upper/Lower */
#define DBAT6L	573
#define DBAT7U	574	/* Instruction BAT #7 Upper/Lower */
#define DBAT7L	575

#define DMISS	976	/* TLB Lookup/Refresh registers */
#define DCMP	977
#define HASH1	978
#define HASH2	979
#define IMISS	980
#define ICMP	981
#define PPC_RPA	982
#define SDR1	25	/* MMU hash base register */
#define PPC_DAR	19	/* Data Address Register */
#define DEAR_BOOKE 61
#define DEAR_405 981
#define SPR0	272	/* Supervisor Private Registers */
#define SPRG0   272
#define SPR1	273
#define SPRG1   273
#define SPR2	274
#define SPRG2   274
#define SPR3	275
#define SPRG3   275
#define SPRG4   276
#define SPRG5   277
#define SPRG6   278
#define SPRG7   279
#define USPRG0  256
#define DSISR	18
#define SRR0	26	/* Saved Registers (exception) */
#define SRR1	27
#define IABR	1010	/* Instruction Address Breakpoint */
#define PPC_DEC	22	/* Decrementer */
#define PPC_EAR	282	/* External Address Register */

#define MSSCR0   1014   /* Memory Subsystem Control Register */

#define L2CR	1017    /* PPC 750 and 74xx L2 control register */

#define L2CR_L2E   (1<<31)	/* enable */
#define L2CR_L2I   (1<<21)	/* global invalidate */

/* watch out L2IO and L2DO are different between 745x and 7400/7410 */
/* Oddly, the following L2CR bit defintions in 745x
 * is different from that of 7400 and 7410.
 * Though not used in 7400 and 7410, it is appeded with _745x just
 * to be clarified.
 */	
#define L2CR_L2IO_745x  0x100000  /* (1<<20) L2 Instruction-Only  */
#define L2CR_L2DO_745x  0x10000   /* (1<<16) L2 Data-Only */
#define L2CR_LOCK_745x  (L2CR_L2IO_745x|L2CR_L2DO_745x)
#define L2CR_L3OH0      0x00080000 /* 12:L3 output hold 0 */
	
#define L3CR    1018    /* PPC 7450/7455 L3 control register */
#define L3CR_L3IO_745x  0x400000  /* (1<<22) L3 Instruction-Only */
#define L3CR_L3DO_745x  0x40	  /* (1<<6) L3 Data-Only */

#define L3CR_LOCK_745x  (L3CR_L3IO_745x|L3CR_L3DO_745x)

#define	  L3CR_RESERVED		  0x0438003a /* Reserved bits in L3CR */
#define	  L3CR_L3E		  0x80000000 /* 0: L3 enable */
#define	  L3CR_L3PE		  0x40000000 /* 1: L3 data parity checking enable */
#define	  L3CR_L3APE		  0x20000000 /* 2: L3 address parity checking enable */
#define	  L3CR_L3SIZ		  0x10000000 /* 3: L3 size (0=1MB, 1=2MB) */
#define	   L3SIZ_1M		  0x00000000
#define	   L3SIZ_2M		  0x10000000
#define	  L3CR_L3CLKEN		  0x08000000 /* 4: Enables the L3_CLK[0:1] signals */
#define	  L3CR_L3CLK		  0x03800000 /* 6-8: L3 clock ratio */
#define	   L3CLK_60		  0x00000000 /* core clock / 6   */
#define	   L3CLK_20		  0x01000000 /*            / 2   */
#define	   L3CLK_25		  0x01800000 /*            / 2.5 */
#define	   L3CLK_30		  0x02000000 /*            / 3   */
#define	   L3CLK_35		  0x02800000 /*            / 3.5 */
#define	   L3CLK_40		  0x03000000 /*            / 4   */
#define	   L3CLK_50		  0x03800000 /*            / 5   */
#define	  L3CR_L3IO		  0x00400000 /* 9: L3 instruction-only mode */
#define	  L3CR_L3SPO		  0x00040000 /* 13: L3 sample point override */
#define	  L3CR_L3CKSP		  0x00030000 /* 14-15: L3 clock sample point */
#define	   L3CKSP_2		  0x00000000 /* 2 clocks */
#define	   L3CKSP_3		  0x00010000 /* 3 clocks */
#define	   L3CKSP_4		  0x00020000 /* 4 clocks */
#define	   L3CKSP_5		  0x00030000 /* 5 clocks */
#define	  L3CR_L3PSP		  0x0000e000 /* 16-18: L3 P-clock sample point */
#define	   L3PSP_0		  0x00000000 /* 0 clocks */
#define	   L3PSP_1		  0x00002000 /* 1 clocks */
#define	   L3PSP_2		  0x00004000 /* 2 clocks */
#define	   L3PSP_3		  0x00006000 /* 3 clocks */
#define	   L3PSP_4		  0x00008000 /* 4 clocks */
#define	   L3PSP_5		  0x0000a000 /* 5 clocks */
#define	  L3CR_L3REP		  0x00001000 /* 19: L3 replacement algorithm (0=default, 1=alternate) */
#define	  L3CR_L3HWF		  0x00000800 /* 20: L3 hardware flush */
#define	  L3CR_L3I		  0x00000400 /* 21: L3 global invaregisters.h.orig
lidate */
#define	  L3CR_L3RT		  0x00000300 /* 22-23: L3 SRAM type */
#define	   L3RT_MSUG2_DDR	  0x00000000 /* MSUG2 DDR SRAM */
#define	   L3RT_PIPELINE_LATE	  0x00000100 /* Pipelined (register-register) synchronous late-write SRAM */
#define	   L3RT_PB2_SRAM	  0x00000300 /* PB2 SRAM */
#define	  L3CR_L3NIRCA		  0x00000080 /* 24: L3 non-integer ratios clock adjustment for the SRAM */
#define	  L3CR_L3DO		  0x00000040 /* 25: L3 data-only mode */
#define	  L3CR_PMEN		  0x00000004 /* 29: Private memory enable */
#define	  L3CR_PMSIZ		  0x00000004 /* 31: Private memory size (0=1MB, 1=2MB) */

#define THRM1	1020
#define THRM2	1021
#define THRM3	1022
#define THRM1_TIN (1<<(31-0))
#define THRM1_TIV (1<<(31-1))
#define THRM1_THRES (0x7f<<(31-8))
#define THRM1_TID (1<<(31-29))
#define THRM1_TIE (1<<(31-30))
#define THRM1_V   (1<<(31-31))
#define THRM3_SITV (0x1fff << (31-30))
#define THRM3_E   (1<<(31-31))

/* Segment Registers */
#define PPC_SR0	0
#define PPC_SR1	1
#define PPC_SR2	2
#define PPC_SR3	3
#define PPC_SR4	4
#define PPC_SR5	5
#define PPC_SR6	6
#define PPC_SR7	7
#define PPC_SR8	8
#define PPC_SR9	9
#define PPC_SR10	10
#define PPC_SR11	11
#define PPC_SR12	12
#define PPC_SR13	13
#define PPC_SR14	14
#define PPC_SR15	15

#define BOOKE_DECAR	54

#define PPC405_TSR	0x3D8
#define BOOKE_TSR	336
#define BOOKE_TSR_ENW		(1<<31)
#define BOOKE_TSR_WIS		(1<<30)
#define BOOKE_TSR_DIS		(1<<27)
#define BOOKE_TSR_FIS		(1<<26)

#define PPC405_TCR	0x3DA
#define BOOKE_TCR	340
#define BOOKE_TCR_WP(x)		(((x)&3)<<30)
#define BOOKE_TCR_WP_MASK	(3<<30)
#define BOOKE_TCR_WRC(x)	(((x)&3)<<28)
#define BOOKE_TCR_WRC_MASK	(3<<28)
#define BOOKE_TCR_WIE		(1<<27)
#define BOOKE_TCR_DIE		(1<<26)
#define BOOKE_TCR_FP(x)		(((x)&3)<<24)
#define BOOKE_TCR_FIE		(1<<23)
#define BOOKE_TCR_ARE		(1<<22)
#define BOOKE_TCR_WPEXT(x)	(((x)&0xf)<<17)
#define BOOKE_TCR_WPEXT_MASK	(0xf<<17)
#define BOOKE_TCR_FPEXT(x)	(((x)&0xf)<<13)
#define BOOKE_TCR_FPEXT_MASK	(0xf<<13)

#define BOOKE_PID 48
#define BOOKE_ESR 62
#define BOOKE_IVPR 63
#define BOOKE_PIR 286
#define BOOKE_DBSR 304
#define BOOKE_DBCR0 308
#define BOOKE_DBCR1 309
#define BOOKE_DBCR2 310
#define BOOKE_DAC1 316
#define BOOKE_DAC2 317
#define BOOKE_DVC1 318
#define BOOKE_DVC2 319

/* Freescale Book E Implementation Standards (EIS): Branch Operations */

#define FSL_EIS_BUCSR 1013
#define FSL_EIS_BUCSR_BBFI (1 << (63 - 54))
#define FSL_EIS_BUCSR_BPEN (1 << (63 - 63))

/* Freescale Book E Implementation Standards (EIS): Hardware Implementation-Dependent Registers */

#define FSL_EIS_SVR 1023

/* Freescale Book E Implementation Standards (EIS): MMU Control and Status */

#define FSL_EIS_MAS0 624
#define FSL_EIS_MAS0_TLBSEL (1 << (63 - 35))
#define FSL_EIS_MAS0_ESEL(n) ((0xf & (n)) << (63 - 47))
#define FSL_EIS_MAS0_ESEL_GET(m) (((m) >> (63 - 47)) & 0xf)
#define FSL_EIS_MAS0_NV (1 << (63 - 63))

#define FSL_EIS_MAS1 625
#define FSL_EIS_MAS1_V (1 << (63 - 32))
#define FSL_EIS_MAS1_IPROT (1 << (63 - 33))
#define FSL_EIS_MAS1_TID(n) ((0xff & (n)) << (63 - 47))
#define FSL_EIS_MAS1_TID_GET(n) (((n) >> (63 - 47)) & 0xfff)
#define FSL_EIS_MAS1_TS (1 << (63 - 51))
#define FSL_EIS_MAS1_TSIZE(n) ((0xf & (n)) << (63 - 55))
#define FSL_EIS_MAS1_TSIZE_GET(n) (((n)>>(63 - 55)) & 0xf)

#define FSL_EIS_MAS2 626
#define FSL_EIS_MAS2_EPN(n) ((((1 << 21) - 1)&(n)) << (63-51))
#define FSL_EIS_MAS2_EPN_GET(n) (((n) >> (63 - 51)) & 0xfffff)
#define FSL_EIS_MAS2_EA(n) FSL_EIS_MAS2_EPN((n) >> 12)
#define FSL_EIS_MAS2_EA_GET(n) (FSL_EIS_MAS2_EPN_GET(n) << 12)
#define FSL_EIS_MAS2_X0 (1 << (63 - 57))
#define FSL_EIS_MAS2_X1 (1 << (63 - 58))
#define FSL_EIS_MAS2_W (1 << (63 - 59))
#define FSL_EIS_MAS2_I (1 << (63 - 60))
#define FSL_EIS_MAS2_M (1 << (63 - 61))
#define FSL_EIS_MAS2_G (1 << (63 - 62))
#define FSL_EIS_MAS2_E (1 << (63 - 63))
#define FSL_EIS_MAS2_ATTR(x) ((x) & 0x7f)
#define FSL_EIS_MAS2_ATTR_GET(x) ((x) & 0x7f)

#define FSL_EIS_MAS3 627
#define FSL_EIS_MAS3_RPN(n) ((((1 << 21) - 1) & (n)) << (63-51))
#define FSL_EIS_MAS3_RPN_GET(n) (((n)>>(63 - 51)) & 0xfffff)
#define FSL_EIS_MAS3_RA(n) FSL_EIS_MAS3_RPN((n) >> 12)
#define FSL_EIS_MAS3_RA_GET(n) (FSL_EIS_MAS3_RPN_GET(n) << 12)
#define FSL_EIS_MAS3_U0 (1 << (63 - 54))
#define FSL_EIS_MAS3_U1 (1 << (63 - 55))
#define FSL_EIS_MAS3_U2 (1 << (63 - 56))
#define FSL_EIS_MAS3_U3 (1 << (63 - 57))
#define FSL_EIS_MAS3_UX (1 << (63 - 58))
#define FSL_EIS_MAS3_SX (1 << (63 - 59))
#define FSL_EIS_MAS3_UW (1 << (63 - 60))
#define FSL_EIS_MAS3_SW (1 << (63 - 61))
#define FSL_EIS_MAS3_UR (1 << (63 - 62))
#define FSL_EIS_MAS3_SR (1 << (63 - 63))
#define FSL_EIS_MAS3_PERM(n) ((n) & 0x3ff)
#define FSL_EIS_MAS3_PERM_GET(n) ((n) & 0x3ff)

#define FSL_EIS_MAS4 628
#define FSL_EIS_MAS4_TLBSELD (1 << (63 - 35))
#define FSL_EIS_MAS4_TIDSELD(n) ((0x3 & (n)) << (63 - 47))
#define FSL_EIS_MAS4_TSIZED(n) ((0xf & (n)) << (63 - 55))
#define FSL_EIS_MAS4_X0D FSL_EIS_MAS2_X0
#define FSL_EIS_MAS4_X1D FSL_EIS_MAS2_X1
#define FSL_EIS_MAS4_WD FSL_EIS_MAS2_W
#define FSL_EIS_MAS4_ID FSL_EIS_MAS2_I
#define FSL_EIS_MAS4_MD FSL_EIS_MAS2_M
#define FSL_EIS_MAS4_GD FSL_EIS_MAS2_G
#define FSL_EIS_MAS4_ED FSL_EIS_MAS2_E

#define FSL_EIS_MAS5 629

#define FSL_EIS_MAS6 630
#define FSL_EIS_MAS6_SPID0(n) ((0xff & (n)) << (63 - 55))
#define FSL_EIS_MAS6_SAS (1 << (63 - 63))

#define FSL_EIS_MAS7 944

#define FSL_EIS_MMUCFG 1015
#define FSL_EIS_MMUCSR0 1012
#define FSL_EIS_PID0 48
#define FSL_EIS_PID1 633
#define FSL_EIS_PID2 634
#define FSL_EIS_TLB0CFG 688
#define FSL_EIS_TLB1CFG 689

/* Freescale Book E Implementation Standards (EIS): L1 Cache */

#define FSL_EIS_L1CFG0 515
#define FSL_EIS_L1CFG1 516
#define FSL_EIS_L1CSR0 1010
#define FSL_EIS_L1CSR1 1011

/* Freescale Book E Implementation Standards (EIS): Timer */

#define FSL_EIS_ATBL 526
#define FSL_EIS_ATBU 527

/* Freescale Book E Implementation Standards (EIS): Interrupt */

#define FSL_EIS_MCAR 573 
#define FSL_EIS_DSRR0 574 
#define FSL_EIS_DSRR1 575 

/* Freescale Book E Implementation Standards (EIS): Signal Processing Engine (SPE) */

#define FSL_EIS_SPEFSCR 512

/* Freescale Book E Implementation Standards (EIS): Software-Use SPRs */

#define FSL_EIS_SPRG8 604 
#define FSL_EIS_SPRG9 605 

/* Freescale Book E Implementation Standards (EIS): Debug */

#define FSL_EIS_DBCR3 561 
#define FSL_EIS_DBCR4 563 
#define FSL_EIS_DBCR5 564 
#define FSL_EIS_DBCR6 603 
#define FSL_EIS_DBCNT 562 

/**
 * @brief Default value for the interrupt disable mask.
 *
 * The interrupt disable mask is stored in the SPRG0 (= special purpose
 * register 272).
 */
#define PPC_INTERRUPT_DISABLE_MASK_DEFAULT MSR_EE

#ifndef ASM

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _CPU_MSR_GET( _msr_value ) \
  do { \
    _msr_value = 0; \
    __asm__ volatile ("mfmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

#define _CPU_MSR_SET( _msr_value ) \
{ __asm__ volatile ("mtmsr %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

static inline void ppc_interrupt_set_disable_mask( uint32_t mask )
{
  __asm__ volatile (
    "mtspr 272, %0"
    :
    : "r" (mask)
  );
}

static inline uint32_t ppc_interrupt_get_disable_mask( void )
{
  uint32_t mask;

  __asm__ volatile (
    "mfspr %0, 272"
    : "=r" (mask)
  );

  return mask;
}

static inline uint32_t ppc_interrupt_disable( void )
{
  uint32_t level;
  uint32_t mask;

  __asm__ volatile (
    "mfmsr %0;"
    "mfspr %1, 272;"
    "andc %1, %0, %1;"
    "mtmsr %1"
    : "=r" (level), "=r" (mask)
  );

  return level;
}

static inline void ppc_interrupt_enable( uint32_t level )
{
  __asm__ volatile (
    "mtmsr %0"
    :
    : "r" (level)
  );
}

static inline void ppc_interrupt_flash( uint32_t level )
{
  uint32_t current_level;

  __asm__ volatile (
    "mfmsr %0;"
    "mtmsr %1;"
    "mtmsr %0"
    : "=&r" (current_level)
    : "r" (level)
  );
}

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    _isr_cookie = ppc_interrupt_disable(); \
  } while (0)

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of an RTEMS critical section.  The parameter
 *  _isr_cookie is not modified.
 */

#define _CPU_ISR_Enable( _isr_cookie )  \
  ppc_interrupt_enable(_isr_cookie)

/*
 *  This temporarily restores the interrupt to _isr_cookie before immediately
 *  disabling them again.  This is used to divide long RTEMS critical
 *  sections into two or more parts.  The parameter _isr_cookie is not
 *  modified.
 *
 *  NOTE:  The version being used is not very optimized but it does
 *         not trip a problem in gcc where the disable mask does not
 *         get loaded.  Check this for future (post 10/97 gcc versions.
 */

#define _CPU_ISR_Flash( _isr_cookie ) \
  ppc_interrupt_flash(_isr_cookie)

/* end of ISR handler macros */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* _RTEMS_POWERPC_REGISTERS_H */
