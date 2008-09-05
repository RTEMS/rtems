/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
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
| this file contains the MSCAN driver                             |
\*===============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <rtems.h>
#include <rtems/error.h>
#include <rtems/libio.h>
#include <string.h>

#include "../include/bsp.h"
#include <bsp/irq.h>
#include "../mscan/mscan_int.h"

/* #define MSCAN_LOOPBACK */

struct mpc5200_rx_cntrl mpc5200_mscan_rx_cntrl[MPC5200_CAN_NO];
static struct mscan_channel_info chan_info[MPC5200_CAN_NO];

/*
 * MPC5x00 MSCAN tx ring buffer function to get a can message buffer from the head of the tx ring buffer
 */
static struct can_message *get_tx_buffer(struct mscan_channel_info *chan)
{
  /* define a temp. mess ptr. */
  struct can_message *tmp_mess_ptr = NULL,
    *temp_head_ptr;

  /* set temp. head pointer */
  temp_head_ptr = chan->tx_ring_buf.head_ptr;

  /* check buffer empty condition */
  if (temp_head_ptr != chan->tx_ring_buf.tail_ptr) {

    /* current buffer head. ptr. */
    tmp_mess_ptr = temp_head_ptr;

    /* increment the head pointer */
    temp_head_ptr++;

    /* check for wrap around condition */
    if (temp_head_ptr > chan->tx_ring_buf.buf_ptr + NO_OF_MSCAN_TX_BUFF) {

      /* set head ptr. to the begin of the ring buffer */
      temp_head_ptr = chan->tx_ring_buf.buf_ptr;

    }

    /* end of crtical section restore head ptr. */
    chan->tx_ring_buf.head_ptr = temp_head_ptr;
  }

  /* return the current head pointer */
  return tmp_mess_ptr;
}

/*
 * MPC5x00 MSCAN tx ring buffer function to write a can message buffer to the tail of the tx ring buffer
 */
static struct can_message *fill_tx_buffer(struct mscan_channel_info *chan,
                                          struct can_message *mess_ptr)
{
  /* define a temp. mess ptr. to the entry which follows the current tail entry */
  struct can_message *tmp_mess_ptr = chan->tx_ring_buf.tail_ptr + 1;

  /* check for the wrap around condition */
  if (tmp_mess_ptr > chan->tx_ring_buf.buf_ptr + NO_OF_MSCAN_TX_BUFF) {
    /* set temp. mess. ptr to the begin of the ring buffer */
    tmp_mess_ptr = chan->tx_ring_buf.buf_ptr;
  }

  /* check buffer full condition */
  if (tmp_mess_ptr == chan->tx_ring_buf.head_ptr) {
    /* return NULL in case buffer is full */
    return NULL;
  } else {
    /* copy the can mess. to the tail of the buffer */
    memcpy((void *) chan->tx_ring_buf.tail_ptr, (void *) mess_ptr,
           sizeof (struct can_message));

    /* set new tail equal to temp. mess. ptr. */
    chan->tx_ring_buf.tail_ptr = tmp_mess_ptr;
  }

  /* return the actual tail ptr. (next free entry) */
  return chan->tx_ring_buf.tail_ptr;
}

/*
 * MPC5x00 MSCAN interrupt handler
 */
static void mpc5200_mscan_interrupt_handler(rtems_irq_hdl_param handle)
{
  rtems_status_code status;
  mscan_handle *mscan_hdl = (mscan_handle *) handle;
  struct mscan_channel_info *chan = &chan_info[mscan_hdl->mscan_channel];
  struct can_message rx_mess,
   *rx_mess_ptr,
   *tx_mess_ptr;
  mscan *m = chan->regs;
  register uint8_t idx;

  /*
     handle tx ring buffer
   */

  /* loop over all 3 tx buffers */
  for (idx = TFLG_TXE0; idx <= TFLG_TXE2; idx = idx << 1) {

    /* check for tx buffer vacation */
    if ((m->tflg) & idx) {

      /* try to get a message */
      tx_mess_ptr = get_tx_buffer(chan);

      /* check for new tx message */
      if (tx_mess_ptr != NULL) {

        /* select the tx buffer */
        m->bsel = idx;

        /* check for toucan interface */
        if ((mscan_hdl->toucan_callback) == NULL) {

          /* set tx id */
          m->txidr0 = SET_IDR0(tx_mess_ptr->mess_id);
          m->txidr1 = SET_IDR1(tx_mess_ptr->mess_id);
          m->txidr2 = 0;
          m->txidr3 = 0;

        }

        /* fill in tx data if TOUCAN is activ an TOUCAN index have a match with the tx buffer or TOUCAN is disabled */
        if (((mscan_hdl->toucan_callback) == NULL)
            || (((mscan_hdl->toucan_callback) != NULL)
                && ((tx_mess_ptr->toucan_tx_idx) == idx))) {

          /* insert dlc into m register */
          m->txdlr = (uint8_t) ((tx_mess_ptr->mess_len) & 0x000F);

          /* skip data copy in case of RTR */
          if (!(MSCAN_MESS_ID_HAS_RTR(tx_mess_ptr->mess_id))) {
            /* copy tx data to MSCAN registers */
            switch (m->txdlr) {
              case 8:
                m->txdsr7 = tx_mess_ptr->mess_data[7];
              case 7:
                m->txdsr6 = tx_mess_ptr->mess_data[6];
              case 6:
                m->txdsr5 = tx_mess_ptr->mess_data[5];
              case 5:
                m->txdsr4 = tx_mess_ptr->mess_data[4];
              case 4:
                m->txdsr3 = tx_mess_ptr->mess_data[3];
              case 3:
                m->txdsr2 = tx_mess_ptr->mess_data[2];
              case 2:
                m->txdsr1 = tx_mess_ptr->mess_data[1];
              case 1:
                m->txdsr0 = tx_mess_ptr->mess_data[0];
                break;
              default:
                break;
            }
          }

          /* enable message buffer specific interrupt */
          m->tier |= m->bsel;

          /* start transfer */
          m->tflg = m->bsel;

          /* release counting semaphore of tx ring buffer */
          rtems_semaphore_release((rtems_id) (chan->tx_rb_sid));

        } else {

          /* refill the tx ring buffer with the message */
          fill_tx_buffer(chan, tx_mess_ptr);

        }
      } else {
        /* reset interrupt enable bit */
        m->tier &= ~(idx);
      }
    }
  }

  /*
     handle rx interrupts
   */

  /* check for rx interrupt source */
  if (m->rier & RIER_RXFIE) {

    /* can messages received ? */
    while (m->rflg & RFLG_RXF) {

      if (mscan_hdl->toucan_callback == NULL) {

        /* select temporary rx buffer */
        rx_mess_ptr = &rx_mess;

      } else {

        /* check the rx fliter-match indicators (16-bit filter mode) */
        /* in case of more than one hit, lower hit has priority */
        idx = (m->idac) & 0x7;
        switch (idx) {

          case 0:
          case 1:
          case 2:
          case 3:
            rx_mess_ptr =
              (struct can_message *)
              &(mpc5200_mscan_rx_cntrl[mscan_hdl->mscan_channel].
                can_rx_message[idx]);
            break;

            /* this case should never happen */
          default:
            /* reset the rx indication flag */
            m->rflg |= RFLG_RXF;

            return;
            break;
        }

      }

      /* get rx ID */
      rx_mess_ptr->mess_id = GET_IDR0(m->rxidr0) | GET_IDR1(m->rxidr1);

      /* get rx len */
      rx_mess_ptr->mess_len = ((m->rxdlr) & 0x0F);

      /* get time stamp */
      rx_mess_ptr->mess_time_stamp = ((m->rxtimh << 8) | (m->rxtiml));

      /* skip data copy in case of RTR */
      if (!(MSCAN_MESS_ID_HAS_RTR(rx_mess_ptr->mess_id)))
      {

        /* get the data */
        switch (rx_mess_ptr->mess_len) {
          case 8:
            rx_mess_ptr->mess_data[7] = m->rxdsr7;
          case 7:
            rx_mess_ptr->mess_data[6] = m->rxdsr6;
          case 6:
            rx_mess_ptr->mess_data[5] = m->rxdsr5;
          case 5:
            rx_mess_ptr->mess_data[4] = m->rxdsr4;
          case 4:
            rx_mess_ptr->mess_data[3] = m->rxdsr3;
          case 3:
            rx_mess_ptr->mess_data[2] = m->rxdsr2;
          case 2:
            rx_mess_ptr->mess_data[1] = m->rxdsr1;
          case 1:
            rx_mess_ptr->mess_data[0] = m->rxdsr0;
          case 0:
          default:
            break;
        }
      }

      if (mscan_hdl->toucan_callback == NULL) {

        if ((status =
             rtems_message_queue_send(chan->rx_qid, (void *) rx_mess_ptr,
                                      sizeof (struct can_message))) !=
            RTEMS_SUCCESSFUL) {

          chan->int_rx_err++;

        }

      } else {

        mscan_hdl->toucan_callback((int16_t) (((m->idac) & 0x7) + 3));

      }

      /* reset the rx indication flag */
      m->rflg |= RFLG_RXF;

    }                           /* end of while(m->rflg & RFLG_RXF) */

  }

  /* status change detected */
  if (m->rflg & RFLG_CSCIF) {

    m->rflg |= RFLG_CSCIF;

    if (mscan_hdl->toucan_callback != NULL) {

      mscan_hdl->toucan_callback((int16_t) (-1));

    }

  }

}

/**
 * @brief Enables some interrupts for the MSCAN module @a m.
 *
 * Enabled interrupts:
 *  - Receiver or transmitter enters or leaves bus off state
 *  - Receiver buffer full
 */
static void mscan_interrupts_enable(mscan *m)
{

  /* RX Interrupt Enable on MSCAN_A/_B ----------------------------- */
  /*    [07]:WUPIE         0 : WakeUp interrupt disabled            */
  /*    [06]:CSCIE         1 : Status Change interrupt enabled      */
  /*    [05]:RSTATE1       0 : Recv. Status Change int. ,Bit 1      */
  /*    [04]:RSTATE0       1 : Recv. Status Change int. ,Bit 0      */
  /*                           -> 01 BusOff status changes enabled  */
  /*    [03]:TSTAT1        0 : Transmit. Status Change int. , Bit 1 */
  /*    [02]:TSTAT0        1 : Transmit. Status Change int. , Bit 0 */
  /*                           -> 01 BusOff status changes enabled  */
  /*    [01]:OVRIE         0 : Overrun Interrupt is disabled        */
  /*    [00]:RXFIE         1 : Recv. Full interrupt is enabled      */
  m->rier |= (RIER_CSCIE | RIER_RXFIE | RIER_RSTAT(1) | RIER_TSTAT(1));

  return;

}

/*
 * Unmask MPC5x00 MSCAN_A interrupts
 */
void mpc5200_mscan_a_on(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_A])->regs;

  mscan_interrupts_enable(m);

  return;

}

/*
 * Mask MPC5x00 MSCAN_A interrupts
 */
void mpc5200_mscan_a_off(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_A])->regs;

  mscan_interrupts_disable(m);

  return;

}

/*
 *  Get MSCAN_A interrupt mask setting
 */
int mpc5200_mscan_a_isOn(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_A])->regs;

  if ((m->rier & RIER_CSCIE) && (m->rier & RIER_RXFIE))
    return RTEMS_SUCCESSFUL;
  else
    return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;

}

/*
 * Unmask MPC5x00 MSCAN_B interrupts
 */
void mpc5200_mscan_b_on(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_B])->regs;

  mscan_interrupts_enable(m);

  return;

}

/*
 * Mask MPC5x00 MSCAN_B interrupts
 */
void mpc5200_mscan_b_off(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_B])->regs;

  mscan_interrupts_disable(m);

  return;

}

/*
 *  Get MSCAN_B interrupt mask setting
 */
int mpc5200_mscan_b_isOn(const rtems_irq_connect_data * ptr)
{
  mscan *m = (&chan_info[MSCAN_B])->regs;

  if ((m->rier & RIER_CSCIE) && (m->rier & RIER_RXFIE))
    return RTEMS_SUCCESSFUL;
  else
    return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;

}

static mscan_handle mscan_a_handle = {
  MSCAN_A,
  NULL
};

static mscan_handle mscan_b_handle = {
  MSCAN_B,
  NULL
};

/*
 * MPC5x00 MSCAN_A/_B irq data
 */
static rtems_irq_connect_data mpc5200_mscan_irq_data[MPC5200_CAN_NO] = {
  {
    BSP_SIU_IRQ_MSCAN1,
    (rtems_irq_hdl) mpc5200_mscan_interrupt_handler,
    (rtems_irq_hdl_param) & mscan_a_handle,
    (rtems_irq_enable) mpc5200_mscan_a_on,
    (rtems_irq_disable) mpc5200_mscan_a_off,
    (rtems_irq_is_enabled) mpc5200_mscan_a_isOn
  }, {
    BSP_SIU_IRQ_MSCAN2,
    (rtems_irq_hdl) mpc5200_mscan_interrupt_handler,
    (rtems_irq_hdl_param) & mscan_b_handle,
    (rtems_irq_enable) mpc5200_mscan_b_on,
    (rtems_irq_disable) mpc5200_mscan_b_off,
    (rtems_irq_is_enabled) mpc5200_mscan_b_isOn
  }
};

/*
 * MPC5x00 MSCAN wait for sync. with CAN bus
 */
void mpc5200_mscan_wait_sync(mscan *m)
{

  /* Control Register 0 -------------------------------------------- */
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH    0->1 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp Disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* wait for MSCAN A_/_B bus synch. */

#if 0                           /* we don't have a need to wait for sync. */
  while (!((m->ctl0) & CTL0_SYNCH));
#endif
  return;

}

/*
 * MPC5x00 MSCAN perform settings in init mode
 */
void mpc5200_mscan_perform_initialization_mode_settings(mscan *m)
{
  mscan_context context;

  /* perform all can bit time settings */
  (void) mscan_set_bit_rate(m, MSCAN_BIT_RATE_DEFAULT);

  /* Enter initialization mode */
  mscan_initialization_mode_enter( m, &context);

  /* Control Register 1 -------------------------------------------- */
  /*    [07]:CANE        0 : MSCAN Module is disabled               */
  /*    [06]:CLKSRC      0 : Clock Source -> IPB_CLOCK (bsp.h)      */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK    1->0 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK      0 : Init. Mode Acknowledge (rd. only)      */
  /* Set CLK source, disable loopback & listen-only mode */
#ifndef MSCAN_LOOPBACK
  m->ctl1 &= ~(CTL1_LISTEN | CTL1_LOOPB | CTL1_CLKSRC);
#else
  m->ctl1 &= ~(CTL1_LISTEN | CTL1_CLKSRC);
  m->ctl1 |= (CTL1_LOOPB);
#endif

  /* IPB clock       -> IPB_CLOCK                                                            */
  /* bitrate         -> CAN_BIT_RATE                                                         */
  /* Max. no of Tq   -> CAN_MAX_NO_OF_TQ                                                     */
  /* Prescaler value -> prescale_val = ROUND_UP(IPB_CLOCK/(CAN_BIT_RATE * CAN_MAX_NO_OF_TQ)) */
  /* SYNC_SEG        ->  1 tq                                                                */
  /* time segment 1  -> 16 tq (PROP_SEG+PHASE_SEG), CAN_MAX_NO_OF_TQ_TSEG1 = 15              */
  /* time segment 2  ->  8 tq (PHASE_SEG2)        , CAN_MAX_NO_OF_TQ_TSEG2 =  7              */
  /* SJW             ->  3 (fixed 0...3)          , CAN_MAX_NO_OF_TQ_SJW   =  2              */

  /* ID Acceptance Control MSCAN_A/_B ------------------------------ */
  /*    [07]:res.        0 : reserved                               */
  /*    [06]:res.        0 : reserved                               */
  /*    [05]:IDAM1       0 : ID acceptance control, Bit1            */
  /*    [04]:IDAM0       1 : ID acceptance control, Bit0            */
  /*                         -> filter 16 bit mode                  */
  /*    [03]:res.        0 : reserved                               */
  /*    [02]:IDHIT2      0 : ID acceptance hit indication, Bit 2    */
  /*    [01]:IDHIT1      0 : ID acceptance hit indication, Bit 1    */
  /*    [00]:IDHIT0      0 : ID acceptance hit indication, Bit 0    */
  m->idac &= ~(IDAC_IDAM1);
  m->idac |= (IDAC_IDAM0);

  /* initialize rx filter masks (16 bit), don't care including rtr */
  m->idmr0 = SET_IDMR0(0x7FF);
  m->idmr1 = SET_IDMR1(0x7FF);
  m->idmr2 = SET_IDMR2(0x7FF);
  m->idmr3 = SET_IDMR3(0x7FF);
  m->idmr4 = SET_IDMR4(0x7FF);
  m->idmr5 = SET_IDMR5(0x7FF);
  m->idmr6 = SET_IDMR6(0x7FF);
  m->idmr7 = SET_IDMR7(0x7FF);

  /* Control Register 1 -------------------------------------------- */
  /*    [07]:CANE     0->1 : MSCAN Module is enabled                */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB_CLOCK (bsp.h)      */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK       0 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK      0 : Init. Mode Acknowledge (rd. only)      */
  /* enable MSCAN A_/_B */
  m->ctl1 |= (CTL1_CANE);

  /* Leave initialization mode */
  mscan_initialization_mode_leave( m, &context);

  return;

}

/*
 * MPC5x00 MSCAN perform settings in normal mode
 */
void mpc5200_mscan_perform_normal_mode_settings(mscan
                                                *m)
{

  /* Control Register 0 -------------------------------------------- */
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp Disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* Disable wait mode, enable timestamps */
  m->ctl0 &= ~(CTL0_CSWAI);
  m->ctl0 |= (CTL0_TIME);

  return;

}

rtems_status_code mpc5200_mscan_set_mode(rtems_device_minor_number minor,
                                         uint8_t mode)
{
  struct mscan_channel_info *chan = NULL;
  mscan *m = NULL;

  switch (minor) {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      m = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  if (chan->mode == mode)
    return RTEMS_SUCCESSFUL;

  switch (mode) {

    case MSCAN_INIT_NORMAL_MODE:
      /* perform initialization which has to be done in init mode */
      mpc5200_mscan_perform_initialization_mode_settings(m);
      break;

    case MSCAN_NORMAL_MODE:
      if ((chan->mode) == MSCAN_INITIALIZED_MODE) {
        /* perform initialization which has to be done in init mode */
        mpc5200_mscan_perform_initialization_mode_settings(m);
      }

      if ((chan->mode) == MSCAN_SLEEP_MODE) {

        /* exit sleep mode */
        mscan_sleep_mode_leave(m);
      }
      /* enable ints. */
      mscan_interrupts_enable(m);
      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(m);
      break;

    case MSCAN_SLEEP_MODE:
      /* disable ints. */
      mscan_interrupts_disable(m);
      /* knter sleep mode */
      mscan_sleep_mode_enter(m);
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;

  }

  /* set new channel mode */
  chan->mode = mode;

  return RTEMS_SUCCESSFUL;

}

/*
 * initialization of channel info.
 */
rtems_status_code mscan_channel_initialize(rtems_device_major_number major,
                                           rtems_device_minor_number minor)
{
  rtems_status_code status;
  struct mscan_channel_info *chan = &chan_info[minor];

  /* set registers according to MSCAN channel information */
  switch (minor) {

    case MSCAN_A:
      chan->rx_qname = rtems_build_name('C', 'N', 'A', 'Q');
      chan->tx_rb_sname = rtems_build_name('C', 'N', 'A', 'S');

      /* register RTEMS device names for MSCAN A */
      if ((status =
           rtems_io_register_name(MSCAN_A_DEV_NAME, major,
                                  MSCAN_A)) != RTEMS_SUCCESSFUL)
        return status;

      /* register RTEMS device names for MSCAN 0 */
      if ((status =
           rtems_io_register_name(MSCAN_0_DEV_NAME, major,
                                  MSCAN_A)) != RTEMS_SUCCESSFUL)
        return status;

      /* allocate the space for MSCAN A tx ring buffer */
      if (((chan->tx_ring_buf.buf_ptr) =
           malloc(sizeof (struct can_message) * (NO_OF_MSCAN_TX_BUFF + 1))) !=
          NULL) {
        chan->tx_ring_buf.head_ptr = chan->tx_ring_buf.tail_ptr =
          chan->tx_ring_buf.buf_ptr;
      } else {
        return RTEMS_UNSATISFIED;
      }
      break;

    case MSCAN_B:
      chan->rx_qname = rtems_build_name('C', 'N', 'B', 'Q');
      chan->tx_rb_sname = rtems_build_name('C', 'N', 'B', 'S');

      /* register RTEMS device names for MSCAN B */
      if ((status =
           rtems_io_register_name(MSCAN_B_DEV_NAME, major,
                                  MSCAN_B)) != RTEMS_SUCCESSFUL)
        return status;

      /* register RTEMS device names for MSCAN 1 */
      if ((status =
           rtems_io_register_name(MSCAN_1_DEV_NAME, major,
                                  MSCAN_B)) != RTEMS_SUCCESSFUL)
        return status;

      /* allocate the space for MSCAN B tx ring buffer */
      if (((chan->tx_ring_buf.buf_ptr) =
           malloc(sizeof (struct can_message) * (NO_OF_MSCAN_TX_BUFF + 1))) !=
          NULL) {
        chan->tx_ring_buf.head_ptr = chan->tx_ring_buf.tail_ptr =
          chan->tx_ring_buf.buf_ptr;
      } else {
        return RTEMS_UNSATISFIED;
      }
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  /* create RTEMS rx message queue */
  status =
    rtems_message_queue_create(chan->rx_qname, (uint32_t) NO_OF_MSCAN_RX_BUFF,
                               (uint32_t)
                               MSCAN_MESSAGE_SIZE(sizeof (struct can_message)),
                               (rtems_attribute) RTEMS_LOCAL | RTEMS_FIFO,
                               (rtems_id *) & (chan->rx_qid));

  /* create counting RTEMS tx ring buffer semaphore */
  status =
    rtems_semaphore_create(chan->tx_rb_sname, (uint32_t) (NO_OF_MSCAN_TX_BUFF),
                           RTEMS_COUNTING_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY
                           | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL,
                           (rtems_task_priority) 0,
                           (rtems_id *) & (chan->tx_rb_sid));

  /* Set up interrupts */
  if (!BSP_install_rtems_irq_handler(&(mpc5200_mscan_irq_data[minor])))
    rtems_panic("Can't attach MPC5x00 MSCAN interrupt handler %d\n", minor);

  /* basic setup for channel info. struct. */
  chan->regs = (mscan *) &(mpc5200.mscan[minor]);
  chan->int_rx_err = 0;
  chan->id_extended = FALSE;
  chan->mode = MSCAN_INITIALIZED_MODE;
  chan->tx_buf_no = NO_OF_MSCAN_TX_BUFF;

  return status;

}

/*
 * MPC5x00 MSCAN device initialization
 */
rtems_device_driver mscan_initialize(rtems_device_major_number major,
                                     rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;

  /* Initialization requested via RTEMS */
  if ((status = mscan_channel_initialize(major, MSCAN_A)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if ((status = mscan_channel_initialize(major, MSCAN_B)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if ((status =
       mpc5200_mscan_set_mode(MSCAN_A,
                              MSCAN_INIT_NORMAL_MODE)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if ((status =
       mpc5200_mscan_set_mode(MSCAN_B,
                              MSCAN_INIT_NORMAL_MODE)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return status;

}

/*
 * MPC5x00 MSCAN device open
 */
rtems_device_driver mscan_open(rtems_device_major_number major,
                               rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status = RTEMS_SUCCESSFUL;
  struct mscan_channel_info *chan = NULL;

  switch (minor) {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  /* check mode */
  if ((chan->mode) == MSCAN_SLEEP_MODE) {

    /* if not already set enter init mode */
    status = mpc5200_mscan_set_mode(minor, MSCAN_NORMAL_MODE);
  }

  return status;

}

/*
 * MPC5x00 MSCAN device close
 */
rtems_device_driver mscan_close(rtems_device_major_number major,
                                rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;
  struct mscan_channel_info *chan = NULL;

  switch (minor) {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  /* enter deep sleep mode */
  status = mpc5200_mscan_set_mode(minor, MSCAN_SLEEP_MODE);

  return status;

}

/*
 * MPC5x00 MSCAN device read
 */
rtems_device_driver mscan_read(rtems_device_major_number major,
                               rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;
  size_t message_size = 0;
  rtems_libio_rw_args_t *parms = (rtems_libio_rw_args_t *) arg;
  struct mscan_rx_parms *rx_parms = (struct mscan_rx_parms *) (parms->buffer);
  struct can_message *rx_mess = (struct can_message *) (rx_parms->rx_mess);
  struct mscan_channel_info *chan = NULL;

  switch (minor) {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  /* end init mode if it is first read */
  if ((chan->mode) == MSCAN_INIT_NORMAL_MODE) {

    /* if not already set enter init mode */
    mpc5200_mscan_set_mode(minor, MSCAN_NORMAL_MODE);
  }

  if ((status =
       rtems_message_queue_receive(chan->rx_qid, (void *) (rx_mess),
                                   &message_size,
                                   (uint32_t) (rx_parms->rx_flags),
                                   (rtems_interval) (rx_parms->rx_timeout)))
      != RTEMS_SUCCESSFUL) {

    parms->bytes_moved = 0;

  } else {

    parms->bytes_moved = sizeof (struct can_message);

  }

  return status;

}

/*
 * MPC5x00 MSCAN device write
 */
rtems_device_driver mscan_write(rtems_device_major_number major,
                                rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;
  rtems_libio_rw_args_t *parms = (rtems_libio_rw_args_t *) arg;
  struct mscan_tx_parms *tx_parms = (struct mscan_tx_parms *) (parms->buffer);
  struct can_message *tx_mess = (struct can_message *) (tx_parms->tx_mess);
  struct mscan_channel_info *chan = NULL;
  mscan_handle *mscan_hdl = NULL;
  mscan *m = NULL;

  switch (minor) {
    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      mscan_hdl = mpc5200_mscan_irq_data[minor].handle;
      m = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  /* end init mode if it is first write */
  if ((chan->mode) == MSCAN_INIT_NORMAL_MODE) {

    /* if not already set enter init mode */
    mpc5200_mscan_set_mode(minor, MSCAN_NORMAL_MODE);
  }

  /* preset moved bytes */
  parms->bytes_moved = 0;

  /* obtain counting semaphore of tx ring buffer */
  if ((status =
       rtems_semaphore_obtain((rtems_id) (chan->tx_rb_sid), RTEMS_NO_WAIT,
                              (rtems_interval) 0))
      == RTEMS_SUCCESSFUL) {

    /* append the TOUCAN tx_id to the mess. due to interrupt handling */
    tx_mess->toucan_tx_idx = tx_parms->tx_idx;

    /* fill the tx ring buffer with the message */
    fill_tx_buffer(chan, tx_mess);

    /* enable message buffer specific interrupt */
    m->tier |= (TIER_TXEI0 | TIER_TXEI1 | TIER_TXEI2);

    /* calculate moved bytes */
    parms->bytes_moved = (tx_mess->mess_len) & 0x000F;

  }

  return status;

}

/*
 * MPC5x00 MSCAN device control
 */
rtems_device_driver mscan_control(rtems_device_major_number major,
                                  rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;
  uint16_t tx_id;
  rtems_libio_ioctl_args_t *parms = (rtems_libio_ioctl_args_t *) arg;
  struct mscan_ctrl_parms *ctrl_parms =
    (struct mscan_ctrl_parms *) (parms->buffer);
  struct mscan_channel_info *chan = NULL;
  mscan_handle *mscan_hdl = NULL;
  mscan *m = NULL;
  mscan_context context;
  uint8_t tx_buf_count = 0;

  switch (minor) {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      mscan_hdl = mpc5200_mscan_irq_data[minor].handle;
      m = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
  }

  switch (parms->command) {

      /* TOUCAN callback initialization for MSCAN */
    case TOUCAN_MSCAN_INIT:
      mscan_hdl->toucan_callback = ctrl_parms->toucan_cb_fnc;
      break;

      /* set rx buffer ID */
    case MSCAN_SET_RX_ID:

      /* enter init mode */
      mscan_initialization_mode_enter(m, &context);

      switch (ctrl_parms->ctrl_reg_no) {

        case RX_BUFFER_0:
          m->idar0 = SET_IDR0(ctrl_parms->ctrl_id);
          m->idar1 = SET_IDR1(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_1:
          m->idar2 = SET_IDR2(ctrl_parms->ctrl_id);
          m->idar3 = SET_IDR3(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_2:
          m->idar4 = SET_IDR4(ctrl_parms->ctrl_id);
          m->idar5 = SET_IDR5(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_3:
          m->idar6 = SET_IDR6(ctrl_parms->ctrl_id);
          m->idar7 = SET_IDR7(ctrl_parms->ctrl_id);
          break;

        default:
          break;

      }

      /* exit init mode and perform further initialization which is required in the normal mode */
      mscan_initialization_mode_leave(m, &context);

      /* enable ints. */
      mscan_interrupts_enable(m);

      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(m);

      return RTEMS_SUCCESSFUL;
      break;

      /* get rx buffer ID */
    case MSCAN_GET_RX_ID:

      switch (ctrl_parms->ctrl_reg_no) {

        case RX_BUFFER_0:
          ctrl_parms->ctrl_id = GET_IDR0(m->idar0) | GET_IDR1(m->idar1);
          break;

        case RX_BUFFER_1:
          ctrl_parms->ctrl_id = GET_IDR2(m->idar2) | GET_IDR3(m->idar3);
          break;

        case RX_BUFFER_2:
          ctrl_parms->ctrl_id = GET_IDR4(m->idar4) | GET_IDR5(m->idar5);
          break;

        case RX_BUFFER_3:
          ctrl_parms->ctrl_id = GET_IDR6(m->idar6) | GET_IDR7(m->idar7);
          break;

        default:
          break;

      }

      break;

      /* set rx buffer ID mask */
    case MSCAN_SET_RX_ID_MASK:

      /* enter init mode */
      mscan_initialization_mode_enter(m, &context);

      switch (ctrl_parms->ctrl_reg_no) {

        case RX_BUFFER_0:
          m->idmr0 = SET_IDMR0(ctrl_parms->ctrl_id_mask);
          m->idmr1 = SET_IDMR1(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_1:
          m->idmr2 = SET_IDMR2(ctrl_parms->ctrl_id_mask);
          m->idmr3 = SET_IDMR3(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_2:
          m->idmr4 = SET_IDMR4(ctrl_parms->ctrl_id_mask);
          m->idmr5 = SET_IDMR5(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_3:
          m->idmr6 = SET_IDMR6(ctrl_parms->ctrl_id_mask);
          m->idmr7 = SET_IDMR7(ctrl_parms->ctrl_id_mask);
          break;

        default:
          break;

      }

      /* exit init mode and perform further initialization which is required in the normal mode */
      mscan_initialization_mode_leave(m, &context);

      /* enable ints. */
      mscan_interrupts_enable(m);

      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(m);

      break;

      /* get rx buffer ID mask */
    case MSCAN_GET_RX_ID_MASK:

      switch (ctrl_parms->ctrl_reg_no) {

        case RX_BUFFER_0:
          ctrl_parms->ctrl_id_mask =
            (GET_IDMR0(m->idmr0) | GET_IDMR1(m->idmr1));
          break;

        case RX_BUFFER_1:
          ctrl_parms->ctrl_id_mask =
            (GET_IDMR2(m->idmr2) | GET_IDMR3(m->idmr3));
          break;

        case RX_BUFFER_2:
          ctrl_parms->ctrl_id_mask =
            (GET_IDMR4(m->idmr4) | GET_IDMR5(m->idmr5));
          break;

        case RX_BUFFER_3:
          ctrl_parms->ctrl_id_mask =
            (GET_IDMR6(m->idmr6) | GET_IDMR7(m->idmr7));
          break;

        default:
          break;

      }

      /* set tx buffer ID */
    case MSCAN_SET_TX_ID:

      /* check for availability of tx buffer */
      if (!((m->tflg) & (uint8_t) (ctrl_parms->ctrl_reg_no))) {

        /* do abort tx buf. request */
        m->tarq = (uint8_t) (ctrl_parms->ctrl_reg_no);

        /* wait for abort tx buf. ack. */
        while ((m->taak) & (uint8_t) (ctrl_parms->ctrl_reg_no));

      }

      /* select tx buf. */
      m->bsel = (uint8_t) (ctrl_parms->ctrl_reg_no);

      /* set the tx id of selected buf. */
      tx_id = ctrl_parms->ctrl_id;
      m->txidr0 = SET_IDR0(tx_id);
      m->txidr1 = SET_IDR1(tx_id);
      m->txidr2 = 0;
      m->txidr3 = 0;

      break;

      /* get tx buffer ID */
    case MSCAN_GET_TX_ID:

      /* select tx buf. */
      m->bsel = (uint8_t) (ctrl_parms->ctrl_reg_no);

      /* get tx id. of selected buf. */
      ctrl_parms->ctrl_id = GET_IDR0(m->txidr0) | GET_IDR1(m->txidr1);

      break;

      /* set can bitrate */
    case MSCAN_SET_BAUDRATE:
      /* perform all can bit time settings */
      if (!mscan_set_bit_rate(m, ctrl_parms->ctrl_can_bitrate)) {
        return RTEMS_UNSATISFIED;
      }

      /* enable ints. */
      mscan_interrupts_enable(m);

      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(m);

      break;

    case SET_TX_BUF_NO:

      /* check for different settings of tx ring buffer */
      if ((tx_buf_count =
           chan->tx_buf_no) != (uint8_t) (ctrl_parms->ctrl_tx_buf_no)) {

        /* preset the channel specific no of messages in the tx ring buffer */
        tx_buf_count = chan->tx_buf_no;

        /* try to obtain all of the tx ring buffers */
        while (tx_buf_count > 0) {

          /* obtain semaphore of all tx ring buffers */
          if ((status =
               rtems_semaphore_obtain((rtems_id) (chan->tx_rb_sid), RTEMS_WAIT,
                                      (rtems_interval) 10))
              == RTEMS_SUCCESSFUL) {

            tx_buf_count--;

          }

        }

        /* free the former tx ring buffer */
        free((void *) chan->tx_ring_buf.buf_ptr);

        /* allocate the tx ring buffer with new size */
        if (((chan->tx_ring_buf.buf_ptr) =
             malloc(sizeof (struct can_message) *
                    ((uint8_t) (ctrl_parms->ctrl_tx_buf_no) + 1))) != NULL) {
          chan->tx_ring_buf.head_ptr = chan->tx_ring_buf.tail_ptr =
            chan->tx_ring_buf.buf_ptr;
        } else {
          return RTEMS_UNSATISFIED;
        }

        /* set the new amount of tx buffers */
        chan->tx_buf_no = (uint8_t) (ctrl_parms->ctrl_tx_buf_no);

        /* release the semaphore of all tx ring buffers */
        while (tx_buf_count < chan->tx_buf_no) {

          /* obtain semaphore of all tx ring buffers */
          rtems_semaphore_release((rtems_id) (chan->tx_rb_sid));

          tx_buf_count++;

        }

      } else {

        return RTEMS_SUCCESSFUL;

      }
      break;

    default:
      break;

  }

  return RTEMS_SUCCESSFUL;

}
