/*
 *******************************************************************
 *******************************************************************
 **                                                               **
 **           DECLARATIONS FOR NATIONAL DP83932 `SONIC'           **
 **         SYSTEMS-ORIENTED NETWORK INTERFACE CONTROLLER         **
 **                                                               **
 *******************************************************************
 *******************************************************************
 */

/*
 * $Revision$   $Date$   $Author$
 * $State$
 */

#ifndef _SONIC_DP83932_
#define _SONIC_DP83932_

#include <bsp.h>

/*
 ******************************************************************
 *                                                                *
 *                        Device Registers                        *
 *                                                                *
 ******************************************************************
 */
struct SonicRegisters {
	/*
	 * Command and status registers
	 */
	rtems_unsigned32	cr;	/* Command */
	rtems_unsigned32	dcr;	/* Data configuration */
	rtems_unsigned32	rcr;	/* Receive control */
	rtems_unsigned32	tcr;	/* Transmit control */
	rtems_unsigned32	imr;	/* Interrupt mask */
	rtems_unsigned32	isr;	/* Interrupt status */

	/*
	 * Transmit registers
	 */
	rtems_unsigned32	utda;	/* Upper transmit descriptor address */
	rtems_unsigned32	ctda;	/* Current transmit descriptor address */

	/*
	 * Receive registers
	 */
	rtems_unsigned32 pad0[5];
	rtems_unsigned32	urda;	/* Upper receive descriptor address */
	rtems_unsigned32	crda;	/* Current receive descriptor address */
	rtems_unsigned32 pad1[4];
	rtems_unsigned32	eobc;	/* End of buffer word count */
	rtems_unsigned32	urra;	/* Upper receive resource */
	rtems_unsigned32	rsa;	/* Resource start address */
	rtems_unsigned32	rea;	/* Resource end address */
	rtems_unsigned32	rrp;	/* Resouce read pointer */
	rtems_unsigned32	rwp;	/* Resouce read pointer */

	/*
	 * Content-addressable memory registers
	 */
	rtems_unsigned32 pad2[8];
	rtems_unsigned32	cep;	/* CAM entry pointer */
	rtems_unsigned32	cap2;	/* CAM address port 2 */
	rtems_unsigned32	cap1;	/* CAM address port 1 */
	rtems_unsigned32	cap0;	/* CAM address port 0 */
	rtems_unsigned32	ce;	/* CAM enable */
	rtems_unsigned32	cdp;	/* CAM descriptor pointer */
	rtems_unsigned32	cdc;	/* CAM descriptor count */

	/*
	 * Silicon revision
	 */
	rtems_unsigned32	sr;	/* Silicon revision */

	/*
	 * Watchdog counters
	 */
	rtems_unsigned32	wt0;	/* Watchdog timer 0 */
	rtems_unsigned32	wt1;	/* Watchdog timer 1 */

	/*
	 * Another receive register
	 */
	rtems_unsigned32	rsc;	/* Receive sequence counter */

	/*
	 * Tally counters
	 */
	rtems_unsigned32	crct;	/* CRC error tally */
	rtems_unsigned32	faet;	/* FAE tally */
	rtems_unsigned32	mpt;	/* Missed packet tally */

	/*
	 * Another command and status register
	 */
	rtems_unsigned32 pad3[16];
	rtems_unsigned32	dcr2;	/* Data configuration 2 */
};

/*
 * Command register
 */
#define CR_LCAM		0x0200
#define CR_RRRA		0x0100
#define CR_RST		0x0080
#define CR_ST		0x0020
#define CR_STP		0x0010
#define CR_RXEN		0x0008
#define CR_RXDIS	0x0004
#define CR_TXP		0x0002
#define CR_HTX		0x0001

/*
 * Data configuration register
 */
#define DCR_EXBUS	0x8000
#define DCR_LBR		0x2000
#define DCR_PO1		0x1000
#define DCR_PO0		0x0800
#define DCR_SBUS	0x0400
#define DCR_USR1	0x0200
#define DCR_USR0	0x0100
#define DCR_WC1		0x0080
#define DCR_WC0		0x0040
#define DCR_DW		0x0020
#define DCR_BMS		0x0010
#define DCR_RFT1	0x0008
#define DCR_RFT0	0x0004
#define DCR_TFT1	0x0002
#define DCR_TFT0	0x0001

/*
 * Receive control register
 */
#define RCR_ERR		0x8000
#define RCR_RNT		0x4000
#define RCR_BRD		0x2000
#define RCR_PRO		0x1000
#define RCR_AMC		0x0800
#define RCR_LB1		0x0400
#define RCR_LB0		0x0200
#define RCR_MC		0x0100
#define RCR_BC		0x0080
#define RCR_LPKT	0x0040
#define RCR_CRS		0x0020
#define RCR_COL		0x0010
#define RCR_CRCR	0x0008
#define RCR_FAER	0x0004
#define RCR_LBK		0x0002
#define RCR_PRX		0x0001

/*
 * Transmit control register
 */
#define TCR_PINT	0x8000
#define TCR_POWC	0x4000
#define TCR_CRCI	0x2000
#define TCR_EXDIS	0x1000
#define TCR_EXD		0x0400
#define TCR_DEF		0x0200
#define TCR_NCRS	0x0100
#define TCR_CRSL	0x0080
#define TCR_EXC		0x0040
#define TCR_OWC		0x0020
#define TCR_PMB		0x0008
#define TCR_FU		0x0004
#define TCR_BCM		0x0002
#define TCR_PTX		0x0001

/*
 * Interrupt mask register
 */
#define IMR_BREN	0x4000
#define IMR_HBLEN	0x2000
#define IMR_LCDEN	0x1000
#define IMR_PINTEN	0x0800
#define IMR_PRXEN	0x0400
#define IMR_PTXEN	0x0200
#define IMR_TXEREN	0x0100
#define IMR_TCEN	0x0080
#define IMR_RDEEN	0x0040
#define IMR_RBEEN	0x0020
#define IMR_RBAEEN	0x0010
#define IMR_CRCEN	0x0008
#define IMR_FAEEN	0x0004
#define IMR_MPEN	0x0002
#define IMR_RFOEN	0x0001

/*
 * Interrupt status register
 */
#define ISR_BR		0x4000
#define ISR_HBL		0x2000
#define ISR_LCD		0x1000
#define ISR_PINT	0x0800
#define ISR_PKTRX	0x0400
#define ISR_TXDN	0x0200
#define ISR_TXER	0x0100
#define ISR_TC		0x0080
#define ISR_RDE		0x0040
#define ISR_RBE		0x0020
#define ISR_RBAE	0x0010
#define ISR_CRC		0x0008
#define ISR_FAE		0x0004
#define ISR_MP		0x0002
#define ISR_RFO		0x0001

/*
 * Data configuration register 2
 */
#define DCR2_EXPO3	0x8000
#define DCR2_EXPO2	0x4000
#define DCR2_EXPO1	0x2000
#define DCR2_EXPO0	0x1000
#define DCR2_PH		0x0010
#define DCR2_PCM	0x0004
#define DCR2_PCNM	0x0002
#define DCR2_RJCM	0x0001

/*
 ******************************************************************
 *                                                                *
 *                   Transmit Buffer Management                   *
 *                                                                *
 ******************************************************************
 */

/*
 * Transmit descriptor area entry.
 * There is one transmit descriptor for each packet to be transmitted.
 * Statically reserve space for up to MAXIMUM_FRAGS_PER_PACKET fragments
 * per descriptor.
 */
#define MAXIMUM_FRAGS_PER_DESCRIPTOR	6
struct TransmitDescriptor {
	rtems_unsigned32	status;
	rtems_unsigned32	pkt_config;
	rtems_unsigned32	pkt_size;
	rtems_unsigned32	frag_count;

	/*
	 * Packet fragment pointers
	 */
	struct TransmitDescriptorFragLink {
	    rtems_unsigned32 frag_lsw;	/* LSW of fragment address */
#define			     frag_link frag_lsw
	    rtems_unsigned32 frag_msw;	/* MSW of fragment address */
	    rtems_unsigned32 frag_size;
	}			frag[MAXIMUM_FRAGS_PER_DESCRIPTOR];

	/*
	 * Space for link if all fragment pointers are used.
	 */
	rtems_unsigned32 	link_pad;

	/*
	 * Extra RTEMS/KA9Q stuff 
	 */
	struct TransmitDescriptor	*next;	/* Circularly-linked list */
	struct mbuf			*mbufp;	/* First mbuf in packet */
	volatile rtems_unsigned32	*linkp;	/* Pointer to un[xxx].link */
};
typedef struct TransmitDescriptor TransmitDescriptor_t;
typedef volatile TransmitDescriptor_t *TransmitDescriptorPointer_t;

/*
 * Transmit Configuration.
 * For standard Ethernet transmission, all bits in the transmit
 * configuration field are set to 0.
 */
#define TDA_CONFIG_PINT	0x8000
#define TDA_CONFIG_POWC	0x4000
#define TDA_CONFIG_CRCI	0x2000
#define TDA_CONFIG_EXDIS	0x1000

/*
 * Transmit status
 */
#define TDA_STATUS_COLLISION_MASK	0xF800
#define TDA_STATUS_COLLISION_SHIFT	11
#define TDA_STATUS_EXD		0x0400
#define TDA_STATUS_DEF		0x0200
#define TDA_STATUS_NCRS		0x0100
#define TDA_STATUS_CRSL		0x0080
#define TDA_STATUS_EXC		0x0040
#define TDA_STATUS_OWC		0x0020
#define TDA_STATUS_PMB		0x0008
#define TDA_STATUS_FU		0x0004
#define TDA_STATUS_BCM		0x0002
#define TDA_STATUS_PTX		0x0001

#define TDA_LINK_EOL 0x1



/*
 ******************************************************************
 *                                                                *
 *                    Receive Buffer Management                   *
 *                                                                *
 ******************************************************************
 */

/*
 * Receive resource area entry.
 * There is one receive resource entry for each receive buffer area (RBA).
 * This driver allows only one packet per receive buffer area, so one
 * receive resource entry corresponds to one correctly-received packet.
 */
struct ReceiveResource {
	rtems_unsigned32	buff_ptr_lsw;	/* LSW of RBA address */
	rtems_unsigned32	buff_ptr_msw;	/* MSW of RBA address */
	rtems_unsigned32	buff_wc_lsw;	/* LSW of RBA size (16-bit words) */
	rtems_unsigned32	buff_wc_msw;	/* MSW of RBA size (16-bit words) */
};
typedef struct ReceiveResource ReceiveResource_t;
typedef volatile ReceiveResource_t *ReceiveResourcePointer_t;

/*
 * Receive descriptor area entry.
 * There is one receive descriptor for each packet received.
 */
struct ReceiveDescriptor {
	rtems_unsigned32	status;
	rtems_unsigned32	byte_count;
	rtems_unsigned32	pkt_lsw;	/* LSW of packet address */
	rtems_unsigned32	pkt_msw;	/* MSW of packet address */
	rtems_unsigned32	seq_no;
	rtems_unsigned32	link;
	rtems_unsigned32	in_use;

	/*
	 * Extra RTEMS/KA9Q stuff 
	 */
	struct ReceiveDescriptor	*next;	/* Circularly-linked list */
};
typedef struct ReceiveDescriptor ReceiveDescriptor_t;
typedef volatile ReceiveDescriptor_t *ReceiveDescriptorPointer_t;

/*
 * Receive status
 */
#define RDA_STATUS_ERR		0x8800
#define RDA_STATUS_RNT		0x4000
#define RDA_STATUS_BRD		0x2000
#define RDA_STATUS_PRO		0x1000
#define RDA_STATUS_AMC		0x0800
#define RDA_STATUS_LB1		0x0400
#define RDA_STATUS_LB0		0x0200
#define RDA_STATUS_MC		0x0100
#define RDA_STATUS_BC		0x0080
#define RDA_STATUS_LPKT		0x0040
#define RDA_STATUS_CRS		0x0020
#define RDA_STATUS_COL		0x0010
#define RDA_STATUS_CRCR		0x0008
#define RDA_STATUS_FAER		0x0004
#define RDA_STATUS_LBK		0x0002
#define RDA_STATUS_PRX		0x0001

#define RDA_LINK_EOL 0x1
	
#endif /* _SONIC_DP83932_ */
