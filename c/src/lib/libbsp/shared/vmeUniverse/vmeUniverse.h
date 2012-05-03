#ifndef VME_UNIVERSE_UTIL_H
#define VME_UNIVERSE_UTIL_H

/* Driver for the Tundra Universe II pci-vme bridge */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2000-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

/* Register definitions */
/* NOTE: all registers contents in PCI space are LITTLE ENDIAN */

#ifdef __vxworks
#include <vme.h>
#else

#include <bsp/vme_am_defs.h>

#endif

/* These bits can be or'ed with the address-modifier when calling
 * the 'XlateAddr' routine below to further qualify the
 * search criteria.
 */
#define VME_MODE_MATCH_MASK					(3<<30)
#define VME_MODE_EXACT_MATCH				(2<<30)	/* all bits must match */
#define VME_MODE_AS_MATCH					(1<<30)	/* only A16/24/32 must match */


typedef unsigned long LERegister; /* emphasize contents are little endian */

/****** NOTE: USE OF VmeUniverseDMAPacket IS DEPRECATED *********
 ******       USE API IN VMEDMA.h INSTEAD               *********/

/* NOTE: DMA packet descriptors MUST be 32 byte aligned */
typedef struct VmeUniverseDMAPacketRec_ {
	LERegister	dctl	__attribute__((aligned(32)));
	LERegister	dtbc	__attribute__((packed));
	LERegister	dla		__attribute__((packed));
	LERegister	dummy1	__attribute__((packed));
	LERegister	dva		__attribute__((packed));
	LERegister	dummy2	__attribute__((packed));
	LERegister	dcpp	__attribute__((packed));
	LERegister	dummy3	__attribute__((packed));
} VmeUniverseDMAPacketRec, *VmeUniverseDMAPacket; /* DEPRECATED */

/* PCI CSR register */
#define		UNIV_REGOFF_PCI_CSR		0x4
# define	UNIV_PCI_CSR_D_PE		(1<<31)	/* detected parity error; write 1 to clear */
# define	UNIV_PCI_CSR_S_SERR		(1<<30)	/* SERR (signalled error) asserted; write 1 to clear */
# define	UNIV_PCI_CSR_R_MA		(1<<29)	/* received master abort; write 1 to clear */
# define	UNIV_PCI_CSR_R_TA		(1<<28)	/* received target abort; write 1 to clear */
# define	UNIV_PCI_CSR_S_TA		(1<<27)	/* signalled target abort; write 1 to clear */
# define	UNIV_PCI_CSR_DEVSEL_MASK (3<<25)	/* device select timing (RO) */
# define	UNIV_PCI_CSR_DP_D		(1<<24)	/* data parity error detected; write 1 to clear */
# define	UNIV_PCI_CSR_TFBBC		(1<<23)	/* target fast back to back capable (RO) */
# define	UNIV_PCI_CSR_MFBBC		(1<<9)	/* master fast back to back capable (RO) */
# define	UNIV_PCI_CSR_SERR_EN	(1<<8)	/* enable SERR driver */
# define	UNIV_PCI_CSR_WAIT		(1<<7)	/* wait cycle control (RO) */
# define	UNIV_PCI_CSR_PERESP		(1<<6)	/* parity error response enable */
# define	UNIV_PCI_CSR_VGAPS		(1<<5)	/* VGA palette snoop (RO) */
# define	UNIV_PCI_CSR_MWI_EN		(1<<4)	/* Memory write and invalidate enable (RO) */
# define	UNIV_PCI_CSR_SC			(1<<3)	/* special cycles (RO) */
# define	UNIV_PCI_CSR_BM			(1<<2)	/* master enable (MUST SET TO ENABLE VME SLAVES) */
# define	UNIV_PCI_CSR_MS			(1<<1)	/* target memory enable */
# define	UNIV_PCI_CSR_IOS		(1<<0)	/* target IO enable */

/* Special cycle (ADOH, RMW) control register */
#define		UNIV_REGOFF_SCYC_CTL	0x170	/* write 0 to disable */
# define	UNIV_SCYC_CTL_LAS_IO	(1<<2)	/* PCI address space (1: IO, 0: mem) */
# define	UNIV_SCYC_CTL_SCYC_RMW	(1<<0)	/* do a RMW cycle when reading  PCI address */
# define	UNIV_SCYC_CTL_SCYC_ADOH	(2<<0)	/* do a ADOH cycle when reading/writing  PCI address */

/* Special cycle address register */
#define		UNIV_REGOFF_SCYC_ADDR	0x174	/* PCI address (must be long word aligned) */

/* Special cycle Swap/Compare/Enable */
#define		UNIV_REGOFF_SCYC_EN	0x178	/* mask determining the bits involved in the compare and swap operations for VME RMW cycles */

/* Special cycle compare data register */
#define		UNIV_REGOFF_SCYC_CMP	0x17c	/* data to compare with word returned from VME RMW read */

/* Special cycle swap data register */
#define		UNIV_REGOFF_SCYC_SWP	0x180	/* If enabled bits of CMP match, corresponding SWP bits are written back to VME (under control of EN) */

/* PCI miscellaneous register */
#define		UNIV_REGOFF_LMISC	0x184
# define	UNIV_LMISC_CRT_MASK	(7<<28)	/* Univ. I only, not used on II */
# define	UNIV_LMISC_CRT_INF	(0<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_128_US	(1<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_256_US	(2<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_512_US	(3<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_1024_US	(4<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_2048_US	(5<<28)	/* Coupled Request Timeout */
# define	UNIV_LMISC_CRT_4096_US	(6<<28)	/* Coupled Request Timeout */

# define	UNIV_LMISC_CWT_MASK	(7<<24)	/* coupled window timer */
# define	UNIV_LMISC_CWT_DISABLE	0	/* disabled (release VME after 1 coupled xaction) */
# define	UNIV_LMISC_CWT_16	(1<<24)	/* 16 PCI clock cycles */
# define	UNIV_LMISC_CWT_32	(2<<24)	/* 32 PCI clock cycles */
# define	UNIV_LMISC_CWT_64	(3<<24)	/* 64 PCI clock cycles */
# define	UNIV_LMISC_CWT_128	(4<<24)	/* 128 PCI clock cycles */
# define	UNIV_LMISC_CWT_256	(5<<24)	/* 256 PCI clock cycles */
# define	UNIV_LMISC_CWT_512	(6<<24)	/* 512 PCI clock cycles */

/* PCI Command Error Log Register */
#define		UNIV_REGOFF_L_CMDERR	0x18c
# define	UNIV_L_CMDERR_CMDERR(reg) (((reg)>>28)&0xf) /* extract PCI cmd error log */
# define	UNIV_L_CMDERR_M_ERR	(1<<27)	/* multiple errors have occurred */
# define	UNIV_L_CMDERR_L_STAT	(1<<23)	/* PCI error log status valid (write 1 to clear and enable logging) */

/* PCI Address Error Log */
#define		UNIV_REGOFF_LAERR	0x190	/* PCI fault address (if L_CMDERR_L_STAT valid) */
/* DMA Xfer Control Register */
#define		UNIV_REGOFF_DCTL	0x200
# define	UNIV_DCTL_L2V		(1<<31)	/* PCI->VME if set */
# define	UNIV_DCTL_VDW_MSK	(3<<22)	/* VME max. width mask 0x00c00000 */
# define	UNIV_DCTL_VDW_8		(0<<22)	/* VME max. width 8 */
# define	UNIV_DCTL_VDW_16	(1<<22)	/* VME max. width 16 */
# define	UNIV_DCTL_VDW_32	(2<<22)	/* VME max. width 32 */
# define	UNIV_DCTL_VDW_64	(3<<22)	/* VME max. width 64 */
# define	UNIV_DCTL_VAS_MSK	(7<<16)	/* VME AS mask 0x00070000 */
# define	UNIV_DCTL_VAS_A16	(0<<16)	/* VME A16 */
# define	UNIV_DCTL_VAS_A24	(1<<16)	/* VME A24 */
# define	UNIV_DCTL_VAS_A32	(2<<16)	/* VME A32 */
# define	UNIV_DCTL_PGM_MSK	(3<<14)	/* VME PGM/DATA mask 0x0000c000 */
# define	UNIV_DCTL_PGM		(1<<14)	/* VME PGM(1)/DATA(0) */
# define	UNIV_DCTL_SUPER_MSK	(3<<12)	/* VME SUPER/USR mask 0x00003000 */
# define	UNIV_DCTL_SUPER		(1<<12)	/* VME SUPER(1)/USR(0) */
# define	UNIV_DCTL_NO_VINC	(1<<9)	/* VME no VME address increment [Universe IIa/b ONLY */
# define	UNIV_DCTL_VCT		(1<<8)	/* VME enable BLT */
# define	UNIV_DCTL_LD64EN	(1<<7)	/* PCI 64 enable  */

/* DMA Xfer byte count register (is updated by DMA) */
#define		UNIV_REGOFF_DTBC	0x204
/* DMA Xfer local (PCI) address (direction is  set in DCTL) */
#define		UNIV_REGOFF_DLA		0x208
/* DMA Xfer VME address (direction is  set in DCTL)
 * NOTE: (*UNIV_DVA) & ~7 == (*UNIV_DLA) & ~7 MUST HOLD
 */
#define		UNIV_REGOFF_DVA		0x210

/* DMA Xfer VME command packet pointer
 * NOTE: The address stored here MUST be 32-byte aligned
 */
#define		UNIV_REGOFF_DCPP	0x218
/* these bits are only used in linked lists */
# define	UNIV_DCPP_IMG_NULL	(1<<0)	/* last packet in list */
# define	UNIV_DCPP_IMG_PROCESSED (1<<1)	/* packet processed */

/* DMA Xfer General Control/Status register */
#define		UNIV_REGOFF_DGCS	0x220
# define	UNIV_DGCS_GO		(1<<31)	/* start xfer */
# define	UNIV_DGCS_STOP_REQ	(1<<30) /* stop xfer (immediate abort) */
# define	UNIV_DGCS_HALT_REQ	(1<<29) /* halt xfer (abort after current packet) */
# define	UNIV_DGCS_CHAIN		(1<<27) /* enable linked list mode */
# define	UNIV_DGCS_VON_MSK	(7<<20) /* VON mask */
# define	UNIV_DGCS_VON_DONE	(0<<20) /* VON counter disabled (do until done) */
# define	UNIV_DGCS_VON_256	(1<<20) /* VON yield bus after 256 bytes */
# define	UNIV_DGCS_VON_512	(2<<20) /* VON yield bus after 512 bytes */
# define	UNIV_DGCS_VON_1024	(3<<20) /* VON yield bus after 1024 bytes */
# define	UNIV_DGCS_VON_2048	(4<<20) /* VON yield bus after 2048 bytes */
# define	UNIV_DGCS_VON_4096	(5<<20) /* VON yield bus after 4096 bytes */
# define	UNIV_DGCS_VON_8192	(6<<20) /* VON yield bus after 8192 bytes */
# define	UNIV_DGCS_VON_16384	(7<<20) /* VON yield bus after 16384 bytes */
# define	UNIV_DGCS_VOFF_MSK	(15<<16) /* VOFF mask */
# define	UNIV_DGCS_VOFF_0_US	(0<<16)	/* re-request VME master after 0 us */
# define	UNIV_DGCS_VOFF_2_US	(8<<16)	/* re-request VME master after 2 us */
# define	UNIV_DGCS_VOFF_4_US	(9<<16)	/* re-request VME master after 4 us */
# define	UNIV_DGCS_VOFF_8_US	(10<<16)/* re-request VME master after 8 us */
# define	UNIV_DGCS_VOFF_16_US	(1<<16)	/* re-request VME master after 16 us */
# define	UNIV_DGCS_VOFF_32_US	(2<<16)	/* re-request VME master after 32 us */
# define	UNIV_DGCS_VOFF_64_US	(3<<16)	/* re-request VME master after 64 us */
# define	UNIV_DGCS_VOFF_128_US	(4<<16)	/* re-request VME master after 128 us */
# define	UNIV_DGCS_VOFF_256_US	(5<<16)	/* re-request VME master after 256 us */
# define	UNIV_DGCS_VOFF_512_US	(6<<16)	/* re-request VME master after 512 us */
# define	UNIV_DGCS_VOFF_1024_US	(7<<16)	/* re-request VME master after 1024 us */
/* Status Bits (write 1 to clear) */
# define	UNIV_DGCS_ACT		(1<<15)	/* DMA active */
# define	UNIV_DGCS_STOP		(1<<14)	/* DMA stopped */
# define	UNIV_DGCS_HALT		(1<<13)	/* DMA halted */
# define	UNIV_DGCS_DONE		(1<<11)	/* DMA done (OK) */
# define	UNIV_DGCS_LERR		(1<<10)	/* PCI bus error */
# define	UNIV_DGCS_VERR		(1<<9)	/* VME bus error */
# define	UNIV_DGCS_P_ERR		(1<<8)	/* programming protocol error (e.g. PCI master disabled) */
# define	UNIV_DGCS_STATUS_CLEAR\
	(UNIV_DGCS_ACT|UNIV_DGCS_STOP|UNIV_DGCS_HALT|\
	 UNIV_DGCS_DONE|UNIV_DGCS_LERR|UNIV_DGCS_VERR|UNIV_DGCS_P_ERR)
# define	UNIV_DGCS_P_ERR		(1<<8)	/* programming protocol error (e.g. PCI master disabled) */
/* Interrupt Mask Bits */
# define	UNIV_DGCS_INT_STOP	(1<<6)	/* interrupt when stopped */
# define	UNIV_DGCS_INT_HALT	(1<<5)  /* interrupt when halted */
# define	UNIV_DGCS_INT_DONE	(1<<3)	/* interrupt when done */
# define	UNIV_DGCS_INT_LERR	(1<<2)	/* interrupt on LERR */
# define	UNIV_DGCS_INT_VERR	(1<<1)	/* interrupt on VERR */
# define	UNIV_DGCS_INT_P_ERR	(1<<0)	/* interrupt on P_ERR */
# define	UNIV_DGCS_INT_MSK	(0x0000006f) /* interrupt mask */

/* DMA Linked List Update Enable Register */
#define		UNIV_REGOFF_D_LLUE	0x224
# define	UNIV_D_LLUE_UPDATE	(1<<31)


/* PCI (local) interrupt enable register */
#define		UNIV_REGOFF_LINT_EN	0x300
# define	UNIV_LINT_EN_LM3	(1<<23)	/* location monitor 3 mask */
# define	UNIV_LINT_EN_LM2	(1<<22)	/* location monitor 2 mask */
# define	UNIV_LINT_EN_LM1	(1<<21)	/* location monitor 1 mask */
# define	UNIV_LINT_EN_LM0	(1<<20)	/* location monitor 0 mask */
# define	UNIV_LINT_EN_MBOX3	(1<<19)	/* mailbox 3 mask */
# define	UNIV_LINT_EN_MBOX2	(1<<18)	/* mailbox 2 mask */
# define	UNIV_LINT_EN_MBOX1	(1<<17)	/* mailbox 1 mask */
# define	UNIV_LINT_EN_MBOX0	(1<<16)	/* mailbox 0 mask */
# define 	UNIV_LINT_EN_ACFAIL	(1<<15)	/* ACFAIL irq mask */
# define 	UNIV_LINT_EN_SYSFAIL	(1<<14)	/* SYSFAIL irq mask */
# define 	UNIV_LINT_EN_SW_INT	(1<<13)	/* PCI (local) software irq */
# define 	UNIV_LINT_EN_SW_IACK	(1<<12)	/* VME software IACK mask */
# define	UNIV_LINT_EN_VERR	(1<<10)	/* PCI VERR irq mask */
# define	UNIV_LINT_EN_LERR	(1<<9)	/* PCI LERR irq mask */
# define	UNIV_LINT_EN_DMA	(1<<8)	/* PCI DMA irq mask */
# define	UNIV_LINT_EN_VIRQ7	(1<<7)	/* VIRQ7 mask (universe does IACK automatically) */
# define	UNIV_LINT_EN_VIRQ6	(1<<6)	/* VIRQ6 mask */
# define	UNIV_LINT_EN_VIRQ5	(1<<5)	/* VIRQ5 mask */
# define	UNIV_LINT_EN_VIRQ4	(1<<4)	/* VIRQ4 mask */
# define	UNIV_LINT_EN_VIRQ3	(1<<3)	/* VIRQ3 mask */
# define	UNIV_LINT_EN_VIRQ2	(1<<2)	/* VIRQ2 mask */
# define	UNIV_LINT_EN_VIRQ1	(1<<1)	/* VIRQ1 mask */
# define	UNIV_LINT_EN_VOWN	(1<<0)	/* VOWN mask */

/* PCI (local) interrupt status register */
#define		UNIV_REGOFF_LINT_STAT	0x304
# define	UNIV_LINT_STAT_LM3	(1<<23)	/* location monitor 3 status */
# define	UNIV_LINT_STAT_LM2	(1<<22)	/* location monitor 2 status */
# define	UNIV_LINT_STAT_LM1	(1<<21)	/* location monitor 1 status */
# define	UNIV_LINT_STAT_LM0	(1<<20)	/* location monitor 0 status */
# define	UNIV_LINT_STAT_MBOX3	(1<<19)	/* mailbox 3 status */
# define	UNIV_LINT_STAT_MBOX2	(1<<18)	/* mailbox 2 status */
# define	UNIV_LINT_STAT_MBOX1	(1<<17)	/* mailbox 1 status */
# define	UNIV_LINT_STAT_MBOX0	(1<<16)	/* mailbox 0 status */
# define 	UNIV_LINT_STAT_ACFAIL	(1<<15)	/* ACFAIL irq status */
# define 	UNIV_LINT_STAT_SYSFAIL	(1<<14)	/* SYSFAIL irq status */
# define 	UNIV_LINT_STAT_SW_INT	(1<<13)	/* PCI (local) software irq */
# define 	UNIV_LINT_STAT_SW_IACK	(1<<12)	/* VME software IACK status */
# define	UNIV_LINT_STAT_VERR		(1<<10)	/* PCI VERR irq status */
# define	UNIV_LINT_STAT_LERR		(1<<9)	/* PCI LERR irq status */
# define	UNIV_LINT_STAT_DMA		(1<<8)	/* PCI DMA irq status */
# define	UNIV_LINT_STAT_VIRQ7	(1<<7)	/* VIRQ7 status */
# define	UNIV_LINT_STAT_VIRQ6	(1<<6)	/* VIRQ6 status */
# define	UNIV_LINT_STAT_VIRQ5	(1<<5)	/* VIRQ5 status */
# define	UNIV_LINT_STAT_VIRQ4	(1<<4)	/* VIRQ4 status */
# define	UNIV_LINT_STAT_VIRQ3	(1<<3)	/* VIRQ3 status */
# define	UNIV_LINT_STAT_VIRQ2	(1<<2)	/* VIRQ2 status */
# define	UNIV_LINT_STAT_VIRQ1	(1<<1)	/* VIRQ1 status */
# define	UNIV_LINT_STAT_VOWN		(1<<0)	/* VOWN status */
# define	UNIV_LINT_STAT_CLR		(0xfff7ff)/* Clear all status bits */

/* PCI (local) interrupt map 0 register */
#define		UNIV_REGOFF_LINT_MAP0	0x308	/* mapping of VME IRQ sources to PCI irqs */
# define	UNIV_LINT_MAP0_VIRQ7(lint)	(((lint)&0x7)<<(7*4))
# define	UNIV_LINT_MAP0_VIRQ6(lint)	(((lint)&0x7)<<(6*4))
# define	UNIV_LINT_MAP0_VIRQ5(lint)	(((lint)&0x7)<<(5*4))
# define	UNIV_LINT_MAP0_VIRQ4(lint)	(((lint)&0x7)<<(4*4))
# define	UNIV_LINT_MAP0_VIRQ3(lint)	(((lint)&0x7)<<(3*4))
# define	UNIV_LINT_MAP0_VIRQ2(lint)	(((lint)&0x7)<<(2*4))
# define	UNIV_LINT_MAP0_VIRQ1(lint)	(((lint)&0x7)<<(1*4))
# define	UNIV_LINT_MAP0_VOWN(lint)	(((lint)&0x7)<<(0*4))

#define		UNIV_REGOFF_LINT_MAP1	0x30c	/* mapping of internal / VME IRQ sources to PCI irqs */
# define	UNIV_LINT_MAP1_ACFAIL(lint)	(((lint)&0x7)<<(7*4))
# define	UNIV_LINT_MAP1_SYSFAIL(lint)	(((lint)&0x7)<<(6*4))
# define	UNIV_LINT_MAP1_SW_INT(lint)	(((lint)&0x7)<<(5*4))
# define	UNIV_LINT_MAP1_SW_IACK(lint)	(((lint)&0x7)<<(4*4))
# define	UNIV_LINT_MAP1_VERR(lint)	(((lint)&0x7)<<(2*4))
# define	UNIV_LINT_MAP1_LERR(lint)	(((lint)&0x7)<<(1*4))
# define	UNIV_LINT_MAP1_DMA(lint)	(((lint)&0x7)<<(0*4))

/* enabling of generation of VME bus IRQs, TODO */
#define		UNIV_REGOFF_VINT_EN		0x310
# define	UNIV_VINT_EN_DISABLE_ALL    0
# define	UNIV_VINT_EN_SWINT			(1<<12)
# define	UNIV_VINT_EN_SWINT_LVL(l)	(1<<(((l)&7)+24))	/* universe II only */


/* status of generation of VME bus IRQs */
#define		UNIV_REGOFF_VINT_STAT	0x314
# define	UNIV_VINT_STAT_LINT(lint)	(1<<((lint)&7))
# define	UNIV_VINT_STAT_LINT_MASK	(0xff)
# define	UNIV_VINT_STAT_CLR			(0xfe0f17ff)
# define	UNIV_VINT_STAT_SWINT(l)	    (1<<(((l)&7)+24))

#define		UNIV_REGOFF_VINT_MAP0	0x318	/* VME destination of PCI IRQ source, TODO */

#define		UNIV_REGOFF_VINT_MAP1	0x31c	/* VME destination of PCI IRQ source, TODO */
# define	UNIV_VINT_MAP1_SWINT(level)	(((level)&0x7)<<16)

/* NOTE: The universe seems to always set LSB (which has a special purpose in
 *       the STATID register: enable raising a SW_INT on IACK) on the
 *		 vector it puts out on the bus...
 */
#define		UNIV_REGOFF_VINT_STATID	0x320	/* our status/id response to IACK, TODO */
# define	UNIV_VINT_STATID(id)	    ((id)<<24)

#define		UNIV_REGOFF_VIRQ1_STATID 0x324	/* status/id of VME IRQ level 1 */
#define		UNIV_REGOFF_VIRQ2_STATID 0x328	/* status/id of VME IRQ level 2 */
#define		UNIV_REGOFF_VIRQ3_STATID 0x32c	/* status/id of VME IRQ level 3 */
#define		UNIV_REGOFF_VIRQ4_STATID 0x330	/* status/id of VME IRQ level 4 */
#define		UNIV_REGOFF_VIRQ5_STATID 0x334	/* status/id of VME IRQ level 5 */
#define		UNIV_REGOFF_VIRQ6_STATID 0x338	/* status/id of VME IRQ level 6 */
#define		UNIV_REGOFF_VIRQ7_STATID 0x33c	/* status/id of VME IRQ level 7 */
# define	UNIV_VIRQ_ERR			(1<<8)	/* set if universe encountered a bus error when doing IACK */
# define	UNIV_VIRQ_STATID_MASK		(0xff)

#define		UNIV_REGOFF_LINT_MAP2	0x340	/* mapping of internal sources to PCI irqs */
# define	UNIV_LINT_MAP2_LM3(lint)	(((lint)&0x7)<<7*4)	/* location monitor 3 */
# define	UNIV_LINT_MAP2_LM2(lint)	(((lint)&0x7)<<6*4)	/* location monitor 2 */
# define	UNIV_LINT_MAP2_LM1(lint)	(((lint)&0x7)<<5*4)	/* location monitor 1 */
# define	UNIV_LINT_MAP2_LM0(lint)	(((lint)&0x7)<<4*4)	/* location monitor 0 */
# define	UNIV_LINT_MAP2_MBOX3(lint)	(((lint)&0x7)<<3*4)	/* mailbox 3 */
# define	UNIV_LINT_MAP2_MBOX2(lint)	(((lint)&0x7)<<2*4)	/* mailbox 2 */
# define	UNIV_LINT_MAP2_MBOX1(lint)	(((lint)&0x7)<<1*4)	/* mailbox 1 */
# define	UNIV_LINT_MAP2_MBOX0(lint)	(((lint)&0x7)<<0*4)	/* mailbox 0 */

#define		UNIV_REGOFF_VINT_MAP2	0x344	/* mapping of internal sources to VME irqs */
# define	UNIV_VINT_MAP2_MBOX3(vint)	(((vint)&0x7)<<3*4)	/* mailbox 3 */
# define	UNIV_VINT_MAP2_MBOX2(vint)	(((vint)&0x7)<<2*4)	/* mailbox 2 */
# define	UNIV_VINT_MAP2_MBOX1(vint)	(((vint)&0x7)<<1*4)	/* mailbox 1 */
# define	UNIV_VINT_MAP2_MBOX0(vint)	(((vint)&0x7)<<0*4)	/* mailbox 0 */

#define		UNIV_REGOFF_MBOX0	0x348	/* mailbox 0 */
#define		UNIV_REGOFF_MBOX1	0x34c	/* mailbox 1 */
#define		UNIV_REGOFF_MBOX2	0x350	/* mailbox 2 */
#define		UNIV_REGOFF_MBOX3	0x354	/* mailbox 3 */

#define		UNIV_REGOFF_SEMA0	0x358	/* semaphore 0 */
#define		UNIV_REGOFF_SEMA1	0x35c	/* semaphore 0 */
/* TODO define semaphore register bits */

#define		UNIV_REGOFF_MAST_CTL	0x400	/* master control register */
# define	UNIV_MAST_CTL_MAXRTRY(val)	(((val)&0xf)<<7*4)	/* max # of pci master retries */
# define	UNIV_MAST_CTL_PWON(val)		(((val)&0xf)<<6*4)	/* posted write xfer count */
# define	UNIV_MAST_CTL_VRL(val)		(((val)&0x3)<<22)	/* VME bus request level */
# define	UNIV_MAST_CTL_VRM			(1<<21)	/* bus request mode (demand = 0, fair = 1) */
# define	UNIV_MAST_CTL_VREL			(1<<20)	/* bus release mode (when done = 0, on request = 1) */
# define	UNIV_MAST_CTL_VOWN			(1<<19)	/* bus ownership (release = 0, acquire/hold = 1) */
# define	UNIV_MAST_CTL_VOWN_ACK		(1<<18)	/* bus ownership (not owned = 0, acquired/held = 1) */
# define	UNIV_MAST_CTL_PABS(val)		(((val)&0x3)<<3*4)	/* PCI aligned burst size (32,64,128 byte / 0x3 is reserved) */
# define	UNIV_MAST_CTL_BUS_NO(val)	(((val)&0xff)<<0*4)	/* PCI bus number */

#define		UNIV_REGOFF_MISC_CTL	0x404	/* misc control register */
# define	UNIV_MISC_CTL_VBTO(val)		(((val)&0x7)<<7*4)	/* VME bus timeout (0=disable, 16*2^(val-1) us) */
# define	UNIV_MISC_CTL_VARB			(1<<26)	/* VME bus arbitration mode (0=round robin, 1= priority) */
# define	UNIV_MISC_CTL_VARBTO(val)	(((val)&0x3)<<6*4)	/* arbitration time out: disable, 16us, 256us, reserved */
# define	UNIV_MISC_CTL_SW_LRST		(1<<23)	/* software PCI reset */
# define	UNIV_MISC_CTL_SW_SYSRST		(1<<22)	/* software VME reset */
# define	UNIV_MISC_CTL_BI			(1<<20)	/* BI mode */
# define	UNIV_MISC_CTL_ENGBI			(1<<19)	/* enable global BI mode initiator */
# define	UNIV_MISC_CTL_SYSCON		(1<<17)	/* (R/W) 1:universe is system controller */
# define	UNIV_MISC_CTL_V64AUTO		(1<<16)	/* (R/W) 1:initiate VME64 auto id slave participation */

/* U2SPEC described in VGM manual */
/* NOTE: the Joerger vtr10012_8 needs the timing to be tweaked!!!! READt27 must be _no_delay_
 */
#define		UNIV_REGOFF_U2SPEC		0x4fc
# define	UNIV_U2SPEC_DTKFLTR			(1<<12)	/* DTAck filter: 0: slow, better filter; 1: fast, poorer filter */
# define	UNIV_U2SPEC_MASt11			(1<<10)	/* Master parameter t11 (DS hi time during BLT and MBLTs) */
# define	UNIV_U2SPEC_READt27_DEFAULT	(0<<8)	/* VME master parameter t27: (latch data after DTAck + 25ns) */
# define	UNIV_U2SPEC_READt27_FAST	(1<<8)	/* VME master parameter t27: (latch data faster than 25ns)  */
# define	UNIV_U2SPEC_READt27_NODELAY	(2<<8)	/* VME master parameter t27: (latch data without any delay)  */
# define	UNIV_U2SPEC_POSt28_FAST		(1<<2)	/* VME slave parameter t28: (faster time of DS to DTAck for posted write) */
# define	UNIV_U2SPEC_PREt28_FAST		(1<<0)	/* VME slave parameter t28: (faster time of DS to DTAck for prefetch read) */

/* Location Monitor control register */
#define		UNIV_REGOFF_LM_CTL		0xf64
# define	UNIV_LM_CTL_EN				(1<<31)	/* image enable */
# define	UNIV_LM_CTL_PGM				(1<<23)	/* program AM */
# define	UNIV_LM_CTL_DATA			(1<<22)	/* data AM */
# define	UNIV_LM_CTL_SUPER			(1<<21)	/* supervisor AM */
# define	UNIV_LM_CTL_USER			(1<<20)	/* user AM */
# define	UNIV_LM_CTL_VAS_A16			(0<<16)	/* A16 */
# define	UNIV_LM_CTL_VAS_A24			(1<<16)	/* A16 */
# define	UNIV_LM_CTL_VAS_A32			(2<<16)	/* A16 */

/* Location Monitor base address */
#define		UNIV_REGOFF_LM_BS		0xf68

/* VMEbus register access image control register */
#define		UNIV_REGOFF_VRAI_CTL	0xf70
# define	UNIV_VRAI_CTL_EN			(1<<31)	/* image enable */
# define	UNIV_VRAI_CTL_PGM			(1<<23)	/* program AM */
# define	UNIV_VRAI_CTL_DATA			(1<<22)	/* data AM */
# define	UNIV_VRAI_CTL_SUPER			(1<<21)	/* supervisor AM */
# define	UNIV_VRAI_CTL_USER			(1<<20)	/* user AM */
# define	UNIV_VRAI_CTL_VAS_A16		(0<<16)	/* A16 */
# define	UNIV_VRAI_CTL_VAS_A24		(1<<16)	/* A14 */
# define	UNIV_VRAI_CTL_VAS_A32		(2<<16)	/* A32 */
# define	UNIV_VRAI_CTL_VAS_MSK		(3<<16)

/* VMEbus register acces image base address register */
#define		UNIV_REGOFF_VRAI_BS		0xf74

/* VMEbus CSR control register */
#define		UNIV_REGOFF_VCSR_CTL	0xf80
# define	UNIV_VCSR_CTL_EN			(1<<31)	/* image enable */
# define	UNIV_VCSR_CTL_LAS_PCI_MEM	(0<<0)	/* pci mem space */
# define	UNIV_VCSR_CTL_LAS_PCI_IO	(1<<0)	/* pci IO space */
# define	UNIV_VCSR_CTL_LAS_PCI_CFG	(2<<0)	/* pci config space */

/* VMEbus CSR translation offset */
#define		UNIV_REGOFF_VCSR_TO		0xf84

/* VMEbus AM code error log */
#define		UNIV_REGOFF_V_AMERR		0xf88
# define	UNIV_V_AMERR_AMERR(reg)		(((reg)>>26)&0x3f)	/* extract error log code */
# define	UNIV_V_AMERR_IACK			(1<<25)	/* VMEbus IACK signal */
# define	UNIV_V_AMERR_M_ERR			(1<<24)	/* multiple errors occurred */
# define	UNIV_V_AMERR_V_STAT			(1<<23)	/* log status valid (write 1 to clear) */

/* VMEbus address error log */
#define		UNIV_REGOFF_VAERR		0xf8c		/* address of fault address (if MERR_V_STAT valid) */

/* VMEbus CSR bit clear register */
#define		UNIV_REGOFF_VCSR_CLR	0xff4
# define	UNIV_VCSR_CLR_RESET			(1<<31)	/* read/negate LRST (can only be written from VME bus */
# define	UNIV_VCSR_CLR_SYSFAIL		(1<<30)	/* read/negate SYSFAIL */
# define	UNIV_VCSR_CLR_FAIL			(1<<29)	/* read: board has failed */

/* VMEbus CSR bit set register */
#define		UNIV_REGOFF_VCSR_SET		(0xff8)
# define	UNIV_VCSR_SET_RESET			(1<<31)	/* read/assert LRST (can only be written from VME bus */
# define	UNIV_VCSR_SET_SYSFAIL		(1<<30)	/* read/assert SYSFAIL */
# define	UNIV_VCSR_SET_FAIL			(1<<29)	/* read: board has failed */

/* VMEbus CSR base address register */
#define		UNIV_REGOFF_VCSR_BS		0xffc
#define		UNIV_VCSR_BS_MASK			(0xf8000000)

/* offset of universe registers in VME-CSR slot */
#define		UNIV_CSR_OFFSET				0x7f000

#ifdef __cplusplus
extern "C" {
#endif

/* base address and IRQ line of 1st universe bridge
 * NOTE: vmeUniverseInit() must be called before
 *       these may be used.
 */
extern volatile LERegister *vmeUniverse0BaseAddr;
extern int vmeUniverse0PciIrqLine;


/* Initialize the driver */
int
vmeUniverseInit(void);

/* setup the universe chip, i.e. disable most of its
 * mappings, reset interrupts etc.
 */
void
vmeUniverseReset(void);

/* avoid pulling stdio.h into this header.
 * Applications that want a declaration of the
 * following routines should
 *  #include <stdio.h>
 *  #define _VME_UNIVERSE_DECLARE_SHOW_ROUTINES
 *  #include <vmeUniverse.h>
 */
/* print the current configuration of all master ports to
 * f (stderr if NULL)
 */
void
vmeUniverseMasterPortsShow(FILE *f);

/* print the current configuration of all slave ports to
 * f (stderr if NULL)
 */
void
vmeUniverseSlavePortsShow(FILE *f);

/* disable all master or slave ports, respectively */
void
vmeUniverseDisableAllMasters(void);

void
vmeUniverseDisableAllSlaves(void);

/* configure a master port
 *
 *   port:	    port number 0..3  (0..7 for a UniverseII)
 *
 *   address_space: vxWorks compliant addressing mode identifier
 *                  (see vme.h). The most important are:
 *                    0x0d - A32, Sup, Data
 *                    0x3d - A24, Sup, Data
 *                    0x2d - A16, Sup, Data
 *                  additionally, the value 0 is accepted; it will
 *                  disable this port.
 *   vme_address:   address on the vme_bus of this port.
 *   local_address: address on the pci_bus of this port.
 *   length:        size of this port.
 *
 *   NOTE: the addresses and length parameters must be aligned on a
 *         2^16 byte (0x10000) boundary, except for port 4 (only available
 *         on a UniverseII), where the alignment can be 4k (4096).
 *
 *   RETURNS: 0 on success, -1 on failure. Error messages printed to stderr.
 */

int
vmeUniverseMasterPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length);

/* translate an address through the bridge
 *
 * vmeUniverseXlateAddr(0,0,as,addr,&result)
 * yields a VME a address that reflects
 * a local memory location as seen from the VME bus through the universe
 * VME slave.
 *
 * likewise does vmeUniverseXlateAddr(1,0,as,addr,&result)
 * translate a VME bus addr (through the VME master) to the
 * PCI side of the bridge.
 *
 * a valid address space modifier must be specified.
 *
 * The 'reverse' parameter may be used to find a reverse
 * mapping, i.e. the pci address in a master window can be
 * found if the respective vme address is known etc.
 *
 * RETURNS: translated address in *pbusAdrs / *plocalAdrs
 *
 *          0:  success
 *          -1: address/modifier not found in any bridge port
 *          -2: invalid modifier
 */
int
vmeUniverseXlateAddr(
	int master, 		/* look in the master windows */
	int reverse,		/* reverse mapping; for masters: map local to VME */
	unsigned long as,	/* address space */
	unsigned long addr,	/* address to look up */
	unsigned long *paOut/* where to put result */
	);

/* configure a VME slave (PCI master) port */
int
vmeUniverseSlavePortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length);

/****** NOTE: USE OF vmeUniverseStartDMA IS DEPRECATED      *********
 ******       USE API IN VMEDMA.h/vmeUniverseDMA.h INSTEAD  *********/

/* start a (direct, not linked) DMA transfer
 *
 * NOTE:  DCTL and DGCS must be set up
 *        prior to calling this routine
 */
int
vmeUniverseStartDMA(
	unsigned long local_addr,
	unsigned long vme_addr,
	unsigned long count); /* DEPRECATED */

int
vmeUniverseStartDMAXX(
	volatile LERegister *ubase,
	unsigned long local_addr,
	unsigned long vme_addr,
	unsigned long count); /* DEPRECATED */


/* read a register in PCI memory space
 * (offset being one of the declared constants)
 */
unsigned long
vmeUniverseReadReg(unsigned long offset);

/* write a register in PCI memory space */
void
vmeUniverseWriteReg(unsigned long value, unsigned long offset);

/* convert an array of unsigned long values to LE (as needed
 * when the universe reads e.g. DMA descriptors from PCI)
 */
void
vmeUniverseCvtToLE(unsigned long *ptr, unsigned long num);

/* reset the VME bus */
void
vmeUniverseResetBus(void);

/* The ...XX routines take the universe base address as an additional
 * argument - this allows for programming secondary devices.
 */

unsigned long
vmeUniverseReadRegXX(volatile LERegister *ubase, unsigned long offset);

void
vmeUniverseWriteRegXX(volatile LERegister *ubase, unsigned long value, unsigned long offset);

int
vmeUniverseXlateAddrXX(
	volatile LERegister *ubase,
	int master,
	int reverse,
	unsigned long as,
	unsigned long addr,
	unsigned long *paOut
	);

int
vmeUniverseMasterPortCfgXX(
	volatile LERegister *ubase,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length);

int
vmeUniverseSlavePortCfgXX(
	volatile LERegister *ubase,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length);

void
vmeUniverseDisableAllMastersXX(volatile LERegister *ubase);

void
vmeUniverseDisableAllSlavesXX(volatile LERegister *ubase);

/* print the current configuration of all master ports to
 * f (stderr if NULL)
 */
void
vmeUniverseMasterPortsShowXX(
	volatile LERegister *ubase,FILE *f);

/* print the current configuration of all slave ports to
 * f (stderr if NULL)
 */
void
vmeUniverseSlavePortsShowXX(
	volatile LERegister *ubase,FILE *f);

/* Raise a VME Interrupt at 'level' and respond with 'vector' to a
 * handler on the VME bus. (The handler could be a different board
 * or the universe itself - [only works with universe II]).
 *
 * Note that you could install a interrupt handler at UNIV_VME_SW_IACK_INT_VEC
 * to be notified of an IACK cycle having completed.
 *
 * This routine is mainly FOR TESTING.
 *
 * NOTES:
 *   - several registers are modified: the vector is written to VINT_STATID
 *     and (universe 1 chip only) the level is written to the SW_INT bits
 *     int VINT_MAP1
 *   - NO MUTUAL EXCLUSION PROTECTION (reads VINT_EN, modifies then writes back).
 *     If several users need access to VINT_EN and/or VINT_STATID (and VINT_MAP1
 *     on the universe 1) it is their responsibility to serialize access.
 *
 * Arguments:
 *  'level':  interrupt level, 1..7
 *  'vector': vector number (0..254) that the universe puts on the bus in response to
 *            an IACK cycle. NOTE: the vector number *must be even* (hardware restriction
 *            of the universe -- it always clears the LSB when the interrupter is
 *            a software interrupt).
 *
 * RETURNS:
 *        0:  Success
 *       -1:  Invalid argument (level not 1..7, vector odd or >= 256)
 *       -2:  Interrupt 'level' already asserted (maybe nobody handles it).
 *            You can manually clear it be writing the respective bit in
 *            VINT_STAT. Make sure really nobody responds to avoid spurious
 *            interrupts (consult universe docs).
 */

int
vmeUniverseIntRaiseXX(volatile LERegister *base, int level, unsigned vector);

int
vmeUniverseIntRaise(int level, unsigned vector);

/* Map internal register block to VME.
 *
 * This routine is intended for BSP implementors. The registers can be
 * made accessible from VME so that the interrupt handler can flush the
 * bridge FIFO (see below). The preferred method is by accessing VME CSR,
 * though, if these are mapped [and the BSP provides an outbound window].
 * On the universe we can also disable posted writes in the 'ordinary'
 * outbound windows.
 *
 *            vme_base: VME address where the universe registers (4k) can be mapped.
 *                      This VME address must fall into a range covered by
 *                      any pre-configured outbound window.
 *       address_space: The desired VME address space.
 *                      (all of SUP/USR/PGM/DATA are always accepted).
 *
 * See NOTES [vmeUniverseInstallIrqMgrAlt()] below for further information.
 *
 * RETURNS: 0 on success, nonzero on error. It is not possible (and results
 *          in a non-zero return code) to change the CRG VME address after
 *          initializing the interrupt manager as it uses the CRG.
 */
int
vmeUniverseMapCRGXX(volatile LERegister *base, unsigned long vme_base, unsigned long address_space);

int
vmeUniverseMapCRG(unsigned long vme_base, unsigned long address_space);


#ifdef __rtems__

/* VME Interrupt Handler functionality */

/* we dont use the current RTEMS/BSP interrupt API for the
 * following reasons:
 *
 *    - RTEMS/BSP API does not pass an argument to the ISR :-( :-(
 *    - no separate vector space for VME vectors. Some vectors would
 *      have to overlap with existing PCI/ISA vectors.
 *    - RTEMS/BSP API allocates a structure for every possible vector
 *    - the irq_on(), irq_off() functions add more bloat than helping.
 *      They are (currently) only used by the framework to disable
 *      interrupts at the device level before removing a handler
 *      and to enable interrupts after installing a handler.
 *      These operations may as well be done by the driver itself.
 *
 * Hence, we maintain our own (VME) handler table and hook our PCI
 * handler into the standard RTEMS/BSP environment. Our handler then
 * dispatches VME interrupts.
 */

typedef void (*VmeUniverseISR) (void *usrArg, unsigned long vector);

/* use these special vectors to connect a handler to the
 * universe specific interrupts (such as "DMA done",
 * VOWN, error irqs etc.)
 * NOTE: The wrapper clears all status LINT bits (except
 * for regular VME irqs). Also note that it is the user's
 * responsibility to enable the necessary interrupts in
 * LINT_EN
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * DO NOT CHANGE THE ORDER OF THESE VECTORS - THE DRIVER
 * DEPENDS ON IT
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 */
#define UNIV_VOWN_INT_VEC			256
#define UNIV_DMA_INT_VEC			257
#define UNIV_LERR_INT_VEC			258
#define UNIV_VERR_INT_VEC			259
/* 260 is reserved */
#define UNIV_VME_SW_IACK_INT_VEC	261
#define UNIV_PCI_SW_INT_VEC			262
#define UNIV_SYSFAIL_INT_VEC		263
#define UNIV_ACFAIL_INT_VEC			264
#define UNIV_MBOX0_INT_VEC			265
#define UNIV_MBOX1_INT_VEC			266
#define UNIV_MBOX2_INT_VEC			267
#define UNIV_MBOX3_INT_VEC			268
#define UNIV_LM0_INT_VEC			269
#define UNIV_LM1_INT_VEC			270
#define UNIV_LM2_INT_VEC			271
#define UNIV_LM3_INT_VEC			272

#define UNIV_NUM_INT_VECS			273


/* install a handler for a VME vector
 * RETURNS 0 on success, nonzero on failure.
 */
int
vmeUniverseInstallISR(unsigned long vector, VmeUniverseISR handler, void *usrArg);

/* remove a handler for a VME vector. The vector and usrArg parameters
 * must match the respective parameters used when installing the handler.
 * RETURNS 0 on success, nonzero on failure.
 */
int
vmeUniverseRemoveISR(unsigned long vector, VmeUniverseISR handler, void *usrArg);

/* query for the currently installed ISR and usr parameter at a given vector
 * RETURNS: ISR or 0 (vector too big or no ISR installed)
 */
VmeUniverseISR
vmeUniverseISRGet(unsigned long vector, void **parg);

/* utility routines to enable/disable a VME IRQ level.
 *
 * To enable/disable the internal interrupt sources (special vectors above)
 * pass a vector argument > 255.
 *
 * RETURNS 0 on success, nonzero on failure
 */
int
vmeUniverseIntEnable(unsigned int level);
int
vmeUniverseIntDisable(unsigned int level);

/* Check if an interrupt level or internal source is enabled:
 *
 * 'level': VME level 1..7 or internal special vector > 255
 *
 * RETURNS: value > 0 if interrupt is currently enabled,
 *          zero      if interrupt is currently disabled,
 *          -1        on error (invalid argument).
 */
int
vmeUniverseIntIsEnabled(unsigned int level);


/* Change the routing of IRQ 'level' to 'pin'.
 * If the BSP connects more than one of the eight
 * physical interrupt lines from the universe to
 * the board's PIC then you may change the physical
 * line a given 'level' is using. By default,
 * all 7 VME levels use the first wire (pin==0) and
 * all internal sources use the (optional) second
 * wire (pin==1).
 * This feature is useful if you want to make use of
 * different hardware priorities of the PIC. Let's
 * say you want to give IRQ level 7 the highest priority.
 * You could then give 'pin 0' a higher priority (at the
 * PIC) and 'pin 1' a lower priority and issue.
 *
 *   for ( i=1; i<7; i++ ) vmeUniverseIntRoute(i, 1);
 *
 * PARAMETERS:
 *    'level' : VME interrupt level '1..7' or one of
 *              the internal sources. Pass the internal
 *              source's vector number (>=256).
 *    'pin'   : a value of 0 routes the requested IRQ to
 *              the first line registered with the manager
 *              (vmeIrqUnivOut parameter), a value of 1
 *              routes it to the alternate wire
 *              (specialIrqUnivOut)
 * RETURNS: 0 on success, nonzero on error (invalid arguments)
 *
 * NOTES:	- DONT change the universe 'map' registers
 *            directly. The driver caches routing internally.
 *          - support for the 'specialIrqUnivOut' wire is
 *            board dependent. If the board only provides
 *            a single physical wire from the universe to
 *            the PIC then the feature might not be available.
 */
int
vmeUniverseIntRoute(unsigned int level, unsigned int pin);

/* Loopback test of the VME interrupt subsystem.
 *  - installs ISRs on 'vector' and on UNIV_VME_SW_IACK_INT_VEC
 *  - asserts VME interrupt 'level'
 *  - waits for both interrupts: 'ordinary' VME interrupt of 'level' and
 *    IACK completion interrupt ('special' vector UNIV_VME_SW_IACK_INT_VEC).
 *
 * NOTES:
 *  - make sure no other handler responds to 'level'.
 *  - make sure no ISR is installed on both vectors yet.
 *  - ISRs installed by this routine are removed after completion.
 *  - no concurrent access protection of all involved resources
 *    (levels, vectors and registers  [see vmeUniverseIntRaise()])
 *    is implemented.
 *  - this routine is intended for TESTING (when implementing new BSPs etc.).
 *  - one RTEMS message queue is temporarily used (created/deleted).
 *  - the universe 1 always yields a zero vector (VIRQx_STATID) in response
 *    to a self-generated VME interrupt. As a workaround, the routine
 *    only accepts a zero vector when running on a universe 1.
 *
 * RETURNS:
 *                 0: Success.
 *                -1: Invalid arguments.
 *                 1: Test failed (outstanding interrupts).
 * rtems_status_code: Failed RTEMS directive.
 */
int
vmeUniverseIntLoopbackTst(int level, unsigned vector);


/* the universe interrupt handler is capable of routing all sorts of
 * (VME) interrupts to 8 different lines (some of) which may be hooked up
 * in a (board specific) way to a PIC.
 *
 * This driver only supports at most two lines. By default, it routes the
 * 7 VME interrupts to the main line and optionally, it routes the 'special'
 * interrupts generated by the universe itself (DMA done, VOWN etc.)
 * to a second line. If no second line is available, all IRQs are routed
 * to the main line.
 *
 * The routing of interrupts to the two lines can be modified (using
 * the vmeUniverseIntRoute() call - see above - i.e., to make use of
 * different hardware priorities of the two pins.
 *
 * Because the driver has no way to figure out which lines are actually
 * wired to the PIC, this information has to be provided when installing
 * the manager.
 *
 * Hence the manager sets up routing VME interrupts to 1 or 2 universe
 * OUTPUTS. However, it must also be told to which PIC INPUTS they
 * are wired.
 * Optionally, the first PIC input line can be read from PCI config space
 * but the second must be passed to this routine. Note that the info read
 * from PCI config space is wrong for many boards!
 *
 * PARAMETERS:
 *       vmeIrqUnivOut: to which output pin (of the universe) should the 7
 *       				VME irq levels be routed.
 *       vmeIrqPicLine: specifies to which PIC input the 'main' output is
 *                      wired. If passed a value < 0, the driver reads this
 *                      information from PCI config space ("IRQ line").
 *   specialIrqUnivOut: to which output pin (of the universe) should the
 *                      internally irqs be routed. Use 'vmeIRQunivOut'
 *                      if < 0.
 *   specialIrqPicLine: specifies to which PIC input the 'special' output
 *                      pin is wired. The wiring of the 'vmeIRQunivOut' to
 *                      the PIC is determined by reading PCI config space.
 *
 * RETURNS: 0 on success, -1 on failure.
 *
 */

/* This routine is outside of the __INSIDE_RTEMS_BSP__ test for bwrds compatibility ONLY */
int
vmeUniverseInstallIrqMgr(int vmeIrqUnivOut,
						 int vmeIrqPicLine,
						 int specialIrqUnivOut,
						 int specialIrqPicLine);


#if defined(__INSIDE_RTEMS_BSP__)
#include <stdarg.h>

/* up to 4 universe outputs are now supported by this alternate
 * entry point.
 * Terminate the vararg list (uni_pin/pic_pin pairs) with a
 * '-1' uni_pin.
 * E.g., the old interface is now just a wrapper to
 *   vmeUniverseInstallIrqMgrAlt(0, vmeUnivOut, vmePicLint, specUnivOut, specPicLine, -1);
 *
 * The 'IRQ_MGR_SHARED' flag uses the BSP_install_rtems_shared_irq_handler()
 * API. CAVEAT: shared interrupts need RTEMS workspace, i.e., the
 * VME interrupt manager can only be installed *after workspace is initialized*
 * if 'shared' is nonzero (i.e., *not* from bspstart()).
 *
 * If 'PW_WORKAROUND' flag is set then the interrupt manager will try to
 * find a way to access the control registers from VME so that the universe's
 * posted write FIFO can be flushed after the user ISR returns:
 *
 * The installation routine looks first for CSR registers in CSR space (this
 * requires:
 *      - a VME64 crate with autoid or geographical addressing
 *      - the firmware or BSP to figure out the slot number and program the CSR base
 *        in the universe.
 *      - the BSP to open an outbound window to CSR space.
 *
 * If CSR registers cannot be found then the installation routine looks for CRG registers:
 *      - BSP must map CRG on VME
 *      - CRG must be visible in outbound window
 *      CAVEAT: multiple boards with same BSP on single backplane must not map their CRG
 *              to the same address!
 */

#define VMEUNIVERSE_IRQ_MGR_FLAG_SHARED			1	/* use shared interrupts */
#define VMEUNIVERSE_IRQ_MGR_FLAG_PW_WORKAROUND	2	/* use shared interrupts */

int
vmeUniverseInstallIrqMgrAlt(int flags, int uni_pin0, int pic_pin0, ...);

int
vmeUniverseInstallIrqMgrVa(int flags, int uni_pin0, int pic_pin0, va_list ap);

#endif /* __INSIDE_RTEMS_BSP__ */
#endif /* __rtems__ */

#ifdef __cplusplus
}
#endif

#endif
