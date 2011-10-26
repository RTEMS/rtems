/*===============================================================*\
| Project: RTEMS generic MCF548X BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2009                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the networking driver                        |
\*===============================================================*/
/*
 *  RTEMS/TCPIP driver for MCF548X FEC Ethernet
 *
 *  Modified for Motorola MPC5200 by Thomas Doerfler, <Thomas.Doerfler@imd-systems.de>
 *  COPYRIGHT (c) 2003, IMD
 *
 *  Modified for Motorola IceCube (mgt5100) by Peter Rasmussen <prasmus@ipr-engineering.de>
 *  COPYRIGHT (c) 2003, IPR Engineering
 *
 *  Parts of code are also under property of Driver Information Systems and based
 *  on Motorola Proprietary Information.
 *  COPYRIGHT (c) 2002 MOTOROLA INC.
 *
 *  Modified for Motorola MCF548X by Thomas Doerfler, <Thomas.Doerfler@imd-systems.de>
 *  COPYRIGHT (c) 2009, IMD
 *
 */
#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <stdio.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if_var.h>

#include <bsp.h>
#include <mcf548x/mcf548x.h>
#include <rtems/rtems_mii_ioctl.h>
#include <errno.h>

/* freescale-api-specifics... */
#include <mcf548x/MCD_dma.h>
#include <mcf548x/mcdma_glue.h>

#define ETH_PROMISCOUS_MODE 1 /* FIXME: remove me */

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 2

#define FEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */
/*
 * buffer descriptor handling
 */

#define SET_BD_STATUS(bd, stat)	{		\
    (bd)->statCtrl = stat;			\
}
#define SET_BD_LENGTH(bd, len) {		\
    (bd)->length = len;				\
}
#define SET_BD_BUFFER(bd, buf) {		\
    (bd)->dataPointer= (uint32_t)(buf);		\
}
#define GET_BD_STATUS(bd)		((bd)->statCtrl)
#define GET_BD_LENGTH(bd)		((bd)->length)
#define GET_BD_BUFFER(bd)		((void *)((bd)->dataPointer))

#define DMA_BD_RX_NUM	32 /* Number of receive buffer descriptors	*/
#define DMA_BD_TX_NUM	32 /* Number of transmit buffer descriptors	*/

/*
 * internal SRAM
 * Layout:
 * - RxBD channel 0
 * - TxBD channel 0
 * - RxBD channel 1
 * - TxBD channel 1
 * - DMA task memory
 */
extern char _SysSramBase[];
#define SRAM_RXBD_BASE(base,chan) (((MCD_bufDescFec*)(base))	\
  +((chan)							\
    *(DMA_BD_RX_NUM+DMA_BD_TX_NUM)))

#define SRAM_TXBD_BASE(base,chan) (((MCD_bufDescFec*)(base))		\
  +((chan)								\
    *(DMA_BD_RX_NUM+DMA_BD_TX_NUM)					\
    +DMA_BD_RX_NUM))

#define SRAM_DMA_BASE(base) ((void *)SRAM_RXBD_BASE(base,NIFACES+1))


#undef ETH_DEBUG

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     DMA_BD_RX_NUM
#define TX_BUF_COUNT     DMA_BD_TX_NUM
#define TX_BD_PER_BUF    1

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

#define MCF548X_FEC0_IRQ_VECTOR    (39+64)
#define MCF548X_FEC1_IRQ_VECTOR    (38+64)

#define MCF548X_FEC_IRQ_VECTOR(chan) (MCF548X_FEC0_IRQ_VECTOR		\
				      +(chan)*(MCF548X_FEC1_IRQ_VECTOR	\
					       -MCF548X_FEC0_IRQ_VECTOR))

#define MCF548X_FEC_VECTOR2CHAN(vector) (((int)(vector)-MCF548X_FEC0_IRQ_VECTOR) \
					 /(MCF548X_FEC1_IRQ_VECTOR	\
					   -MCF548X_FEC0_IRQ_VECTOR))

#define FEC_RECV_TASK_NO        4
#define FEC_XMIT_TASK_NO        5

#define MCDMA_FEC_RX_CHAN(chan) (0 + NIFACES*(chan))
#define MCDMA_FEC_TX_CHAN(chan) (1 + NIFACES*(chan))

#define MCF548X_FEC0_RX_INITIATOR  (16)
#define MCF548X_FEC1_RX_INITIATOR  (30)
#define MCF548X_FEC_RX_INITIATOR(chan) (MCF548X_FEC0_RX_INITIATOR		\
				      +(chan)*(MCF548X_FEC1_RX_INITIATOR	\
					       -MCF548X_FEC0_RX_INITIATOR))
#define MCF548X_FEC0_TX_INITIATOR  (17)
#define MCF548X_FEC1_TX_INITIATOR  (31)
#define MCF548X_FEC_TX_INITIATOR(chan) (MCF548X_FEC0_TX_INITIATOR		\
				      +(chan)*(MCF548X_FEC1_TX_INITIATOR	\
					       -MCF548X_FEC0_TX_INITIATOR))

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1
#define FATAL_INT_EVENT RTEMS_EVENT_3

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

/* BD and parameters are stored in SRAM(refer to sdma.h) */
#define MCF548X_FEC_BD_BASE    ETH_BD_BASE

/* RBD bits definitions */
#define MCF548X_FEC_RBD_EMPTY  0x8000	/* Buffer is empty */
#define MCF548X_FEC_RBD_WRAP   0x2000	/* Last BD in ring */
#define MCF548X_FEC_RBD_INT    0x1000	/* Interrupt */
#define MCF548X_FEC_RBD_LAST   0x0800	/* Buffer is last in frame(useless) */
#define MCF548X_FEC_RBD_MISS   0x0100	/* Miss bit for prom mode */
#define MCF548X_FEC_RBD_BC     0x0080	/* The received frame is broadcast frame */
#define MCF548X_FEC_RBD_MC     0x0040	/* The received frame is multicast frame */
#define MCF548X_FEC_RBD_LG     0x0020	/* Frame length violation */
#define MCF548X_FEC_RBD_NO     0x0010	/* Nonoctet align frame */
#define MCF548X_FEC_RBD_SH     0x0008	/* Short frame, FEC does not support SH and this bit is always cleared */
#define MCF548X_FEC_RBD_CR     0x0004	/* CRC error */
#define MCF548X_FEC_RBD_OV     0x0002	/* Receive FIFO overrun */
#define MCF548X_FEC_RBD_TR     0x0001	/* The receive frame is truncated */
#define MCF548X_FEC_RBD_ERR    (MCF548X_FEC_RBD_LG  | \
                                MCF548X_FEC_RBD_NO  | \
                                MCF548X_FEC_RBD_CR  | \
                                MCF548X_FEC_RBD_OV  | \
                                MCF548X_FEC_RBD_TR)

/* TBD bits definitions */
#define MCF548X_FEC_TBD_READY  0x8000	/* Buffer is ready */
#define MCF548X_FEC_TBD_WRAP   0x2000	/* Last BD in ring */
#define MCF548X_FEC_TBD_INT    0x1000	/* Interrupt */
#define MCF548X_FEC_TBD_LAST   0x0800	/* Buffer is last in frame */
#define MCF548X_FEC_TBD_TC     0x0400	/* Transmit the CRC */
#define MCF548X_FEC_TBD_ABC    0x0200	/* Append bad CRC */

#define FEC_INTR_MASK_USED \
(MCF548X_FEC_EIMR_LC   | MCF548X_FEC_EIMR_RL    | \
 MCF548X_FEC_EIMR_XFUN | MCF548X_FEC_EIMR_XFERR | MCF548X_FEC_EIMR_RFERR)

/*
 * Device data
 */
struct mcf548x_enet_struct {
  struct arpcom           arpcom;
  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;
  int                     chan;
  int                     acceptBroadcast;
  int                     rxBdCount;
  int                     txBdCount;
  int                     txBdHead;
  int                     txBdTail;
  int                     txBdActiveCount;
  MCD_bufDescFec          *rxBd;
  MCD_bufDescFec          *txBd;
  int                     rxDmaChan; /* dma task */
  int                     txDmaChan; /* dma task */
  rtems_id                rxDaemonTid;
  rtems_id                txDaemonTid;

  /*
   * MDIO/Phy info
   */
  struct rtems_mdio_info mdio_info;
  int phy_default;
  int phy_chan;    /* which fec channel services this phy access? */
  int media_state; /* (last detected) state of media */

  unsigned long           rxInterrupts;
  unsigned long           rxNotLast;
  unsigned long           rxGiant;
  unsigned long           rxNonOctet;
  unsigned long           rxBadCRC;
  unsigned long           rxOverrun;
  unsigned long           rxCollision;

  unsigned long           txInterrupts;
  unsigned long           txDeferred;
  unsigned long           txLateCollision;
  unsigned long           txUnderrun;
  unsigned long           txMisaligned;
  unsigned long           rxNotFirst;
  unsigned long           txRetryLimit;
  };

static struct mcf548x_enet_struct enet_driver[NIFACES];

extern int taskTable;
static void mcf548x_fec_restart(struct mcf548x_enet_struct *sc);



/*
 * Function:	mcf548x_fec_rx_bd_init
 *
 * Description:	Initialize the receive buffer descriptor ring.
 *
 * Returns:		void
 *
 * Notes:       Space for the buffers of rx BDs is allocated by the rx deamon
 *
 */
static void mcf548x_fec_rx_bd_init(struct mcf548x_enet_struct *sc) {
  int rxBdIndex;
  struct mbuf *m;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  /*
   * Fill RX buffer descriptor ring.
   */
  for( rxBdIndex = 0; rxBdIndex < sc->rxBdCount; rxBdIndex++ ) {
    MGETHDR (m, M_WAIT, MT_DATA);
    MCLGET (m, M_WAIT);

    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    rtems_cache_invalidate_multiple_data_lines(mtod(m,const void *),
					       ETHER_MAX_LEN);
    SET_BD_BUFFER(sc->rxBd+rxBdIndex,mtod(m, void *));
    SET_BD_LENGTH(sc->rxBd+rxBdIndex,ETHER_MAX_LEN);
    SET_BD_STATUS(sc->rxBd+rxBdIndex,
		  MCF548X_FEC_RBD_EMPTY
		  | MCF548X_FEC_RBD_INT
		  | ((rxBdIndex == sc->rxBdCount-1)
		     ? MCF548X_FEC_RBD_WRAP
		     : 0));
  }
}

/*
 * Function:	mcf548x_fec_rx_bd_cleanup
 *
 * Description:	put all mbufs pending in rx BDs back to buffer pool
 *
 * Returns:		void
 *
 */
static void mcf548x_fec_rx_bd_cleanup(struct mcf548x_enet_struct *sc) {
  int rxBdIndex;
  struct mbuf *m,*n;

  /*
   * Drain RX buffer descriptor ring.
   */
  for( rxBdIndex = 0; rxBdIndex < sc->rxBdCount; rxBdIndex++ ) {
    n = sc->rxMbuf[rxBdIndex];
    while (n != NULL) {
      m = n;
      MFREE(m,n);
    }
  }
}

/*
 * Function:	MCF548X_eth_addr_filter_set
 *
 * Description:	Set individual address filter for unicast address and
 *				set physical address registers.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
static void mcf548x_eth_addr_filter_set(struct mcf548x_enet_struct *sc)  {
  unsigned char *mac;
  unsigned char currByte;				/* byte for which to compute the CRC */
  int           byte;					/* loop - counter */
  int           bit;					/* loop - counter */
  unsigned long crc = 0xffffffff;		/* initial value */
  int chan     = sc->chan;

 /*
  * Get the mac address of ethernet controller
  */
  mac = (unsigned char *)(&sc->arpcom.ac_enaddr);

 /*
  * The algorithm used is the following:
  * we loop on each of the six bytes of the provided address,
  * and we compute the CRC by left-shifting the previous
  * value by one position, so that each bit in the current
  * byte of the address may contribute the calculation. If
  * the latter and the MSB in the CRC are different, then
  * the CRC value so computed is also ex-ored with the
  * "polynomium generator". The current byte of the address
  * is also shifted right by one bit at each iteration.
  * This is because the CRC generatore in hardware is implemented
  * as a shift-register with as many ex-ores as the radixes
  * in the polynomium. This suggests that we represent the
  * polynomiumm itself as a 32-bit constant.
  */
  for(byte = 0; byte < 6; byte++)
    {

    currByte = mac[byte];

    for(bit = 0; bit < 8; bit++)
      {

      if((currByte & 0x01) ^ (crc & 0x01))
        {

        crc >>= 1;
        crc = crc ^ 0xedb88320;

        }
      else
        {

        crc >>= 1;

        }

      currByte >>= 1;

      }

    }

    crc = crc >> 26;

   /*
    * Set individual hash table register
    */
    if(crc >= 32)
      {

	MCF548X_FEC_IAUR(chan) = (1 << (crc - 32));
	MCF548X_FEC_IALR(chan) = 0;

      }
    else
     {

       MCF548X_FEC_IAUR(chan) = 0;
       MCF548X_FEC_IALR(chan) = (1 << crc);

     }

   /*
    * Set physical address
    */
    MCF548X_FEC_PALR(chan) = ((mac[0] << 24) +
			      (mac[1] << 16) +
			      (mac[2] <<  8) +
			      mac[3]);
    MCF548X_FEC_PAUR(chan) = ((mac[4] << 24)
			      + (mac[5] << 16)) + 0x8808;

   }


/*
 * Function:	mcf548x_eth_mii_read
 *
 * Description:	Read a media independent interface (MII) register on an
 *				18-wire ethernet tranceiver (PHY). Please see your PHY
 *				documentation for the register map.
 *
 * Returns:		0 if ok
 *
 * Notes:
 *
 */
int mcf548x_eth_mii_read(
 int phyAddr,                          /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned regAddr,                     /* register address                 */
 uint32_t *retVal)                     /* ptr to read buffer               */
{
  struct mcf548x_enet_struct *sc = uarg;
  int timeout = 0xffff;
  int chan = sc->phy_chan;

 /*
  * reading from any PHY's register is done by properly
  * programming the FEC's MII data register.
  */
  MCF548X_FEC_MMFR(chan) = (MCF548X_FEC_MMFR_ST_01    |
			    MCF548X_FEC_MMFR_OP_READ  |
			    MCF548X_FEC_MMFR_TA_10    |
			    MCF548X_FEC_MMFR_PA(phyAddr) |
			    MCF548X_FEC_MMFR_RA(regAddr));

 /*
  * wait for the related interrupt
  */
  while ((timeout--) && (!(MCF548X_FEC_EIR(chan) & MCF548X_FEC_EIR_MII)));

  if(timeout == 0) {

#ifdef ETH_DEBUG
    iprintf ("Read MDIO failed..." "\r\n");
#endif

    return 1;

  }

 /*
  * clear mii interrupt bit
  */
  MCF548X_FEC_EIR(chan) = MCF548X_FEC_EIR_MII;

 /*
  * it's now safe to read the PHY's register
  */
  *retVal = (unsigned short)  MCF548X_FEC_MMFR(chan);

  return 0;

}

/*
 * Function:	mcf548x_eth_mii_write
 *
 * Description:	Write a media independent interface (MII) register on an
 *				18-wire ethernet tranceiver (PHY). Please see your PHY
 *				documentation for the register map.
 *
 * Returns:		Success (boolean)
 *
 * Notes:
 *
 */
static int mcf548x_eth_mii_write(
 int phyAddr,                          /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned regAddr,                     /* register address                 */
 uint32_t data)                        /* write data                       */
{
  struct mcf548x_enet_struct *sc = uarg;
  int chan     = sc->phy_chan;
  int timeout  = 0xffff;

  MCF548X_FEC_MMFR(chan) = (MCF548X_FEC_MMFR_ST_01    |
			    MCF548X_FEC_MMFR_OP_WRITE |
			    MCF548X_FEC_MMFR_TA_10    |
			    MCF548X_FEC_MMFR_PA(phyAddr) |
			    MCF548X_FEC_MMFR_RA(regAddr) |
			    MCF548X_FEC_MMFR_DATA(data));

 /*
  * wait for the MII interrupt
  */
  while ((timeout--) && (!(MCF548X_FEC_EIR(chan) & MCF548X_FEC_EIR_MII)));

  if(timeout == 0)
    {

#ifdef ETH_DEBUG
    iprintf ("Write MDIO failed..." "\r\n");
#endif

    return 1;

    }

 /*
  * clear MII interrupt bit
  */
  MCF548X_FEC_EIR(chan) = MCF548X_FEC_EIR_MII;

  return 0;

  }


/*
 * Function:	mcf548x_fec_reset
 *
 * Description:	Reset a running ethernet driver including the hardware
 *				FIFOs and the FEC.
 *
 * Returns:		Success (boolean)
 *
 * Notes:
 *
 */
static int mcf548x_fec_reset(struct mcf548x_enet_struct *sc) {
  volatile int delay;
  int chan     = sc->chan;
  /*
   * Clear FIFO status registers
   */
  MCF548X_FEC_FECRFSR(chan) = ~0;
  MCF548X_FEC_FECTFSR(chan) = ~0;

  /*
   * reset the FIFOs
   */
  MCF548X_FEC_FRST(chan) = 0x03000000;

  for (delay = 0;delay < 16*4;delay++) {};

  MCF548X_FEC_FRST(chan) = 0x01000000;

  /*
   * Issue a reset command to the FEC chip
   */
  MCF548X_FEC_ECR(chan) |= MCF548X_FEC_ECR_RESET;

  /*
   * wait at least 16 clock cycles
   */
  for (delay = 0;delay < 16*4;delay++) {};

  return true;
}


/*
 * Function:	mcf548x_fec_off
 *
 * Description:	Stop the FEC and disable the ethernet SmartComm tasks.
 *				This function "turns off" the driver.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
void mcf548x_fec_off(struct mcf548x_enet_struct *sc)
  {
  int            counter = 0xffff;
  int chan     = sc->chan;


#if defined(ETH_DEBUG)
  uint32_t phyStatus;
  int i;

  for(i = 0; i < 9; i++)
    {

    mcf548x_eth_mii_read(sc->phy_default, sc, i, &phyStatus);
    iprintf ("Mii reg %d: 0x%04lx" "\r\n", i, phyStatus);

    }

  for(i = 16; i < 21; i++)
    {

    mcf548x_eth_mii_read(sc->phy_default, sc, i, &phyStatus);
    iprintf ("Mii reg %d: 0x%04lx" "\r\n", i, phyStatus);

    }
  for(i = 0; i < 32; i++)
    {

    mcf548x_eth_mii_read(i, sc, 0, &phyStatus);
    iprintf ("Mii Phy=%d, reg 0: 0x%04lx" "\r\n", i, phyStatus);

    }
#endif	/* ETH_DEBUG */

 /*
  * block FEC chip interrupts
  */
  MCF548X_FEC_EIMR(chan) = 0;

 /*
  * issue graceful stop command to the FEC transmitter if necessary
  */
  MCF548X_FEC_TCR(chan) |= MCF548X_FEC_TCR_GTS;

 /*
  * wait for graceful stop to register
  * FIXME: add rtems_task_wake_after here, if it takes to long
  */
  while((counter--) && (!(  MCF548X_FEC_EIR(chan) & MCF548X_FEC_EIR_GRA)));

  /*
   * Disable the SmartDMA transmit and receive tasks.
   */
  MCD_killDma( sc->rxDmaChan );
  MCD_killDma( sc->txDmaChan );
 /*
  * Disable transmit / receive interrupts
  */
  mcdma_glue_irq_disable(sc->txDmaChan);
  mcdma_glue_irq_disable(sc->rxDmaChan);

 /*
  * Disable the Ethernet Controller
  */
  MCF548X_FEC_ECR(chan) &= ~(MCF548X_FEC_ECR_ETHER_EN);

  /*
   * cleanup all buffers
   */
  mcf548x_fec_rx_bd_cleanup(sc);

  }

/*
 * MCF548X FEC interrupt handler
 */
void mcf548x_fec_irq_handler(rtems_vector_number vector)
{
  struct mcf548x_enet_struct *sc;
  volatile uint32_t ievent;
  int chan;

  sc     = &(enet_driver[MCF548X_FEC_VECTOR2CHAN(vector)]);
  chan   = sc->chan;
  ievent = MCF548X_FEC_EIR(chan);

  MCF548X_FEC_EIR(chan) = ievent;
  /*
   * check errors, update statistics
   */
  if (ievent & MCF548X_FEC_EIR_LC) {
    sc->txLateCollision++;
  }
  if (ievent & MCF548X_FEC_EIR_RL) {
    sc->txRetryLimit++;
  }
  if (ievent & MCF548X_FEC_EIR_XFUN) {
    sc->txUnderrun++;
  }
  if (ievent & MCF548X_FEC_EIR_XFERR) {
    sc->txUnderrun++;
  }
  if (ievent & MCF548X_FEC_EIR_RFERR) {
    sc->rxOverrun++;
  }
  /*
   * fatal error ocurred?
   */
  if (ievent & (MCF548X_FEC_EIR_RFERR | MCF548X_FEC_EIR_XFERR)) {
    MCF548X_FEC_EIMR(chan) &=~(MCF548X_FEC_EIMR_RFERR | MCF548X_FEC_EIMR_XFERR);
    rtems_event_send(sc->rxDaemonTid, FATAL_INT_EVENT);
  }
}

/*
 * MCF548X DMA ethernet interrupt handler
 */
void mcf548x_mcdma_rx_irq_handler(void * param)
{
  struct mcf548x_enet_struct *sc = (struct mcf548x_enet_struct *)param;
  /* Frame received? */
  if(MCDMA_GET_PENDING(sc->rxDmaChan)) {
    MCDMA_CLR_PENDING(sc->rxDmaChan);

    mcdma_glue_irq_disable(sc->rxDmaChan);/*Disable receive ints*/
    sc->rxInterrupts++; 		/* Rx int has occurred */
    rtems_event_send(sc->rxDaemonTid, INTERRUPT_EVENT);
  }
}

/*
 * MCF548X DMA ethernet interrupt handler
 */
void mcf548x_mcdma_tx_irq_handler(void * param)
{
  struct mcf548x_enet_struct *sc = (struct mcf548x_enet_struct *)param;

 /* Buffer transmitted or transmitter error? */
  if(MCDMA_GET_PENDING(sc->txDmaChan)) {

    MCDMA_CLR_PENDING(sc->txDmaChan);

    mcdma_glue_irq_disable(sc->txDmaChan);/*Disable tx ints*/

    sc->txInterrupts++; /* Tx int has occurred */

    rtems_event_send(sc->txDaemonTid, INTERRUPT_EVENT);
  }
}





 /*
  * Function:	    mcf548x_fec_retire_tbd
  *
  * Description:	Soak up buffer descriptors that have been sent.
  *
  * Returns:		void
  *
  * Notes:
  *
  */
static void mcf548x_fec_retire_tbd(struct mcf548x_enet_struct *sc,
				   bool force)
{
  struct mbuf *n;
  /*
   * Clear already transmitted BDs first. Will not work calling same
   * from fecExceptionHandler(TFINT).
   */

  while ((sc->txBdActiveCount > 0) &&
	 (force ||
	  ((MCF548X_FEC_TBD_READY & GET_BD_STATUS(sc->txBd+sc->txBdTail))
	   == 0x0))) {
    if (sc->txMbuf[sc->txBdTail] != NULL) {
      /*
       * NOTE: txMbuf can be NULL, if mbuf has been split into different BDs
       */
      MFREE (sc->txMbuf[sc->txBdTail],n);
      sc->txMbuf[sc->txBdTail] = NULL;
    }
    sc->txBdActiveCount--;
    if(++sc->txBdTail >= sc->txBdCount) {
      sc->txBdTail = 0;
    }
  }
}


static void mcf548x_fec_sendpacket(struct ifnet *ifp,struct mbuf *m) {
  struct mcf548x_enet_struct *sc = ifp->if_softc;
  struct mbuf *l = NULL;
  int nAdded;
  uint32_t status;
  rtems_event_set events;
  MCD_bufDescFec *thisBd;
  MCD_bufDescFec *firstBd = NULL;
  void *data_ptr;
  size_t data_len;

 /*
  * Free up buffer descriptors
  */
  mcf548x_fec_retire_tbd(sc,false);

 /*
  * Set up the transmit buffer descriptors.
  * No need to pad out short packets since the
  * hardware takes care of that automatically.
  * No need to copy the packet to a contiguous buffer
  * since the hardware is capable of scatter/gather DMA.
  */
  nAdded = 0;

  for(;;) {

   /*
    * Wait for buffer descriptor to become available.
    */
    if((sc->txBdActiveCount + nAdded) == sc->txBdCount) {

      /*
       * Clear old events
       */
      MCDMA_CLR_PENDING(sc->txDmaChan);
      /*
       * Wait for buffer descriptor to become available.
       * Note that the buffer descriptors are checked
       * *before* * entering the wait loop -- this catches
       * the possibility that a buffer descriptor became
       * available between the `if' above, and the clearing
       * of the event register.
       * This is to catch the case where the transmitter
       * stops in the middle of a frame -- and only the
       * last buffer descriptor in a frame can generate
       * an interrupt.
       */
      mcf548x_fec_retire_tbd(sc,false);

      while((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
	mcdma_glue_irq_enable(sc->txDmaChan);
	rtems_bsdnet_event_receive(INTERRUPT_EVENT,
				   RTEMS_WAIT | RTEMS_EVENT_ANY,
				   RTEMS_NO_TIMEOUT, &events);
        mcf548x_fec_retire_tbd(sc,false);
      }
    }

    if(m->m_len == 0) {
      /*
       * Just toss empty mbufs
       */
      struct mbuf *n;
      MFREE(m, n);
      m = n;
      if(l != NULL) {
        l->m_next = m;
      }
    }
    else {
      /*
       * Flush the buffer for this descriptor
       */
      rtems_cache_flush_multiple_data_lines((const void *)mtod(m, void *),
					    m->m_len);
      /*
       * Fill in the buffer descriptor,
       * set "end of frame" bit in status,
       * if last mbuf in chain
       */
      thisBd = sc->txBd + sc->txBdHead;
      /*
       * FIXME: do not send interrupt after every frame
       * doing this every quarter of BDs is much more efficent
       */
      status = (((m->m_next == NULL)
		 ? MCF548X_FEC_TBD_LAST | MCF548X_FEC_TBD_INT
		 : 0)
		| ((sc->txBdHead == sc->txBdCount-1)
		   ? MCF548X_FEC_TBD_WRAP
		   :0 ));
      /*
       * Don't set the READY flag till the
       * whole packet has been readied.
       */
      if (firstBd != NULL) {
	status |= MCF548X_FEC_TBD_READY;
      }
      else {
	firstBd = thisBd;
      }

      data_ptr = mtod(m, void *);
      data_len = m->m_len;
      sc->txMbuf[sc->txBdHead] = m;
      /* go to next part in chain */
      l = m;
      m = m->m_next;

      SET_BD_BUFFER(thisBd, data_ptr);
      SET_BD_LENGTH(thisBd, data_len);
      SET_BD_STATUS(thisBd, status);

      nAdded++;
      if(++(sc->txBdHead) == sc->txBdCount) {
        sc->txBdHead = 0;
      }
    }
    /*
     * Set the transmit buffer status.
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if(m == NULL) {
      if(nAdded) {
	SET_BD_STATUS(firstBd,
		      GET_BD_STATUS(firstBd) | MCF548X_FEC_TBD_READY);
	MCD_continDma(sc->txDmaChan);
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
  } /* end of for(;;) */
}


/*
 * Driver transmit daemon
 */
void mcf548x_fec_txDaemon(void *arg)
  {
  struct mcf548x_enet_struct *sc = (struct mcf548x_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for(;;) {
   /*
    * Wait for packet
    */
    mcdma_glue_irq_enable(sc->txDmaChan);
    rtems_bsdnet_event_receive(START_TRANSMIT_EVENT|INTERRUPT_EVENT,
			       RTEMS_EVENT_ANY | RTEMS_WAIT,
			       RTEMS_NO_TIMEOUT,
			       &events);

    /*
     * Send packets till queue is empty
     */
    for(;;)
      {

      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);

      if (!m)
        break;

      mcf548x_fec_sendpacket(ifp, m);

      }

    ifp->if_flags &= ~IFF_OACTIVE;

    }

  }


/*
 * reader task
 */
static void mcf548x_fec_rxDaemon(void *arg){
  struct mcf548x_enet_struct *sc = (struct mcf548x_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  struct ether_header *eh;
  int rxBdIndex;
  uint32_t status;
  size_t size;
  rtems_event_set events;
  size_t len = 1;
  MCD_bufDescFec *bd;

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;

  for (;;) {
    /*
     * Clear old events
     */
    MCDMA_CLR_PENDING(sc->rxDmaChan);
    /*
     * Get the first BD pointer and its length.
     */
    bd     = sc->rxBd + rxBdIndex;
    status = GET_BD_STATUS( bd );
    len    = GET_BD_LENGTH( bd );

    /*
     * Loop through BDs until we find an empty one. This indicates that
     * the DMA is still using it.
     */
    while( !(status & MCF548X_FEC_RBD_EMPTY) ) {

      /*
       * Remember the data pointer from this transfer.
       */
      GET_BD_BUFFER(bd);
      m    = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = (len
				    - sizeof(uint32_t)
				    - sizeof(struct ether_header));
      eh = mtod(m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      ether_input(ifp, eh, m);

      /*
       * Done w/ the BD. Clean it.
       */
      sc->rxMbuf[rxBdIndex] = NULL;

      /*
       * Add a new buffer to the ring.
       */
      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;
      size = ETHER_MAX_LEN;

      sc->rxMbuf[rxBdIndex] = m;
      rtems_cache_invalidate_multiple_data_lines(mtod(m,const void *),
						 size);

      SET_BD_BUFFER(bd,mtod(m, void *));
      SET_BD_LENGTH(bd,size);
      SET_BD_STATUS(bd,
		    MCF548X_FEC_RBD_EMPTY
		    |MCF548X_FEC_RBD_INT
		    |((rxBdIndex == sc->rxBdCount-1)
		      ? MCF548X_FEC_RBD_WRAP
		      : 0)
		    );

      /*
       * advance to next BD
       */
      if (++rxBdIndex >= sc->rxBdCount) {
	rxBdIndex = 0;
      }
      /*
       * Get next BD pointer and its length.
       */
      bd     = sc->rxBd + rxBdIndex;
      status = GET_BD_STATUS( bd );
      len    = GET_BD_LENGTH( bd );
    }
    /*
     * Unmask RXF (Full frame received) event
     */
    mcdma_glue_irq_enable(sc->rxDmaChan);

    rtems_bsdnet_event_receive (INTERRUPT_EVENT | FATAL_INT_EVENT,
				RTEMS_WAIT | RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT, &events);
    if (events & FATAL_INT_EVENT) {
      /*
       * fatal interrupt ocurred, so reinit fec and restart mcdma tasks
       */
      mcf548x_fec_restart(sc);
      rxBdIndex = 0;
    }
  }
}


/*
 * Function:	mcf548x_fec_initialize_hardware
 *
 * Description:	Configure the MCF548X FEC registers and enable the
 *				SmartComm tasks. This function "turns on" the driver.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
static void mcf548x_fec_initialize_hardware(struct mcf548x_enet_struct *sc)
  {
  int chan = sc->chan;

 /*
  * Reset mcf548x FEC
  */
  mcf548x_fec_reset(sc);

 /*
  * Clear FEC-Lite interrupt event register (IEVENT)
  */
  MCF548X_FEC_EIR(chan) = MCF548X_FEC_EIR_CLEAR_ALL;

 /*
  * Set interrupt mask register
  */
  MCF548X_FEC_EIMR(chan) = FEC_INTR_MASK_USED;
  /*
   * Set FEC-Lite receive control register (R_CNTRL)
   * frame length=1518, MII mode for 18-wire-transceiver
   */
  MCF548X_FEC_RCR(chan) = (MCF548X_FEC_RCR_MAX_FL(ETHER_MAX_LEN)
			   | MCF548X_FEC_RCR_FCE
			   | MCF548X_FEC_RCR_MII_MODE);

  /*
   * Set FEC-Lite transmit control register (X_CNTRL)
   * full-duplex, heartbeat disabled
   */
  MCF548X_FEC_TCR(chan) = MCF548X_FEC_TCR_FDEN;



 /*
  * Set MII_SPEED = (1/(mii_speed * 2)) * System Clock(33Mhz)
  * and do not drop the Preamble.
  */
  MCF548X_FEC_MSCR(chan) = MCF548X_FEC_MSCR_MII_SPEED(7); /* ipb_clk = 33 MHz */

 /*
  * Set Opcode/Pause Duration Register
  */
  MCF548X_FEC_PAUR(chan) = 0x00010020;

  /*
   * Set Rx FIFO alarm and granularity value
   */
  MCF548X_FEC_FECRFCR(chan) = (MCF548X_FEC_FECRFCR_FRM
			       | MCF548X_FEC_FECRFCR_GR(0x7));
  MCF548X_FEC_FECRFAR(chan) = MCF548X_FEC_FECRFAR_ALARM(256);

  /*
   * Set Tx FIFO granularity value
   */
  MCF548X_FEC_FECTFCR(chan) = (MCF548X_FEC_FECTFCR_FRM
			       | MCF548X_FEC_FECTFCR_GR(7));

  /*
   * Set transmit fifo watermark register (X_WMRK), default = 64
   */
  MCF548X_FEC_FECTFAR(chan) = MCF548X_FEC_FECTFAR_ALARM(256);	/* 256 bytes */
  MCF548X_FEC_FECTFWR(chan) = MCF548X_FEC_FECTFWR_X_WMRK_64;	/* 64 bytes */

 /*
  * Set individual address filter for unicast address
  * and set physical address registers.
  */
  mcf548x_eth_addr_filter_set(sc);

 /*
  * Set multicast address filter
  */
  MCF548X_FEC_GAUR(chan) = 0x00000000;
  MCF548X_FEC_GALR(chan) = 0x00000000;

 /*
  * enable CRC in finite state machine register
  */
  MCF548X_FEC_CTCWR(chan) = MCF548X_FEC_CTCWR_TFCW | MCF548X_FEC_CTCWR_CRC;
  }


/*
 * Send packet (caller provides header).
 */
static void mcf548x_fec_tx_start(struct ifnet *ifp)
  {

  struct mcf548x_enet_struct *sc = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);

  }


/*
 * start the DMA channel
 */
static void mcf548x_fec_startDMA(struct mcf548x_enet_struct *sc)
{
  int chan = sc->chan;
  int mcdma_rc;
      /*
       * Enable the SmartDMA receive task.
       */
      mcdma_rc = MCD_startDma
	(sc->rxDmaChan, /* the channel on which to run the DMA */
	 (void *)sc->rxBd, /* the address to move data from, or buffer-descriptor addr */
	 0,             /* the amount to increment the source address per transfer */
	 (void *)&MCF548X_FEC_FECRFDR(chan), /* the address to move data to */
	 0,             /* the amount to increment the destination address per transfer */
	 ETHER_MAX_LEN, /* the number of bytes to transfer independent of the transfer size */
         0,             /* the number bytes in of each data movement (1, 2, or 4) */
	 MCF548X_FEC_RX_INITIATOR(chan), /* what device initiates the DMA */
	 2,  /* priority of the DMA */
	 0 /* flags describing the DMA */
	 | MCD_FECRX_DMA
	 | MCD_INTERRUPT
	 | MCD_TT_FLAGS_CW
	 | MCD_TT_FLAGS_RL
	 | MCD_TT_FLAGS_SP
	 ,
	 0 /* a description of byte swapping, bit swapping, and CRC actions */
	 | MCD_NO_CSUM
	 | MCD_NO_BYTE_SWAP
	 );
      if (mcdma_rc != MCD_OK) {
	rtems_panic("FEC: cannot start rx DMA");
      }
      mcdma_rc = MCD_startDma
	(sc->txDmaChan, /* the channel on which to run the DMA */
	 (void *)sc->txBd, /* the address to move data from, or buffer-descriptor addr */
	 0,             /* the amount to increment the source address per transfer */
	 (void *)&MCF548X_FEC_FECTFDR(chan), /* the address to move data to */
	 0,             /* the amount to increment the destination address per transfer */
	 ETHER_MAX_LEN, /* the number of bytes to transfer independent of the transfer size */
         0,             /* the number bytes in of each data movement (1, 2, or 4) */
	 MCF548X_FEC_TX_INITIATOR(chan), /* what device initiates the DMA */
	 1,  /* priority of the DMA */
	 0 /* flags describing the DMA */
	 | MCD_FECTX_DMA
	 | MCD_INTERRUPT
	 | MCD_TT_FLAGS_CW
	 | MCD_TT_FLAGS_RL
	 | MCD_TT_FLAGS_SP
	 ,
	 0 /* a description of byte swapping, bit swapping, and CRC actions */
	 | MCD_NO_CSUM
	 | MCD_NO_BYTE_SWAP
	 );
      if (mcdma_rc != MCD_OK) {
	rtems_panic("FEC: cannot start tx DMA");
      }
}
/*
 * Initialize and start the device
 */
static void mcf548x_fec_init(void *arg)
{
  struct mcf548x_enet_struct *sc = (struct mcf548x_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  int chan = sc->chan;
  rtems_isr_entry old_handler;
  char *txTaskName = "FTx0";
  char *rxTaskName = "FRx0";
  if(sc->txDaemonTid == 0)
    {
      /*
       * Allocate a set of BDs
       */
      sc->rxBd =  SRAM_RXBD_BASE(_SysSramBase,chan);
      sc->txBd =  SRAM_TXBD_BASE(_SysSramBase,chan);

      if(!sc->rxBd || !sc->txBd)
	rtems_panic ("No memory for BDs");
      /*
       * clear the BDs
       */
      memset((void *)sc->rxBd,0,sc->rxBdCount * sizeof *(sc->rxBd));
      memset((void *)sc->txBd,0,sc->txBdCount * sizeof *(sc->txBd));
      /*
       * Allocate a set of mbuf pointers
       */
      sc->rxMbuf =
	malloc(sc->rxBdCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
      sc->txMbuf =
	malloc(sc->txBdCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);

      if(!sc->rxMbuf || !sc->txMbuf)
	rtems_panic ("No memory for mbuf pointers");

      sc->txDmaChan = MCDMA_FEC_TX_CHAN(chan);
      sc->rxDmaChan = MCDMA_FEC_RX_CHAN(chan);

      mcdma_glue_init(SRAM_DMA_BASE(_SysSramBase));

      /*
       * Set up interrupts
       */
      mcdma_glue_irq_install(sc->rxDmaChan,
			     mcf548x_mcdma_rx_irq_handler,
			     sc);
      mcdma_glue_irq_install(sc->txDmaChan,
			     mcf548x_mcdma_tx_irq_handler,
			     sc);
      if(rtems_interrupt_catch(mcf548x_fec_irq_handler,
			       MCF548X_FEC_IRQ_VECTOR(chan),
			       &old_handler)) {
	rtems_panic ("Can't attach MFC54xx FEX interrupt handler\n");
      }

      MCF548X_INTC_ICRn(MCF548X_FEC_IRQ_VECTOR(chan) % 64) =
	MCF548X_INTC_ICRn_IL(FEC_IRQ_LEVEL) |
	MCF548X_INTC_ICRn_IP(FEC_IRQ_PRIORITY);

      MCF548X_INTC_IMRH &= ~(1 << (MCF548X_FEC_IRQ_VECTOR(chan) % 32));

      MCF548X_FEC_EIMR(chan) = FEC_INTR_MASK_USED;
      mcf548x_fec_rx_bd_init(sc);

      /*
       * reset and Set up mcf548x FEC hardware
       */
      mcf548x_fec_initialize_hardware(sc);

      /*
       * Start driver tasks
       */
      txTaskName[3] = '0'+chan;
      rxTaskName[3] = '0'+chan;
      sc->txDaemonTid = rtems_bsdnet_newproc(txTaskName, 4096,
					     mcf548x_fec_txDaemon, sc);
      sc->rxDaemonTid = rtems_bsdnet_newproc(rxTaskName, 4096,
					     mcf548x_fec_rxDaemon, sc);
      /*
       * Clear SmartDMA task interrupt pending bits.
       */
      MCDMA_CLR_PENDING(sc->rxDmaChan );
      MCDMA_CLR_PENDING(sc->txDmaChan );

      /*
       * start the DMA channels
       */
      mcf548x_fec_startDMA(sc);
      /*
       * Enable FEC-Lite controller
       */
      MCF548X_FEC_ECR(chan) |= MCF548X_FEC_ECR_ETHER_EN;


    }

  /*
   * Set flags appropriately
   */
  if(ifp->if_flags & IFF_PROMISC)
    MCF548X_FEC_RCR(chan) |=  MCF548X_FEC_RCR_PROM;
  else
    MCF548X_FEC_RCR(chan) &= ~MCF548X_FEC_RCR_PROM;

  /*
   * init timer so the "watchdog function gets called periodically
   */
  ifp->if_timer    = 1;
  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
}


static void enet_stats (struct mcf548x_enet_struct *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("       Not First:%-8lu", sc->rxNotFirst);
  printf ("        Not Last:%-8lu\n", sc->rxNotLast);
  printf ("              Giant:%-8lu", sc->rxGiant);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("       Collision:%-8lu\n", sc->rxCollision);

  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("        Deferred:%-8lu", sc->txDeferred);
  printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
  printf ("   Retransmit Limit:%-8lu", sc->txRetryLimit);
  printf ("        Underrun:%-8lu", sc->txUnderrun);
  printf ("      Misaligned:%-8lu\n", sc->txMisaligned);

}

/*
 * restart the driver, reinit the fec
 * this function is responsible to reinitialize the FEC in case a fatal
 * error has ocurred. This is needed, wen a RxFIFO Overrun or a TxFIFO underrun
 * has ocurred. In these cases, the FEC is automatically disabled, and
 * both FIFOs must be reset and the BestComm tasks must be restarted
 *
 * Note: the daemon tasks will continue to run
 * (in fact this function will be called in the context of the rx daemon task)
 */
#define NEW_DMA_SETUP

static void mcf548x_fec_restart(struct mcf548x_enet_struct *sc)
{
  int chan = sc->chan;
  /*
   * FIXME: bring Tx Daemon into idle state
   */
#ifdef NEW_DMA_SETUP
  /*
   * cleanup remaining receive mbufs
   */
  mcf548x_fec_rx_bd_cleanup(sc);
#endif
  /*
   * Stop DMA tasks
   */
  MCD_killDma (sc->rxDmaChan);
  MCD_killDma (sc->txDmaChan);
  /*
   * FIXME: wait, until Tx Daemon is in idle state
   */

  /*
   * Disable transmit / receive interrupts
   */
  mcdma_glue_irq_disable(sc->txDmaChan);
  mcdma_glue_irq_disable(sc->rxDmaChan);
#ifdef NEW_DMA_SETUP
  /*
   * recycle pending tx buffers
   * FIXME: try to extract pending Tx buffers
   */
  mcf548x_fec_retire_tbd(sc,true);
#endif
  /*
   * re-initialize the FEC hardware
   */
  mcf548x_fec_initialize_hardware(sc);

#ifdef NEW_DMA_SETUP

  /*
   * reinit receive mbufs
   */
  mcf548x_fec_rx_bd_init(sc);
#endif
  /*
   * Clear SmartDMA task interrupt pending bits.
   */
  MCDMA_CLR_PENDING( sc->rxDmaChan );

  /*
   * start the DMA channels again
   */
  mcf548x_fec_startDMA(sc);
  /*
   * reenable rx/tx interrupts
   */
  mcdma_glue_irq_enable(sc->rxDmaChan);
  mcdma_glue_irq_enable(sc->txDmaChan);
  /*
   * (re-)init fec hardware
   */
  mcf548x_fec_initialize_hardware(sc);
  /*
   * reenable fec FIFO error interrupts
   */
  MCF548X_FEC_EIMR(chan) = FEC_INTR_MASK_USED;
  /*
   * Enable FEC-Lite controller
   */
  MCF548X_FEC_ECR(chan) |= MCF548X_FEC_ECR_ETHER_EN;
}

int32_t mcf548x_fec_setMultiFilter(struct ifnet *ifp)
{
  /*struct mcf548x_enet_struct *sc = ifp->if_softc; */
  /* XXX anything to do? */
  return 0;
}


/*
 * Driver ioctl handler
 */
static int mcf548x_fec_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
  {
  struct mcf548x_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch(command)
    {

    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      rtems_mii_ioctl (&(sc->mdio_info),sc,command,(void *)data);
      break;

    case SIOCGIFADDR:
    case SIOCSIFADDR:

      ether_ioctl(ifp, command, data);

      break;

    case SIOCADDMULTI:
    case SIOCDELMULTI: {
      struct ifreq* ifr = (struct ifreq*) data;
      error = (command == SIOCADDMULTI)
                  ? ether_addmulti(ifr, &sc->arpcom)
                  : ether_delmulti(ifr, &sc->arpcom);

       if (error == ENETRESET) {
         if (ifp->if_flags & IFF_RUNNING)
           error = mcf548x_fec_setMultiFilter(ifp);
         else
           error = 0;
       }
       break;
    }

    case SIOCSIFFLAGS:

      switch(ifp->if_flags & (IFF_UP | IFF_RUNNING))
        {

        case IFF_RUNNING:

          mcf548x_fec_off(sc);

          break;

        case IFF_UP:

          mcf548x_fec_init(sc);

          break;

        case IFF_UP | IFF_RUNNING:

          mcf548x_fec_off(sc);
          mcf548x_fec_init(sc);

          break;

        default:
          break;

        }

      break;

    case SIO_RTEMS_SHOW_STATS:

      enet_stats(sc);

      break;

   /*
    * FIXME: All sorts of multicast commands need to be added here!
    */
    default:

    error = EINVAL;

    break;

    }

  return error;

  }


/*
 * init the PHY and adapt FEC settings
 */
int mcf548x_fec_mode_adapt(struct ifnet *ifp)
{
  int result = 0;
  struct mcf548x_enet_struct *sc = ifp->if_softc;
  int media = IFM_MAKEWORD( 0, 0, 0, sc->phy_default);
  int chan = sc->chan;

  /*
   * fetch media status
   */
  result = mcf548x_fec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
  if (result != 0) {
    return result;
  }
  /*
   * status is unchanged? then do nothing
   */
  if (media == sc->media_state) {
    return 0;
  }
  /*
   * otherwise: for the first call, try to negotiate mode
   */
  if (sc->media_state == 0) {
    /*
     * set media status: set auto negotiation -> start auto-negotiation
     */
    media = IFM_MAKEWORD(0,IFM_AUTO,0,sc->phy_default);
    result = mcf548x_fec_ioctl(ifp,SIOCSIFMEDIA,(caddr_t)&media);
    if (result != 0) {
      return result;
    }
    /*
     * wait for auto-negotiation to terminate
     */
    do {
      media = IFM_MAKEWORD(0,0,0,sc->phy_default);
      result = mcf548x_fec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
      if (result != 0) {
	return result;
      }
    } while (IFM_NONE == IFM_SUBTYPE(media));
  }

  /*
   * now set HW according to media results:
   */

  /*
   * if we are half duplex then switch to half duplex
   */
  if (0 == (IFM_FDX & IFM_OPTIONS(media))) {
    MCF548X_FEC_TCR(chan) &= ~MCF548X_FEC_TCR_FDEN;
  }
  else {
    MCF548X_FEC_TCR(chan) |=  MCF548X_FEC_TCR_FDEN;
  }
  /*
   * store current media state for future compares
   */
  sc->media_state = media;

  return 0;
}

/*
 * periodically poll the PHY. if mode has changed,
 * then adjust the FEC settings
 */
static void mcf548x_fec_watchdog( struct ifnet *ifp)
{
  mcf548x_fec_mode_adapt(ifp);
  ifp->if_timer    = FEC_WATCHDOG_TIMEOUT;
}

/*
 * Attach the MCF548X fec driver to the system
 */
int rtems_mcf548x_fec_driver_attach(struct rtems_bsdnet_ifconfig *config)
  {
  struct mcf548x_enet_struct *sc;
  struct ifnet *ifp;
  int    mtu;
  int    unitNumber;
  char   *unitName;

 /*
  * Parse driver name
  */
  if((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0)
    return 0;

 /*
  * Is driver free?
  */
  if ((unitNumber <= 0) || (unitNumber > NIFACES))
    {

    printf ("Bad FEC unit number.\n");
    return 0;

    }

  sc = &enet_driver[unitNumber - 1];
  sc->chan = unitNumber-1;
  ifp = &sc->arpcom.ac_if;

  if(ifp->if_softc != NULL)
    {

    printf ("Driver already in use.\n");
    return 0;

    }

  /*
   * Process options
   */
#if NVRAM_CONFIGURE == 1

  /* Configure from NVRAM */
  if(addr = nvram->ipaddr)
    {

    /* We have a non-zero entry, copy the value */
    if(pAddr = malloc(INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT))
      config->ip_address = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1);
    else
      rtems_panic("Can't allocate ip_address buffer!\n");

    }

  if(addr = nvram->netmask)
    {

    /* We have a non-zero entry, copy the value */
    if (pAddr = malloc (INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT))
      config->ip_netmask = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1);
    else
      rtems_panic("Can't allocate ip_netmask buffer!\n");

    }

  /* Ethernet address requires special handling -- it must be copied into
   * the arpcom struct. The following if construct serves only to give the
   * User Area NVRAM parameter the highest priority.
   *
   * If the ethernet address is specified in NVRAM, go ahead and copy it.
   * (ETHER_ADDR_LEN = 6 bytes).
   */
  if(nvram->enaddr[0] || nvram->enaddr[1] || nvram->enaddr[2])
    {

    /* Anything in the first three bytes indicates a non-zero entry, copy value */
  	memcpy((void *)sc->arpcom.ac_enaddr, &nvram->enaddr, ETHER_ADDR_LEN);

    }
  else
    if(config->hardware_address)
      {

      /* There is no entry in NVRAM, but there is in the ifconfig struct, so use it. */
      memcpy((void *)sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
      }

#else /* NVRAM_CONFIGURE != 1 */

  if(config->hardware_address)
    {

    memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

    }

#endif /* NVRAM_CONFIGURE != 1 */
#ifdef HAS_UBOOT
  if ((sc->arpcom.ac_enaddr[0] == 0) &&
      (sc->arpcom.ac_enaddr[1] == 0) &&
      (sc->arpcom.ac_enaddr[2] == 0)) {
      memcpy(
        (void *)sc->arpcom.ac_enaddr,
        bsp_uboot_board_info.bi_enetaddr,
        ETHER_ADDR_LEN
      );
  }
#endif
#ifdef HAS_DBUG
  if ((sc->arpcom.ac_enaddr[0] == 0) &&
      (sc->arpcom.ac_enaddr[1] == 0) &&
      (sc->arpcom.ac_enaddr[2] == 0)) {
      memcpy(
        (void *)sc->arpcom.ac_enaddr,
        DBUG_SETTINGS.macaddr,
        ETHER_ADDR_LEN
      );
  }
#endif
  if ((sc->arpcom.ac_enaddr[0] == 0) &&
      (sc->arpcom.ac_enaddr[1] == 0) &&
      (sc->arpcom.ac_enaddr[2] == 0)) {
    /* There is no ethernet address provided, so it could be read
     * from the Ethernet protocol block of SCC1 in DPRAM.
     */
    rtems_panic("No Ethernet address specified!\n");
  }
  if(config->mtu)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;

  if(config->rbuf_count)
    sc->rxBdCount = config->rbuf_count;
  else
    sc->rxBdCount = RX_BUF_COUNT;

  if(config->xbuf_count)
    sc->txBdCount = config->xbuf_count;
  else
    sc->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;

  sc->acceptBroadcast = !config->ignore_broadcast;

  /*
   * setup info about mdio interface
   */
  sc->mdio_info.mdio_r   = mcf548x_eth_mii_read;
  sc->mdio_info.mdio_w   = mcf548x_eth_mii_write;
  sc->mdio_info.has_gmii = 0; /* we do not support gigabit IF */

  /*
   * XXX: Although most hardware builders will assign the PHY addresses
   * like this, this should be more configurable
   */
  sc->phy_default = unitNumber-1;
  sc->phy_chan    = 0; /* assume all MII accesses are via FEC0 */

 /*
  * Set up network interface values
  */
  ifp->if_softc   = sc;
  ifp->if_unit    = unitNumber;
  ifp->if_name    = unitName;
  ifp->if_mtu     = mtu;
  ifp->if_init    = mcf548x_fec_init;
  ifp->if_ioctl   = mcf548x_fec_ioctl;
  ifp->if_start   = mcf548x_fec_tx_start;
  ifp->if_output  = ether_output;
  ifp->if_watchdog =  mcf548x_fec_watchdog; /* XXX: timer is set in "init" */
  ifp->if_flags   = IFF_BROADCAST | IFF_MULTICAST;
  /*ifp->if_flags   = IFF_BROADCAST | IFF_SIMPLEX;*/

  if(ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /*
   * Attach the interface
   */
  if_attach(ifp);

  ether_ifattach(ifp);

  return 1;
  }


int rtems_mcf548x_fec_driver_attach_detach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
  if (attaching) {
    return rtems_mcf548x_fec_driver_attach(config);
  }
  else {
    return 0;
  }
}


