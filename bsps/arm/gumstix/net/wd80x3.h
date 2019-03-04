/**
 * @file
 * @ingroup gumstix_dp8390
 * @brief DP8390 Ethernet Controller Support
 */

/*
 *  Information about the DP8390 Ethernet controller.
 */

#ifndef __BSP_WD80x3_h
#define __BSP_WD80x3_h

/* Register descriptions */

/**
 * @defgroup gumstix_dp8390 DP8390 Support
 * @ingroup RTEMSBSPsARMGumstix
 * @brief DP8390 Ethernet Controller Support
 * @{
 */

/**
 * @name Controller DP8390.
 * @{
 */

/** @brief Port Window. */
#define DATAPORT	0x10
/** @brief Issue a read for reset */
#define RESET		0x1f
/** @brief I/O port definition */
#define W83CREG		0x00
#define	ADDROM		0x08

/** @} */

/**
 * @name page 0 read or read/write registers
 * @{
 */

#define	CMDR		0x00+RO
/** @brief current local dma addr 0 for read */
#define CLDA0  		0x01+RO
/** @brief current local dma addr 1 for read */
#define CLDA1  		0x02+RO
/** @brief boundary reg for rd and wr */
#define BNRY   		0x03+RO
/** @brief tx status reg for rd */
#define TSR    		0x04+RO
/** @brief number of collision reg for rd */
#define NCR    		0x05+RO
/** @breif FIFO for rd */
#define FIFO   		0x06+RO
/** @brief interrupt status reg for rd and wr */
#define ISR    		0x07+RO
/** @brief current remote dma address 0 for rd */
#define CRDA0  		0x08+RO
/** @brief current remote dma address 1 for rd */
#define CRDA1  		0x09+RO
/** @brief rx status reg for rd */
#define RSR    		0x0C+RO
/** @brief tally cnt 0 for frm alg err for rd */
#define CNTR0  		0x0D+RO
/** @brief tally cnt 1 for crc err for rd */
#define CNTR1  		RO+0x0E
/** @brief tally cnt 2 for missed pkt for rd */
#define CNTR2  		0x0F+RO

/** @} */

/**
 * @name page 0 write registers
 * @{
 */

/** @brief page start register */
#define PSTART 		0x01+RO
/** @brief page stop register */
#define PSTOP  		0x02+RO
/** @breif tx start page start reg */
#define TPSR   		0x04+RO
/** @brief tx byte count 0 reg */
#define TBCR0  		0x05+RO
/** @brief tx byte count 1 reg */
#define TBCR1  		0x06+RO
/** @brief remote start address reg 0  */
#define RSAR0  		0x08+RO
/** @brief remote start address reg 1 */
#define RSAR1  		0x09+RO
/** @brief remote byte count reg 0 */
#define RBCR0  		0x0A+RO
/** @brief remote byte count reg 1 */
#define RBCR1  		0x0B+RO
/** @brief rx configuration reg */
#define RCR    		0x0C+RO
/** @brief tx configuration reg */
#define TCR    		0x0D+RO
/** @brief data configuration reg */
#define DCR    		RO+0x0E
/** @brief interrupt mask reg */
#define IMR    		0x0F+RO

/** @} */

/**
 * @name page 1 registers
 * @{
 */

/** @brief physical addr reg base for rd and wr */
#define PAR   		0x01+RO
/** @brief current page reg for rd and wr */
#define CURR   		0x07+RO
/** @brief multicast addr reg base fro rd and WR */
#define MAR   		0x08+RO
/** @brief size of multicast addr space */
#define MARsize	8

/** @} */

/**
 * @name W83CREG command bits
 * @{
 */

/** @brief W83CREG masks */
#define MSK_RESET  0x80
#define MSK_ENASH  0x40
/** @brief memory decode bits, corresponding */
#define MSK_DECOD  0x3F

/** @} */

/**
 * @name CMDR command bits
 * @{
 */

/** @brief stop the chip */
#define MSK_STP		0x01
/** @brief start the chip */
#define MSK_STA		0x02
/** @brief initial txing of a frm */
#define MSK_TXP	       	0x04
/** @brief remote read */
#define MSK_RRE		0x08
/** @brief remote write */
#define MSK_RWR		0x10
/** @brief no DMA used */
#define MSK_RD2		0x20
/** @brief select register page 0 */
#define MSK_PG0	       	0x00
/** @brief select register page 1 */
#define MSK_PG1	       	0x40
/** @brief select register page 2 */
#define MSK_PG2	       	0x80

/** @} */

/**
 * @name ISR and TSR status bits
 * @{
 */

/* @brief rx with no error */
#define MSK_PRX 	0x01
/* @brief tx with no error */
#define MSK_PTX 	0x02
/* @brief rx with error */
#define MSK_RXE 	0x04
/* @brief tx with error */
#define MSK_TXE 	0x08
/* @brief overwrite warning */
#define MSK_OVW 	0x10
/* @brief MSB of one of the tally counters is set */
#define MSK_CNT 	0x20
/* @brief remote dma completed */
#define MSK_RDC 	0x40
/* @brief reset state indicator */
#define MSK_RST		0x80

/** @} */

/**
 * @name DCR command bits
 * @{
 */

/** @brief word transfer mode selection */
#define MSK_WTS		0x01
/** @brief byte order selection */
#define MSK_BOS		0x02
/** @brief long addr selection */
#define MSK_LAS		0x04
/** @brief burst mode selection */
#define MSK_BMS		0x08
/** @brief autoinitialize remote */
#define MSK_ARM		0x10
/** @brief burst lrngth selection */
#define MSK_FT00	0x00
/** @brief burst lrngth selection */
#define MSK_FT01	0x20
/** @brief burst lrngth selection */
#define MSK_FT10	0x40
/** @brief burst lrngth selection */
#define MSK_FT11	0x60

/** @} */

/**
 * @name RCR command bits
 * @{
 */

/** @brief save error pkts */
#define MSK_SEP		0x01
/** @brief accept runt pkt */
#define MSK_AR	 	0x02
/** @brief 8390 RCR */
#define MSK_AB		0x04
/** @brief accept multicast  */
#define MSK_AM	 	0x08
/** @brief accept all pkt with physical adr */
#define MSK_PRO		0x10
/** @brief monitor mode */
#define MSK_MON		0x20

/** @} */

/**
 * @name TCR command bits
 * @{
 */

/** @brief inhibit CRC, do not append crc */
#define MSK_CRC		0x01
/** @brief set loopback mode */
#define MSK_LOOP	0x02
/** @brief Accept broadcasts */
#define MSK_BCST	0x04
/** @brief encoded loopback control */
#define MSK_LB01	0x06
/** @brief auto tx disable */
#define MSK_ATD		0x08
/** @brief collision offset enable  */
#define MSK_OFST	0x10

/** @} */

/**
 * @name receive status bits
 * @{
 */

/** @brief rx without error */
#define SMK_PRX   0x01
/** @brief CRC error */
#define SMK_CRC   0x02
/** @brief frame alignment error */
#define SMK_FAE   0x04
/** @brief FIFO overrun */
#define SMK_FO    0x08
/** @brief missed pkt */
#define SMK_MPA   0x10
/** @brief physical/multicase address */
#define SMK_PHY   0x20
/** @brief receiver disable. set in monitor mode */
#define SMK_DIS   0x40
/** @brief deferring */
#define SMK_DEF	  0x80

/** @} */

/**
 * @name transmit status bits
 * @{
 */

/** @brief tx without error */
#define SMK_PTX   0x01
/** @brief non deferred tx */
#define SMK_DFR   0x02
/** @brief tx collided */
#define SMK_COL   0x04
/** @brief tx abort because of excessive collisions */
#define SMK_ABT   0x08
/** @brief carrier sense lost */
#define SMK_CRS   0x10
/** @brief FIFO underrun */
#define SMK_FU    0x20
/** @brief collision detect heartbeat */
#define SMK_CDH   0x40
/** @brief out of window collision */
#define SMK_OWC	  0x80

/** @} */

/** @} */

#endif
/* end of include */
