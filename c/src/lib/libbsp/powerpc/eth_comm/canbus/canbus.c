/*
 * RTEMS CANBUS driver for eth-comm BSP
 *
 * Written by Jay Monkman (jmonkman@frasca.com)
 *
 *  COPYRIGHT (c) 1998
 *  Frasca International, Inc.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Note: All of this code assumes a 10Mhz clock input to the 82527
 *
 *  $Id$
 */
#include <stdio.h>
#include <bsp.h>
#include <rtems/error.h>
#include <canbus.h>
/* How many CAN interfaces are there? */
#define NUM_CAN_DEVS 3

/* How many received messages should be buffered for each channel */
#define RX_CAN_BUF_SIZE 16

int rxMsgBufHead[NUM_CAN_DEVS];
int rxMsgBufTail[NUM_CAN_DEVS];
i82527_msg_t rxMsgBuf[NUM_CAN_DEVS][RX_CAN_BUF_SIZE];

volatile i82527_t *candev[NUM_CAN_DEVS];


static rtems_isr
canInterruptHandler (rtems_vector_number v)
{
  int dev;
  int tmpTail;

  switch (v) {
  case PPC_IRQ_IRQ3: dev = 0; break;
  case PPC_IRQ_IRQ4: dev = 1; break;
  case PPC_IRQ_IRQ2: dev = 2; break;
  default:           return;    /* something screwed up */
  }

  /* we only do rx interrupts right now */
  if (!(candev[dev]->msg15.ctrl1 & I82527_MSG_CTRL_NEWDAT)) {
    /* Hmmm, that's odd. Why were we triggered? */
    candev[dev]->msg15.ctrl0 = 0xff & (I82527_MSG_CTRL_INTPND_CLR |
                                       I82527_MSG_CTRL_MSGVAL_SET);
    candev[dev]->msg15.ctrl1 = 0xff & (I82527_MSG_CTRL_RMTPND_CLR |
                                       I82527_MSG_CTRL_MSGLST_CLR |
                                       I82527_MSG_CTRL_NEWDAT_CLR);
    return;
  }
  tmpTail = rxMsgBufTail[dev];
  while (1) {
    if ((tmpTail == rxMsgBufHead[dev]) && 
        (rxMsgBuf[dev][tmpTail].ctrl1 & I82527_MSG_CTRL_NEWDAT)) {
      break;  /* Buf is full */
    }

    if (!(rxMsgBuf[dev][tmpTail].ctrl1 & I82527_MSG_CTRL_NEWDAT)) {
      int pkt_len;
      int i;

      rxMsgBuf[dev][tmpTail].ctrl0 = candev[dev]->msg15.ctrl0;
      rxMsgBuf[dev][tmpTail].ctrl1 = candev[dev]->msg15.ctrl1;
      rxMsgBuf[dev][tmpTail].arb = candev[dev]->msg15.arb;
      rxMsgBuf[dev][tmpTail].cfg = candev[dev]->msg15.cfg;
      
      pkt_len = (rxMsgBuf[dev][tmpTail].cfg >> 4) & 0xf;
      for (i=0; i<pkt_len; i++) {
        rxMsgBuf[dev][tmpTail].data[i] = candev[dev]->msg15.data[i];
      }

      tmpTail++;
      if (tmpTail == RX_CAN_BUF_SIZE) {
        tmpTail = 0;
      }

      rxMsgBufTail[dev] = tmpTail;

      break;
    }

    tmpTail++;
    if (tmpTail == RX_CAN_BUF_SIZE) {
      tmpTail = 0;
    }
    if (tmpTail == rxMsgBufTail[dev]) {
      break;
    }
  }


  candev[dev]->msg15.ctrl0  = 0xff & (I82527_MSG_CTRL_MSGVAL_SET |
                                      I82527_MSG_CTRL_INTPND_CLR);
  candev[dev]->msg15.ctrl1  = 0xff & (I82527_MSG_CTRL_NEWDAT_CLR | 
                                      I82527_MSG_CTRL_RMTPND_CLR);
  candev[dev]->status = 0x0;
}

rtems_device_driver canbus_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  int i,j;
#if 0
  char dev_str[16]; /* This allows us to have a device name up to */
                    /* 15 chars long. If we only use names like   */
                    /* /dev/can0 (9 chars) we will be fine up to  */
                    /* /dev/can9999999 */
#endif
  rtems_status_code status;
  rtems_isr_entry old_handler;
  
#if (NUM_CAN_DEVS > 0)
  candev[0]=&canbus0;
  rtems_interrupt_catch (canInterruptHandler,
                         PPC_IRQ_IRQ3,
                         &old_handler);

#if (NUM_CAN_DEVS > 1)
  candev[1]=&canbus1;
  rtems_interrupt_catch (canInterruptHandler,
                         PPC_IRQ_IRQ4,
                         &old_handler);

#if (NUM_CAN_DEVS > 2)
  candev[2]=&canbus2;
  rtems_interrupt_catch (canInterruptHandler,
                         PPC_IRQ_IRQ2,
                         &old_handler);
  
  /* Right now, we only support 3 CAN interfaces */
#else
#error NUM_CAN_DEVS is too big. Fix it, damnit!
#endif /* NUM_CAN_DEVS > 2 */
#endif /* NUM_CAN_DEVS > 1 */
#else
#error NUM_CAN_DEVS is 0. It needs to be at least 1
#endif /* NUM_CAN_DEVS > 0 */


  for (i=0; i < NUM_CAN_DEVS; i++) {
    
    /* clear rx buffers */
    rxMsgBufHead[i] = 0;
    rxMsgBufTail[i] = 0;
    for (j=0; j < RX_CAN_BUF_SIZE; j++) {
      rxMsgBuf[i][j].ctrl0 = 0x55; /* all flags are cleared */
      rxMsgBuf[i][j].ctrl1 = 0x55; /* all flags are cleared */
    }

    candev[i]->ctrl = I82527_CTRL_CCE | /* Enable cfg reg writes */
                      I82527_CTRL_INIT; /* Disable external xfers */
  
    candev[i]->cir = I82527_CIR_DMC;    /* Divide memory clock by 2 */

    
    /* We want 250 kbps so assuming an input clock rate of 10 MHz:
     *   DSC = 0  =>  SCLK = 10 MHz, tSCLK = 100ns
     *   BRP = 1  =>  tq = 200ns
     *   tSYNC_SEG = 1 tq
     *   tSEG1 = TSEG1+1 = 14+1 = 15
     *   tSEG2 = TSEG2+1 = 3+1  = 4
     *
     *  bittime = tSYNC_SEG + tSEG1 + tSEG2
     *          = 1 + 15 + 4 = 20
     *  baudrate = 1/(bittime * tq) = 1/(20 * 200ns) = 1/(4000ns) = 250 kbps
     */
    candev[i]->btr0 = 0xc1;  /* Baud rate prescaler=0, Sync jump width=3 */

    candev[i]->btr1 = I82527_BTR1_SPL | /* go for noise immunity */
                      (0x3 << 4) |      /* TSEG2 = 3 */
                      (0xe);            /* TSEG1 = 14 */

    candev[i]->gms = 0xffff;            /* addresses must match exactly */
    candev[i]->gml = 0xffffffff;        /* addresses must match exactly */
    
    candev[i]->mlm = 0x0;               /* all addresses accepted */

    candev[i]->p2conf = 0xff;           /* make all outputs */

    candev[i]->msg1.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg1.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg2.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg2.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg3.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg3.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg4.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg4.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR  | /* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg5.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg5.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg6.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg6.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg7.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg7.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg8.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg8.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg9.cfg    = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg9.ctrl0  = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg10.cfg   = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg10.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR  | /* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg11.cfg   = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg11.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg12.cfg   = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg12.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg13.cfg   = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg13.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg14.cfg   = I82527_MSG_CFG_DIR ;        /* dir is xmit */
    candev[i]->msg14.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR; 

    candev[i]->msg15.cfg   = 0 ;        /* dir is rcv */
    candev[i]->msg15.ctrl0 = I82527_MSG_CTRL_MSGVAL_CLR |/* this msg invalid */
                             I82527_MSG_CTRL_TXIE_CLR   |/* no tx interrupts */
                             I82527_MSG_CTRL_RXIE_CLR   |/* no rx interrupts */
                             I82527_MSG_CTRL_INTPND_CLR;
    candev[i]->msg15.ctrl1 = I82527_MSG_CTRL_RMTPND_CLR |
                             I82527_MSG_CTRL_TXRQ_CLR   |
                             I82527_MSG_CTRL_MSGLST_CLR |
                             I82527_MSG_CTRL_NEWDAT_CLR;

  }

  if ((status=rtems_io_register_name ("/dev/can0", major, 0)) !=
      RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred (status);
  }
  if ((status=rtems_io_register_name ("/dev/can1", major, 1)) !=
      RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred (status);
  }
  if ((status=rtems_io_register_name ("/dev/can2", major, 2)) !=
      RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred (status);
  }
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver canbus_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
    /* msg is in use, rx interrupts are enabled */
  candev[minor]->msg15.ctrl0 = 0xff & (I82527_MSG_CTRL_MSGVAL_SET |
                                       I82527_MSG_CTRL_RXIE_SET);
    
  candev[minor]->ctrl |= I82527_CTRL_IE;
  candev[minor]->ctrl &= ~(I82527_CTRL_CCE | I82527_CTRL_INIT);
  switch (minor) {
  case 0: m8xx.simask |= M8xx_SIMASK_IRM3; break;
  case 1: m8xx.simask |= M8xx_SIMASK_IRM4; break;
  case 2: m8xx.simask |= M8xx_SIMASK_IRM2; break;
  default: return 0xffffffff;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver canbus_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  /* msg is not in use, rx & txinterrupts are disbled */
  candev[minor]->msg15.ctrl0 = 0xff & (I82527_MSG_CTRL_MSGVAL_CLR |
                                       I82527_MSG_CTRL_RXIE_CLR |
                                       I82527_MSG_CTRL_TXIE_CLR);
    
  /* Take transceiver off the bus, enable cfg. reg. writes */
  candev[minor]->ctrl |= (I82527_CTRL_CCE | I82527_CTRL_INIT);
    
  return RTEMS_SUCCESSFUL;
}


rtems_device_driver canbus_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  i82527_msg_t *msg;
  int i;
  int tmpHead;

  msg = arg;
  tmpHead = rxMsgBufHead[minor];

  while (1){
    if ((tmpHead == rxMsgBufTail[minor]) && 
        !(rxMsgBuf[minor][tmpHead].ctrl1 & I82527_MSG_CTRL_NEWDAT)) {
      break;
    }
    if (rxMsgBuf[minor][tmpHead].ctrl1 & I82527_MSG_CTRL_NEWDAT) {
      int pkt_len;
      msg->ctrl0 = rxMsgBuf[minor][tmpHead].ctrl0;
      msg->ctrl1 = rxMsgBuf[minor][tmpHead].ctrl1;
      msg->arb = rxMsgBuf[minor][tmpHead].arb;
      msg->cfg = rxMsgBuf[minor][tmpHead].cfg;
      
      pkt_len = (msg->cfg >> 4) & 0xf;
      for (i=0; i<pkt_len; i++) {
        msg->data[i] = rxMsgBuf[minor][tmpHead].data[i];
      }
      rxMsgBuf[minor][tmpHead].ctrl1 = 0xff & I82527_MSG_CTRL_NEWDAT_CLR;

      tmpHead++;
      if (tmpHead == RX_CAN_BUF_SIZE) {
        tmpHead = 0;
      }
      rxMsgBufHead[minor] = tmpHead;

      return RTEMS_SUCCESSFUL;

    }

    tmpHead++;
    if (tmpHead == RX_CAN_BUF_SIZE) {
      tmpHead = 0;
    }
    if (tmpHead == rxMsgBufHead[minor]) {
      break;
    }
  }

  return RTEMS_UNSATISFIED;

}
    
rtems_device_driver canbus_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  i82527_msg_t *msg;
  int i;

  msg = arg;
  while(candev[minor]->msg1.ctrl1 & I82527_MSG_CTRL_TXRQ){
    continue;
  }
  candev[minor]->msg1.ctrl1  = 0xff & I82527_MSG_CTRL_CPUUPD_SET;

  candev[minor]->msg1.cfg = msg->cfg;
  candev[minor]->msg1.arb = msg->arb;

  for (i=0; i < ((msg->cfg >> 4) & 0xff);  i++) {
    candev[minor]->msg1.data[i] = msg->data[i];
  }

  candev[minor]->msg1.ctrl0  = 0xff & (I82527_MSG_CTRL_INTPND_CLR |
                                       I82527_MSG_CTRL_MSGVAL_SET |
                                       I82527_MSG_CTRL_TXIE_CLR);
  candev[minor]->msg1.cfg    |= I82527_MSG_CFG_DIR;
  candev[minor]->msg1.ctrl1  = 0xff & (I82527_MSG_CTRL_NEWDAT_SET |
                                       I82527_MSG_CTRL_CPUUPD_CLR |
                                       I82527_MSG_CTRL_TXRQ_SET);



  return RTEMS_SUCCESSFUL;
}
rtems_device_driver canbus_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}


/* part of old canbus_read */
#if 0 
  for (i=0; i < RX_CAN_BUF_SIZE) {
    if (rxMsgBuf[minor][i].ctrl1 & I82527_MSG_CTRL_NEWDAT)
      break;
  }

  if (i < RX_CAN_BUF_SIZE) {
    int pkt_len;
    int j;
    msg.arb = rxMsgBuf[minor][i].arb;
    msg.cfg = rxMsgBuf[minor][i].cfg;
    
    pkt_len = (msg.cfg >> 4) & 0xf;

    for (j=0; j < pkt_len; j++) 
      msg.data[j] = rxMsgBuf[minor][i].data[j];
  
  
  /* wait until there is a msg */
  while (!(candev->msg15.ctrl1 & I82527_MSG_CTRL_NEWDAT))
     continue;
 
  msg->ctrl1 = candev->msg15.ctrl1;
  msg->cfg = candev->msg15.cfg;
  msg->arb = candev->msg15.arb;

  for (i=0; i < ((candev->msg15.cfg >> 4) & 0xff);  i++) {
    msg->data[i] = candev->msg15.data[i];
  }

  candev->msg15.ctrl0  = 0xff & (I82527_MSG_CTRL_MSGVAL_SET |
                                 I82527_MSG_CTRL_INTPND_CLR);
  candev->msg15.ctrl1  = 0xff & (I82527_MSG_CTRL_NEWDAT_CLR | 
                                 I82527_MSG_CTRL_RMTPND_CLR);

  candev->status = 0x0;


  return RTEMS_SUCCESSFUL;
#endif


