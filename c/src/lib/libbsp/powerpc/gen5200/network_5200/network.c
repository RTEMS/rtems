/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
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
 *  RTEMS/TCPIP driver for MPC5200 FEC Ethernet
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
 *  Modified for MPC860 by Jay Monkman (jmonkman@frasca.com)
 *
 *  This supports Ethernet on either SCC1 or the FEC of the MPC860T.
 *  Right now, we only do 10 Mbps, even with the FEC. The function
 *  rtems_enet_driver_attach determines which one to use. Currently,
 *  only one may be used at a time.
 *
 *  Based on the MC68360 network driver by
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 *
 *  This supports ethernet on SCC1. Right now, we only do 10 Mbps.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>
 *  Copyright (c) 1999, National Research Council of Canada
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
#include <bsp.h>
#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include <net/if_var.h>
#include <errno.h>

/* motorola-capi-specifics... */
#include "../bestcomm/include/ppctypes.h"	/* uint32, et. al.			*/
#include "../bestcomm/dma_image.h"
#include "../bestcomm/bestcomm_glue.h"


#define SDMA_BD_TFD	0x08000000	/*< Transmit Frame Done		*/
#define SDMA_BD_INT	0x04000000	/*< Interrupt on frame done	*/
#define SDMA_BD_RX_NUM	64 /* Number of receive buffer descriptors	*/
#define SDMA_BD_TX_NUM	64 /* Number of transmit buffer descriptors	*/

#define SET_BD_STATUS(bd, stat)	{		\
	(bd)->Status &= 0x0000ffff;			\
	(bd)->Status |= 0xffff0000 & stat;	\
}
#define SET_BD_LENGTH(bd, len) {		\
	(bd)->Status &= 0xffff0000;			\
	(bd)->Status |= 0x0000ffff & len;	\
}
#define GET_BD_STATUS(bd)		((bd)->Status & 0xffff0000)
#define GET_BD_LENGTH(bd)		((bd)->Status & 0x0000ffff)
#define GET_SDMA_PENDINGBIT(Bit)   \
   (mpc5200.IntPend & (uint32)(1<<Bit))

#include "../bestcomm/bestcomm_api.h"
#include "../bestcomm/task_api/bestcomm_cntrl.h"
#include "../bestcomm/task_api/tasksetup_bdtable.h"

extern TaskBDIdxTable_t TaskBDIdxTable[MAX_TASKS];

static TaskId rxTaskId;	/* SDMA RX task ID */
static TaskId txTaskId;	/* SDMA TX task ID */

/* #define ETH_DEBUG */

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     SDMA_BD_RX_NUM
#define TX_BUF_COUNT     SDMA_BD_TX_NUM
#define TX_BD_PER_BUF    1

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")


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

/* Task number assignment */
#define FEC_RECV_TASK_NO            TASK_FEC_RX
#define FEC_XMIT_TASK_NO            TASK_FEC_TX


/* BD and parameters are stored in SRAM(refer to sdma.h) */
#define MPC5200_FEC_BD_BASE    ETH_BD_BASE

/* FEC transmit watermark settings */
#define MPC5200_FEC_X_WMRK_64	0x0	/* or 0x1 */
#define MPC5200_FEC_X_WMRK_128	0x2
#define MPC5200_FEC_X_WMRK_192	0x3

/* RBD bits definitions */
#define MPC5200_FEC_RBD_EMPTY  0x8000	/* Buffer is empty */
#define MPC5200_FEC_RBD_WRAP   0x2000	/* Last BD in ring */
#define MPC5200_FEC_RBD_INT    0x1000	/* Interrupt */
#define MPC5200_FEC_RBD_LAST   0x0800	/* Buffer is last in frame(useless) */
#define MPC5200_FEC_RBD_MISS   0x0100	/* Miss bit for prom mode */
#define MPC5200_FEC_RBD_BC     0x0080	/* The received frame is broadcast frame */
#define MPC5200_FEC_RBD_MC     0x0040	/* The received frame is multicast frame */
#define MPC5200_FEC_RBD_LG     0x0020	/* Frame length violation */
#define MPC5200_FEC_RBD_NO     0x0010	/* Nonoctet align frame */
#define MPC5200_FEC_RBD_SH     0x0008	/* Short frame, FEC does not support SH and this bit is always cleared */
#define MPC5200_FEC_RBD_CR     0x0004	/* CRC error */
#define MPC5200_FEC_RBD_OV     0x0002	/* Receive FIFO overrun */
#define MPC5200_FEC_RBD_TR     0x0001	/* The receive frame is truncated */
#define MPC5200_FEC_RBD_ERR    (MPC5200_FEC_RBD_LG  | \
                                MPC5200_FEC_RBD_NO  | \
                                MPC5200_FEC_RBD_CR  | \
                                MPC5200_FEC_RBD_OV  | \
                                MPC5200_FEC_RBD_TR)

/* TBD bits definitions */
#define MPC5200_FEC_TBD_READY  0x8000	/* Buffer is ready */
#define MPC5200_FEC_TBD_WRAP   0x2000	/* Last BD in ring */
#define MPC5200_FEC_TBD_INT    0x1000	/* Interrupt */
#define MPC5200_FEC_TBD_LAST   0x0800	/* Buffer is last in frame */
#define MPC5200_FEC_TBD_TC     0x0400	/* Transmit the CRC */
#define MPC5200_FEC_TBD_ABC    0x0200	/* Append bad CRC */

/* MII-related definitios */
#define MPC5200_FEC_MII_DATA_ST       0x40000000	/* Start of frame delimiter */
#define MPC5200_FEC_MII_DATA_OP_RD    0x20000000	/* Perform a read operation */
#define MPC5200_FEC_MII_DATA_OP_WR    0x10000000	/* Perform a write operation */
#define MPC5200_FEC_MII_DATA_PA_MSK   0x0f800000	/* PHY Address field mask */
#define MPC5200_FEC_MII_DATA_RA_MSK   0x007c0000	/* PHY Register field mask */
#define MPC5200_FEC_MII_DATA_TA       0x00020000	/* Turnaround */
#define MPC5200_FEC_MII_DATA_DATAMSK  0x0000fff	    /* PHY data field */

#define MPC5200_FEC_MII_DATA_RA_SHIFT 0x12	/* MII Register address bits */
#define MPC5200_FEC_MII_DATA_PA_SHIFT 0x17	/* MII PHY address bits */


/* Receive & Transmit Buffer Descriptor definitions */
typedef struct mpc5200_buffer_desc_
  {
  volatile uint16_t status;
  volatile uint16_t length;
  volatile void             *buffer;
  } mpc5200_buffer_desc_t;


#define FEC_INTR_MASK_USED \
(FEC_INTR_LCEN  |FEC_INTR_CRLEN	|\
 FEC_INTR_XFUNEN|FEC_INTR_XFERREN|FEC_INTR_RFERREN)

/*
 * Device data
 */
struct mpc5200_enet_struct {
#if 0
  struct ifnet            ac_if;
#else
  struct arpcom           arpcom;
#endif
  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;
  int                     acceptBroadcast;
  int                     rxBdCount;
  int                     txBdCount;
  int                     txBdHead;
  int                     txBdTail;
  int                     txBdActiveCount;

  rtems_id                rxDaemonTid;
  rtems_id                txDaemonTid;

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

static struct mpc5200_enet_struct enet_driver[NIFACES];

extern int taskTable;
static void mpc5200_fec_restart(struct mpc5200_enet_struct *sc);



/*
 * Function:	mpc5200_fec_rx_bd_init
 *
 * Description:	Initialize the receive buffer descriptor ring.
 *
 * Returns:		void
 *
 * Notes:       Space for the buffers of rx BDs is allocated by the rx deamon
 *
 */
static void mpc5200_fec_rx_bd_init(struct mpc5200_enet_struct *sc) {
  int rxBdIndex;
  struct mbuf *m;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  BDIdx bdi;

  /*
   * Fill RX buffer descriptor ring.
   */
  for( rxBdIndex = 0; rxBdIndex < sc->rxBdCount; rxBdIndex++ ) {
    MGETHDR (m, M_WAIT, MT_DATA);
    MCLGET (m, M_WAIT);

    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    bdi = TaskBDAssign( rxTaskId,
			mtod(m, void *),
			NULL,
			ETHER_MAX_LEN,
			0 );
    if (bdi != rxBdIndex) {
      rtems_panic("network rx buffer indices out of sync");
    }
  }
}

/*
 * Function:	mpc5200_fec_rx_bd_cleanup
 *
 * Description:	put all mbufs pending in rx BDs back to buffer pool
 *
 * Returns:		void
 *
 */
static void mpc5200_fec_rx_bd_cleanup(struct mpc5200_enet_struct *sc) {
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
 * Function:	MPC5200_eth_addr_filter_set
 *
 * Description:	Set individual address filter for unicast address and
 *				set physical address registers.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
static void mpc5200_eth_addr_filter_set(struct mpc5200_enet_struct *sc)  {
  unsigned char *mac;
  unsigned char currByte;				/* byte for which to compute the CRC */
  int           byte;					/* loop - counter */
  int           bit;					/* loop - counter */
  unsigned long crc = 0xffffffff;		/* initial value */

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

      mpc5200.iaddr1 = (1 << (crc - 32));
      mpc5200.iaddr2 = 0;

      }
    else
     {

     mpc5200.iaddr1 = 0;
     mpc5200.iaddr2 = (1 << crc);

     }

   /*
    * Set physical address
    */
    mpc5200.paddr1 = (mac[0] << 24) + (mac[1] << 16) + (mac[2] << 8) + mac[3];
    mpc5200.paddr2 = (mac[4] << 24) + (mac[5] << 16) + 0x8808;

   }


/*
 * Function:	mpc5200_eth_mii_read
 *
 * Description:	Read a media independent interface (MII) register on an
 *				18-wire ethernet tranceiver (PHY). Please see your PHY
 *				documentation for the register map.
 *
 * Returns:		32-bit register value
 *
 * Notes:
 *
 */
int mpc5200_eth_mii_read(struct mpc5200_enet_struct *sc, unsigned char phyAddr, unsigned char regAddr, unsigned short * retVal)
  {
  unsigned long reg;		/* convenient holder for the PHY register */
  unsigned long phy;		/* convenient holder for the PHY */
  int timeout = 0xffff;

 /*
  * reading from any PHY's register is done by properly
  * programming the FEC's MII data register.
  */
  reg = regAddr << MPC5200_FEC_MII_DATA_RA_SHIFT;
  phy = phyAddr << MPC5200_FEC_MII_DATA_PA_SHIFT;

  mpc5200.mii_data = (MPC5200_FEC_MII_DATA_ST | MPC5200_FEC_MII_DATA_OP_RD | MPC5200_FEC_MII_DATA_TA | phy | reg);

 /*
  * wait for the related interrupt
  */
  while ((timeout--) && (!(mpc5200.ievent & 0x00800000)));

  if(timeout == 0)
    {

#ifdef ETH_DEBUG
    printf ("Read MDIO failed..." "\r\n");
#endif

	return false;

	}

 /*
  * clear mii interrupt bit
  */
  mpc5200.ievent = 0x00800000;

 /*
  * it's now safe to read the PHY's register
  */
  *retVal = (unsigned short)mpc5200.mii_data;

  return true;

  }

/*
 * Function:	mpc5200_eth_mii_write
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
static int mpc5200_eth_mii_write(struct mpc5200_enet_struct *sc, unsigned char phyAddr, unsigned char regAddr, unsigned short data)
  {
  unsigned long reg;					/* convenient holder for the PHY register */
  unsigned long phy;					/* convenient holder for the PHY */
  int timeout = 0xffff;

  reg = regAddr << MPC5200_FEC_MII_DATA_RA_SHIFT;
  phy = phyAddr << MPC5200_FEC_MII_DATA_PA_SHIFT;

  mpc5200.mii_data = (MPC5200_FEC_MII_DATA_ST | MPC5200_FEC_MII_DATA_OP_WR | MPC5200_FEC_MII_DATA_TA | phy | reg | data);

 /*
  * wait for the MII interrupt
  */
  while ((timeout--) && (!(mpc5200.ievent & 0x00800000)));

  if(timeout == 0)
    {

#ifdef ETH_DEBUG
    printf ("Write MDIO failed..." "\r\n");
#endif

    return false;

    }

 /*
  * clear MII interrupt bit
  */
  mpc5200.ievent = 0x00800000;

  return true;

  }


/*
 * Function:	mpc5200_fec_reset
 *
 * Description:	Reset a running ethernet driver including the hardware
 *				FIFOs and the FEC.
 *
 * Returns:		Success (boolean)
 *
 * Notes:
 *
 */
static int mpc5200_fec_reset(struct mpc5200_enet_struct *sc) {
  volatile int delay;
  /*
   * Clear FIFO status registers
   */
  mpc5200.rfifo_status &= FEC_FIFO_STAT_ERROR;
  mpc5200.tfifo_status &= FEC_FIFO_STAT_ERROR;

  /*
   * reset the FIFOs
   */
  mpc5200.reset_cntrl = 0x03000000;

  for (delay = 0;delay < 16*4;delay++) {};

  mpc5200.reset_cntrl = 0x01000000;

  /*
   * Issue a reset command to the FEC chip
   */
  mpc5200.ecntrl |= FEC_ECNTRL_RESET;

  /*
   * wait at least 16 clock cycles
   */
  for (delay = 0;delay < 16*4;delay++) {};

  return true;
}


/*
 * Function:	mpc5200_fec_off
 *
 * Description:	Stop the FEC and disable the ethernet SmartComm tasks.
 *				This function "turns off" the driver.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
void mpc5200_fec_off(struct mpc5200_enet_struct *sc)
  {
  int            counter = 0xffff;


#if defined(ETH_DEBUG)
  unsigned short phyStatus, i;
  unsigned char  phyAddr = 0;

  for(i = 0; i < 9; i++)
    {

    mpc5200_eth_mii_read(sc, phyAddr, i, &phyStatus);
    printf ("Mii reg %d: 0x%04x" "\r\n", i, phyStatus);

    }

  for(i = 16; i < 21; i++)
    {

    mpc5200_eth_mii_read(sc, phyAddr, i, &phyStatus);
    printf ("Mii reg %d: 0x%04x" "\r\n", i, phyStatus);

    }
#endif	/* ETH_DEBUG */

 /*
  * block FEC chip interrupts
  */
  mpc5200.imask = 0;

 /*
  * issue graceful stop command to the FEC transmitter if necessary
  */
  mpc5200.x_cntrl |= FEC_XCNTRL_GTS;

 /*
  * wait for graceful stop to register
  * FIXME: add rtems_task_wake_after here, if it takes to long
  */
  while((counter--) && (!(mpc5200.ievent & FEC_INTR_GRA)));

  /*
   * Disable the SmartDMA transmit and receive tasks.
   */
  TaskStop( rxTaskId );
  TaskStop( txTaskId );
 /*
  * Disable transmit / receive interrupts
  */
  bestcomm_glue_irq_disable(FEC_XMIT_TASK_NO);
  bestcomm_glue_irq_disable(FEC_RECV_TASK_NO);

 /*
  * Disable the Ethernet Controller
  */
  mpc5200.ecntrl &= ~(FEC_ECNTRL_OE | FEC_ECNTRL_EN);

  /*
   * cleanup all buffers
   */
  mpc5200_fec_rx_bd_cleanup(sc);

  }

/*
 * MPC5200 FEC interrupt handler
 */
void mpc5200_fec_irq_handler(rtems_irq_hdl_param handle)
{
  struct mpc5200_enet_struct *sc = (struct mpc5200_enet_struct *) handle;
  volatile uint32_t ievent;

  ievent = mpc5200.ievent;

  mpc5200.ievent = ievent;
  /*
   * check errors, update statistics
   */
  if (ievent & FEC_INTR_LATE_COL) {
    sc->txLateCollision++;
  }
  if (ievent & FEC_INTR_COL_RETRY) {
    sc->txRetryLimit++;
  }
  if (ievent & FEC_INTR_XFIFO_UN) {
    sc->txUnderrun++;
  }
  if (ievent & FEC_INTR_XFIFO_ERR) {
    sc->txUnderrun++;
  }
  if (ievent & FEC_INTR_RFIFO_ERR) {
    sc->rxOverrun++;
  }
  /*
   * fatal error ocurred?
   */
  if (ievent & (FEC_INTR_XFIFO_ERR | FEC_INTR_RFIFO_ERR)) {
    mpc5200.imask &= ~(FEC_INTR_XFERREN | FEC_INTR_RFERREN);
    rtems_event_send(enet_driver[0].rxDaemonTid, FATAL_INT_EVENT);
  }
}

/*
 * MPC5200 SmartComm ethernet interrupt handler
 */
void mpc5200_smartcomm_rx_irq_handler(rtems_irq_hdl_param unused)
  {
 /* Frame received? */
  if(GET_SDMA_PENDINGBIT(FEC_RECV_TASK_NO))
    {

      SDMA_CLEAR_IEVENT(&mpc5200.IntPend,FEC_RECV_TASK_NO);

      bestcomm_glue_irq_disable(FEC_RECV_TASK_NO);/*Disable receive ints*/

      enet_driver[0].rxInterrupts++; 		/* Rx int has occurred */

      rtems_event_send(enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);

    }
  }

/*
 * MPC5200 SmartComm ethernet interrupt handler
 */
void mpc5200_smartcomm_tx_irq_handler(rtems_irq_hdl_param unused)
  {
 /* Buffer transmitted or transmitter error? */
  if(GET_SDMA_PENDINGBIT(FEC_XMIT_TASK_NO))
    {

      SDMA_CLEAR_IEVENT(&mpc5200.IntPend,FEC_XMIT_TASK_NO);

      bestcomm_glue_irq_disable(FEC_XMIT_TASK_NO);/*Disable tx ints*/

      enet_driver[0].txInterrupts++; /* Tx int has occurred */

      rtems_event_send(enet_driver[0].txDaemonTid, INTERRUPT_EVENT);

    }

  }





 /*
  * Function:	    mpc5200_fec_retire_tbd
  *
  * Description:	Soak up buffer descriptors that have been sent.
  *
  * Returns:		void
  *
  * Notes:
  *
  */
static void mpc5200_fec_retire_tbd(struct mpc5200_enet_struct *sc,
				   bool force)
{
  struct mbuf *n;
  TaskBD1_t   *bdRing = (TaskBD1_t *)TaskGetBDRing( txTaskId );;
  /*
   * Clear already transmitted BDs first. Will not work calling same
   * from fecExceptionHandler(TFINT).
   */

  while ((sc->txBdActiveCount > 0) &&
	 (force || (bdRing[sc->txBdTail].Status == 0x0))) {
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

#if 0
 /*
  * Function:	     mpc5200_fec_tx_bd_requeue
  *
  * Description:	put buffers back to interface output queue
  *
  * Returns:		void
  *
  * Notes:
  *
  */
static void mpc5200_fec_tx_bd_requeue(struct mpc5200_enet_struct *sc)
{
  /*
   * Clear already transmitted BDs first. Will not work calling same
   * from fecExceptionHandler(TFINT).
   */

  while (sc->txBdActiveCount > 0) {
    if (sc->txMbuf[sc->txBdHead] != NULL) {
      /*
       * NOTE: txMbuf can be NULL, if mbuf has been split into different BDs
       */
      IF_PREPEND(&(sc->arpcom.ac_if.if_snd),sc->txMbuf[sc->txBdHead]);
      sc->txMbuf[sc->txBdHead] = NULL;
    }
    sc->txBdActiveCount--;
    if(--sc->txBdHead < 0) {
      sc->txBdHead = sc->txBdCount-1;
    }
  }
}
#endif

static void mpc5200_fec_sendpacket(struct ifnet *ifp,struct mbuf *m) {
  struct mpc5200_enet_struct *sc = ifp->if_softc;
  struct mbuf *l = NULL;
  int nAdded;
  uint32_t status;
  rtems_event_set events;
  TaskBD1_t *bdRing = (TaskBD1_t *)TaskGetBDRing( txTaskId );
  TaskBD1_t *thisBd;
  TaskBD1_t *firstBd = NULL;
  void *data_ptr;
  size_t data_len;

 /*
  * Free up buffer descriptors
  */
  mpc5200_fec_retire_tbd(sc,false);

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
      SDMA_CLEAR_IEVENT(&mpc5200.IntPend,FEC_XMIT_TASK_NO);

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
      mpc5200_fec_retire_tbd(sc,false);

      while((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
	bestcomm_glue_irq_enable(FEC_XMIT_TASK_NO);
	rtems_bsdnet_event_receive(INTERRUPT_EVENT,
				   RTEMS_WAIT | RTEMS_EVENT_ANY,
				   RTEMS_NO_TIMEOUT, &events);
        mpc5200_fec_retire_tbd(sc,false);
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
      /*rtems_cache_flush_multiple_data_lines((const void *)mtod(m, void *), m->m_len);*/
      /*
       * Fill in the buffer descriptor,
       * set "end of frame" bit in status,
       * if last mbuf in chain
       */
      thisBd             = bdRing + sc->txBdHead;
      /*
       * FIXME: do not send interrupt after every frame
       * doing this every quarter of BDs is much more efficent
       */
      status             = ((m->m_next == NULL)
			    ? TASK_BD_TFD | TASK_BD_INT
			    : 0);
      /*
       * Don't set the READY flag till the
       * whole packet has been readied.
       */
      if (firstBd != NULL) {
	status |= (uint32)SDMA_BD_MASK_READY;
      }
      else {
	firstBd = thisBd;
      }

      data_ptr = mtod(m, void *);
      data_len = (uint32)m->m_len;
      sc->txMbuf[sc->txBdHead] = m;
      /* go to next part in chain */
      l = m;
      m = m->m_next;

      thisBd->DataPtr[0] = (uint32)data_ptr;
      thisBd->Status     = (status
			    |((uint32)SDMA_DRD_MASK_LENGTH & data_len));

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
        firstBd->Status     |= SDMA_BD_MASK_READY;
	SDMA_TASK_ENABLE(SDMA_TCR, txTaskId);
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
  } /* end of for(;;) */
}


/*
 * Driver transmit daemon
 */
void mpc5200_fec_txDaemon(void *arg)
  {
  struct mpc5200_enet_struct *sc = (struct mpc5200_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for(;;) {
   /*
    * Wait for packet
    */
    bestcomm_glue_irq_enable(FEC_XMIT_TASK_NO);
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

      mpc5200_fec_sendpacket(ifp, m);

      }

    ifp->if_flags &= ~IFF_OACTIVE;

    }

  }


/*
 * reader task
 */
static void mpc5200_fec_rxDaemon(void *arg){
  struct mpc5200_enet_struct *sc = (struct mpc5200_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  struct ether_header *eh;
  int rxBdIndex;
  uint32_t status;
  size_t size;
  rtems_event_set events;
  uint16    len = 1;
  TaskBD1_t *bd;
  void	    *dptr;
  TaskBD1_t   *bdRing = (TaskBD1_t *)TaskGetBDRing( rxTaskId );;

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;

  for (;;) {
    /*
     * Clear old events
     */
    SDMA_CLEAR_IEVENT(&mpc5200.IntPend,FEC_RECV_TASK_NO);
    /*
     * Get the first BD pointer and its length.
     */
    bd     = bdRing + rxBdIndex;
    status = bd->Status;
    len    = (uint16)GET_BD_LENGTH( bd );

    /*
     * Loop through BDs until we find an empty one. This indicates that
     * the SmartDMA is still using it.
     */
    while( !(status & SDMA_BD_MASK_READY) ) {

      /*
       * Remember the data pointer from this transfer.
       */
      dptr = (void *)bd->DataPtr[0];
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
      bd->DataPtr[0] = (uint32)mtod(m, void *);
      bd->Status = ( (  (uint32)SDMA_DRD_MASK_LENGTH & (uint32)size)
		     | ((uint32)SDMA_BD_MASK_READY));

      /*
       * advance to next BD
       */
      if (++rxBdIndex >= sc->rxBdCount) {
	rxBdIndex = 0;
      }
      /*
       * Get next BD pointer and its length.
       */
      bd     = bdRing + rxBdIndex;
      status = bd->Status;
      len    = (uint16)GET_BD_LENGTH( bd );
    }
    /*
     * Unmask RXF (Full frame received) event
     */
    bestcomm_glue_irq_enable(FEC_RECV_TASK_NO);

    rtems_bsdnet_event_receive (INTERRUPT_EVENT | FATAL_INT_EVENT,
				RTEMS_WAIT | RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT, &events);
    if (events & FATAL_INT_EVENT) {
      /*
       * fatal interrupt ocurred, so reinit fec and restart bestcomm tasks
       */
      mpc5200_fec_restart(sc);
      rxBdIndex = 0;
    }
  }
}


/*
 * Function:	mpc5200_fec_initialize_hardware
 *
 * Description:	Configure the MPC5200 FEC registers and enable the
 *				SmartComm tasks. This function "turns on" the driver.
 *
 * Returns:		void
 *
 * Notes:
 *
 */
static void mpc5200_fec_initialize_hardware(struct mpc5200_enet_struct *sc)
  {

 /*
  * Reset mpc5200 FEC
  */
  mpc5200_fec_reset(sc);

 /*
  * Clear FEC-Lite interrupt event register (IEVENT)
  */
  mpc5200.ievent = FEC_INTR_CLEAR_ALL;

 /*
  * Set interrupt mask register
  */
  mpc5200.imask = FEC_INTR_MASK_USED;
  /*
   * Set FEC-Lite receive control register (R_CNTRL)
   * frame length=1518, MII mode for 18-wire-transceiver
   */
  mpc5200.r_cntrl = ((ETHER_MAX_LEN << FEC_RCNTRL_MAX_FL_SHIFT)
		   | FEC_RCNTRL_FCE
		   | FEC_RCNTRL_MII_MODE);

  /*
   * Set FEC-Lite transmit control register (X_CNTRL)
   * full-duplex, heartbeat disabled
   */
  mpc5200.x_cntrl = FEC_XCNTRL_FDEN;



 /*
  * Set MII_SPEED = (1/(mii_speed * 2)) * System Clock(33Mhz)
  * and do not drop the Preamble.
  */
  mpc5200.mii_speed = (7 << 1); /* ipb_clk = 33 MHz */

 /*
  * Set Opcode/Pause Duration Register
  */
  mpc5200.op_pause = 0x00010020;

  /*
   * Set Rx FIFO alarm and granularity value
   */
  mpc5200.rfifo_cntrl = (FEC_FIFO_CNTRL_FRAME
		       | (0x7 << FEC_FIFO_CNTRL_GR_SHIFT));
  mpc5200.rfifo_alarm = 0x0000030c;

  /*
   * Set Tx FIFO granularity value
   */
  mpc5200.tfifo_cntrl = (FEC_FIFO_CNTRL_FRAME
		       | (0x7 << FEC_FIFO_CNTRL_GR_SHIFT));

  /*
   * Set transmit fifo watermark register (X_WMRK), default = 64
   */
  mpc5200.tfifo_alarm = 0x00000100;	/* 256 bytes */
  mpc5200.x_wmrk = FEC_XWMRK_256;	/* 256 bytes */

 /*
  * Set individual address filter for unicast address
  * and set physical address registers.
  */
  mpc5200_eth_addr_filter_set(sc);

 /*
  * Set multicast address filter
  */
  mpc5200.gaddr1 = 0x00000000;
  mpc5200.gaddr2 = 0x00000000;

 /*
  * enable CRC in finite state machine register
  */
  mpc5200.xmit_fsm = FEC_FSM_CRC | FEC_FSM_ENFSM;
  }

 /*
  * Initialize PHY(LXT971A):
  *
  *   Generally, on power up, the LXT971A reads its configuration
  *   pins to check for forced operation, If not cofigured for
  *   forced operation, it uses auto-negotiation/parallel detection
  *   to automatically determine line operating conditions.
  *   If the PHY device on the other side of the link supports
  *   auto-negotiation, the LXT971A auto-negotiates with it
  *   using Fast Link Pulse(FLP) Bursts. If the PHY partner does not
  *   support auto-negotiation, the LXT971A automatically detects
  *   the presence of either link pulses(10Mbps PHY) or Idle
  *   symbols(100Mbps) and sets its operating conditions accordingly.
  *
  *   When auto-negotiation is controlled by software, the following
  *   steps are recommended.
  *
  * Note:
  *   The physical address is dependent on hardware configuration.
  *
  * Returns:		void
  *
  * Notes:
  *
  */
static void mpc5200_fec_initialize_phy(struct mpc5200_enet_struct *sc)
  {
  int            timeout;
  unsigned short phyAddr = 0;


 /*
  * Reset PHY, then delay 300ns
  */
  mpc5200_eth_mii_write(sc, phyAddr, 0x0, 0x8000);

  rtems_task_wake_after(2);

 /* MII100 */

 /*
  * Set the auto-negotiation advertisement register bits
  */
  mpc5200_eth_mii_write(sc, phyAddr, 0x4, 0x01e1);

 /*
  * Set MDIO bit 0.12 = 1(&& bit 0.9=1?) to enable auto-negotiation
  */
  mpc5200_eth_mii_write(sc, phyAddr, 0x0, 0x1200);

 /*
  * Wait for AN completion
  */
  timeout = 0x100;
#if 0
  do
    {

    rtems_task_wake_after(2);

    if((timeout--) == 0)
      {

#if defined(ETH_DEBUG)
    printf ("MPC5200FEC PHY auto neg failed." "\r\n");
#endif

      }

    if(mpc5200_eth_mii_read(sc, phyAddr, 0x1, &phyStatus) != true)
      {

#if defined(ETH_DEBUG)
      printf ("MPC5200FEC PHY auto neg failed: 0x%04x." "\r\n", phyStatus);
#endif

	  return;

	  }

    } while((phyStatus & 0x0020) != 0x0020);

#endif
#if ETH_PROMISCOUS_MODE
  mpc5200.r_cntrl |= 0x00000008;   /* set to promiscous mode */
#endif

#if ETH_LOOP_MODE
  mpc5200.r_cntrl |= 0x00000001;   /* set to loop mode */
#endif

#if defined(ETH_DEBUG)
  int i;
  unsigned short phyStatus;
 /*
  * Print PHY registers after initialization.
  */
  for(i = 0; i < 9; i++)
    {

	mpc5200_eth_mii_read(sc, phyAddr, i, &phyStatus);
	printf ("Mii reg %d: 0x%04x" "\r\n", i, phyStatus);

	}

  for(i = 16; i < 21; i++)
    {

    mpc5200_eth_mii_read(sc, phyAddr, i, &phyStatus);
    printf ("Mii reg %d: 0x%04x" "\r\n", i, phyStatus);

    }
#endif	/* ETH_DEBUG */

  }


/*
 * Send packet (caller provides header).
 */
static void mpc5200_fec_tx_start(struct ifnet *ifp)
  {

  struct mpc5200_enet_struct *sc = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);

  }


/*
 * set up sdma tasks for ethernet
 */
static void mpc5200_sdma_task_setup(struct mpc5200_enet_struct *sc) {
  TaskSetupParamSet_t	rxParam;	/* RX task setup parameters	*/
  TaskSetupParamSet_t	txParam;	/* TX task setup parameters	*/

  /*
   * Setup the SDMA RX task.
   */
  rxParam.NumBD        = sc->rxBdCount;
  rxParam.Size.MaxBuf  = ETHER_MAX_LEN;
  rxParam.Initiator    = 0;
  rxParam.StartAddrSrc = (uint32)&(mpc5200.rfifo_data);
  rxParam.IncrSrc      = 0;
  rxParam.SzSrc	       = sizeof(uint32_t);
  rxParam.StartAddrDst = (uint32)NULL;
  rxParam.IncrDst      = sizeof(uint32_t);
  rxParam.SzDst	       = sizeof(uint32_t);
  rxTaskId	       = TaskSetup(TASK_FEC_RX,&rxParam );

  /*
   * Setup the TX task.
   */
  txParam.NumBD        = sc->txBdCount;
  txParam.Size.MaxBuf  = ETHER_MAX_LEN;
  txParam.Initiator    = 0;
  txParam.StartAddrSrc = (uint32)NULL;
  txParam.IncrSrc      = sizeof(uint32_t);
  txParam.SzSrc        = sizeof(uint32_t);
  txParam.StartAddrDst = (uint32)&(mpc5200.tfifo_data);
  txParam.IncrDst      = 0;
  txParam.SzDst        = sizeof(uint32_t);

  txTaskId             = TaskSetup( TASK_FEC_TX, &txParam );

}

void mpc5200_fec_irq_on(const rtems_irq_connect_data* ptr)
{
  mpc5200.imask = FEC_INTR_MASK_USED;
}


int mpc5200_fec_irq_isOn(const rtems_irq_connect_data* ptr)
{
  return mpc5200.imask != 0;
}


void mpc5200_fec_irq_off(const rtems_irq_connect_data* ptr)
{
  mpc5200.imask = 0;
}


/*
 * Initialize and start the device
 */
static void mpc5200_fec_init(void *arg)
{
  struct mpc5200_enet_struct *sc = (struct mpc5200_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  rtems_irq_connect_data fec_irq_data = {
    BSP_SIU_IRQ_ETH,
    mpc5200_fec_irq_handler, /* rtems_irq_hdl           */
    (rtems_irq_hdl_param)sc, /* (rtems_irq_hdl_param)   */
    mpc5200_fec_irq_on,	     /* (rtems_irq_enable)      */
    mpc5200_fec_irq_off,     /* (rtems_irq_disable)     */
    mpc5200_fec_irq_isOn     /* (rtems_irq_is_enabled)  */
  };


  if(sc->txDaemonTid == 0)
    {
      /*
       * Allocate a set of mbuf pointers
       */
      sc->rxMbuf =
	malloc(sc->rxBdCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
      sc->txMbuf =
	malloc(sc->txBdCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);

      if(!sc->rxMbuf || !sc->txMbuf)
	rtems_panic ("No memory for mbuf pointers");

      bestcomm_glue_init();

      mpc5200_sdma_task_setup(sc);

      /*
       * Set up interrupts
       */
      bestcomm_glue_irq_install(FEC_RECV_TASK_NO,
				mpc5200_smartcomm_rx_irq_handler,
				NULL);
      bestcomm_glue_irq_install(FEC_XMIT_TASK_NO,
				mpc5200_smartcomm_tx_irq_handler,
				NULL);
      if(!BSP_install_rtems_irq_handler (&fec_irq_data)) {
	rtems_panic ("Can't attach MPC5x00 FEX interrupt handler\n");
      }

      /* mpc5200_fec_tx_bd_init(sc); */
      mpc5200_fec_rx_bd_init(sc);

      /*
       * reset and Set up mpc5200 FEC hardware
       */
      mpc5200_fec_initialize_hardware(sc);
      /*
       * Set up the phy
       */
      mpc5200_fec_initialize_phy(sc);
      /*
       * Set priority of different initiators
       */
      mpc5200.IPR0 = 7;	/* always initiator	*/
      mpc5200.IPR3 = 6;	/* eth rx initiator	*/
      mpc5200.IPR4 = 5;	/* eth tx initiator	*/

      /*
       * Start driver tasks
       */
      sc->txDaemonTid = rtems_bsdnet_newproc("FEtx", 4096, mpc5200_fec_txDaemon, sc);
      sc->rxDaemonTid = rtems_bsdnet_newproc("FErx", 4096, mpc5200_fec_rxDaemon, sc);
      /*
       * Clear SmartDMA task interrupt pending bits.
       */
      TaskIntClear( rxTaskId );

      /*
       * Enable the SmartDMA receive task.
       */
      TaskStart( rxTaskId, 1, rxTaskId, 1 );
      TaskStart( txTaskId, 1, txTaskId, 1 );
      /*
       * Enable FEC-Lite controller
       */
      mpc5200.ecntrl |= (FEC_ECNTRL_OE | FEC_ECNTRL_EN);


    }

  /*
   * Set flags appropriately
   */
  if(ifp->if_flags & IFF_PROMISC)
    mpc5200.r_cntrl |= 0x08;
  else
    mpc5200.r_cntrl &= ~0x08;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
}


static void enet_stats (struct mpc5200_enet_struct *sc)
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
#define NEW_SDMA_SETUP

static void mpc5200_fec_restart(struct mpc5200_enet_struct *sc)
{
  /*
   * FIXME: bring Tx Daemon into idle state
   */
#ifdef NEW_SDMA_SETUP
  /*
   * cleanup remaining receive mbufs
   */
  mpc5200_fec_rx_bd_cleanup(sc);
#endif
  /*
   * Stop SDMA tasks
   */
  TaskStop( rxTaskId);
  TaskStop( txTaskId);
  /*
   * FIXME: wait, until Tx Daemon is in idle state
   */

  /*
   * Disable transmit / receive interrupts
   */
  bestcomm_glue_irq_disable(FEC_XMIT_TASK_NO);
  bestcomm_glue_irq_disable(FEC_RECV_TASK_NO);
#ifdef NEW_SDMA_SETUP
  /*
   * recycle pending tx buffers
   * FIXME: try to extract pending Tx buffers
   */
#if 0
  mpc5200_fec_tx_bd_requeue(sc);
#else
  mpc5200_fec_retire_tbd(sc,true);
#endif
#endif
  /*
   * re-initialize the FEC hardware
   */
  mpc5200_fec_initialize_hardware(sc);

#ifdef NEW_SDMA_SETUP
  /*
   * completely reinitialize Bestcomm tasks
   */
  mpc5200_sdma_task_setup(sc);

  /*
   * reinit receive mbufs
   */
  mpc5200_fec_rx_bd_init(sc);
#endif
  /*
   * Clear SmartDMA task interrupt pending bits.
   */
  TaskIntClear( rxTaskId );

  /*
   * Enable the SmartDMA receive/transmit task.
   */
  TaskStart( rxTaskId, 1, rxTaskId, 1 );
  TaskStart( txTaskId, 1, txTaskId, 1 );
  /*
   * reenable rx/tx interrupts
   */
  bestcomm_glue_irq_enable(FEC_XMIT_TASK_NO);
  bestcomm_glue_irq_enable(FEC_RECV_TASK_NO);
  /*
   * (re-)init fec hardware
   */
  mpc5200_fec_initialize_hardware(sc);
  /*
   * reenable fec FIFO error interrupts
   */
  mpc5200.imask = FEC_INTR_MASK_USED;
  /*
   * Enable FEC-Lite controller
   */
  mpc5200.ecntrl |= (FEC_ECNTRL_OE | FEC_ECNTRL_EN);
}

int32_t mpc5200_fec_setMultiFilter(struct ifnet *ifp)
{
  /*struct mpc5200_enet_struct *sc = ifp->if_softc; */
  /* XXX anything to do? */
  return 0;
}


/*
 * Driver ioctl handler
 */
static int mpc5200_fec_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
  {
  struct mpc5200_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch(command)
    {

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
           error = mpc5200_fec_setMultiFilter(ifp);
         else
           error = 0;
       }
       break;
    }

    case SIOCSIFFLAGS:

      switch(ifp->if_flags & (IFF_UP | IFF_RUNNING))
        {

        case IFF_RUNNING:

          mpc5200_fec_off(sc);

          break;

        case IFF_UP:

          mpc5200_fec_init(sc);

          break;

        case IFF_UP | IFF_RUNNING:

          mpc5200_fec_off(sc);
          mpc5200_fec_init(sc);

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
 * Attach the MPC5200 fec driver to the system
 */
int rtems_mpc5200_fec_driver_attach(struct rtems_bsdnet_ifconfig *config)
  {
  struct mpc5200_enet_struct *sc;
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
    else
      {
      /* There is no ethernet address provided, so it could be read
       * from the Ethernet protocol block of SCC1 in DPRAM.
       */
      rtems_panic("No Ethernet address specified!\n");
      }

#else /* NVRAM_CONFIGURE != 1 */

  if(config->hardware_address)
    {

    memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

    }
  else
    {

    /* There is no ethernet address provided, so it could be read
     * from the Ethernet protocol block of SCC1 in DPRAM.
     */
    rtems_panic("No Ethernet address specified!\n");

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
  * Set up network interface values
  */
  ifp->if_softc   = sc;
  ifp->if_unit    = unitNumber;
  ifp->if_name    = unitName;
  ifp->if_mtu     = mtu;
  ifp->if_init    = mpc5200_fec_init;
  ifp->if_ioctl   = mpc5200_fec_ioctl;
  ifp->if_start   = mpc5200_fec_tx_start;
  ifp->if_output  = ether_output;
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


int rtems_mpc5200_fec_driver_attach_detach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
  if (attaching) {
    return rtems_mpc5200_fec_driver_attach(config);
  }
  else {
    return 0;
  }
}


