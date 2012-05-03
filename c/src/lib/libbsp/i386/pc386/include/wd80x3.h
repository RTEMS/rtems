/*
 *  Information about the DP8390 Ethernet controller.
 */

#ifndef __BSP_WD80x3_h
#define __BSP_WD80x3_h

/* Register descriptions */
/* Controller DP8390.    */

#define DATAPORT	0x10	/* Port Window. */
#define RESET		0x1f	/* Issue a read for reset */
#define W83CREG		0x00	/* I/O port definition */
#define	ADDROM		0x08

/* page 0 read or read/write registers */

#define	CMDR		0x00+RO
#define CLDA0  		0x01+RO	/* current local dma addr 0 for read */
#define CLDA1  		0x02+RO	/* current local dma addr 1 for read */
#define BNRY   		0x03+RO	/* boundary reg for rd and wr */
#define TSR    		0x04+RO	/* tx status reg for rd */
#define NCR    		0x05+RO	/* number of collision reg for rd */
#define FIFO   		0x06+RO	/* FIFO for rd */
#define ISR    		0x07+RO	/* interrupt status reg for rd and wr */
#define CRDA0  		0x08+RO	/* current remote dma address 0 for rd */
#define CRDA1  		0x09+RO	/* current remote dma address 1 for rd */
#define RSR    		0x0C+RO	/* rx status reg for rd */
#define CNTR0  		0x0D+RO	/* tally cnt 0 for frm alg err for rd */
#define CNTR1  		RO+0x0E	/* tally cnt 1 for crc err for rd */
#define CNTR2  		0x0F+RO	/* tally cnt 2 for missed pkt for rd */

/* page 0 write registers */

#define PSTART 		0x01+RO	/* page start register */
#define PSTOP  		0x02+RO	/* page stop register */
#define TPSR   		0x04+RO	/* tx start page start reg */
#define TBCR0  		0x05+RO	/* tx byte count 0 reg */
#define TBCR1  		0x06+RO	/* tx byte count 1 reg */
#define RSAR0  		0x08+RO	/* remote start address reg 0  */
#define RSAR1  		0x09+RO	/* remote start address reg 1 */
#define RBCR0  		0x0A+RO	/* remote byte count reg 0 */
#define RBCR1  		0x0B+RO	/* remote byte count reg 1 */
#define RCR    		0x0C+RO	/* rx configuration reg */
#define TCR    		0x0D+RO	/* tx configuration reg */
#define DCR    		RO+0x0E	/* data configuration reg */
#define IMR    		0x0F+RO	/* interrupt mask reg */

/* page 1 registers */

#define PAR   		0x01+RO	/* physical addr reg base for rd and wr */
#define CURR   		0x07+RO	/* current page reg for rd and wr */
#define MAR   		0x08+RO	/* multicast addr reg base fro rd and WR */
#define MARsize	8		/* size of multicast addr space */

/*-----W83CREG command bits-----*/
#define MSK_RESET  0x80		/* W83CREG masks */
#define MSK_ENASH  0x40
#define MSK_DECOD  0x3F		/* memory decode bits, corresponding */
				/* to SA 18-13. SA 19 assumed to be 1 */

/*-----CMDR command bits-----*/
#define MSK_STP		0x01	/* stop the chip */
#define MSK_STA		0x02	/* start the chip */
#define MSK_TXP	       	0x04	/* initial txing of a frm */
#define MSK_RRE		0x08	/* remote read */
#define MSK_RWR		0x10	/* remote write */
#define MSK_RD2		0x20	/* no DMA used */
#define MSK_PG0	       	0x00	/* select register page 0 */
#define MSK_PG1	       	0x40	/* select register page 1 */
#define MSK_PG2	       	0x80	/* select register page 2 */

/*-----ISR and TSR status bits-----*/
#define MSK_PRX 	0x01	/* rx with no error */
#define MSK_PTX 	0x02	/* tx with no error */
#define MSK_RXE 	0x04	/* rx with error */
#define MSK_TXE 	0x08	/* tx with error */
#define MSK_OVW 	0x10	/* overwrite warning */
#define MSK_CNT 	0x20	/* MSB of one of the tally counters is set */
#define MSK_RDC 	0x40	/* remote dma completed */
#define MSK_RST		0x80	/* reset state indicator */

/*-----DCR command bits-----*/
#define MSK_WTS		0x01	/* word transfer mode selection */
#define MSK_BOS		0x02	/* byte order selection */
#define MSK_LAS		0x04	/* long addr selection */
#define MSK_BMS		0x08	/* burst mode selection */
#define MSK_ARM		0x10	/* autoinitialize remote */
#define MSK_FT00	0x00	/* burst lrngth selection */
#define MSK_FT01	0x20	/* burst lrngth selection */
#define MSK_FT10	0x40	/* burst lrngth selection */
#define MSK_FT11	0x60	/* burst lrngth selection */

/*-----RCR command bits-----*/
#define MSK_SEP		0x01	/* save error pkts */
#define MSK_AR	 	0x02	/* accept runt pkt */
#define MSK_AB		0x04	/* 8390 RCR */
#define MSK_AM	 	0x08	/* accept multicast  */
#define MSK_PRO		0x10	/* accept all pkt with physical adr */
#define MSK_MON		0x20	/* monitor mode */

/*-----TCR command bits-----*/
#define MSK_CRC		0x01	/* inhibit CRC, do not append crc */
#define MSK_LOOP	0x02	/* set loopback mode */
#define MSK_BCST	0x04	/* Accept broadcasts */
#define MSK_LB01	0x06	/* encoded loopback control */
#define MSK_ATD		0x08	/* auto tx disable */
#define MSK_OFST	0x10	/* collision offset enable  */

/*-----receive status bits-----*/
#define SMK_PRX   0x01		/* rx without error */
#define SMK_CRC   0x02		/* CRC error */
#define SMK_FAE   0x04		/* frame alignment error */
#define SMK_FO    0x08		/* FIFO overrun */
#define SMK_MPA   0x10		/* missed pkt */
#define SMK_PHY   0x20		/* physical/multicase address */
#define SMK_DIS   0x40		/* receiver disable. set in monitor mode */
#define SMK_DEF	  0x80		/* deferring */

/*-----transmit status bits-----*/
#define SMK_PTX   0x01		/* tx without error */
#define SMK_DFR   0x02		/* non deferred tx */
#define SMK_COL   0x04		/* tx collided */
#define SMK_ABT   0x08		/* tx abort because of excessive collisions */
#define SMK_CRS   0x10		/* carrier sense lost */
#define SMK_FU    0x20		/* FIFO underrun */
#define SMK_CDH   0x40		/* collision detect heartbeat */
#define SMK_OWC	  0x80		/* out of window collision */

#endif
/* end of include */
