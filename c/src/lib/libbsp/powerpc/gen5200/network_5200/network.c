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

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/mpc5200.h>
#include <net/if_var.h>
#include <errno.h>

#include <bsp/bestcomm/include/ppctypes.h>
#include <bsp/bestcomm/dma_image.h>
#include <bsp/bestcomm/bestcomm_glue.h>
#include <bsp/bestcomm/bestcomm_api.h>
#include <bsp/bestcomm/task_api/bestcomm_cntrl.h>
#include <bsp/bestcomm/task_api/tasksetup_bdtable.h>

/* #define ETH_DEBUG */

#define FEC_BD_LAST TASK_BD_TFD
#define FEC_BD_INT TASK_BD_INT
#define FEC_BD_READY SDMA_BD_MASK_READY

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     64
#define TX_BUF_COUNT     64

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

#define FEC_EVENT RTEMS_EVENT_0

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

#define FEC_INTR_MASK_USED \
(FEC_INTR_LCEN  |FEC_INTR_CRLEN	|\
 FEC_INTR_XFUNEN|FEC_INTR_XFERREN|FEC_INTR_RFERREN)

typedef enum {
  FEC_STATE_RESTART_0,
  FEC_STATE_RESTART_1,
  FEC_STATE_NORMAL,
} mpc5200_fec_state;

/*
 * Device data
 */
typedef struct {
  struct arpcom           arpcom;
  struct mbuf             **rxMbuf;
  struct mbuf             **txMbuf;
  mpc5200_fec_state       state;
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

  struct rtems_mdio_info  mdio;
  int                     phyAddr;
  bool                    phyInitialized;
} mpc5200_fec_context;

static mpc5200_fec_context enet_driver[NIFACES];

static void mpc5200_fec_send_event(rtems_id task)
{
  rtems_event_send(task, FEC_EVENT);
}

static void mpc5200_fec_wait_for_event(void)
{
  rtems_event_set out;
  rtems_bsdnet_event_receive(
    FEC_EVENT,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &out
  );
}

static void mpc5200_fec_stop_dma(void)
{
  TaskStop(FEC_RECV_TASK_NO);
  TaskStop(FEC_XMIT_TASK_NO);
}

static void mpc5200_fec_request_restart(mpc5200_fec_context *self)
{
  self->state = FEC_STATE_RESTART_0;
  mpc5200_fec_send_event(self->txDaemonTid);
  mpc5200_fec_send_event(self->rxDaemonTid);
}

static void mpc5200_fec_start_dma_and_controller(void)
{
  TaskStart(FEC_RECV_TASK_NO, 1, FEC_RECV_TASK_NO, 1);
  TaskStart(FEC_XMIT_TASK_NO, 1, FEC_XMIT_TASK_NO, 1);

  mpc5200.ecntrl |= FEC_ECNTRL_OE | FEC_ECNTRL_EN;
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
static void mpc5200_eth_addr_filter_set(mpc5200_fec_context *self)  {
  unsigned char *mac;
  unsigned char currByte;				/* byte for which to compute the CRC */
  int           byte;					/* loop - counter */
  int           bit;					/* loop - counter */
  unsigned long crc = 0xffffffff;		/* initial value */

 /*
  * Get the mac address of ethernet controller
  */
  mac = (unsigned char *)(&self->arpcom.ac_enaddr);

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

static int mpc5200_eth_mii_transfer(
  int phyAddr,
  unsigned regAddr,
  uint32_t data
)
{
  int timeout = 0xffff;

  mpc5200.ievent = FEC_INTR_MII;

  mpc5200.mii_data = MPC5200_FEC_MII_DATA_ST
    | MPC5200_FEC_MII_DATA_TA
    | (phyAddr << MPC5200_FEC_MII_DATA_PA_SHIFT)
    | (regAddr << MPC5200_FEC_MII_DATA_RA_SHIFT)
    | data;

  while (timeout > 0 && (mpc5200.ievent & FEC_INTR_MII) == 0) {
    --timeout;
  }

  return timeout > 0 ? 0 : -1;
}

/* FIXME: Make this static, this needs a fix in an application */
int mpc5200_eth_mii_read(
  int phyAddr,
  void *arg,
  unsigned regAddr,
  uint32_t *retVal
)
{
  int rv = mpc5200_eth_mii_transfer(
    phyAddr,
    regAddr,
    MPC5200_FEC_MII_DATA_OP_RD
  );

  *retVal = mpc5200.mii_data & 0xffff;

  return rv;
}

static int mpc5200_eth_mii_write(
  int phyAddr,
  void *arg,
  unsigned regAddr,
  uint32_t data
)
{
  return mpc5200_eth_mii_transfer(
    phyAddr,
    regAddr,
    MPC5200_FEC_MII_DATA_OP_WR | data
  );
}


/*
 * Reset a running ethernet driver including the hardware FIFOs and the FEC.
 */
static void mpc5200_fec_reset(mpc5200_fec_context *self)
{
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
void mpc5200_fec_off(mpc5200_fec_context *self)
  {
  int            counter = 0xffff;


#if defined(ETH_DEBUG)
  uint32_t phyStatus;
  int i;

  for(i = 0; i < 9; i++)
    {

    mpc5200_eth_mii_read(self->phyAddr, self, i, &phyStatus);
    printf ("Mii reg %d: 0x%04" PRIx32 "\r\n", i, phyStatus);

    }

  for(i = 16; i < 21; i++)
    {

    mpc5200_eth_mii_read(self->phyAddr, self, i, &phyStatus);
    printf ("Mii reg %d: 0x%04" PRIx32 "\r\n", i, phyStatus);

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

  mpc5200_fec_stop_dma();

 /*
  * Disable transmit / receive interrupts
  */
  bestcomm_glue_irq_disable(FEC_XMIT_TASK_NO);
  bestcomm_glue_irq_disable(FEC_RECV_TASK_NO);

 /*
  * Disable the Ethernet Controller
  */
  mpc5200.ecntrl &= ~(FEC_ECNTRL_OE | FEC_ECNTRL_EN);
}

/*
 * MPC5200 FEC interrupt handler
 */
void mpc5200_fec_irq_handler(rtems_irq_hdl_param handle)
{
  mpc5200_fec_context *self = (mpc5200_fec_context *) handle;
  volatile uint32_t ievent;

  ievent = mpc5200.ievent;

  mpc5200.ievent = ievent;
  /*
   * check errors, update statistics
   */
  if (ievent & FEC_INTR_LATE_COL) {
    self->txLateCollision++;
  }
  if (ievent & FEC_INTR_COL_RETRY) {
    self->txRetryLimit++;
  }
  if (ievent & FEC_INTR_XFIFO_UN) {
    self->txUnderrun++;
  }
  if (ievent & FEC_INTR_XFIFO_ERR) {
    self->txFIFOError++;
  }
  if (ievent & FEC_INTR_RFIFO_ERR) {
    self->rxFIFOError++;
  }
  /*
   * fatal error ocurred?
   */
  if (ievent & (FEC_INTR_XFIFO_ERR | FEC_INTR_RFIFO_ERR)) {
    mpc5200.imask &= ~(FEC_INTR_XFERREN | FEC_INTR_RFERREN);
    mpc5200_fec_request_restart(self);
  }
}

void mpc5200_smartcomm_rx_irq_handler(void *arg)
{
  mpc5200_fec_context *self = arg;

  ++self->rxInterrupts;
  mpc5200_fec_send_event(self->rxDaemonTid);
  SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend, FEC_RECV_TASK_NO);
  bestcomm_glue_irq_disable(FEC_RECV_TASK_NO);
}

void mpc5200_smartcomm_tx_irq_handler(void *arg)
{
  mpc5200_fec_context *self = arg;

  ++self->txInterrupts;
  mpc5200_fec_send_event(self->txDaemonTid);
  SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend, FEC_XMIT_TASK_NO);
  bestcomm_glue_irq_disable(FEC_XMIT_TASK_NO);
}

static void mpc5200_fec_init_mib(mpc5200_fec_context *self)
{
  memset(&mpc5200.RES [0], 0, 0x2e4);
  mpc5200.mib_control = 0x40000000;
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
static void mpc5200_fec_initialize_hardware(mpc5200_fec_context *self)
{
  /* We want at most 2.5MHz */
  uint32_t div = 2 * 2500000;
  uint32_t mii_speed = (IPB_CLOCK + div - 1) / div;

 /*
  * Reset mpc5200 FEC
  */
  mpc5200_fec_reset(self);
  mpc5200_fec_init_mib(self);

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
  * Set MII_SPEED = IPB clock / (2 * mii_speed))
  * and do not drop the Preamble.
  */
  mpc5200.mii_speed = mii_speed << 1;

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
  mpc5200_eth_addr_filter_set(self);

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
static void mpc5200_fec_initialize_phy(mpc5200_fec_context *self)
{
  if (self->phyInitialized) {
    return;
  } else {
    self->phyInitialized = true;
  }

 /*
  * Reset PHY, then delay 300ns
  */
  mpc5200_eth_mii_write(self->phyAddr, self, 0x0, 0x8000);

  rtems_task_wake_after(2);

 /* MII100 */

 /*
  * Set the auto-negotiation advertisement register bits
  */
  mpc5200_eth_mii_write(self->phyAddr, self, 0x4, 0x01e1);

 /*
  * Set MDIO bit 0.12 = 1(&& bit 0.9=1?) to enable auto-negotiation
  */
  mpc5200_eth_mii_write(self->phyAddr, self, 0x0, 0x1200);

 /*
  * Wait for AN completion
  */
#if 0
  int timeout = 0x100;
  uint32_t phyStatus;
  do
    {

    rtems_task_wake_after(2);

    if((timeout--) == 0)
      {

#if defined(ETH_DEBUG)
    printf ("MPC5200FEC PHY auto neg failed." "\r\n");
#endif

      }

    if(mpc5200_eth_mii_read(self->phyAddr, self, 0x1, &phyStatus) != true)
      {

#if defined(ETH_DEBUG)
      printf ("MPC5200FEC PHY auto neg failed: 0x%04" PRIx32 ".\r\n", phyStatus);
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
  uint32_t phyStatus;
 /*
  * Print PHY registers after initialization.
  */
  for(i = 0; i < 9; i++)
    {

	mpc5200_eth_mii_read(self->phyAddr, self, i, &phyStatus);
	printf ("Mii reg %d: 0x%04" PRIx32 "\r\n", i, phyStatus);

	}

  for(i = 16; i < 21; i++)
    {

    mpc5200_eth_mii_read(self->phyAddr, self, i, &phyStatus);
    printf ("Mii reg %d: 0x%04" PRIx32 "\r\n", i, phyStatus);

    }
#endif	/* ETH_DEBUG */

  }

static void mpc5200_fec_restart(mpc5200_fec_context *self, rtems_id otherDaemon)
{
  if (self->state == FEC_STATE_RESTART_1) {
    mpc5200_fec_initialize_hardware(self);
    mpc5200_fec_initialize_phy(self);
    mpc5200_fec_start_dma_and_controller();
    self->state = FEC_STATE_NORMAL;
  } else {
    self->state = FEC_STATE_RESTART_1;
  }

  mpc5200_fec_send_event(otherDaemon);
  while (self->state != FEC_STATE_NORMAL) {
    mpc5200_fec_wait_for_event();
  }
}

/*
 * Send packet (caller provides header).
 */
static void mpc5200_fec_tx_start(struct ifnet *ifp)
  {

  mpc5200_fec_context *self = ifp->if_softc;

  ifp->if_flags |= IFF_OACTIVE;

  mpc5200_fec_send_event(self->txDaemonTid);

  }

static TaskBD1_t *mpc5200_fec_init_tx_dma(int bdCount, struct mbuf **mbufs)
{
  TaskSetupParamSet_t param = {
    .NumBD = bdCount,
    .Size = {
      .MaxBuf = ETHER_MAX_LEN
    },
    .Initiator = 0,
    .StartAddrSrc = 0,
    .IncrSrc = sizeof(uint32_t),
    .SzSrc = sizeof(uint32_t),
    .StartAddrDst = (uint32) &mpc5200.tfifo_data,
    .IncrDst = 0,
    .SzDst = sizeof(uint32_t)
  };
  int bdIndex = 0;

  TaskSetup(FEC_XMIT_TASK_NO, &param);

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    mbufs [bdIndex] = NULL;
  }

  return (TaskBD1_t *) TaskGetBDRing(FEC_XMIT_TASK_NO);
}

#if 0
static void mpc5200_fec_requeue_and_discard_tx_frames(
  int bdIndex,
  int bdCount,
  TaskBD1_t *bdRing,
  struct mbuf **mbufs
)
{
  int bdStop = bdIndex;
  struct mbuf *previous = NULL;

  do {
    struct mbuf *current = NULL;
    uint32 status = 0;
    TaskBD1_t *bd = NULL;

    if (bdIndex > 1) {
      --bdIndex;
    } else {
      bdIndex = bdCount - 1;
    }

    current = mbufs [bdIndex];
    mbufs [bdIndex] = NULL;

    status = bdRing [bdIndex].Status;
    bdRing [bdIndex].Status = 0;

    if (current != NULL) {
      if ((status & FEC_BD_LAST) != 0) {
        if (previous != NULL) {
          IF_PREPEND(&ifp->if_snd, previous);
        }
      }
    } else {
      break;
    }

    previous = current;
  } while (bdIndex != bdStop);
}
#endif

static void mpc5200_fec_discard_tx_frames(
  int bdCount,
  struct mbuf **mbufs
)
{
  int bdIndex = 0;

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    struct mbuf *m = mbufs [bdIndex];

    if (m != NULL) {
      mbufs [bdIndex] = NULL;
      m_free(m);
    }
  }
}

static void mpc5200_fec_reset_tx_dma(
  int bdCount,
  TaskBD1_t *bdRing,
  struct mbuf **mbufs,
  struct mbuf *m
)
{
  TaskStop(FEC_XMIT_TASK_NO);
  TaskBDReset(FEC_XMIT_TASK_NO);
  mpc5200_fec_discard_tx_frames(bdCount, mbufs);
  while (m != NULL) {
    m = m_free(m);
  }
}

static struct mbuf *mpc5200_fec_next_fragment(
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

static bool mpc5200_fec_transmit(
  struct ifnet *ifp,
  int bdCount,
  TaskBD1_t *bdRing,
  struct mbuf **mbufs,
  int *bdIndexPtr,
  struct mbuf **mPtr,
  TaskBD1_t **firstPtr
)
{
  bool bdShortage = false;
  int bdIndex = *bdIndexPtr;
  struct mbuf *m = *mPtr;
  TaskBD1_t *first = *firstPtr;

  while (true) {
    TaskBD1_t *bd = &bdRing [bdIndex];

    if (bd->Status == 0) {
      struct mbuf *done = mbufs [bdIndex];
      bool isFirst = false;

      if (done != NULL) {
        m_free(done);
        mbufs [bdIndex] = NULL;
      }

      m = mpc5200_fec_next_fragment(ifp, m, &isFirst);
      if (m != NULL) {
        uint32 status = (uint32) m->m_len;

        mbufs [bdIndex] = m;

        rtems_cache_flush_multiple_data_lines(mtod(m, void *), m->m_len);

        if (isFirst) {
          first = bd;
        } else {
          status |= FEC_BD_READY;
        }

        bd->DataPtr [0] = mtod(m, uint32);

        if (m->m_next != NULL) {
          bd->Status = status;
        } else {
          bd->Status = status | FEC_BD_INT | FEC_BD_LAST;
          first->Status |= FEC_BD_READY;
          SDMA_TASK_ENABLE(SDMA_TCR, FEC_XMIT_TASK_NO);
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

static void mpc5200_fec_tx_daemon(void *arg)
{
  mpc5200_fec_context *self = arg;
  struct ifnet *ifp = &self->arpcom.ac_if;
  int bdIndex = 0;
  int bdCount = self->txBdCount;
  struct mbuf **mbufs = &self->txMbuf [0];
  struct mbuf *m = NULL;
  TaskBD1_t *bdRing = mpc5200_fec_init_tx_dma(bdCount, mbufs);
  TaskBD1_t *first = NULL;
  bool bdShortage = false;

  while (true) {
    if (bdShortage) {
      bestcomm_glue_irq_enable(FEC_XMIT_TASK_NO);
    }
    mpc5200_fec_wait_for_event();

    if (self->state != FEC_STATE_NORMAL) {
      mpc5200_fec_reset_tx_dma(bdCount, bdRing, mbufs, m);
      mpc5200_fec_restart(self, self->rxDaemonTid);
      bdIndex = 0;
      m = NULL;
      first = NULL;
    }

    bdShortage = mpc5200_fec_transmit(
      ifp,
      bdCount,
      bdRing,
      mbufs,
      &bdIndex,
      &m,
      &first
    );
  }
}

static struct mbuf *mpc5200_fec_add_mbuf(struct ifnet *ifp, TaskBD1_t *bd)
{
  struct mbuf *m = NULL;

  MGETHDR (m, M_WAIT, MT_DATA);
  MCLGET (m, M_WAIT);
  m->m_pkthdr.rcvif = ifp;

  /* XXX: The dcbi operation does not work properly */
  rtems_cache_flush_multiple_data_lines(mtod(m, void *), ETHER_MAX_LEN);

  bd->DataPtr [0] = mtod(m, uint32);
  bd->Status = ETHER_MAX_LEN | FEC_BD_READY;

  return m;
}

static TaskBD1_t *mpc5200_fec_init_rx_dma(
  struct ifnet *ifp,
  int bdCount,
  struct mbuf **mbufs
)
{
  TaskSetupParamSet_t param = {
    .NumBD = bdCount,
    .Size = {
      .MaxBuf = ETHER_MAX_LEN
    },
    .Initiator = 0,
    .StartAddrSrc = (uint32) &mpc5200.rfifo_data,
    .IncrSrc = 0,
    .SzSrc = sizeof(uint32_t),
    .StartAddrDst = 0,
    .IncrDst = sizeof(uint32_t),
    .SzDst = sizeof(uint32_t)
  };
  TaskBD1_t *bdRing = NULL;
  int bdIndex = 0;

  TaskSetup(FEC_RECV_TASK_NO, &param);
  bdRing = (TaskBD1_t *) TaskGetBDRing(FEC_RECV_TASK_NO);

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    mbufs [bdIndex] = mpc5200_fec_add_mbuf(ifp, &bdRing [bdIndex]);
  }

  return bdRing;
}

static void mpc5200_fec_reset_rx_dma(int bdCount, TaskBD1_t *bdRing)
{
  int bdIndex = 0;

  TaskStop(FEC_RECV_TASK_NO);
  TaskBDReset(FEC_RECV_TASK_NO);

  for (bdIndex = 0; bdIndex < bdCount; ++bdIndex) {
    bdRing [bdIndex].Status = ETHER_MAX_LEN | FEC_BD_READY;
  }
}

static int mpc5200_fec_ether_input(
  struct ifnet *ifp,
  int bdIndex,
  int bdCount,
  TaskBD1_t *bdRing,
  struct mbuf **mbufs
)
{
  while (true) {
    TaskBD1_t *bd = &bdRing [bdIndex];
    struct mbuf *m = mbufs [bdIndex];
    uint32 status = 0;
    int len = 0;
    struct ether_header *eh = NULL;

    SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend, FEC_RECV_TASK_NO);
    status = bd->Status;

    if ((status & FEC_BD_READY) != 0) {
      break;
    }

    eh = mtod(m, struct ether_header *);

    len = (status & 0xffff) - ETHER_HDR_LEN - ETHER_CRC_LEN;
    m->m_len = len;
    m->m_pkthdr.len = len;
    m->m_data = mtod(m, char *) + ETHER_HDR_LEN;

    ether_input(ifp, eh, m);

    mbufs [bdIndex] = mpc5200_fec_add_mbuf(ifp, bd);

    if (bdIndex < bdCount - 1) {
      ++bdIndex;
    } else {
      bdIndex = 0;
    }
  }

  return bdIndex;
}

static void mpc5200_fec_rx_daemon(void *arg)
{
  mpc5200_fec_context *self = arg;
  struct ifnet *ifp = &self->arpcom.ac_if;
  int bdIndex = 0;
  int bdCount = self->rxBdCount;
  struct mbuf **mbufs = &self->rxMbuf [0];
  TaskBD1_t *bdRing = mpc5200_fec_init_rx_dma(ifp, bdCount, mbufs);

  while (true) {
    bestcomm_glue_irq_enable(FEC_RECV_TASK_NO);
    mpc5200_fec_wait_for_event();

    bdIndex = mpc5200_fec_ether_input(ifp, bdIndex, bdCount, bdRing, mbufs);

    if (self->state != FEC_STATE_NORMAL) {
      mpc5200_fec_reset_rx_dma(bdCount, bdRing);
      mpc5200_fec_restart(self, self->txDaemonTid);
      bdIndex = 0;
    }
  }
}

/*
 * Initialize and start the device
 */
static void mpc5200_fec_init(void *arg)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  mpc5200_fec_context *self = (mpc5200_fec_context *)arg;
  struct ifnet *ifp = &self->arpcom.ac_if;

  if(self->txDaemonTid == 0)
    {
      size_t rxMbufTableSize = self->rxBdCount * sizeof(*self->rxMbuf);
      size_t txMbufTableSize = self->txBdCount * sizeof(*self->txMbuf);

      /*
       * Allocate a set of mbuf pointers
       */
      self->rxMbuf = malloc(rxMbufTableSize, M_MBUF, M_NOWAIT);
      self->txMbuf = malloc(txMbufTableSize, M_MBUF, M_NOWAIT);

      if(!self->rxMbuf || !self->txMbuf)
	rtems_panic ("No memory for mbuf pointers");

      /*
       * DMA setup
       */
      bestcomm_glue_init();
      mpc5200.sdma.ipr [0] = 4;	/* always initiator	*/
      mpc5200.sdma.ipr [3] = 6;	/* eth rx initiator	*/
      mpc5200.sdma.ipr [4] = 5;	/* eth tx initiator	*/

      /*
       * Set up interrupts
       */
      bestcomm_glue_irq_install(FEC_RECV_TASK_NO,
				mpc5200_smartcomm_rx_irq_handler,
				self);
      bestcomm_glue_irq_install(FEC_XMIT_TASK_NO,
				mpc5200_smartcomm_tx_irq_handler,
				self);
      sc = rtems_interrupt_handler_install(
        BSP_SIU_IRQ_ETH,
        "FEC",
        RTEMS_INTERRUPT_UNIQUE,
        mpc5200_fec_irq_handler,
        self
      );
      if(sc != RTEMS_SUCCESSFUL) {
	rtems_panic ("Can't attach MPC5x00 FEX interrupt handler\n");
      }

      /*
       * Start driver tasks
       */
      self->txDaemonTid = rtems_bsdnet_newproc("FEtx", 4096, mpc5200_fec_tx_daemon, self);
      self->rxDaemonTid = rtems_bsdnet_newproc("FErx", 4096, mpc5200_fec_rx_daemon, self);
    }

  mpc5200_fec_request_restart(self);

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

static void enet_stats (mpc5200_fec_context *self)
{
  if (self->phyAddr >= 0) {
    struct ifnet *ifp = &self->arpcom.ac_if;
    int media = IFM_MAKEWORD(0, 0, 0, self->phyAddr);
    int rv = (*ifp->if_ioctl)(ifp, SIOCGIFMEDIA, (caddr_t) &media);

    if (rv == 0) {
      rtems_ifmedia2str(media, NULL, 0);
      printf ("\n");
    } else {
      printf ("PHY communication error\n");
    }
  }
  printf ("       Rx Interrupts:%-8lu", self->rxInterrupts);
  printf ("        Rx Not First:%-8lu", self->rxNotFirst);
  printf ("         Rx Not Last:%-8lu\n", self->rxNotLast);
  printf ("            Rx Giant:%-8lu", self->rxGiant);
  printf ("        Rx Non-octet:%-8lu", self->rxNonOctet);
  printf ("          Rx Bad CRC:%-8lu\n", self->rxBadCRC);
  printf ("       Rx FIFO Error:%-8lu", self->rxFIFOError);
  printf ("        Rx Collision:%-8lu", self->rxCollision);

  printf ("       Tx Interrupts:%-8lu\n", self->txInterrupts);
  printf ("         Tx Deferred:%-8lu", self->txDeferred);
  printf ("   Tx Late Collision:%-8lu", self->txLateCollision);
  printf (" Tx Retransmit Limit:%-8lu\n", self->txRetryLimit);
  printf ("         Tx Underrun:%-8lu", self->txUnderrun);
  printf ("       Tx FIFO Error:%-8lu", self->txFIFOError);
  printf ("       Tx Misaligned:%-8lu\n", self->txMisaligned);
}

int32_t mpc5200_fec_setMultiFilter(struct ifnet *ifp)
{
  /*mpc5200_fec_context *self = ifp->if_softc; */
  /* XXX anything to do? */
  return 0;
}


/*
 * Driver ioctl handler
 */
static int mpc5200_fec_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
  {
  mpc5200_fec_context *self = ifp->if_softc;
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
                  ? ether_addmulti(ifr, &self->arpcom)
                  : ether_delmulti(ifr, &self->arpcom);

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

          mpc5200_fec_off(self);

          break;

        case IFF_UP:

          mpc5200_fec_init(self);

          break;

        case IFF_UP | IFF_RUNNING:

          mpc5200_fec_off(self);
          mpc5200_fec_init(self);

          break;

        default:
          break;

        }

      break;

    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      error = rtems_mii_ioctl(&self->mdio, self, command, (int *) data);
      break;

    case SIO_RTEMS_SHOW_STATS:

      enet_stats(self);

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
  mpc5200_fec_context *self;
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

  self = &enet_driver[unitNumber - 1];
  ifp = &self->arpcom.ac_if;

  if(ifp->if_softc != NULL)
    {

    printf ("Driver already in use.\n");
    return 0;

    }

  self->mdio.mdio_r = mpc5200_eth_mii_read;
  self->mdio.mdio_w = mpc5200_eth_mii_write;

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
  	memcpy((void *)self->arpcom.ac_enaddr, &nvram->enaddr, ETHER_ADDR_LEN);

    }
  else
    if(config->hardware_address)
      {

      /* There is no entry in NVRAM, but there is in the ifconfig struct, so use it. */
      memcpy((void *)self->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
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

    memcpy(self->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

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
  if ((self->arpcom.ac_enaddr[0] == 0) &&
      (self->arpcom.ac_enaddr[1] == 0) &&
      (self->arpcom.ac_enaddr[2] == 0)) {
      memcpy(
        (void *)self->arpcom.ac_enaddr,
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
    self->rxBdCount = config->rbuf_count;
  else
    self->rxBdCount = RX_BUF_COUNT;

  if(config->xbuf_count)
    self->txBdCount = config->xbuf_count;
  else
    self->txBdCount = TX_BUF_COUNT;

  self->acceptBroadcast = !config->ignore_broadcast;

 /*
  * Set up network interface values
  */
  ifp->if_softc   = self;
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


