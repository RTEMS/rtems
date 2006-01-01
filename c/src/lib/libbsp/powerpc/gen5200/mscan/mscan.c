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
#include <bsp.h>
#include "../irq/irq.h"
#include "../include/mpc5200.h"
#include "mscan.h"

/* #define MSCAN_LOOPBACK */

struct mpc5200_rx_cntrl mpc5200_mscan_rx_cntrl[MPC5200_CAN_NO];
static struct mscan_channel_info chan_info[MPC5200_CAN_NO];

/*
 * MPC5x00 MSCAN interrupt handler
 */
static void mpc5200_mscan_interrupt_handler(rtems_irq_hdl_param handle)
  {
  rtems_status_code status;
  mscan_handle *mscan_hdl = (mscan_handle *)handle;
  struct mscan_channel_info *chan = &chan_info[mscan_hdl->mscan_channel];
  struct can_message rx_mess, *rx_mess_ptr;
  volatile struct mpc5200_mscan *mscan = chan->regs;

  /*
   handle tx interrupts
   */

  /* check and disable tx interrupt for message buffer 0 */
  if(mscan->tier & MSCAN_TX_BUFF0)
    mscan->tier &= ~(MSCAN_TX_BUFF0);

  /* check and disable tx interrupt for message buffer 1 */
  if(mscan->tier & MSCAN_TX_BUFF1)
    mscan->tier &= ~(MSCAN_TX_BUFF1);

  /* check and disable tx interrupt for message buffer 2 */
  if(mscan->tier & MSCAN_TX_BUFF2)
    mscan->tier &= ~(MSCAN_TX_BUFF2);

  /*
   handle rx interrupts
   */

  /* check for rx interrupt source */
  if(mscan->rier & RIER_RXFIE)
    {

    /* can messages received ? */
    while(mscan->rflg & RFLG_RXF)
      {

      if(mscan_hdl->toucan_callback == NULL)
        {

        /* select temporary rx buffer */
        rx_mess_ptr = &rx_mess;

        }
      else
        {

        /* check the rx fliter-match indicators (16-bit filter mode) */
        /* in case of more than one hit, lower hit has priority */
        switch((mscan->idac) & 0x7)
          {

          case 0:
            rx_mess_ptr = (struct can_message *)&(mpc5200_mscan_rx_cntrl[MSCAN_A].can_rx_message[0]);
            break;

	      case 1:
            rx_mess_ptr = (struct can_message *)&(mpc5200_mscan_rx_cntrl[MSCAN_A].can_rx_message[1]);
            break;

     	  case 2:
            rx_mess_ptr = (struct can_message *)&(mpc5200_mscan_rx_cntrl[MSCAN_A].can_rx_message[2]);
		    break;

  		  case 3:
            rx_mess_ptr = (struct can_message *)&(mpc5200_mscan_rx_cntrl[MSCAN_A].can_rx_message[3]);
		    break;

          /* this case should never happen */
		  default:
            /* reset the rx indication flag */
            mscan->rflg |= RFLG_RXF;

            return;
            break;
	      }

        }

      /* get rx ID */
      rx_mess_ptr->mess_id = GET_IDR0(mscan->rxidr0) | GET_IDR1(mscan->rxidr1);

      /* get rx len */
      rx_mess_ptr->mess_len = ((mscan->rxdlr) & 0x0F);

      /* get time stamp */
      rx_mess_ptr->mess_time_stamp = ((mscan->rxtimh << 8) | (mscan->rxtiml));

      /* get the data */
	  switch(rx_mess_ptr->mess_len)
	    {

	    case 8:
	      rx_mess_ptr->mess_data[7] = mscan->rxdsr7;
	    case 7:
	      rx_mess_ptr->mess_data[6] = mscan->rxdsr6;
	    case 6:
	      rx_mess_ptr->mess_data[5] = mscan->rxdsr5;
	    case 5:
	      rx_mess_ptr->mess_data[4] = mscan->rxdsr4;
	    case 4:
	      rx_mess_ptr->mess_data[3] = mscan->rxdsr3;
	    case 3:
	      rx_mess_ptr->mess_data[2] = mscan->rxdsr2;
	    case 2:
	      rx_mess_ptr->mess_data[1] = mscan->rxdsr1;
	    case 1:
	      rx_mess_ptr->mess_data[0] = mscan->rxdsr0;
	    case 0:
	    default:
	      break;

	    }

      if(mscan_hdl->toucan_callback == NULL)
        {

        if((status = rtems_message_queue_send(chan->rx_qid, (void *)rx_mess_ptr, sizeof(struct can_message))) != RTEMS_SUCCESSFUL)
		  {

		  chan->int_rx_err++;

          }

	    }
      else
        {

        mscan_hdl->toucan_callback((int16_t)(((mscan->idac) & 0x7) + 3));

	    }

      /* reset the rx indication flag */
      mscan->rflg |= RFLG_RXF;

      } /* end of while(mscan->rflg & RFLG_RXF) */

    }

    /* status change detected */
    if(mscan->rflg & RFLG_CSCIF)
      {

      mscan->rflg |= RFLG_CSCIF;

      if(mscan_hdl->toucan_callback != NULL)
        {

        mscan_hdl->toucan_callback((int16_t)(-1));

        }

      }

  }


/*
 * Disable MSCAN ints.
 */
void mpc5200_mscan_int_disable(volatile struct mpc5200_mscan *mscan)
  {

  /* RX Interrupt Enable on MSCAN_A/_B -----------------------------*/
  /*    [07]:WUPIE         0 : WakeUp interrupt disabled            */
  /*    [06]:CSCIE         0 : Status Change interrupt disabled     */
  /*    [05]:RSTATE1       0 : Recv. Status Change int. ,Bit 1      */
  /*    [04]:RSTATE0       0 : Recv. Status Change int. ,Bit 0      */
  /*                           -> 00 rx status change int. disabled */
  /*    [03]:TSTAT1        0 : Transmit. Status Change int. , Bit 1 */
  /*    [02]:TSTAT0        0 : Transmit. Status Change int. , Bit 0 */
  /*                           -> 00 tx status change int. disabled */
  /*    [01]:OVRIE         0 : Overrun Interrupt is disabled        */
  /*    [00]:RXFIE         0 : Recv. Full interrupt is disabled     */
  mscan->rier &= ~(RIER_CSCIE | RIER_RXFIE);

  /* TX Interrupt Enable on MSCAN_A/_B -----------------------------*/
  /*    [07]:res.          0 : reserved                             */
  /*    [06]:res.          0 : reserved                             */
  /*    [05]:res.          0 : reserved                             */
  /*    [04]:res.          0 : reserved                             */
  /*    [03]:res.          0 : reserved                             */
  /*    [02]:TSEG12        0 : TX2message buffer int. is disabled   */
  /*    [01]:TSEG11        0 : TX1 message buffer int. is disabled  */
  /*    [00]:TSEG10        0 : TX0 message buffer int. is disabled  */
  mscan->tier &= ~(TIER_TXEI2 | TIER_TXEI1 | TIER_TXEI0);

  return;

  }


/*
 * Enable MSCAN ints.
 */
void mpc5200_mscan_int_enable(volatile struct mpc5200_mscan *mscan)
  {

  /* RX Interrupt Enable on MSCAN_A/_B -----------------------------*/
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
  mscan->rier |= (RIER_CSCIE  | RIER_RXFIE | RIER_RSTAT(1) | RIER_TSTAT(1));

  return;

  }

/*
 * Unmask MPC5x00 MSCAN_A interrupts
 */
void mpc5200_mscan_a_on(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_A])->regs;

  mpc5200_mscan_int_enable(mscan);

  return;

  }


/*
 * Mask MPC5x00 MSCAN_A interrupts
 */
void mpc5200_mscan_a_off(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_A])->regs;

  mpc5200_mscan_int_disable(mscan);

  return;

  }


/*
 *  Get MSCAN_A interrupt mask setting
 */
int mpc5200_mscan_a_isOn(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_A])->regs;

  if((mscan->rier & RIER_CSCIE) && (mscan->rier & RIER_RXFIE))
    return RTEMS_SUCCESSFUL;
  else
    return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;

  }


/*
 * Unmask MPC5x00 MSCAN_B interrupts
 */
void mpc5200_mscan_b_on(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_B])->regs;

  mpc5200_mscan_int_enable(mscan);

  return;

  }


/*
 * Mask MPC5x00 MSCAN_B interrupts
 */
void mpc5200_mscan_b_off(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_B])->regs;

  mpc5200_mscan_int_disable(mscan);

  return;

  }


/*
 *  Get MSCAN_B interrupt mask setting
 */
int mpc5200_mscan_b_isOn(const rtems_irq_connect_data* ptr)
  {
  volatile struct mpc5200_mscan *mscan = (&chan_info[MSCAN_B])->regs;

  if((mscan->rier & RIER_CSCIE) && (mscan->rier & RIER_RXFIE))
    return RTEMS_SUCCESSFUL;
  else
    return RTEMS_UNSATISFIED;

  return RTEMS_SUCCESSFUL;

  }

static mscan_handle mscan_a_handle =
  {
  MSCAN_A,
  NULL
  };


static mscan_handle mscan_b_handle =
  {
  MSCAN_B,
  NULL
  };

/*
 * MPC5x00 MSCAN_A/_B irq data
 */
static rtems_irq_connect_data mpc5200_mscan_irq_data[MPC5200_CAN_NO] =
  {{
  BSP_SIU_IRQ_MSCAN1,
  (rtems_irq_hdl) mpc5200_mscan_interrupt_handler,
  (rtems_irq_hdl_param) &mscan_a_handle,
  (rtems_irq_enable) mpc5200_mscan_a_on,
  (rtems_irq_disable) mpc5200_mscan_a_off,
  (rtems_irq_is_enabled) mpc5200_mscan_a_isOn
  },
  {
  BSP_SIU_IRQ_MSCAN2,
  (rtems_irq_hdl) mpc5200_mscan_interrupt_handler,
  (rtems_irq_hdl_param) &mscan_b_handle,
  (rtems_irq_enable) mpc5200_mscan_b_on,
  (rtems_irq_disable) mpc5200_mscan_b_off,
  (rtems_irq_is_enabled) mpc5200_mscan_b_isOn
  }};


/*
 * Enter MSCAN sleep mode
 */
void mpc5200_mscan_enter_sleep_mode(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        1 : WakeUp Enabled                         */
  /*    [01]:SLPRQ    0->1 : Sleep Mode Request                     */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* select sleep mode */
  mscan->ctl0 |= (CTL0_SLPRQ);

  /* Control Register 1 --------------------------------------------*/
  /*    [07]:CANE        0 : MSCAN Module is disabled               */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB-Clock (33 MHz)     */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK    0->1 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK      0 : Init. Mode Acknowledge (rd. only)      */
  /* wait for sleep mode acknowledge */
  while(!(mscan->ctl1 & CTL1_SLPAK));

  return;

  }


/*
 * Exit MSCAN sleep mode
 */
void mpc5200_mscan_exit_sleep_mode(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        1 : WakeUp Enabled                         */
  /*    [01]:SLPRQ    0->1 : Sleep Mode Request                     */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* select sleep mode */
  mscan->ctl0 &= ~(CTL0_SLPRQ);

  /* Control Register 1 --------------------------------------------*/
  /*    [07]:CANE        0 : MSCAN Module is disabled               */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB-Clock (33 MHz)     */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK    0->1 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK      0 : Init. Mode Acknowledge (rd. only)      */
  /* wait for sleep mode acknowledge */
  while((mscan->ctl1 & CTL1_SLPAK));

  return;

  }


/*
 * Enter MSCAN init mode and disconnect from bus
 */
void mpc5200_mscan_enter_init_mode(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ   0->1 : Init. Mode Request                     */
  /* select init mode */
  mscan->ctl0 |= (CTL0_INITRQ);

  /* Control Register 1 --------------------------------------------*/
  /*    [07]:CANE        0 : MSCAN Module is disabled               */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB-Clock (33 MHz)     */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK       0 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK   0->1 : Init. Mode Acknowledge (rd. only)      */
  /* wait for init mode acknowledge */
  while(!(mscan->ctl1 & CTL1_INITAK));

  return;

  }


/*
 * Exit MSCAN init mode
 */
void mpc5200_mscan_exit_init_mode(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp Disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ   1->0 : Init. Mode Request                     */
  /* select normal mode */
  mscan->ctl0 &= ~(CTL0_INITRQ);

  /* Control Register 1 --------------------------------------------*/
  /*    [07]:CANE        0 : MSCAN Module is disabled               */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB-Clock (33 MHz)     */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK       0 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK   1->0 : Init. Mode Acknowledge (rd. only)      */
  /* wait for normal mode acknowledge */
  while(mscan->ctl1 & CTL1_INITAK);

  return;

  }


/*
 * MPC5x00 MSCAN wait for sync. with CAN bus
 */
void mpc5200_mscan_wait_sync(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH    0->1 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp Disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* wait for MSCAN A_/_B bus synch. */
  while(!((mscan->ctl0) & CTL0_SYNCH));

  return;

  }


/*
 * MPC5x00 MSCAN set up the bit timing
 */
void mpc5200_mscan_perform_bit_time_settings(volatile struct mpc5200_mscan *mscan)
{
  uint32_t prescale_val = 0;
  uint32_t tq_num;
  uint32_t sync_seg,tseg1,tseg2;
  
  if(IPB_CLOCK%(CAN_BIT_RATE * CAN_MAX_NO_OF_TQ))
    prescale_val = (IPB_CLOCK/(CAN_BIT_RATE * (CAN_MAX_NO_OF_TQ*2/3))) + 1;
  else
    prescale_val = IPB_CLOCK/(CAN_BIT_RATE* (CAN_MAX_NO_OF_TQ*2/3));
  
  tq_num = ((IPB_CLOCK/prescale_val)+CAN_BIT_RATE/2)/CAN_BIT_RATE;
  /*
   * XXX: make this table controlled according to MPC5200UM/Rev.3,Table 19-34
   */
  sync_seg = 2;
  tseg2 = (tq_num-sync_seg)/6;
  tseg1 = tq_num - sync_seg - tseg2;

  /* Bus Timing Register 0 MSCAN_A/_B ------------------------------*/
  /*    [07]:SJW1        1 : Synchronization jump width, Bit1       */
  /*    [06]:SJW0        0 : Synchronization jump width, Bit0       */
  /*                         SJW = 2 -> 3 Tq clock cycles           */
  /*    [05]:BRP5        0 : Baud Rate Prescaler, Bit 5             */
  /*    [04]:BRP4        0 : Baud Rate Prescaler, Bit 4             */
  /*    [03]:BRP3        0 : Baud Rate Prescaler, Bit 3             */
  /*    [02]:BRP2        1 : Baud Rate Prescaler, Bit 2             */
  /*    [01]:BRP1        0 : Baud Rate Prescaler, Bit 1             */
  /*    [00]:BRP0        1 : Baud Rate Prescaler, Bit 0             */
  mscan->btr0 |= (BTR0_SJW(sync_seg-1) | BTR0_BRP(prescale_val - 1));

  /* Bus Timing Register 1 MSCAN_A/_B ------------------------------*/
  /*    [07]:SAMP        0 : One Sample per bit                     */
  /*    [06]:TSEG22      0 : Time Segment 2, Bit 2                  */
  /*    [05]:TSEG21      1 : Time Segment 2, Bit 1                  */
  /*    [04]:TSEG20      0 : Time Segment 2, Bit 0                  */
  /*                         -> PHASE_SEG2 = 3 Tq                   */
  /*    [03]:TSEG13      0 : Time Segment 1, Bit 3                  */
  /*    [02]:TSEG12      1 : Time Segment 1, Bit 2                  */
  /*    [01]:TSEG11      1 : Time Segment 1, Bit 1                  */
  /*    [00]:TSEG10      0 : Time Segment 1, Bit 0                  */
  mscan->btr1 &= ~(BTR1_SAMP);
  mscan->btr1 |=  (BTR1_TSEG_22_20(tseg2-1) | BTR1_TSEG_13_10(tseg1-1));

  return;

  }


/*
 * MPC5x00 MSCAN perform settings in init mode
 */
void mpc5200_mscan_perform_init_mode_settings(volatile struct mpc5200_mscan *mscan)
  {
    mpc5200_mscan_perform_bit_time_settings(mscan);
  /* Control Register 1 --------------------------------------------*/
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
  mscan->ctl1 &= ~(CTL1_LISTEN | CTL1_LOOPB | CTL1_CLKSRC);
#else
  mscan->ctl1 &= ~(CTL1_LISTEN | CTL1_CLKSRC);
  mscan->ctl1 |=  (CTL1_LOOPB);
#endif

  /* IPB clock                  -> IPB_CLOCK (bsp.h)                                                    */
  /* bitrate                    -> CAN_BIT_RATE (mscan.h)                                               */
  /* Max. no of Tq              -> CAN_MAX_NO_OF_TQ (mscan.h)                                           */
  /* Prescaler value            -> prescale_val = ROUND_UP(IPB_CLOCK/(CAN_BIT_RATE * CAN_MAX_NO_OF_TQ)) */
  /* SYNC_SEG                   ->  1 tq                                                                */
  /* time segment 1             -> 16 tq (PROP_SEG+PHASE_SEG), CAN_MAX_NO_OF_TQ_TSEG1 = 15 (mscan.h)    */
  /* time segment 2             ->  8 tq (PHASE_SEG2)        , CAN_MAX_NO_OF_TQ_TSEG2 =  7 (mscan.h)    */
  /* SJW                        ->  3 (fixed 0...3)          , CAN_MAX_NO_OF_TQ_SJW   =  2 (mscan.h)    */

  /* ID Acceptance Control MSCAN_A/_B ------------------------------*/
  /*    [07]:res.        0 : reserved                               */
  /*    [06]:res.        0 : reserved                               */
  /*    [05]:IDAM1       0 : ID acceptance control, Bit1            */
  /*    [04]:IDAM0       1 : ID acceptance control, Bit0            */
  /*                         -> filter 16 bit mode                  */
  /*    [03]:res.        0 : reserved                               */
  /*    [02]:IDHIT2      0 : ID acceptance hit indication, Bit 2    */
  /*    [01]:IDHIT1      0 : ID acceptance hit indication, Bit 1    */
  /*    [00]:IDHIT0      0 : ID acceptance hit indication, Bit 0    */
  mscan->idac &= ~(IDAC_IDAM1);
  mscan->idac |=  (IDAC_IDAM0);

  /* initialize rx filter masks: only accept exact matches */
  mscan->idmr0  = 0x00;
  mscan->idmr1  = 0x00;
  mscan->idmr2  = 0x00;
  mscan->idmr3  = 0x00;
  mscan->idmr4  = 0x00;
  mscan->idmr5  = 0x00;
  mscan->idmr6  = 0x00;
  mscan->idmr7  = 0x00;

  /* initialize rx filters: set to illegal values, so no matches occure */
  mscan->idar0  = 0xff;
  mscan->idar1  = 0xff;
  mscan->idar2  = 0xff;
  mscan->idar3  = 0xff;
  mscan->idar4  = 0xff;
  mscan->idar5  = 0xff;
  mscan->idar6  = 0xff;
  mscan->idar7  = 0xff;

  /* Control Register 1 --------------------------------------------*/
  /*    [07]:CANE     0->1 : MSCAN Module is enabled                */
  /*    [06]:CLKSRC      1 : Clock Source -> IPB_CLOCK (bsp.h)      */
  /*    [05]:LOOPB       0 : No Loopback                            */
  /*    [04]:LISTEN      0 : Normal Operation                       */
  /*    [03]:res         0 : reserved                               */
  /*    [02]:WUPM        0 : No protect. from spurious WakeUp       */
  /*    [01]:SLPAK       0 : Sleep Mode Acknowledge (rd. only)      */
  /*    [00]:INITAK      0 : Init. Mode Acknowledge (rd. only)      */
  /* enable MSCAN A_/_B */
  mscan->ctl1 |= (CTL1_CANE);

  return;

  }


/*
 * MPC5x00 MSCAN perform settings in normal mode
 */
void mpc5200_mscan_perform_normal_mode_settings(volatile struct mpc5200_mscan *mscan)
  {

  /* Control Register 0 --------------------------------------------*/
  /*    [07]:RXFRM       0 : Recv. Frame, Flag Bit (rd.&clear only) */
  /*    [06]:RXACT       0 : Recv. Active, Status Bit (rd. only)    */
  /*    [05]:CSWAI       0 : CAN Stops in Wait Mode                 */
  /*    [04]:SYNCH       0 : Synchronized, Status Bit (rd. only)    */
  /*    [03]:TIME        1 : Generate Timestamps                    */
  /*    [02]:WUPE        0 : WakeUp Disabled                        */
  /*    [01]:SLPRQ       0 : No Sleep Mode Request                  */
  /*    [00]:INITRQ      0 : No init. Mode Request                  */
  /* Disable wait mode, enable timestamps */
  mscan->ctl0 &= ~(CTL0_CSWAI);
  mscan->ctl0 |=  (CTL0_TIME);

  return;

  }

rtems_status_code mpc5200_mscan_set_mode(rtems_device_minor_number minor, uint8_t mode)
  {
  struct mscan_channel_info *chan = NULL;
  volatile struct mpc5200_mscan *mscan     = NULL;

  switch(minor)
    {

    case MSCAN_A:
    case MSCAN_B:
      chan  = &chan_info[minor];
      mscan = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  if(chan->mode == mode)
    return RTEMS_SUCCESSFUL;

  switch(mode)
    {

    case MSCAN_NORMAL_MODE:
      /* if not already set enter init mode */
      mpc5200_mscan_enter_init_mode(mscan);

      if((chan->mode) == MSCAN_INITIALIZED_MODE)
        /* perform initialization which has to be done in init mode */
        mpc5200_mscan_perform_init_mode_settings(mscan);
      else
        /* exit sleep mode */
	    mpc5200_mscan_exit_sleep_mode(mscan);

	  /* exit init mode */
      mpc5200_mscan_exit_init_mode(mscan);
      /* enable ints. */
      mpc5200_mscan_int_enable(mscan);
      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(mscan);
      return RTEMS_SUCCESSFUL;
      break;

    case MSCAN_SLEEP_MODE:
      /* enable ints. */
      mpc5200_mscan_int_disable(mscan);
      /* if not already set enter init mode */
      mpc5200_mscan_enter_init_mode(mscan);
      /* exit sleep mode */
	  mpc5200_mscan_enter_sleep_mode(mscan);
	  /* exit init mode */
      mpc5200_mscan_exit_init_mode(mscan);
      return RTEMS_SUCCESSFUL;
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
rtems_status_code mscan_channel_initialize(rtems_device_major_number major, rtems_device_minor_number minor)
  {
  rtems_status_code status;
  struct mscan_channel_info *chan = &chan_info[minor];

  /* set registers according to MSCAN channel information */
  switch(minor)
    {

    case MSCAN_A:
      chan->rx_qname = rtems_build_name ('C', 'N', 'A', 'Q');

      /* register RTEMS device names for MSCAN A */
	  if((status = rtems_io_register_name (MSCAN_A_DEV_NAME, major, MSCAN_A)) != RTEMS_SUCCESSFUL)
	    return status;

      break;

    case MSCAN_B:
	  chan->rx_qname = rtems_build_name ('C', 'N', 'B', 'Q');

	  /* register RTEMS device names for MSCAN B */
	  if((status = rtems_io_register_name (MSCAN_B_DEV_NAME, major, MSCAN_B)) != RTEMS_SUCCESSFUL)
	    return status;

      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  /* create RTEMS rx message queue for MSCAN A */
  if((status = rtems_message_queue_create(chan->rx_qname,
		                                 (uint32_t) NO_OF_MSCAN_A_RX_BUFF,
		                                 (uint32_t) MSCAN_MESSAGE_SIZE(sizeof(struct can_message)),
		                                 (rtems_attribute) RTEMS_LOCAL | RTEMS_FIFO,
		                                 (rtems_id *)&(chan->rx_qid)))
		                                 != RTEMS_SUCCESSFUL)
    {
      return status;

    }

  /* Set up interrupts MSCAN A */
  if(!BSP_install_rtems_irq_handler(&(mpc5200_mscan_irq_data[minor])))
    rtems_panic("Can't attach MPC5x00 MSCAN interrupt handler %d\n", minor);

  /* basic setup for channel info. struct. */
  chan->regs        = (struct mpc5200_mscan *)&(mpc5200.mscan[minor]);
  chan->int_rx_err  = 0;
  chan->id_extended = FALSE;
  chan->mode        = MSCAN_INITIALIZED_MODE;

  return status;

  }


/*
 * MPC5x00 MSCAN device initialization
 */
rtems_device_driver mscan_initialize(rtems_device_major_number major,
                                     rtems_device_minor_number minor,
                                     void *arg
                                    )
  {
  rtems_status_code status;

  /* Initialization requested via RTEMS */
  if((status = mscan_channel_initialize(major,MSCAN_A)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if((status = mscan_channel_initialize(major,MSCAN_B)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if((status = mpc5200_mscan_set_mode(MSCAN_A, MSCAN_NORMAL_MODE)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  if((status = mpc5200_mscan_set_mode(MSCAN_B, MSCAN_NORMAL_MODE)) != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return status;

  }


/*
 * MPC5x00 MSCAN device open
 */
rtems_device_driver mscan_open( rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void * arg
                               )
  {
  rtems_status_code status;
  struct mscan_channel_info *chan = NULL;

  switch(minor)
    {

    case MSCAN_A:
    case MSCAN_B:
      chan = &chan_info[minor];
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  /* if not already set enter init mode */
  status = mpc5200_mscan_set_mode(minor, MSCAN_NORMAL_MODE);

  return status;

  }


/*
 * MPC5x00 MSCAN device close
 */
rtems_device_driver mscan_close( rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void * arg
                                )
  {
  rtems_status_code status;
  struct mscan_channel_info *chan = NULL;

  switch(minor)
    {

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
rtems_device_driver mscan_read( rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void * arg
                               )
  {
  rtems_status_code status;
  uint32_t          message_size = 0;
  rtems_libio_rw_args_t     *parms       = (rtems_libio_rw_args_t *)arg;
  struct mscan_rx_parms     *rx_parms    = (struct mscan_rx_parms *)(parms->buffer);
  struct can_message        *rx_mess     = (struct can_message *)(rx_parms->rx_mess);
  struct mscan_channel_info *chan        = NULL;

  switch(minor)
    {

    case MSCAN_A:
    case MSCAN_B:
	  chan = &chan_info[minor];
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  if((status = rtems_message_queue_receive(chan->rx_qid,
                                          (void *)(rx_mess),
                                          (uint32_t *)&message_size,
                                          (uint32_t)(rx_parms->rx_flags),
                                          (rtems_interval)(rx_parms->rx_timeout)))
                                          != RTEMS_SUCCESSFUL)
    {

    parms->bytes_moved = 0;

    }
  else
    {

    parms->bytes_moved = sizeof(struct can_message);

    }

  return status;

  }


/*
 * MPC5x00 MSCAN device write
 */
rtems_device_driver mscan_write( rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void * arg
                                )
  {
  rtems_libio_rw_args_t         *parms      = (rtems_libio_rw_args_t *)arg;
  struct mscan_tx_parms         *tx_parms   = (struct mscan_tx_parms *)(parms->buffer);
  struct can_message            *tx_mess    = (struct can_message *)(tx_parms->tx_mess);
  struct mscan_channel_info     *chan       = NULL;
  mscan_handle                  *mscan_hdl  = NULL;
  volatile struct mpc5200_mscan *mscan      = NULL;

  switch(minor)
    {

    case MSCAN_A:
    case MSCAN_B:
	  chan      = &chan_info[minor];
      mscan_hdl = mpc5200_mscan_irq_data[minor].handle;
      mscan     = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  /* select one free tx buffer (TOUCAN not registered) */
  if((mscan_hdl->toucan_callback) == NULL)
    {

    if(mscan->tflg & MSCAN_TX_BUFF2)
      mscan->bsel  = MSCAN_TX_BUFF2;

    if(mscan->tflg & MSCAN_TX_BUFF1)
      mscan->bsel  = MSCAN_TX_BUFF1;

    if(mscan->tflg & MSCAN_TX_BUFF0)
      mscan->bsel  = MSCAN_TX_BUFF0;
    }
  else
    {

    /* select a specific, preconfigured tx buffer (TOUCAN registered) */
    switch(tx_parms->tx_id)
      {

      case 0:
	  	if(mscan->tflg & MSCAN_TX_BUFF0)
          mscan->bsel  = MSCAN_TX_BUFF0;
        break;

	  case 1:
	    if(mscan->tflg & MSCAN_TX_BUFF1)
          mscan->bsel  = MSCAN_TX_BUFF1;
	    break;

	  case 2:
	 	if(mscan->tflg & MSCAN_TX_BUFF2)
          mscan->bsel  = MSCAN_TX_BUFF2;
	    break;

      default:
        break;

	  }

    }

  /* if no tx buffer is available, teminate the write request */
  if(!(mscan->bsel))
    {

    parms->bytes_moved = 0;
    return RTEMS_UNSATISFIED;

    }

  /* prepare tx id and dlc (TOUCAN not initialized) */
  if((mscan_hdl->toucan_callback) == NULL)
    {

    /* check for tx length */
    if((tx_mess->mess_len) & 0x000F)
      {

      /* set tx id */
      mscan->txidr0 = SET_IDR0(tx_mess->mess_id);
      mscan->txidr1 = SET_IDR1(tx_mess->mess_id);
      mscan->txidr2 = 0;
      mscan->txidr3 = 0;

      /* insert dlc into mscan register */
      mscan->txdlr = (uint8_t)((tx_mess->mess_len) & 0x000F);

      }
    else
      {

      parms->bytes_moved = 0;
	  return RTEMS_UNSATISFIED;

      }

    }

  /* copy tx data to MSCAN registers */
  switch(mscan->txdlr)
    {

    case 8:
      mscan->txdsr7 = tx_mess->mess_data[7];
    case 7:
      mscan->txdsr6 = tx_mess->mess_data[6];
    case 6:
      mscan->txdsr5 = tx_mess->mess_data[5];
    case 5:
      mscan->txdsr4 = tx_mess->mess_data[4];
    case 4:
      mscan->txdsr3 = tx_mess->mess_data[3];
    case 3:
      mscan->txdsr2 = tx_mess->mess_data[2];
    case 2:
      mscan->txdsr1 = tx_mess->mess_data[1];
    case 1:
      mscan->txdsr0 = tx_mess->mess_data[0];
      break;
    default:
      break;

    }

  /* enable message buffer specific interrupt */
  mscan->tier |= mscan->bsel;

  /* start transfer */
  mscan->tflg = mscan->bsel;

  return RTEMS_SUCCESSFUL;

  }


/*
 * MPC5x00 MSCAN device control
 */
rtems_device_driver mscan_control( rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void * arg
                                  )
  {
  uint16_t tx_id;
  rtems_libio_ioctl_args_t      *parms      = (rtems_libio_ioctl_args_t *)arg;
  struct mscan_ctrl_parms       *ctrl_parms = (struct mscan_ctrl_parms *)(parms->buffer);
  struct mscan_channel_info     *chan       = NULL;
  mscan_handle                  *mscan_hdl  = NULL;
  volatile struct mpc5200_mscan *mscan      = NULL;

  switch(minor)
    {

    case MSCAN_A:
    case MSCAN_B:
	  chan = &chan_info[minor];
      mscan_hdl  = mpc5200_mscan_irq_data[minor].handle;
      mscan = chan->regs;
      break;

    default:
      return RTEMS_UNSATISFIED;
      break;
    }

  switch(parms->command)
    {

      /* TOUCAN callback initialization for MSCAN */
    case TOUCAN_MSCAN_INIT:
      mscan_hdl->toucan_callback = ctrl_parms->toucan_cb_fnc;
      break;

    /* set rx buffer ID */
    case MSCAN_SET_RX_ID:

      /* enter init mode */
      mpc5200_mscan_enter_init_mode(mscan);

      switch(ctrl_parms->ctrl_reg_no)
        {

        case RX_BUFFER_0:
          mscan->idar0  = SET_IDR0(ctrl_parms->ctrl_id);
          mscan->idar1  = SET_IDR1(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_1:
          mscan->idar2  = SET_IDR2(ctrl_parms->ctrl_id);
          mscan->idar3  = SET_IDR3(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_2:
          mscan->idar4  = SET_IDR4(ctrl_parms->ctrl_id);
          mscan->idar5  = SET_IDR5(ctrl_parms->ctrl_id);
          break;

        case RX_BUFFER_3:
          mscan->idar6  = SET_IDR6(ctrl_parms->ctrl_id);
          mscan->idar7  = SET_IDR7(ctrl_parms->ctrl_id);
          break;

        default:
        break;

        }

      /* exit init mode and perform further initialization which is required in the normal mode */
      mpc5200_mscan_exit_init_mode(mscan);

      /* enable ints. */
      mpc5200_mscan_int_enable(mscan);

      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(mscan);

      return RTEMS_SUCCESSFUL;
      break;

    /* get rx buffer ID */
    case MSCAN_GET_RX_ID:

      switch(ctrl_parms->ctrl_reg_no)
	    {

        case RX_BUFFER_0:
          ctrl_parms->ctrl_id = GET_IDR0(mscan->idar0) | GET_IDR1(mscan->idar1);
          break;

        case RX_BUFFER_1:
          ctrl_parms->ctrl_id = GET_IDR2(mscan->idar2) | GET_IDR3(mscan->idar3);
          break;

        case RX_BUFFER_2:
          ctrl_parms->ctrl_id = GET_IDR4(mscan->idar4) | GET_IDR5(mscan->idar5);
          break;

        case RX_BUFFER_3:
          ctrl_parms->ctrl_id = GET_IDR6(mscan->idar6) | GET_IDR7(mscan->idar7);
          break;

        default:
        break;

        }

      break;

    /* set rx buffer ID mask */
    case MSCAN_SET_RX_ID_MASK:

      /* enter init mode */
      mpc5200_mscan_enter_init_mode(mscan);

      switch(ctrl_parms->ctrl_reg_no)
        {

        case RX_BUFFER_0:
          mscan->idmr0  = SET_IDR0(ctrl_parms->ctrl_id_mask);
          mscan->idmr1  = SET_IDR1(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_1:
          mscan->idmr2  = SET_IDR2(ctrl_parms->ctrl_id_mask);
          mscan->idmr3  = SET_IDR3(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_2:
          mscan->idmr4  = SET_IDR4(ctrl_parms->ctrl_id_mask);
          mscan->idmr5  = SET_IDR5(ctrl_parms->ctrl_id_mask);
          break;

        case RX_BUFFER_3:
          mscan->idmr6  = SET_IDR6(ctrl_parms->ctrl_id_mask);
          mscan->idmr7  = SET_IDR7(ctrl_parms->ctrl_id_mask);
          break;

        default:
        break;

        }

      /* exit init mode and perform further initialization which is required in the normal mode */
      mpc5200_mscan_exit_init_mode(mscan);

      /* enable ints. */
      mpc5200_mscan_int_enable(mscan);

      /* wait for bus sync. */
      mpc5200_mscan_wait_sync(mscan);

      break;

    /* get rx buffer ID mask */
    case MSCAN_GET_RX_ID_MASK:

      switch(ctrl_parms->ctrl_reg_no)
        {

        case RX_BUFFER_0:
          ctrl_parms->ctrl_id_mask = GET_IDR0(mscan->idmr0) | GET_IDR1(mscan->idmr1);
          break;

        case RX_BUFFER_1:
          ctrl_parms->ctrl_id_mask = GET_IDR2(mscan->idmr2) | GET_IDR3(mscan->idmr3);
          break;

        case RX_BUFFER_2:
          ctrl_parms->ctrl_id_mask = GET_IDR4(mscan->idmr4) | GET_IDR5(mscan->idmr5);
          break;

        case RX_BUFFER_3:
          ctrl_parms->ctrl_id_mask = GET_IDR6(mscan->idmr6) | GET_IDR7(mscan->idmr7);
          break;

        default:
        break;

        }

    /* set tx buffer ID */
    case MSCAN_SET_TX_ID:

      /* check for availability of tx buffer */
      if(!((mscan->tflg) & (uint8_t)(ctrl_parms->ctrl_reg_no)))
        {

        /* do abort tx buf. request */
        mscan->tarq = (uint8_t)(ctrl_parms->ctrl_reg_no);

        /* wait for abort tx buf. ack. */
        while((mscan->taak) & (uint8_t)(ctrl_parms->ctrl_reg_no));

        }

      /* select tx buf. */
      mscan->bsel   = (uint8_t)(ctrl_parms->ctrl_reg_no);

      /* set the tx id of selected buf. */
      tx_id = ctrl_parms->ctrl_id;
      mscan->txidr0 = SET_IDR0(tx_id);
      mscan->txidr1 = SET_IDR1(tx_id);
      mscan->txidr2 = 0;
      mscan->txidr3 = 0;

      break;

    /* get tx buffer ID */
    case MSCAN_GET_TX_ID:

      /* select tx buf. */
      mscan->bsel   = (uint8_t)(ctrl_parms->ctrl_reg_no);

      /* get tx id. of selected buf. */
      ctrl_parms->ctrl_id = GET_IDR0(mscan->txidr0) | GET_IDR1(mscan->txidr1);

      break;

    default:
      break;

    }

  return RTEMS_SUCCESSFUL;

  }

