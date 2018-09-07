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
| http://www.rtems.org/license/LICENSE.                           |
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

#include <machine/rtems-bsd-kernel-space.h>

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
#include <bsp/irq-generic.h>
#include <mcf548x/mcf548x.h>
#include <rtems/rtems_mii_ioctl.h>
#include <errno.h>

/* freescale-api-specifics... */
#include <mcf548x/MCD_dma.h>
#include <mcf548x/mcdma_glue.h>

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 2

#define FEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */

#define DMA_BD_RX_NUM	32 /* Number of receive buffer descriptors	*/
#define DMA_BD_TX_NUM	32 /* Number of transmit buffer descriptors	*/

#define FEC_EVENT RTEMS_EVENT_0

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

typedef enum {
  FEC_STATE_RESTART_0,
  FEC_STATE_RESTART_1,
  FEC_STATE_NORMAL,
} fec_state;

/*
 * Device data
 */
struct mcf548x_enet_struct {
  struct arpcom           arpcom;
  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;
  int                     chan;
  fec_state               state;
  int                     acceptBroadcast;
  int                     rxBdCount;
  int                     txBdCount;
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
  unsigned long           rxFIFOError;
  unsigned long           rxCollision;

  unsigned long           txInterrupts;
  unsigned long           txDeferred;
  unsigned long           txLateCollision;
  unsigned long           txUnderrun;
  unsigned long           txFIFOError;
  unsigned long           txMisaligned;
  unsigned long           rxNotFirst;
  unsigned long           txRetryLimit;
  };

static struct mcf548x_enet_struct enet_driver[NIFACES];

static void mcf548x_fec_restart(struct mcf548x_enet_struct *sc, rtems_id otherDaemon);

static void fec_send_event(rtems_id task)
{
  rtems_bsdnet_event_send(task, FEC_EVENT);
}

static void fec_wait_for_event(void)
{
  rtems_event_set out;
  rtems_bsdnet_event_receive(
    FEC_EVENT,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &out
  );
}

static void mcf548x_fec_request_restart(struct mcf548x_enet_struct *sc)
{
  sc->state = FEC_STATE_RESTART_0;
  fec_send_event(sc->txDaemonTid);
  fec_send_event(sc->rxDaemonTid);
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
  * polynomiumm itsc as a 32-bit constant.
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
static void mcf548x_fec_reset(struct mcf548x_enet_struct *sc) {
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
    sc->txFIFOError++;
  }
  if (ievent & MCF548X_FEC_EIR_RFERR) {
    sc->rxFIFOError++;
  }
  /*
   * fatal error ocurred?
   */
  if (ievent & (MCF548X_FEC_EIR_RFERR | MCF548X_FEC_EIR_XFERR)) {
    MCF548X_FEC_EIMR(chan) &=~(MCF548X_FEC_EIMR_RFERR | MCF548X_FEC_EIMR_XFERR);
    printk("fifo\n");
    mcf548x_fec_request_restart(sc);
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
    fec_send_event(sc->rxDaemonTid);
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

    fec_send_event(sc->txDaemonTid);
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
  MCF548X_FEC_FECTFAR(chan) = MCF548X_FEC_FECTFAR_ALARM(512);
  MCF548X_FEC_FECTFWR(chan) = MCF548X_FEC_FECTFWR_X_WMRK_256;

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

  fec_send_event(sc->txDaemonTid);

  }

static void fec_start_dma_and_controller(struct mcf548x_enet_struct *sc)
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

  /*
   * Enable FEC-Lite controller
   */
  MCF548X_FEC_ECR(chan) |= MCF548X_FEC_ECR_ETHER_EN;
}

static void mcf548x_fec_restart(struct mcf548x_enet_struct *sc, rtems_id otherDaemon)
{
  if (sc->state == FEC_STATE_RESTART_1) {
    mcf548x_fec_initialize_hardware(sc);
    fec_start_dma_and_controller(sc);
    sc->state = FEC_STATE_NORMAL;
  } else {
    sc->state = FEC_STATE_RESTART_1;
  }

  fec_send_event(otherDaemon);
  while (sc->state != FEC_STATE_NORMAL) {
    fec_wait_for_event();
  }
}

static void fec_reset_bd_and_discard_tx_frames(
  int bdCount,
  MCD_bufDescFec *bdRing,
  struct mbuf **mbufs
)
{
  int bdIndex = 0;

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    bool bdIsLast = bdIndex == bdCount - 1;
    struct mbuf *m = mbufs[bdIndex];

    bdRing[bdIndex].statCtrl = bdIsLast ? MCF548X_FEC_TBD_WRAP : 0;

    if (m != NULL) {
      mbufs[bdIndex] = NULL;
      m_free(m);
    }
  }
}

static void fec_reset_tx_dma(
  int dmaChan,
  int bdCount,
  MCD_bufDescFec *bdRing,
  struct mbuf **mbufs,
  struct mbuf *m
)
{
  if (m != NULL) {
    m_freem(m);
  }

  MCD_killDma(dmaChan);

  fec_reset_bd_and_discard_tx_frames(bdCount, bdRing, mbufs);
}

static struct mbuf *fec_next_fragment(
  struct ifnet *ifp,
  struct mbuf *m,
  bool *isFirst
)
{
  struct mbuf *n = NULL;

  *isFirst = false;

  while (true) {
    if (m == NULL) {
      IF_DEQUEUE(&ifp->if_snd, m);

      if (m != NULL) {
        *isFirst = true;
      } else {
        ifp->if_flags &= ~IFF_OACTIVE;

        return NULL;
      }
    }

    if (m->m_len > 0) {
      break;
    } else {
      m = m_free(m);
    }
  }

  n = m->m_next;
  while (n != NULL && n->m_len <= 0) {
    n = m_free(n);
  }
  m->m_next = n;

  return m;
}

static bool fec_transmit(
  struct ifnet *ifp,
  int dmaChan,
  int bdCount,
  MCD_bufDescFec *bdRing,
  struct mbuf **mbufs,
  int *bdIndexPtr,
  struct mbuf **mPtr,
  MCD_bufDescFec **firstPtr
)
{
  bool bdShortage = false;
  int bdIndex = *bdIndexPtr;
  struct mbuf *m = *mPtr;
  MCD_bufDescFec *first = *firstPtr;

  while (true) {
    MCD_bufDescFec *bd = &bdRing[bdIndex];

    MCDMA_CLR_PENDING(dmaChan);
    if ((bd->statCtrl & MCF548X_FEC_TBD_READY) == 0) {
      struct mbuf *done = mbufs[bdIndex];
      bool isFirst = false;

      if (done != NULL) {
        m_free(done);
        mbufs[bdIndex] = NULL;
      }

      m = fec_next_fragment(ifp, m, &isFirst);
      if (m != NULL) {
        bool bdIsLast = bdIndex == bdCount - 1;
        u16 status = bdIsLast ? MCF548X_FEC_TBD_WRAP : 0;

        bd->length = (u16) m->m_len;
        bd->dataPointer = mtod(m, u32);

        mbufs[bdIndex] = m;

        rtems_cache_flush_multiple_data_lines(mtod(m, void *), m->m_len);

        if (isFirst) {
          first = bd;
        } else {
          status |= MCF548X_FEC_TBD_READY;
        }

        if (m->m_next != NULL) {
          bd->statCtrl = status;
        } else {
          bd->statCtrl = status | MCF548X_FEC_TBD_INT | MCF548X_FEC_TBD_LAST;
          first->statCtrl |= MCF548X_FEC_TBD_READY;
          MCD_continDma(dmaChan);
        }

        m = m->m_next;
      } else {
        break;
      }
    } else {
      bdShortage = true;
      break;
    }

    if (bdIndex < bdCount - 1) {
      ++bdIndex;
    } else {
      bdIndex = 0;
    }
  }

  *bdIndexPtr = bdIndex;
  *mPtr = m;
  *firstPtr = first;

  return bdShortage;
}

static MCD_bufDescFec *fec_init_tx_dma(
  MCD_bufDescFec *bdRing,
  int bdCount
)
{
  int bdIndex;

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    bool bdIsLast = bdIndex == bdCount - 1;

    bdRing[bdIndex].dataPointer = 0;
    bdRing[bdIndex].length = 0;
    bdRing[bdIndex].statCtrl = bdIsLast ? MCF548X_FEC_RBD_WRAP : 0;
  }

  return bdRing;
}

static void mcf548x_fec_txDaemon(void *arg)
{
  struct mcf548x_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  int dmaChan = sc->txDmaChan;
  int bdIndex = 0;
  int bdCount = sc->txBdCount;
  struct mbuf **mbufs = &sc->txMbuf[0];
  struct mbuf *m = NULL;
  MCD_bufDescFec *bdRing = fec_init_tx_dma(sc->txBd, bdCount);
  MCD_bufDescFec *first = NULL;
  bool bdShortage = false;

  memset(mbufs, 0, bdCount * sizeof(*mbufs));

  while (true) {
    if (bdShortage) {
      mcdma_glue_irq_enable(dmaChan);
    }
    fec_wait_for_event();

    if (sc->state != FEC_STATE_NORMAL) {
      fec_reset_tx_dma(dmaChan, bdCount, bdRing, mbufs, m);
      mcf548x_fec_restart(sc, sc->rxDaemonTid);
      bdIndex = 0;
      m = NULL;
      first = NULL;
    }

    bdShortage = fec_transmit(
      ifp,
      dmaChan,
      bdCount,
      bdRing,
      mbufs,
      &bdIndex,
      &m,
      &first
    );
  }
}

static struct mbuf *fec_add_mbuf(
  int how,
  struct ifnet *ifp,
  MCD_bufDescFec *bd,
  bool bdIsLast
)
{
  struct mbuf *m;

  MGETHDR(m, how, MT_DATA);
  if (m != NULL) {
    MCLGET(m, how);
    if ((m->m_flags & M_EXT) != 0) {
      m->m_pkthdr.rcvif = ifp;

      rtems_cache_invalidate_multiple_data_lines(mtod(m, void *), ETHER_MAX_LEN);

      bd->dataPointer = mtod(m, u32);
      bd->length = ETHER_MAX_LEN;
      bd->statCtrl = MCF548X_FEC_RBD_EMPTY
        | MCF548X_FEC_RBD_INT
        | (bdIsLast ? MCF548X_FEC_RBD_WRAP : 0);
    } else {
      m_free(m);
    }
  }

  return m;
}

static MCD_bufDescFec *fec_init_rx_dma(
  MCD_bufDescFec *bdRing,
  struct ifnet *ifp,
  int bdCount,
  struct mbuf **mbufs
)
{
  int bdIndex;

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    bool bdIsLast = bdIndex == bdCount - 1;

    mbufs[bdIndex] = fec_add_mbuf(M_WAIT, ifp, &bdRing[bdIndex], bdIsLast);
  }

  return bdRing;
}

static void fec_reset_rx_dma(
  int dmaChan,
  int bdCount,
  MCD_bufDescFec *bdRing
)
{
  int bdIndex;

  MCD_killDma(dmaChan);

  for (bdIndex = 0; bdIndex < bdCount - 1; ++bdIndex) {
    bdRing[bdIndex].length = ETHER_MAX_LEN;
    bdRing[bdIndex].statCtrl = MCF548X_FEC_RBD_EMPTY | MCF548X_FEC_RBD_INT;
  }

  bdRing[bdIndex].length = ETHER_MAX_LEN;
  bdRing[bdIndex].statCtrl = MCF548X_FEC_RBD_EMPTY | MCF548X_FEC_RBD_INT | MCF548X_FEC_RBD_WRAP;
}

static int fec_ether_input(
  struct ifnet *ifp,
  int dmaChan,
  int bdIndex,
  int bdCount,
  MCD_bufDescFec *bdRing,
  struct mbuf **mbufs
)
{
  while (true) {
    bool bdIsLast = bdIndex == bdCount - 1;
    MCD_bufDescFec *bd = &bdRing[bdIndex];
    struct mbuf *m = mbufs[bdIndex];
    struct mbuf *n;
    u16 status;

    MCDMA_CLR_PENDING(dmaChan);
    status = bd->statCtrl;

    if ((status & MCF548X_FEC_RBD_EMPTY) != 0) {
      break;
    }

    n = fec_add_mbuf(0, ifp, bd, bdIsLast);
    if (n != NULL) {
      int len = bd->length - ETHER_HDR_LEN - ETHER_CRC_LEN;
      struct ether_header *eh = mtod(m, struct ether_header *);

      m->m_len = len;
      m->m_pkthdr.len = len;
      m->m_data = mtod(m, char *) + ETHER_HDR_LEN;

      ether_input(ifp, eh, m);
    } else {
      n = m;
    }

    mbufs[bdIndex] = n;

    if (bdIndex < bdCount - 1) {
      ++bdIndex;
    } else {
      bdIndex = 0;
    }
  }

  return bdIndex;
}

static void mcf548x_fec_rxDaemon(void *arg)
{
  struct mcf548x_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  int dmaChan = sc->rxDmaChan;
  int bdIndex = 0;
  int bdCount = sc->rxBdCount;
  struct mbuf **mbufs = &sc->rxMbuf[0];
  MCD_bufDescFec *bdRing = fec_init_rx_dma(sc->rxBd, ifp, bdCount, mbufs);

  while (true) {
    mcdma_glue_irq_enable(dmaChan);
    fec_wait_for_event();

    bdIndex = fec_ether_input(ifp, dmaChan, bdIndex, bdCount, bdRing, mbufs);

    if (sc->state != FEC_STATE_NORMAL) {
      fec_reset_rx_dma(dmaChan, bdCount, bdRing);
      mcf548x_fec_restart(sc, sc->txDaemonTid);
      bdIndex = 0;
    }
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

      bsp_interrupt_vector_enable(MCF548X_IRQ_FEC(chan));

      MCF548X_FEC_EIMR(chan) = FEC_INTR_MASK_USED;

      /*
       * Start driver tasks
       */
      txTaskName[3] = '0'+chan;
      rxTaskName[3] = '0'+chan;
      sc->txDaemonTid = rtems_bsdnet_newproc(txTaskName, 4096,
					     mcf548x_fec_txDaemon, sc);
      sc->rxDaemonTid = rtems_bsdnet_newproc(rxTaskName, 4096,
					     mcf548x_fec_rxDaemon, sc);
    }

  mcf548x_fec_request_restart(sc);

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
  printf ("       Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("        Rx Not First:%-8lu", sc->rxNotFirst);
  printf ("         Rx Not Last:%-8lu\n", sc->rxNotLast);
  printf ("            Rx Giant:%-8lu", sc->rxGiant);
  printf ("        Rx Non-octet:%-8lu", sc->rxNonOctet);
  printf ("          Rx Bad CRC:%-8lu\n", sc->rxBadCRC);
  printf ("       Rx FIFO Error:%-8lu", sc->rxFIFOError);
  printf ("        Rx Collision:%-8lu", sc->rxCollision);

  printf ("       Tx Interrupts:%-8lu\n", sc->txInterrupts);
  printf ("         Tx Deferred:%-8lu", sc->txDeferred);
  printf ("   Tx Late Collision:%-8lu", sc->txLateCollision);
  printf (" Tx Retransmit Limit:%-8lu\n", sc->txRetryLimit);
  printf ("         Tx Underrun:%-8lu", sc->txUnderrun);
  printf ("       Tx FIFO Error:%-8lu", sc->txFIFOError);
  printf ("       Tx Misaligned:%-8lu\n", sc->txMisaligned);

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


