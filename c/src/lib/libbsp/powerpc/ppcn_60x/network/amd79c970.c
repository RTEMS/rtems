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
/*
 * RTEMS/KA9Q driver for PC-NET
 */
#include <bsp.h>
#include <rtems/error.h>
#include <ka9q/rtems_ka9q.h>
#include <ka9q/global.h>
#include <ka9q/enet.h>
#include <ka9q/iface.h>
#include <ka9q/netuser.h>
#include <ka9q/trace.h>
#include <ka9q/commands.h>

#include <pci.h>
#include "amd79c970.h"

/*
 * Number of PC-NETs supported by this driver
 */
#define NPCNETDRIVER	1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 *
 * Set the number of Tx and Rx buffers, using Log_2(# buffers). 
 */
#define LANCE_LOG2_TX_BUFFERS 4
#define LANCE_LOG2_RX_BUFFERS 4
#define TX_RING_SIZE			(1 << (LANCE_LOG2_TX_BUFFERS))
#define TX_RING_MOD_MASK		(TX_RING_SIZE - 1)
#define TX_RING_LEN_BITS		((LANCE_LOG2_TX_BUFFERS) << 4)
#define RX_RING_SIZE			(1 << (LANCE_LOG2_RX_BUFFERS))
#define RX_RING_MOD_MASK		(RX_RING_SIZE - 1)
#define RX_RING_LEN_BITS		((LANCE_LOG2_RX_BUFFERS) << 4)

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the KA9Q task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * Receive buffer size -- Allow for a full ethernet packet plus a pointer
 */
#define ETHPKT_SIZE	1520
#define RBUF_SIZE	(ETHPKT_SIZE + sizeof (struct iface *))

/*
 * LANCE Register Access Macros
 */
#define PCNET_IO_RD32(dp, reg, value)	\
	inport_32(&dp->pPCNet->u.dwio.##reg, value)
#define PCNET_IO_WR32(dp, reg, value)	\
	outport_32(&dp->pPCNet->u.dwio.##reg, value)

/*
 * LANCE Register Access Macros
 */
#define RD_CSR32(dp, index, value) \
	PCNET_IO_WR32(dp, rap, index); \
	PCNET_IO_RD32(dp, rdp, value)
	
#define WR_CSR32(dp, index, value) \
	PCNET_IO_WR32(dp, rap, index); \
	PCNET_IO_WR32(dp, rdp, value)
	
#define RD_BCR32(dp, index, value) \
	PCNET_IO_WR32(dp, rap, index); \
	PCNET_IO_RD32(dp, bdp, value)

#define WR_BCR32(dp, index, value) \
	PCNET_IO_WR32(dp, rap, index); \
	PCNET_IO_WR32(dp, bdp, value)

/*
 * Hardware-specific storage
 *
 * Note that the enetInitBlk field must be aligned to a 16 byte
 * boundary
 */
typedef struct amd79c970Context {
	rmde_t		rxBdBase[RX_RING_SIZE];
	tmde_t		txBdBase[TX_RING_SIZE];
	initblk_t		initBlk;
	pc_net_t		*pPCNet;
	unsigned32		ulIntVector;
	struct mbuf		**rxMbuf;
	struct mbuf		**txMbuf;
	int			rxBdCount;
	int			txBdCount;
	int			txBdHead;
	int			txBdTail;
	int			txBdActiveCount;
	struct iface		*iface;
	rtems_id		txWaitTid;

	/*
	 * Statistics
	 */
	unsigned long	rxInterrupts;
	unsigned long	rxNotFirst;
	unsigned long	rxNotLast;
	unsigned long	rxGiant;
	unsigned long	rxNonOctet;
	unsigned long	rxRunt;
	unsigned long	rxBadCRC;
	unsigned long	rxOverrun;
	unsigned long	rxCollision;
	unsigned long	rxDiscarded;

	unsigned long	txInterrupts;
	unsigned long	txDeferred;
	unsigned long	txHeartbeat;
	unsigned long	txLateCollision;
	unsigned long	txRetryLimit;
	unsigned long	txUnderrun;
	unsigned long	txLostCarrier;
	unsigned long	txRawWait;
} amd79c970Context_t;
static amd79c970Context_t *pAmd79c970Context[NPCNETDRIVER];

/*
 * PC-NET interrupt handler
 */
static rtems_isr
amd79c970_isr (rtems_vector_number v)
{
	unsigned32 ulCSR0, ulCSR4, ulCSR5;
	amd79c970Context_t *dp;
	int i;

	for(i=0; i<NPCNETDRIVER; i++)
	{
		dp=pAmd79c970Context[i];
		if(dp->ulIntVector==v)
		{
			RD_CSR32(dp, CSR0, ulCSR0);
			if(ulCSR0 & CSR0_RINT)
			{
				/*
				 * We have recieve data
				 */
				dp->rxInterrupts++;
				rtems_event_send(
					dp->iface->rxproc,
					INTERRUPT_EVENT);
			}

			if(ulCSR0 & CSR0_TINT)
			{
				/*
				 * Data tranmitted or error
				 */
				dp->txInterrupts++;
				if(dp->txWaitTid)
				{
					rtems_event_send(
						dp->txWaitTid,
						INTERRUPT_EVENT);
				}
			}

			if((ulCSR0 & CSR0_INTR) &&
			   !(ulCSR0 & (CSR0_RINT | CSR0_TINT)))
			{
				/*
				 * Many possible sources
				 */
				RD_CSR32(dp, CSR4, ulCSR4);
				RD_CSR32(dp, CSR5, ulCSR5);
				DEBUG_puts("CSR0=");
				DEBUG_puth(ulCSR0);
				DEBUG_puts(", CSR4=");
				DEBUG_puth(ulCSR4);
				DEBUG_puts(", CSR5=");
				DEBUG_puth(ulCSR5);
				DEBUG_puts("\n\r");
				/*
				 * Clear it
				 */
				WR_CSR32(dp, CSR4, ulCSR4);
				WR_CSR32(dp, CSR5, ulCSR5);
			}

			/*
			 * Clear interrupts
			 */
			ulCSR0&=CSR0_BABL | CSR0_CERR | CSR0_MISS |
				CSR0_MERR | CSR0_RINT | CSR0_TINT | CSR0_IENA;
			WR_CSR32(dp, CSR0, ulCSR0);

			RD_CSR32(dp, CSR0, ulCSR0);
		}
	}
}

/*
 * Initialize the ethernet hardware
 */
static boolean
amd79c970_initialize_hardware (int instance, int broadcastFlag)
{
	amd79c970Context_t *dp;
	struct mbuf 	*bp;
	int		i;
	unsigned8	ucPCIBusCount;
	unsigned8	ucBusNumber;
	unsigned8	ucSlotNumber;
	unsigned32	ulDeviceID;
	unsigned32	ulBAR0;
	unsigned8	ucIntVector;
	unsigned32	ulInitClkPCIAddr;
	unsigned32	ulAPROM;
	unsigned32	ulCSR0;

	ucPCIBusCount=BusCountPCI();

	/*
	 * Scan the available busses for instance of hardware
	 */
	i=0;

	dp=pAmd79c970Context[instance];
	dp->pPCNet=(pc_net_t *)NULL;

	for(ucBusNumber=0;
	    (ucBusNumber<ucPCIBusCount) && (dp->pPCNet==(pc_net_t *)NULL);
	    ucBusNumber++)
	{
		for(ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++)
		{
			PCIConfigRead32(ucBusNumber,
					ucSlotNumber,
					0,
					PCI_CONFIG_VENDOR_LOW,
					&ulDeviceID);
			if(ulDeviceID!=PCI_ID(0x1022, 0x2000))
			{
				continue;
			}

			/*
			 * We've found a PC-NET controller
			 */
			if(i++<instance)
			{
				continue;
			}

			/*
			 * Read base address
			 */
			PCIConfigRead32(ucBusNumber,
					ucSlotNumber,
					0,
					PCI_CONFIG_BAR_0,
					&ulBAR0);
			dp->pPCNet=(pc_net_t *)(ulBAR0&~PCI_ADDRESS_IO_SPACE);

			/*
			 * Read interrupt vector
			 */
			PCIConfigRead8(ucBusNumber,
				       ucSlotNumber,
				       0,
				       PCI_CONFIG_INTERRUPTLINE,
				       &ucIntVector);
			dp->ulIntVector=PPCN_60X_IRQ_PCI(ucIntVector);

			/*
			 * Ensure that device is enabled
			 */
			PCIConfigWrite16(ucBusNumber,
					 ucSlotNumber,
					 0,
					 PCI_CONFIG_COMMAND,
					 PCI_ENABLE_IO_SPACE |
					 PCI_ENABLE_BUS_MASTER);
			break;
		}
	}

	if(dp->pPCNet==(pc_net_t *)NULL)
	{
		return(FALSE);
	}

	/*
	 * Read the ethernet number
	 */
	if(!dp->iface->hwaddr)
	{
		dp->iface->hwaddr=mallocw (EADDR_LEN);
		PCNET_IO_RD32(dp, aprom[0], ulAPROM);
		for(i=0;i<4;i++)
		{
			dp->iface->hwaddr[i]=(ulAPROM>>(i*8))&0xff;
		}
		PCNET_IO_RD32(dp, aprom[1], ulAPROM);
		for(i=0;i<2;i++)
		{
			dp->iface->hwaddr[i+4]=(ulAPROM>>(i*8))&0xff;
		}
	}

	/*
	 * Allocate mbuf pointers
	 */
	dp->rxMbuf=mallocw (dp->rxBdCount * sizeof(*dp->rxMbuf));
	dp->txMbuf=mallocw (dp->txBdCount * sizeof(*dp->txMbuf));

	/*
	 * Allocate space for incoming packets
	 */
	for(i=0; i<dp->rxBdCount; i++)
	{
		dp->rxMbuf[i]=bp=ambufw (RBUF_SIZE);
		bp->data += sizeof (struct iface *);
                dp->rxBdBase[i].rmde_addr=
			Swap32((unsigned32)bp->data+PCI_SYS_MEM_BASE);
                dp->rxBdBase[i].rmde_bcnt=
			Swap16(-(bp->size-sizeof (struct iface *)));
                dp->rxBdBase[i].rmde_flags=Swap16(RFLG_OWN);
	}

	/*
	 * Set up transmit buffer descriptors
	 */
	for(i=0; i<dp->txBdCount; i++)
	{
                dp->txBdBase[i].tmde_status=Swap16(TST_STP | TST_ENP);
                dp->txBdBase[i].tmde_error=0;
		dp->txMbuf[i]=NULL;
	}

	/*
	 * Initialise initblk
	 */
	if(broadcastFlag)
	{
		dp->initBlk.ib_mode=0;
	}
	else
	{
		dp->initBlk.ib_mode=Swap16(CSR15_DRCVBC);
	}

	/*
	 * Set the receive descriptor ring length 
	 */
	dp->initBlk.ib_rlen=RX_RING_LEN_BITS;
	/*
	 * Set the receive descriptor ring address 
	 */
	dp->initBlk.ib_rdra=Swap32((unsigned32)&dp->rxBdBase[0]+
				     PCI_SYS_MEM_BASE);

	/*
	 * Set the transmit descriptor ring length 
	 */
	dp->initBlk.ib_tlen=TX_RING_LEN_BITS;
	/*
	 * Set the tranmit descriptor ring address 
	 */
	dp->initBlk.ib_tdra=Swap32((unsigned32)&dp->txBdBase[0]+
				     PCI_SYS_MEM_BASE);

	for(i=0;i<6;i++)
	{
		dp->initBlk.ib_padr[i]=dp->iface->hwaddr[i];
	}
	
	/*
	 * Ensure that we are in DWIO mode
	 */
	PCNET_IO_WR32(dp, rdp, 0);

	WR_CSR32(dp, 58,CSR58_PCISTYLE);

	WR_CSR32(dp, CSR3,
		 CSR3_BABLM | CSR3_MERRM | CSR3_IDONM | CSR3_DXSUFLO);

	WR_CSR32(dp, CSR4,
		 CSR4_APADXMIT | CSR4_MFCOM | CSR4_RCVCCOM |
		 CSR4_TXSTRTM | CSR4_JABM);

	WR_CSR32(dp, CSR5, 0);

	ulInitClkPCIAddr=(unsigned32)&dp->initBlk+PCI_SYS_MEM_BASE;
	/*
	 * CSR2 must contain the high order 16 bits of the first word in 
	 * the initialization block 
	 */
	WR_CSR32(dp, CSR2, (ulInitClkPCIAddr >> 16) & 0xffff);
	/*
	 * CSR1 must contain the low order 16 bits of the first word in 
	 * the initialization block 
	 */
	WR_CSR32(dp, CSR1, (ulInitClkPCIAddr & 0xffff));

	/*
	 * Set up interrupts
	 */
	set_vector(amd79c970_isr,
		   dp->ulIntVector,
		   instance);

	/*
	 * Start the device
	 */
	WR_CSR32(dp, CSR0, CSR0_INIT | CSR0_STRT);

	/*
	 * Wait for 100mS for the device to initialise
	 */
	for(i=0; i<100; i++)
	{
		RD_CSR32(dp, CSR0, ulCSR0);
		if(ulCSR0 & CSR0_IDON)
		{
			break;
		}
		rtems_ka9q_ppause(1); /* 1mS */
	}
	if(i >= 100)
	{
		return(FALSE);
	}

	/*
	 * Enable interrupts
	 */
	WR_CSR32(dp, CSR0, CSR0_IENA);

	dp->txBdHead=dp->txBdTail=0;
	dp->txBdActiveCount=0;

	return(TRUE);
}

/*
 * Soak up buffer descriptors that have been sent
 */
static void
amd79c970_retire_tx_bd (amd79c970Context_t *dp)
{
	unsigned16 status;
	unsigned32 error;
	int i;
	int nRetired;

	i = dp->txBdTail;
	nRetired = 0;
	while((dp->txBdActiveCount != 0) &&
	      (((status=Swap16(dp->txBdBase[i].tmde_status)) & TST_OWN)==0))
	{
		/*
		 * See if anything went wrong
		 */
		if(status & TST_ERR)
		{
			/*
			 * Check for errors
			 */
			error=Swap16(dp->txBdBase[i].tmde_error);

			if (error & TERR_LCOL)
				dp->txLateCollision++;
			if (error & TERR_RTRY)
				dp->txRetryLimit++;
			if (error & TERR_UFLO)
				dp->txUnderrun++;
			if (error & TERR_EXDEF)
				dp->txDeferred++;
			if (error & TERR_LCAR)
				dp->txLostCarrier++;
		}
		nRetired++;
		if (status & TST_ENP)
		{
			/*
			 * A full frame has been transmitted.
			 * Free all the associated buffer descriptors.
			 */
			dp->txBdActiveCount -= nRetired;
			while (nRetired) {
				nRetired--;
				free_mbuf (&dp->txMbuf[dp->txBdTail]);
				if (++dp->txBdTail == dp->txBdCount)
					dp->txBdTail = 0;
			}
		}
		if (++i == dp->txBdCount)
		{
			i = 0;
		}
	}
}

/*
 * Send raw packet (caller provides header).
 * This code runs in the context of the interface transmit
 * task or in the context of the network task.
 */
static int
amd79c970_raw (struct iface *iface, struct mbuf **bpp)
{
	amd79c970Context_t *dp;
	struct mbuf *bp;
	tmde_t *firstTxBd, *txBd;
	unsigned16 status;
	int nAdded;

	dp = pAmd79c970Context[iface->dev];

	/*
	 * Fill in some logging data
	 */
	iface->rawsndcnt++;
	iface->lastsent = secclock ();
	dump (iface, IF_TRACE_OUT, *bpp);

	/*
	 * It would not do to have two tasks active in the transmit
	 * loop at the same time.
	 * The blocking is simple-minded since the odds of two tasks
	 * simultaneously attempting to use this code are low.  The only
	 * way that two tasks can try to run here is:
	 *	1) Task A enters this code and ends up having to
	 *	   wait for a transmit buffer descriptor.
	 *	2) Task B  gains control and tries to transmit a packet.
	 * The RTEMS/KA9Q scheduling semaphore ensures that there
	 * are no race conditions associated with manipulating the
	 * txWaitTid variable.
	 */
	if (dp->txWaitTid) {
		dp->txRawWait++;
		while (dp->txWaitTid)
			rtems_ka9q_ppause (10);
	}

	/*
	 * Free up buffer descriptors
	 */
	amd79c970_retire_tx_bd (dp);

	/*
	 * Set up the transmit buffer descriptors.
	 * No need to pad out short packets since the
	 * hardware takes care of that automatically.
	 * No need to copy the packet to a contiguous buffer
	 * since the hardware is capable of scatter/gather DMA.
	 */
	bp = *bpp;
	nAdded = 0;
	txBd = firstTxBd = dp->txBdBase + dp->txBdHead;
	for (;;) {
		/*
		 * Wait for buffer descriptor to become available.
		 */
		if ((dp->txBdActiveCount + nAdded) == dp->txBdCount) {
			/*
			 * Find out who we are
			 */
			if (dp->txWaitTid == 0)
				rtems_task_ident (0, 0, &dp->txWaitTid);

			/*
			 * Wait for buffer descriptor to become available.
			 * Note that the buffer descriptors are checked
			 * *before* * entering the wait loop -- this catches
			 * the possibility that a buffer descriptor became
			 * available between the `if' above, and the clearing
			 * of the event register.
			 * Also, the event receive doesn't wait forever.
			 * This is to catch the case where the transmitter
			 * stops in the middle of a frame -- and only the
			 * last buffer descriptor in a frame can generate
			 * an interrupt.
			 */
			amd79c970_retire_tx_bd (dp);
			while ((dp->txBdActiveCount + nAdded) == dp->txBdCount) {
				rtems_ka9q_event_receive (INTERRUPT_EVENT,
					RTEMS_WAIT|RTEMS_EVENT_ANY,
					1 + 1000000/BSP_Configuration.microseconds_per_tick);
				amd79c970_retire_tx_bd (dp);
			}
		}

		/*
		 * Fill in the buffer descriptor
		 */
		txBd->tmde_addr=Swap32((unsigned32)bp->data+PCI_SYS_MEM_BASE);
		txBd->tmde_bcnt=Swap16(-bp->cnt);
		dp->txMbuf[dp->txBdHead] = bp;

		nAdded++;
		if (++dp->txBdHead == dp->txBdCount)
		{
			dp->txBdHead = 0;
		}

		/*
		 * Set the transmit buffer status.
		 * Break out of the loop if this mbuf is the last in the frame.
		 */
		if ((bp = bp->next) == NULL) {
			if(txBd==firstTxBd)
			{
				/*
				 * There is only one frame
				 */
				txBd->tmde_status=Swap16(TST_OWN |
							 TST_STP |
							 TST_ENP);
			}
			else
			{
				/*
				 * Mark the last buffer
				 */
				txBd->tmde_status=Swap16(TST_OWN |
							 TST_ENP);
				/*
				 * Trigger the first transmit
				 */
				firstTxBd->tmde_status=Swap16(TST_OWN |
							      TST_STP);
			}
			/*
			 * Sync instruction required to overcome the Grackle
			 * stale data bug
			 */
			asm volatile("sync");
			dp->txBdActiveCount += nAdded;
			break;
		}
		else if(txBd!=firstTxBd)
		{
			txBd->tmde_status = Swap16(TST_OWN);
		}
		txBd = dp->txBdBase + dp->txBdHead;
	}

	/*
	 * Show that we've finished with the packet
	 */
	dp->txWaitTid = 0;
	*bpp = NULL;
	return 0;
}

/*
 * PC-NET reader task
 */
static void
amd79c970_rx (int dev, void *p1, void *p2)
{
	struct iface *iface=(struct iface *)p1;
	amd79c970Context_t *dp=(amd79c970Context_t *)p2;
	struct mbuf *bp;
	rtems_unsigned16 status;
	rmde_t *rxBd;
	int rxBdIndex;
	int continuousCount;

	/*
	 * Input packet handling loop
	 */
	continuousCount=0;
	rxBdIndex=0;

	while(TRUE)
	{
		rxBd=&dp->rxBdBase[rxBdIndex];

		/*
		 * Wait for packet if there's not one ready
		 */
		if((status=Swap16(rxBd->rmde_flags)) & RFLG_OWN)
		{
			/*
			 * Reset `continuous-packet' count
			 */
			continuousCount=0;

			/*
			 * Wait for packet
			 * Note that the buffer descriptor is checked
			 * *before* the event wait -- this catches the
			 * possibility that a packet arrived between the
			 * `if' above, and the clearing of the event register.
			 */
			while ((status=Swap16(rxBd->rmde_flags)) & RFLG_OWN)
			{
				rtems_ka9q_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT);
			}
		}

		/*
		 * Check that packet is valid
		 */
		if((status & RFLG_ERR) ||
		   ((status & (RFLG_STP|RFLG_ENP)) != (RFLG_STP|RFLG_ENP)))
		{
			/*
			 * Something went wrong with the reception
			 */
			if(!(status & RFLG_ENP))
				dp->rxNotLast++;
			if(!(status & RFLG_STP))
				dp->rxNotFirst++;
			if(status & RFLG_OFLO)
				dp->rxGiant++;
			if(status & RFLG_FRAM)
				dp->rxNonOctet++;
			if(status & RFLG_CRC)
				dp->rxBadCRC++;
			if(status & RFLG_BUFF)
				dp->rxOverrun++;
		}
		else
		{
			/*
			 * Pass the packet up the chain
			 * The mbuf count is reduced to remove
			 * the frame check sequence at the end
			 * of the packet.
			 */
			bp=dp->rxMbuf[rxBdIndex];
			bp->cnt=Swap16(rxBd->rmde_mcnt) - sizeof (uint32);
			net_route (iface, &bp);

			/*
			 * Give the network code a chance to digest the
			 * packet.  This guards against a flurry of 
			 * incoming packets (usually an ARP storm) from
			 * using up all the available memory.
			 */
			if(++continuousCount >= dp->rxBdCount)
				kwait_null ();

			/*
			 * Allocate a new mbuf
			 * FIXME: It seems to me that it would be better
			 * if there were some way to limit number of mbufs
			 * in use by this interface, but I don't see any
			 * way of determining when the mbuf we pass up
			 * is freed.
			 */
			dp->rxMbuf[rxBdIndex]=bp=ambufw (RBUF_SIZE);
			bp->data += sizeof (struct iface *);
			rxBd->rmde_addr=Swap32(
				(unsigned32)bp->data+PCI_SYS_MEM_BASE);
			rxBd->rmde_bcnt=Swap16(
				-(bp->size-sizeof (struct iface *)));
		}

		/*
		 * Reenable the buffer descriptor
		 */
		rxBd->rmde_flags=Swap16(RFLG_OWN);

		/*
		 * Move to next buffer descriptor
		 */
		if(++rxBdIndex==dp->rxBdCount)
			rxBdIndex=0;
	}
}

/*
 * Shut down the interface
 * FIXME: This is a pretty simple-minded routine.  It doesn't worry
 * about cleaning up mbufs, shutting down daemons, etc.
 */
static int
amd79c970_stop (struct iface *iface)
{
	amd79c970Context_t *dp;
	unsigned32	ulCSR0;
	int		i;

	dp=pAmd79c970Context[iface->dev];

	/*
	 * Stop the device
	 */
	WR_CSR32(dp, CSR0, CSR0_STOP);

	/*
	 * Wait for 100mS for the device to stop
	 */
	for(i=0; i<100; i++)
	{
		RD_CSR32(dp, CSR0, ulCSR0);
		if(!(ulCSR0 & (CSR0_RXON | CSR0_TXON)))
		{
			break;
		}
		rtems_ka9q_ppause(1); /* 1mS */
	}
	if(i >= 100)
	{
		return(-1);
	}

	/*
	 * Free up all the mbufs we've allocated
	 */
	for(i=0; i<dp->rxBdCount; i++)
	{
		free_mbuf(&dp->rxMbuf[i]);
	}

	return 0;
}

/*
 * Show interface statistics
 */
static void
amd79c970_show (struct iface *iface)
{
	int i;

	i=iface->dev;

	printf ("      Rx Interrupts:%-8lu", pAmd79c970Context[i]->rxInterrupts);
	printf ("       Not First:%-8lu", pAmd79c970Context[i]->rxNotFirst);
	printf ("        Not Last:%-8lu\n", pAmd79c970Context[i]->rxNotLast);
	printf ("              Giant:%-8lu", pAmd79c970Context[i]->rxGiant);
	printf ("            Runt:%-8lu", pAmd79c970Context[i]->rxRunt);
	printf ("       Non-octet:%-8lu\n", pAmd79c970Context[i]->rxNonOctet);
	printf ("            Bad CRC:%-8lu", pAmd79c970Context[i]->rxBadCRC);
	printf ("         Overrun:%-8lu", pAmd79c970Context[i]->rxOverrun);
	printf ("       Collision:%-8lu\n", pAmd79c970Context[i]->rxCollision);
	printf ("          Discarded:%-8lu\n", pAmd79c970Context[i]->rxDiscarded);

	printf ("      Tx Interrupts:%-8lu", pAmd79c970Context[i]->txInterrupts);
	printf ("        Deferred:%-8lu", pAmd79c970Context[i]->txDeferred);
	printf (" Missed Hearbeat:%-8lu\n", pAmd79c970Context[i]->txHeartbeat);
	printf ("         No Carrier:%-8lu", pAmd79c970Context[i]->txLostCarrier);
	printf ("Retransmit Limit:%-8lu", pAmd79c970Context[i]->txRetryLimit);
	printf ("  Late Collision:%-8lu\n", pAmd79c970Context[i]->txLateCollision);
	printf ("           Underrun:%-8lu", pAmd79c970Context[i]->txUnderrun);
	printf (" Raw output wait:%-8lu\n", pAmd79c970Context[i]->txRawWait);
}

/*
 * Attach an PC-NET driver to the system
 * This is the only `extern' function in the driver.
 *
 * argv[0]: interface label, e.g., "amd79c970"
 * argv[1]: maximum transmission unit, bytes, e.g., "1500"
 * argv[2]: accept ("broadcast") or ignore ("nobroadcast") broadcast packets
 * Following arguments are optional, but if present, must appear in
 * the following order:
 * Following arguments are optional, but if Ethernet address is
 * specified, Internet address must also be specified. 
 * ###.###.###.###   -- IP address
 * ##:##:##:##:##:## -- Ethernet address
 */
int
rtems_ka9q_driver_attach (int argc, char *argv[], void *p)
{
	struct iface *iface;
	struct amd79c970Context *dp;
	char *cp;
	int i;
	int argIndex;
	int broadcastFlag;

	/*
	 * Find a free driver
	 */
	for(i=0; i<NPCNETDRIVER; i++)
	{
		if(pAmd79c970Context[i]==NULL)
			break;
	}
	if(i==NPCNETDRIVER)
	{
		printf ("Too many PC-NET drivers.\n");
		return -1;
	}
	if(if_lookup (argv[0]) != NULL)
	{
		printf ("Interface %s already exists\n", argv[0]);
		return -1;
	}

	/*
	 * Note that this structure must be aligned to a 16 byte boundary
	 */
	pAmd79c970Context[i]=(amd79c970Context_t *)
		(((unsigned32)callocw(1,
				      sizeof(amd79c970Context_t)+16)+16) & ~15);
	dp=pAmd79c970Context[i];

	/*
	 * Create an interface descriptor
	 */
	iface=callocw (1, sizeof *iface);
	iface->name=strdup (argv[0]);
	iface->mtu=atoi (argv[1]);

	/*
	 * Select broadcast packet handling
	 */
	cp=argv[2];
	if(strnicmp (cp, "broadcast", strlen (cp))==0)
	{
		broadcastFlag=1;
	}
	else if(strnicmp (cp, "nobroadcast", strlen (cp))==0)
	{
		broadcastFlag=0;
	}
	else {
		printf ("Argument `%s' is neither `broadcast' nor `nobroadcast'.\n", cp);
		return -1;
	}
	argIndex=3;

	/*
	 * Set receive buffer descriptor count
	 */
	dp->rxBdCount=RX_RING_SIZE;
		
	/*
	 * Set transmit buffer descriptor count
	 */
	dp->txWaitTid=0;
	dp->txBdCount=TX_RING_SIZE;

	/*
	 * Set Internet address
	 */
	if(argIndex<argc)
		iface->addr=resolve (argv[argIndex++]);
	else
		iface->addr=Ip_addr;

	/*
	 * Set Ethernet address
	 */
	if(argIndex<argc)
	{
		iface->hwaddr=mallocw (EADDR_LEN);
		gether (iface->hwaddr, argv[argIndex++]);
	}

	iface->dev=i;
	iface->raw=amd79c970_raw;
	iface->stop=amd79c970_stop;
	iface->show=amd79c970_show;
	dp->iface=iface;
	setencap (iface, "Ethernet");

	/*
	 * Set up PC-NET hardware
	 */
	if(!amd79c970_initialize_hardware (i, broadcastFlag))
	{
		printf ("Unable to initialize hardware for %s\n", argv[0]);
		return -1;
	}

	/*
	 * Chain onto list of interfaces
	 */
	iface->next=Ifaces;
	Ifaces=iface;

	/*
	 * Start I/O daemons
	 */
	cp=if_name (iface, " tx");
	iface->txproc=newproc (cp, 1024, if_tx, iface->dev, iface, NULL, 0);
	free (cp);
	cp=if_name (iface, " rx");
	iface->rxproc=newproc (cp, 1024, amd79c970_rx, iface->dev, iface, dp, 0);
	free (cp);
	return 0;
}

/*
 * FIXME: There should be an ioctl routine to allow things like
 * enabling/disabling reception of broadcast packets.
 */
