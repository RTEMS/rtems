/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
#ifndef _PCNET_H
#define _PCNET_H

/*
 * IO space structure for the AMD79C970 device 
 */

typedef volatile struct pc_net
{
	union
	{
		struct {
			unsigned16	aprom[8];  /* 0x00 */
			unsigned16	rdp;	   /* 0x10 */
			unsigned16	rap;	   /* 0x14 */
			unsigned16	reset;	   /* 0x18 */
			unsigned16	bdp;	   /* 0x1C */
		} wio;
		struct {
			unsigned32	aprom[4];  /* 0x00 */
			unsigned32	rdp;	   /* 0x10 */
			unsigned32	rap;	   /* 0x12 */
			unsigned32	reset;	   /* 0x14 */
			unsigned32	bdp;	   /* 0x16 */
		} dwio;
	} u;
} pc_net_t;

/*
 * The EEPROM is 2Kbit (128bytes)
 */
#define EEPROM_SIZE 128
#define EEPROM_HEAD_SIZE 36

typedef struct pc_net_eeprom {
	unsigned8	EthNumber[6];
	unsigned16	Reserved1;	/* Must be 0x0000 */
	unsigned16	Reserved2;	/* Must be 0x1000 */
	unsigned16	User1;
	unsigned16	checksum;
	unsigned16	Reserved3;	/* Must be 0x5757 */
	unsigned16	bcr16;
	unsigned16	bcr17;
	unsigned16	bcr18;
	unsigned16	bcr2;
	unsigned16	bcr21;
	unsigned16	Reserved4;	/* Must be 0x0000 */
	unsigned16	Reserved5;	/* Must be 0x0000 */
	unsigned8	Reserved6;	/* Must be 0x00 */
	unsigned8	checksumAdjust;
	unsigned16	Reserved7;	/* Must be 0x0000 */
	unsigned16	crc;		/* CCITT checksum from Serial[] onwards */
	unsigned8	Serial[16];	/* Radstone Serial Number */
} pc_net_eeprom_t;

/*
 * PCnet-PCI Single Chip Ethernet Controller for PCI Local Bus 
 */
/*
 * Register and bit definitions 
 */

#define	CSR0	0
#define	CSR1	1
#define	CSR2	2
#define	CSR3	3
#define	CSR4	4
#define	CSR5	5
#define	CSR6	6
#define	CSR7	7
#define	CSR8	8
#define	CSR9	9
#define	CSR15	15
#define	CSR47	47
#define	CSR82	82	/* Bus Activity Timer */
#define	CSR100	100	/* Memory Error Timeout register */
#define	CSR114	114
#define	CSR122	122	/* Receiver Packet Alignment Register */

#define	BCR2	2	/* Misc. Configuration */
#define	BCR18	18	/* Bus size and burst control */
#define DEFAULT_BCR18 0x2162 /* default BCR18 value - was 0x21e2*/
#define	BCR19	19
#define	BCR20	20	/* Software Style */
#define	BCR21	21

#define	APROM0	0x00
#define	APROM1	0x04
#define	APROM2	0x08

/*
 * CSR0: Bit definitions 
 */
#define CSR0_ERR	0x8000	/* error summary */
#define CSR0_BABL	0x4000	/* babble error */
#define CSR0_CERR	0x2000	/* collision error */
#define CSR0_MISS	0x1000	/* missed packet */
#define CSR0_MERR	0x0800	/* memory error */
#define CSR0_RINT	0x0400	/* receiver interrupt */
#define CSR0_TINT	0x0200	/* transmitter interrupt */
#define CSR0_IDON	0x0100	/* initialization done */
#define CSR0_INTR	0x0080	/* interrupt flag */
#define CSR0_IENA	0x0040	/* interrupt enable */
#define CSR0_RXON	0x0020	/* receiver on */
#define CSR0_TXON	0x0010	/* transmitter on */
#define CSR0_TDMD	0x0008	/* transmit demand */
#define CSR0_STOP	0x0004	/* stop the ilacc */
#define CSR0_STRT	0x0002	/* start the ilacc */
#define CSR0_INIT	0x0001	/* initialize the ilacc */

#define CSR3_BABLM	0x4000	/* BABL Mask */
#define CSR3_MISSM	0x1000	/* Missed packet Mask */
#define CSR3_MERRM	0x0800	/* Memory error Mask */
#define CSR3_RINTM	0x0400	/* Receive Interrupt Mask */
#define CSR3_TINTM	0x0200	/* Transmit Interrupt Mask */
#define CSR3_IDONM	0x0100	/* Initialization Done Mask */
#define CSR3_DXSUFLO	0x0040	/* Disable tx stop on underrun */
#define CSR3_LAPPEN	0x0020	/* lookahead packet proc enable */
#define CSR3_DXMT2PD	0x0010	/* disable 2 part deferral */
#define CSR3_EMBA	0x0008	/* enable modified backoff */
#define CSR3_BSWP	0x0004	/* byte swap */

#define CSR4_DMAPLUS	0x4000	/* DMA burst transfer until FIFO empty */
#define CSR4_BACON_68K  0x0040	/* 32 bit 680x0 */
#define	CSR4_TXSTRT	0x0008	/* Transmit STaRT status */
#define CSR4_TXSTRTM	0x0004	/* Transmit STaRT interrupt Mask  */
#define CSR4_ENTST	0x8000	/* enable test mode */
#define CSR4_TIMER	0x2000	/* enable bus timer csr82 */
#define CSR4_DPOLL	0x1000	/* disable tx polling */
#define CSR4_APADXMIT	0x0800	/* auto pad tx to 64 */
#define CSR4_ASTRPRCV	0x0400	/* auto strip rx pad and fcs */
#define CSR4_MFCO	0x0200	/* missed frame counter oflo interrupt */
#define CSR4_MFCOM	0x0100	/* mask to disable */
#define CSR4_RCVCCO	0x0020	/* rx collision counter oflo interrupt */
#define CSR4_RCVCCOM	0x0010	/* mask to disable */
#define CSR4_JAB	0x0002	/* jabber error 10baseT interrupt */
#define CSR4_JABM	0x0001	/* mask to disable */

#define CSR5_SPND	0x0001	/* Suspend */

#define CSR15_PROM	0x8000	/* Promiscuous */
#define CSR15_DRCVBC	0x4000	/* Disable receiver broadcast */
#define CSR15_DRCVPA	0x2000	/* Disable receiver phys. addr. */
#define CSR15_DLNKTST	0x1000	/* Disable link status */
#define CSR15_DAPC	0x0800	/* Disable auto polarity det. */
#define CSR15_MENDECL	0x0400	/* MENDEC loopback mode */
#define CSR15_LRT	0x0200	/* Low receiver threshold */
#define CSR15_TSEL	0x0200	/* Transmit mode select */
#define CSR15_INTL	0x0040	/* Internal loopback */
#define CSR15_DRTY	0x0020	/* Disable retry */
#define CSR15_FCOLL	0x0010	/* Force collision */
#define CSR15_DXMTFCS	0x0008	/* Disable transmit CRC */
#define CSR15_LOOP	0x0004	/* Loopback enable */
#define CSR15_DTX	0x0002	/* Disable transmitter */
#define CSR15_DRX	0x0001	/* Disable receiver */

#define CSR58_PCISTYLE	0x0002	/* software style */

#define CSR80_RCVFW16	(0<<12)	/* fifo level to trigger rx dma */
#define CSR80_RCVFW32	(1<<12)
#define CSR80_RCVFW64	(2<<12)
#define CSR80_XMTSP4	(0<<10)	/* fifo level to trigger tx */
#define CSR80_XMTSP16	(1<<10)
#define CSR80_XMTSP64	(2<<10)
#define CSR80_XMTSP112	(3<<10)
#define CSR80_XMTFW16	(0<<8)	/* fifo level to stop dma */
#define CSR80_XMTFW32	(1<<8)
#define CSR80_XMTFW64	(2<<8)
/* must also clear csr4 CSR4_DMAPLUS: */ 
#define CSR80_DMATC(x)	((x)&0xff)	/* max transfers per burst. deflt 16 */
/*
 * must also set csr4 CSR4_TIMER: 
 */
#define CSR82_DMABAT(x)	((x)&0xffff)	/* max burst time nanosecs*100 */

#define BCR18_MUSTSET	0x0100	/* this bit must be written as 1 !! */
#define BCR18_BREADE	0x0040	/* linear burst enable. yes ! on pci */
#define BCR18_BWRITE	0x0020	/* in write direction */
#define BCR18_LINBC4	0x0001	/* linear burst count 4 8 or 16 */
#define BCR18_LINBC8	0x0002	/* NOTE LINBC must be <= fifo trigger*/
#define BCR18_LINBC16	0x0004

#define BCR19_PVALID	0x8000	/* aprom (eeprom) read checksum ok */

/*
 * initial setting of csr0 
 */
#define CSR0_IVALUE	(CSR0_IDON | CSR0_IENA | CSR0_STRT | CSR0_INIT)

/*
 * our setting of csr3 
 */
#define CSR3_VALUE	(CSR3_ACON | CSR3_BSWP)

/*
 * Initialization Block.
 *	Chip initialization includes the reading of the init block to obtain
 *	the operating parameters.
 *
 * This essentially consists of 7, 32 bit LE words. In the following the
 * fields are ordered so that they map correctly in BE mode, however each
 * 16 and 32 byte field will require swapping.
 */

typedef volatile struct initblk {
	/* mode can be set in csr15 */
	unsigned16	ib_mode;	/* Chip's operating parameters */
	unsigned8	ib_rlen;	/* rx ring length (power of 2) */
	unsigned8	ib_tlen;	/* tx ring length (power of 2) */
/*
 * The bytes must be swapped within the word, so that, for example,
 * the address 8:0:20:1:25:5a is written in the order
 *             0 8 1 20 5a 25
 * For PCI970 that is long word swapped: so no swapping needed, since 
 * the bus will swap.
 */
	unsigned8	ib_padr[8];	/* physical address */
	unsigned16	ib_ladrf[4];	/* logical address filter */
	unsigned32	ib_rdra;	/* rcv ring desc addr */
	unsigned32	ib_tdra;	/* xmit ring desc addr */
} initblk_t;


/*
 * bits in mode register: allows alteration of the chips operating parameters 
 */
#define IBM_PROM	0x8000	/* promiscuous mode */
/*
 * mode is also in cr15 
 */
#define MODE_DRCVBC	0x4000	/* disable receive broadcast */
#define MODE_DRCVPA	0x2000	/* disable receive physical address */
#define MODE_DLNKTST	0x1000	/* disable link status monitoring 10T */
#define MODE_DAPC	0x0800	/* disable auto polarity 10T */
#define MODE_MENDECL	0x0400	/* mendec loopback */
#define MODE_LRT	0x0200	/* low receive threshold/tx mode sel tmau */
#define MODE_PORTSEL10T	0x0080	/* port select 10T ?? */
#define MODE_PORTSELAUI	0x0000	/* port select aui ?? */
#define IBM_INTL	0x0040	/* internal loopback */
#define IBM_DRTY	0x0020	/* disable retry */
#define IBM_COLL	0x0010	/* force collision */
#define IBM_DTCR	0x0008	/* disable transmit crc */
#define IBM_LOOP	0x0004	/* loopback */
#define IBM_DTX		0x0002	/* disable transmitter */
#define IBM_DRX		0x0001	/* disable receiver */

/* 
 * Buffer Management is accomplished through message descriptors organized
 * in ring structures in main memory. There are two rings allocated for the
 * device: a receive ring and a transmit ring. The following defines the 
 * structure of the descriptor rings.
 */

/*
 *                  Receive  List type definition                            
 *
 * This essentially consists of 4, 32 bit LE words. In the following the
 * fields are ordered so that they map correctly in BE mode, however each
 * 16 and 32 byte field will require swapping.
 */

typedef volatile struct rmde {
	unsigned32	rmde_addr;	/* buf addr */

	unsigned16	rmde_bcnt; 
	unsigned16	rmde_flags;

	unsigned16	rmde_mcnt;
	unsigned16	rmde_misc;

	unsigned32	align;
} rmde_t;


/*
 * bits in the flags field 
 */
#define RFLG_OWN	0x8000	/* ownership bit, 1==LANCE */
#define RFLG_ERR	0x4000	/* error summary */
#define RFLG_FRAM	0x2000	/* framing error */
#define RFLG_OFLO	0x1000	/* overflow error */
#define RFLG_CRC	0x0800	/* crc error */
#define RFLG_BUFF	0x0400	/* buffer error */
#define RFLG_STP	0x0200	/* start of packet */
#define RFLG_ENP	0x0100	/* end of packet */

/*
 * bits in the buffer byte count field 
 */
#define RBCNT_ONES	0xf000	/* must be ones */
#define RBCNT_BCNT	0x0fff	/* buf byte count, in 2's compl */

/*
 * bits in the message byte count field 
 */
#define RMCNT_RES	0xf000	/* reserved, read as zeros */
#define RMCNT_BCNT	0x0fff	/* message byte count */

/*
 *                  Transmit List type definition                            
 *
 * This essentially consists of 4, 32 bit LE words. In the following the
 * fields are ordered so that they map correctly in BE mode, however each
 * 16 and 32 byte field will require swapping.
 */
typedef volatile struct tmde {
    	unsigned32	tmde_addr;	/* buf addr */

	unsigned16	tmde_bcnt; 
	unsigned16	tmde_status;	/* misc error and status bits */

	unsigned32	tmde_error;

	unsigned32	align;
} tmde_t;

/*
 * bits in the status field 
 */
#define TST_OWN		0x8000	/* ownership bit, 1==LANCE */
#define TST_ERR		0x4000	/* error summary */
#define TST_RES		0x2000	/* reserved bit */
#define TST_MORE	0x1000	/* more than one retry was needed */
#define TST_ONE		0x0800	/* one retry was needed */
#define TST_DEF		0x0400	/* defer while trying to transmit */
#define TST_STP		0x0200	/* start of packet */
#define TST_ENP		0x0100	/* end of packet */

/*
 * setting of status field when packet is to be transmitted 
 */
#define TST_XMIT	(TST_STP | TST_ENP | TST_OWN)

/*
 * bits in the buffer byte count field 
 */
#define TBCNT_ONES	0xf000	/* must be ones */
#define TBCNT_BCNT	0x0fff	/* buf byte count, in 2's compl */

/*
 * bits in the error field 
 */
#define TERR_BUFF	0x8000	/* buffer error */
#define TERR_UFLO	0x4000	/* underflow error */
#define TERR_EXDEF	0x2000	/* excessive deferral */
#define TERR_LCOL	0x1000	/* late collision */
#define TERR_LCAR	0x0800	/* loss of carrier */
#define TERR_RTRY	0x0400	/* retry error */
#define TERR_TDR	0x03ff	/* time domain reflectometry */

/*
 * Defines pertaining to statistics gathering (diagnostic only)
 */

/*
 * receive errors 
 */
#define ERR_FRAM	0	/* framing error */
#define ERR_OFLO	1	/* overflow error */
#define ERR_CRC		2	/* crc error */
#define ERR_RBUFF	3	/* receive buffer error */

/*
 * transmit errors 
 */
#define ERR_MORE	4	/* more than one retry */
#define ERR_ONE		5	/* one retry */
#define ERR_DEF		6	/* defer'd packet */
#define ERR_TBUFF	7	/* transmit buffer error */
#define ERR_UFLO	8	/* underflow error */
#define ERR_LCOL	9	/* late collision */
#define ERR_LCAR	10	/* loss of carrier */
#define ERR_RTRY	11	/* retry error, >16 retries */

/*
 * errors reported in csr0 
 */
#define ERR_BABL	12	/* transmitter timeout error */
#define ERR_MISS	13	/* missed packet */
#define ERR_MEM		14	/* memory error */
#define ERR_CERR	15	/* collision errors */
#define XMIT_INT	16	/* transmit interrupts */
#define RCV_INT		17	/* receive interrupts */

#define NHARD_ERRORS	18	/* error types used in diagnostic */

/*
 * other statistics 
 */
#define ERR_TTOUT	18	/* transmit timeouts */
#define ERR_ITOUT	19	/* init timeouts */
#define ERR_INITS	20	/* reinitializations */
#define ERR_RSILO	21	/* silo ptrs misaligned on recv */
#define ERR_TSILO	22	/* silo ptrs misaligned on xmit */
#define ERR_SINTR	23	/* spurious interrupts */

#define NUM_ERRORS	24	/* number of errors types */

/*
 * Bit definitions for BCR19 
 */
#define	prom_EDI	(unsigned16)0x0001
#define	prom_EDO	(unsigned16)0x0001
#define	prom_ESK	(unsigned16)0x0002
#define	prom_ECS	(unsigned16)0x0004
#define	prom_EEN	(unsigned16)0x0010
#define	prom_EEDET	(unsigned16)0x2000
#define	prom_PVALID	(unsigned16)0x8000
#define	prom_PREAD	(unsigned16)0x4000

#endif
