/* lr333x0.h - LSI LR333x0 CPU header */


#ifndef __INClr333x0h
#define __INClr333x0h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _ASMLANGUAGE
#define C0_BPC	$3		/* breakpoint on instr */
#define C0_BDA	$5		/* breakpoint on data */
#define C0_TAR  $6              /* target address register */
#define C0_DCIC	$7		/* cache control */
#define C0_BDAM $9              /* breakpoint data addr mask */
#define C0_BPCM $11             /* breakpoint instr addr mask */
#else
IMPORT	int sysICsize;		/* inst cache size defined in BSP */
IMPORT	int sysDCsize;		/* data cache size defined in BSP */
IMPORT	BOOL sysICset0;		/* inst cache set 0 present ? */
#endif /* _ASMLANGUAGE */

#define DEBUG_VECT	0x00000040
#define K0_DEBUG_VECT	(K0BASE+DEBUG_VECT)
#define K1_DEBUG_VECT	(K1BASE+DEBUG_VECT)

/* Define register addresses */

#define M_TIC1		0xfffe0000	/* timer 1 initial count   */
#define M_TC1		0xfffe0004	/* timer 1 control	   */
#define M_TIC2		0xfffe0008	/* timer 2 initial count   */
#define M_TC2		0xfffe000c	/* timer 2 control	   */
#define M_RTIC		0xfffe0010	/* refresh timer	   */
#define M_SRAM          0xfffe0100      /* SRAM config reg */
#define M_SPEC0         0xfffe0104
#define M_SPEC1         0xfffe0108
#define M_SPEC2         0xfffe010c
#define M_SPEC3         0xfffe0110
#define M_DRAM		0xfffe0120	/* DRAM configuration      */
#define M_BIU		0xfffe0130	/* BIU/cache configuration */

/* Definitions for Debug and Cache Invalidate control (DCIC) register bits */

#define DCIC_TR		0x80000000	/* Trap enable */
#define DCIC_UD		0x40000000	/* User debug enable */
#define DCIC_KD		0x20000000	/* Kernel debug enable */
#define DCIC_TE		0x10000000	/* Trace enable */
#define DCIC_DW		0x08000000	/* Enable data breakpoints on write */
#define DCIC_DR		0x04000000	/* Enable data breakpoints on read */
#define DCIC_DAE	0x02000000	/* Enable data addresss breakpoints */
#define DCIC_PCE	0x01000000	/* Enable instruction breakpoints */
#define DCIC_DE		0x00800000	/* Debug enable */
#define DCIC_T		0x00000020	/* Trace, set by CPU */
#define DCIC_W		0x00000010	/* Write reference, set by CPU */
#define DCIC_R		0x00000008	/* Read reference, set by CPU */
#define DCIC_DA		0x00000004	/* Data address, set by CPU */
#define DCIC_PC		0x00000002	/* Program counter, set by CPU */
#define DCIC_DB		0x00000001	/* Debug, set by CPU */

/* Definitions for counter/timer control register bits */

#define TC_CE	 	0x00000004	/* count enable */
#define TC_IE	 	0x00000002	/* interrupt enable (1 == enable) */
#define TC_INT 	 	0x00000001	/* interrupt acknowlege (0 == ack) */

/* Definitions for Wait-state configuration register bits */

#define	SPC_INHIBITSHFT 24		/* Inhibit shift count */
#define	SPC_EXTGNT	0x00800000	/* External data ready */
#define	SPC_16WIDE	0x00400000	/* 16-bit wide memory */
#define	SPC_8WIDE	0x00200000	/* 8-bit wide memory */
#define	SPC_PENA	0x00100000	/* Parity enable */
#define	SPC_CACHED	0x00080000	/* Cache data */
#define	SPC_CSDLY_3	0x00060000	/* Select delay, 3 cycles */
#define	SPC_CSDLY_2	0x00040000	/* Select delay, 2 cycles */
#define	SPC_CSDLY_1	0x00020000	/* Select delay, 1 cycles */
#define	SPC_CSDLY_0	0x00000000	/* Select delay, 0 cycles */
#define	SPC_BLKENA	0x00010000	/* Block enable */
#define	SPC_BLKWAIT_7	0x0000e000	/* Block delay, 7 cycles */
#define	SPC_BLKWAIT_6	0x0000c000	/* Block delay, 6 cycles */
#define	SPC_BLKWAIT_5	0x0000a000	/* Block delay, 5 cycles */
#define	SPC_BLKWAIT_4	0x00008000	/* Block delay, 4 cycles */
#define	SPC_BLKWAIT_3	0x00006000	/* Block delay, 3 cycles */
#define	SPC_BLKWAIT_2	0x00004000	/* Block delay, 2 cycles */
#define	SPC_BLKWAIT_1	0x00002000	/* Block delay, 1 cycles */
#define	SPC_BLKWAIT_0	0x00000000	/* Block delay, 0 cycles */
#define	SPC_RECSHFT     7		/* Recovery time shift count */
#define	SPC_WAITENA     0x00000040	/* Wait-state generator enable */
#define	SPC_WAITSHFT	0		/* Wait shift count */

/* Definitions for DRAM configuration register bits */

#define DRAM_DLP1	0x10000000	/* Data latch in phase 1 */
#define DRAM_SYNC	0x08000000	/* Sunchronous DRAM mode */
#define DRAM_SCFG	0x04000000	/* Synchronous Configuration mode */
#define DRAM_DMARDY	0x02000000	/* DMA ready */
#define DRAM_DMABLK_64	0x01400000	/* DMA block refill size, 64 words */
#define DRAM_DMABLK_32	0x01000000	/* DMA block refill size, 32 words */
#define DRAM_DMABLK_16	0x00c00000	/* DMA block refill size, 16 words */
#define DRAM_DMABLK_8	0x00800000	/* DMA block refill size, 8 words */
#define DRAM_DMABLK_4	0x00400000	/* DMA block refill size, 4 words */
#define DRAM_DMABLK_2	0x00000000	/* DMA block refill size, 2 words */
#define DRAM_DPTH_8	0x00300000	/* CAS ready depth, 8 per cycle */
#define DRAM_DPTH_4	0x00200000	/* CAS ready depth, 4 per cycle */
#define DRAM_DPTH_2	0x00100000	/* CAS ready depth, 2 per cycle */
#define DRAM_DPTH_1	0x00000000	/* CAS ready depth, 1 per cycle */
#define DRAM_RDYW	0x00080000	/* Ready Wait */
#define DRAM_PGSZ_2K	0x00070000	/* Page size, 2K words */
#define DRAM_PGSZ_1K	0x00060000	/* Page size, 1K words */
#define DRAM_PGSZ_512	0x00050000	/* Page size, 512 words */
#define DRAM_PGSZ_256	0x00040000	/* Page size, 256 words */
#define DRAM_PGSZ_128	0x00030000	/* Page size, 128 words */
#define DRAM_PGSZ_64	0x00020000	/* Page size, 64 words */
#define DRAM_PGSZ_32	0x00010000	/* Page size, 32 words */
#define DRAM_PGSZ_16	0x00000000	/* Page size, 16 words */
#define DRAM_PGMW	0x00008000	/* Page mode write enable */
#define DRAM_RFWE_0	0x00004000	/* Refresh write enable mode, bit 1 */
#define DRAM_RFWE_1	0x00002000	/* Refresh write enable mode, bit 0 */
#define DRAM_RFEN	0x00001000	/* Internal refresh enable */
#define DRAM_RDYEN	0x00000800	/* Internal ready generation */
#define DRAM_BFD	0x00000400	/* Block fetch disable */
#define DRAM_PE		0x00000200	/* Parity checking enable */
#define DRAM_RPC_3	0x00000180	/* RAS precharge, 3 */
#define DRAM_RPC_2	0x00000100	/* RAS precharge, 2 */
#define DRAM_RPC_1	0x00000080	/* RAS precharge, 1 */
#define DRAM_RPC_0	0x00000000	/* RAS precharge, 0 */
#define DRAM_RCD_3	0x00000060	/* RAS to CAS delay, 3 */
#define DRAM_RCD_2	0x00000040	/* RAS to CAS delay, 2 */
#define DRAM_RCD_1	0x00000020	/* RAS to CAS delay, 1 */
#define DRAM_RCD_0	0x00000000	/* RAS to CAS delay, 0 */
#define DRAM_CS		0x00000010	/* CAS short */
#define DRAM_CL_8_5	0x0000000f	/* CAS length, 8.5 cycles */
#define DRAM_CL_7_5	0x0000000c	/* CAS length, 7.5 cycles*/
#define DRAM_CL_6_5	0x0000000a	/* CAS length, 6.5 cycles */
#define DRAM_CL_5_5	0x00000008	/* CAS length, 5.5 cycles */
#define DRAM_CL_4_5	0x00000006	/* CAS length, 4.5 cycles */
#define DRAM_CL_3_5	0x00000004	/* CAS length, 3.5 cycles*/
#define DRAM_CL_2_5	0x00000002	/* CAS length, 2.5 cycles */
#define DRAM_CL_1_5	0x00000000	/* CAS length, 1.5 cycles */
#define DRAM_DCE	0x00000001	/* DRAM controller enable */

/* Definitions for BIU/cache configuration register bits */

#define BIU_NOSTR	0x00020000	/* no instruction streaming */
#define BIU_LDSCH	0x00010000	/* enable load scheduling */
#define BIU_BGNT	0x00008000	/* enable bus grant */
#define BIU_NOPAD	0x00004000	/* no wait state */
#define BIU_RDPRI	0x00002000	/* enable read priority */
#define BIU_INTP	0x00001000	/* interrupt polarity */
#define BIU_IS1		0x00000800	/* enable Inst cache, set 1 */
#define BIU_IS0		0x00000400	/* enable Inst cache, set 0 */
#define BIU_IBLKSZ_16	0x00000300	/* Inst cache fill sz = 16 words */
#define BIU_IBLKSZ_8	0x00000200	/* Inst cache fill sz = 8 words */
#define BIU_IBLKSZ_4	0x00000100	/* Inst cache fill sz = 4 words */
#define BIU_IBLKSZ_2	0x00000000	/* Inst cache fill sz = 2 words */
#define BIU_DS		0x00000080	/* enable Data cache */
#define BIU_DBLKSZ_16	0x00000030	/* Data cache fill sz = 16 words */
#define BIU_DBLKSZ_8	0x00000020	/* Data cache fill sz = 8 words */
#define BIU_DBLKSZ_4	0x00000010	/* Data cache fill sz = 4 words */
#define BIU_DBLKSZ_2	0x00000000	/* Data cache fill sz = 2 words */
#define BIU_RAM		0x00000008	/* scratchpad RAM */
#define BIU_TAG		0x00000004	/* tag test mode */
#define BIU_INV		0x00000002	/* invalidate mode */
#define BIU_LOCK	0x00000001	/* lock mode */

/* Definitions for cache sizes */

#define LR33300_IC_SIZE	0x1000		/* 33300 Inst cache = 4Kbytes */
#define LR33310_IC_SIZE	0x1000		/* 33310 Inst cache = 4Kbytes */
					/*     Note: each set is 4Kbytes! */

#define LR33300_DC_SIZE	0x800		/* 33300 Data cache = 2Kbytes */
#define LR33310_DC_SIZE	0x1000		/* 33310 Data cache = 4Kbytes */

#ifdef __cplusplus
}
#endif

#endif /* __INClr333x0h */
