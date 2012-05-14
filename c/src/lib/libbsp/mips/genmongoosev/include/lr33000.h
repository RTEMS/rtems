/* lr33000.h - LSI LR33000 CPU header */


/*
modification history
--------------------
01c,22sep92,rrr  added support for c++
01b,20apr92,ajm	 added CR_DCAS for ethernet support
01a,10feb92,ajm	 written for FCS
*/

#ifndef __INClr33000h
#define __INClr33000h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _ASMLANGUAGE
#define C0_DCIC	$7		/* cache control */
#define C0_BPC	$3		/* breakpoint on instr */
#define C0_BDA	$5		/* breakpoint on data */
#endif /* _ASMLANGUAGE */

#define DEBUG_VECT	0x00000040
#define K0_DEBUG_VECT	(K0BASE+DEBUG_VECT)
#define K1_DEBUG_VECT	(K1BASE+DEBUG_VECT)

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

/* Define counter/timer register addresses */
#define M_TIC1		0xfffe0000	/* timer 1 initial count */
#define M_TC1		0xfffe0004	/* timer 1 control	 */
#define M_TIC2		0xfffe0008	/* timer 2 initial count */
#define M_TC2		0xfffe000c	/* timer 2 control	 */
#define M_RTIC		0xfffe0010	/* refresh timer	 */
#define M_CFGREG	0xfffe0020	/* configuration reg	 */

/* Definitions for counter/timer control register bits */
#define TC_CE	 	0x00000004	/* count enable */
#define TC_IE	 	0x00000002	/* interrupt enable (1 == enable) */
#define TC_INT 	 	0x00000001	/* interrupt acknowlege (0 == ack) */
#define TCNT_MASK	0x00ffffff	/* 24 bit timer mask */

/* Definitions for Configuration register bits */
#define CR_ICDISABLE	0x00800000	/* Instruction cache disable */
#define CR_DCDISABLE	0x00400000	/* Data cache disable */
#define CR_IBLK_2	0x00000000	/* Instruction cache block size */
#define CR_IBLK_4	0x00100000	/* Instruction cache block size */
#define CR_IBLK_8	0x00200000	/* Instruction cache block size */
#define CR_IBLK_16	0x00300000	/* Instruction cache block size */
#define CR_IBLKMSK	0x00300000	/* Instruction cache block size */
#define CR_DBLK_2	0x00000000	/* Data cache block size */
#define CR_DBLK_4	0x00040000	/* Data cache block size */
#define CR_DBLK_8	0x00080000	/* Data cache block size */
#define CR_DBLK_16	0x000c0000	/* Data cache block size */
#define CR_DBLKMSK	0x000c0000	/* Data cache block size */
#define CR_IODIS	0x00020000	/* Disable DRDY for I/O addresses */
#define CR_IOWAITSHFT	13		/* I/O wait states */
#define CR_PDIS		0x00001000	/* Disable DRDY for PROM addresses */
#define CR_PWAITSHFT	8		/* PROM wait states */
#define CR_DCAS		0x00000080	/* Define # cycles of DCAS */
#define CR_DPEN		0x00000040	/* Enable parity check for DRAM */
#define CR_RDYGEN	0x00000020	/* Disable DRDY for DRAM addresses */
#define CR_BLKFDIS	0x00000010	/* Disable DRAM block refill */
#define CR_RFSHEN	0x00000008	/* Enable refresh generator */
#define CR_RASPCHG	0x00000004	/* Define RAS precharge */
#define CR_CASLNTH	0x00000002	/* Define CAS active time */
#define CR_DRAMEN	0x00000001	/* Enable DRAM controller */
#define CR_PWT(x)	((x) << 8)	/* memory wait states */
#define CR_IOWT(x)	((x) << 13)	/* io wait states */

#define LR33000_DSIZE	0x400		/* Data cache = 1Kbytes */
#define LR33000_ISIZE	0x2000		/* Instructrion cache = 8Kbytes */

#ifdef __cplusplus
}
#endif

#endif /* __INClr33000h */
