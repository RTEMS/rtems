/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: */
/*
 *  SMC1/2 SCC1..4 raw console serial I/O.
 *  adapted to work with up to 4 SCC and 2 SMC
 *
 *  This driver is an example of `TASK DRIVEN' `POLLING' or `INTERRUPT' I/O.
 *
 *  To run with interrupt-driven I/O, ensure m8xx_smc1_interrupt
 *  is set before calling the initialization routine.
 *
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>
#include <mpc8xx.h>
#include <rtems/irq.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <unistd.h>
#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>

/*
 * Interrupt-driven input buffer
 */
#define RXBUFSIZE	16

#define M8xx_SICR_BRG1 (0)
#define M8xx_SICR_BRG2 (1)
#define M8xx_SICR_BRG3 (2)
#define M8xx_SICR_BRG4 (3)

#define M8xx_SICR_SCCRX_MSK(scc) ((  7) << (((scc))*8+3))
#define M8xx_SICR_SCCRX(scc,clk) ((clk) << (((scc))*8+3))

#define M8xx_SICR_SCCTX_MSK(scc) ((  7) << (((scc))*8+0))
#define M8xx_SICR_SCCTX(scc,clk) ((clk) << (((scc))*8+0))

#define M8xx_SIMODE_SMCCS(smc,clk) ((clk) << ((smc)*16+12))
#define M8xx_SIMODE_SMCCS_MSK(smc) M8xx_SIMODE_SMCCS(smc,7)

#define CONS_CHN_CNT 6
#define CONS_CHN_SCC1 0
#define CONS_CHN_SCC2 1
#define CONS_CHN_SCC3 2
#define CONS_CHN_SCC4 3
#define CONS_CHN_SMC1 4
#define CONS_CHN_SMC2 5
#define CONS_CHN_NONE -1

/*
 * possible identifiers for bspopts.h: CONS_SxCy_MODE
 */
#define CONS_MODE_UNUSED -1
#define CONS_MODE_POLLED TERMIOS_POLLED
#define CONS_MODE_IRQ    TERMIOS_IRQ_DRIVEN

#define CHN_IS_SCC(chan) ((chan) < CONS_CHN_SMC1)

#define BRG_CNT 4

#define MAX_IDL_DEFAULT 10
#define DEVICEPREFIX "tty"

/*
 * Interrupt-driven callback
 */
static int m8xx_scc_mode[CONS_CHN_CNT];
static void *sccttyp[CONS_CHN_CNT];
typedef struct m8xx_console_chan_desc_s {
  bool is_scc;                  /* true for SCC */
  struct {
    volatile m8xxSCCparms_t *sccp;
    volatile m8xxSMCparms_t *smcp;
  } parms;
  struct {
    volatile m8xxSCCRegisters_t *sccr;
    volatile m8xxSMCRegisters_t *smcr;
  } regs;
  int ivec_src;
  int cr_chan_code;
  int brg_used;
} m8xx_console_chan_desc_t;

m8xx_console_chan_desc_t m8xx_console_chan_desc[CONS_CHN_CNT] = {
  /* SCC1 */
  {TRUE,
   {(m8xxSCCparms_t *)&(m8xx.scc1p),NULL},
   {&(m8xx.scc1),NULL},
   BSP_CPM_IRQ_SCC1,
   M8xx_CR_CHAN_SCC1,
   -1},
  /* SCC2 */
  {TRUE,
   {&(m8xx.scc2p),NULL},
   {&(m8xx.scc2),NULL},
   BSP_CPM_IRQ_SCC2,
   M8xx_CR_CHAN_SCC2,
   -1},
  /* SCC3 */
  {TRUE,
   {&(m8xx.scc3p),NULL},
   {&(m8xx.scc3),NULL},
   BSP_CPM_IRQ_SCC3,
   M8xx_CR_CHAN_SCC3,
   -1},
  /* SCC4 */
  {TRUE,
   {&(m8xx.scc4p),NULL},
   {&(m8xx.scc4),NULL},
   BSP_CPM_IRQ_SCC4,
   M8xx_CR_CHAN_SCC4,
   -1},
  /* SMC1 */
  {FALSE,
   {NULL,&(m8xx.smc1p)},
   {NULL,&(m8xx.smc1)},
   BSP_CPM_IRQ_SMC1,
   M8xx_CR_CHAN_SMC1,
   -1},
  /* SMC2 */
  {FALSE,
   {NULL,&(m8xx.smc2p)},
   {NULL,&(m8xx.smc2)},
   BSP_CPM_IRQ_SMC2_OR_PIP,
   M8xx_CR_CHAN_SMC2,
   -1}};

#define CHN_PARAM_GET(chan,param)			\
  (m8xx_console_chan_desc[chan].is_scc			\
   ? m8xx_console_chan_desc[chan].parms.sccp->param	\
   : m8xx_console_chan_desc[chan].parms.smcp->param)

#define CHN_PARAM_SET(chan,param,value)				\
  do {if (m8xx_console_chan_desc[chan].is_scc)			\
      m8xx_console_chan_desc[chan].parms.sccp->param = value;	\
    else							\
      m8xx_console_chan_desc[chan].parms.smcp->param = value;	\
  } while (0)

#define CHN_EVENT_GET(chan)				\
  (m8xx_console_chan_desc[chan].is_scc			\
   ? m8xx_console_chan_desc[chan].regs.sccr->scce	\
   : m8xx_console_chan_desc[chan].regs.smcr->smce)

#define CHN_EVENT_CLR(chan,mask) 				\
  do {								\
    if (m8xx_console_chan_desc[chan].is_scc) 			\
      m8xx_console_chan_desc[chan].regs.sccr->scce = (mask);	\
    else							\
      m8xx_console_chan_desc[chan].regs.smcr->smce = (mask);	\
  }while (0)

#define CHN_MASK_GET(chan)				\
  (m8xx_console_chan_desc[chan].is_scc			\
   ? m8xx_console_chan_desc[chan].regs.sccr->sccm	\
   : m8xx_console_chan_desc[chan].regs.smcr->smcm)

#define CHN_MASK_SET(chan,mask) 				\
  do {								\
    if (m8xx_console_chan_desc[chan].is_scc) 			\
      m8xx_console_chan_desc[chan].regs.sccr->sccm = (mask);	\
    else							\
      m8xx_console_chan_desc[chan].regs.smcr->smcm = (mask);	\
  }while (0)


/*
 * I/O buffers and pointers to buffer descriptors
 */
#define SCC_RXBD_CNT 4
#define SCC_TXBD_CNT 4
typedef volatile char sccRxBuf_t[SCC_RXBD_CNT][RXBUFSIZE];
static sccRxBuf_t *rxBuf[CONS_CHN_CNT];

static volatile m8xxBufferDescriptor_t *sccFrstRxBd[CONS_CHN_CNT];
static volatile m8xxBufferDescriptor_t *sccCurrRxBd[CONS_CHN_CNT];
static volatile m8xxBufferDescriptor_t *sccFrstTxBd[CONS_CHN_CNT];
static volatile m8xxBufferDescriptor_t *sccPrepTxBd[CONS_CHN_CNT];
static volatile m8xxBufferDescriptor_t *sccDequTxBd[CONS_CHN_CNT];

/*
 * Compute baud-rate-generator configuration register value
 */
static uint32_t
sccBRGval (int baud)
{
  int divisor;
  int div16 = 0;

  divisor = ((BSP_bus_frequency / 16) + (baud / 2)) / baud;
  if (divisor > 4096) {
    div16 = 1;
    divisor = (divisor + 8) / 16;
  }
  return M8xx_BRG_EN | M8xx_BRG_EXTC_BRGCLK | ((divisor - 1) << 1) | div16;
}

typedef struct {
  uint32_t reg_content;
  int link_cnt;
}brg_state_t;
brg_state_t scc_brg_state[BRG_CNT];

/*
 * initialize brg_state
 */
static void sccBRGinit(void)
{
  int brg_idx;

  for (brg_idx = 0;brg_idx < BRG_CNT;brg_idx++) {
    scc_brg_state[brg_idx].reg_content = 0;
    scc_brg_state[brg_idx].link_cnt    = 0;
  }
#ifndef MDE360
  /*
   * on ZEM40, init CLK4/5 inputs
   */
  m8xx.papar |=  ((1 << 11) | (1 << 12));
  m8xx.padir &= ~((1 << 11) | (1 << 12));
#endif
}

#if CONS_USE_EXT_CLK
/*
 * input clock frq for CPM clock inputs
 */
static uint32_t clkin_frq[2][4] = {
#ifdef MDE360
  {0,0,0,0},
  {0,0,0,0}
#else
  {0,0,0,1843000},
  {1843000,0,0,0}
#endif
};
#endif

/*
 * allocate, set and connect baud rate generators
 * FIXME: or clock input
 * FIXME: set pin to be clock input
 */

static int sccBRGalloc(int chan,int baud)
{
  rtems_interrupt_level level;
  m8xx_console_chan_desc_t *chan_desc = &(m8xx_console_chan_desc[chan]);
  uint32_t reg_val;
  int old_brg;
  int new_brg = -1;
  int brg_idx;
#if CONS_USE_EXT_CLK
  int clk_group;
  int clk_sel;
#endif

  old_brg = chan_desc->brg_used;
  /* compute brg register contents needed */
  reg_val = sccBRGval(baud);

#if CONS_EXT_CLK
  /* search for clock input with this frq */
  clk_group = ((chan == CONS_CHN_SCC3) ||
	       (chan == CONS_CHN_SCC4) ||
	       (chan == CONS_CHN_SMC2)) ? 1 : 0;

  for (clk_sel = 0, new_brg = -1;
       (clk_sel < 4) && (new_brg < 0);
       clk_sel++) {
    if (baud == (clkin_frq[clk_group][clk_sel] / 16)) {
      new_brg = clk_sel + 4;
    }
  }
#endif

  rtems_interrupt_disable(level);

  if (new_brg < 0) {
    /* search for brg with this settings */
    for (brg_idx = 0;
	 (new_brg < 0) && (brg_idx < BRG_CNT);
	 brg_idx++) {
      if (scc_brg_state[brg_idx].reg_content == reg_val) {
	new_brg = brg_idx;
      }
    }
    /*
     * if not found: check, whether brg currently in use
     * is linked only from our channel
     */
    if ((new_brg < 0) &&
	(old_brg >= 0) &&
	(scc_brg_state[old_brg].link_cnt == 1)) {
      new_brg = old_brg;
    }
    /* if not found: search for unused brg, set it  */
    for (brg_idx = 0;
	 (new_brg < 0) && (brg_idx < BRG_CNT);
	 brg_idx++) {
      if (scc_brg_state[brg_idx].link_cnt == 0) {
	new_brg = brg_idx;
      }
    }
  }

  /* decrease old link count */
  if ((old_brg >= 0) &&
      (old_brg < 4)) {
    scc_brg_state[old_brg].link_cnt--;
  }
  /* increase new brg link count, set brg */
  if ((new_brg >= 0) &&
      (new_brg < 4)) {
    scc_brg_state[new_brg].link_cnt++;
    scc_brg_state[new_brg].reg_content = reg_val;
    (&m8xx.brgc1)[new_brg] = reg_val;
  }
  rtems_interrupt_enable(level);

  /* connect to scc/smc */
  if (new_brg >= 0) {
    m8xx_console_chan_desc[chan].brg_used = new_brg;
    /*
     * Put SCC in NMSI mode, connect SCC to BRG or CLKx
     */
    if (m8xx_console_chan_desc[chan].is_scc) {
      m8xx.sicr = ((m8xx.sicr & ~(M8xx_SICR_SCCRX_MSK(chan) |
				  M8xx_SICR_SCCTX_MSK(chan))) |
		   M8xx_SICR_SCCRX(chan,new_brg)|
		   M8xx_SICR_SCCTX(chan,new_brg));
    }
    else {
      /* connect SMC to BRGx or CLKx... */
      m8xx.simode = ((m8xx.simode & ~(M8xx_SIMODE_SMCCS_MSK(chan - CONS_CHN_SMC1)))|
		     M8xx_SIMODE_SMCCS(chan - CONS_CHN_SMC1,new_brg));
    }
  }
  return (new_brg < 0);
}


/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
sccSetAttributes (int minor, const struct termios *t)
{
  int baud;

  switch (t->c_cflag & CBAUD) {
  default:	baud = -1;	break;
  case B50:	baud = 50;	break;
  case B75:	baud = 75;	break;
  case B110:	baud = 110;	break;
  case B134:	baud = 134;	break;
  case B150:	baud = 150;	break;
  case B200:	baud = 200;	break;
  case B300:	baud = 300;	break;
  case B600:	baud = 600;	break;
  case B1200:	baud = 1200;	break;
  case B1800:	baud = 1800;	break;
  case B2400:	baud = 2400;	break;
  case B4800:	baud = 4800;	break;
  case B9600:	baud = 9600;	break;
  case B19200:	baud = 19200;	break;
  case B38400:	baud = 38400;	break;
  case B57600:	baud = 57600;	break;
  case B115200:	baud = 115200;	break;
  case B230400:	baud = 230400;	break;
  case B460800:	baud = 460800;	break;
  }
  return sccBRGalloc(minor,baud);
  return 0;
}

/*
 * Interrupt handler
 */
static rtems_isr
sccInterruptHandler (void *arg)
{
  int chan = (int)arg;

  /*
   * Buffer received?
   */
  if (CHN_EVENT_GET(chan) & 0x1) {
    /*
     * clear SCC event flag
     */
    CHN_EVENT_CLR(chan,0x01);
    /*
     * process event
     */
    while ((sccCurrRxBd[chan]->status & M8xx_BD_EMPTY) == 0) {
      if (sccttyp[chan] != NULL) {
	rtems_cache_invalidate_multiple_data_lines((void *)sccCurrRxBd[chan]->buffer,
						   sccCurrRxBd[chan]->length);
	rtems_termios_enqueue_raw_characters (sccttyp[chan],
					      (char *)sccCurrRxBd[chan]->buffer,
					      sccCurrRxBd[chan]->length);
      }
      /*
       * clear status
       */
      sccCurrRxBd[chan]->status =
	(sccCurrRxBd[chan]->status
	 & (M8xx_BD_WRAP | M8xx_BD_INTERRUPT))
	| M8xx_BD_EMPTY;
      /*
       * advance to next BD
       */
      if ((sccCurrRxBd[chan]->status & M8xx_BD_WRAP) != 0) {
	sccCurrRxBd[chan] = sccFrstRxBd[chan];
      }
      else {
	sccCurrRxBd[chan]++;
      }
    }
  }
  /*
   * Buffer transmitted?
   */
  if (CHN_EVENT_GET(chan) & 0x2) {
    /*
     * then clear interrupt event bit
     */
    CHN_EVENT_CLR(chan,0x2);
    /*
     * and signal successful transmit to termios
     */
    /*
     * FIXME: multiple dequeue calls for multiple buffers
     */
    while((sccDequTxBd[chan] != sccPrepTxBd[chan]) &&
	  ((sccDequTxBd[chan]->status & M8xx_BD_READY) == 0)) {
      if (sccttyp[chan] != NULL) {
	rtems_termios_dequeue_characters (sccttyp[chan],
					  sccDequTxBd[chan]->length);
      }
      /*
       * advance to next BD
       */
      if ((sccDequTxBd[chan]->status & M8xx_BD_WRAP) != 0) {
	sccDequTxBd[chan] = sccFrstTxBd[chan];
      }
      else {
	sccDequTxBd[chan]++;
      }
    }
  }
}

static void
mpc8xx_console_irq_on(const rtems_irq_connect_data *irq)
{
    CHN_MASK_SET(irq->name - BSP_CPM_IRQ_LOWEST_OFFSET,
		 3);	/* Enable TX and RX interrupts */
}

static void
mpc8xx_console_irq_off(const rtems_irq_connect_data *irq)
{
    CHN_MASK_SET(irq->name - BSP_CPM_IRQ_LOWEST_OFFSET,
		 0);	/* Disable TX and RX interrupts */
}

static int
mpc8xx_console_irq_isOn(const rtems_irq_connect_data *irq)
{
  return (0 != CHN_MASK_GET(irq->name - BSP_CPM_IRQ_LOWEST_OFFSET)); /* Check TX and RX interrupts */
}

static void
sccInitialize (int chan)
{
  int i;
  /*
   * allocate buffers
   * FIXME: use a cache-line size boundary alloc here
   */
  rxBuf[chan] = malloc(sizeof(*rxBuf[chan]) + 2*PPC_CACHE_ALIGNMENT);
  if (rxBuf[chan] == NULL) {
    BSP_panic("Cannot allocate console rx buffer\n");
  }
  else {
    /*
     * round up rxBuf[chan] to start at a cache line size
     */
    rxBuf[chan] = (sccRxBuf_t *)
      (((uint32_t)rxBuf[chan]) +
       (PPC_CACHE_ALIGNMENT
	- ((uint32_t)rxBuf[chan]) % PPC_CACHE_ALIGNMENT));
  }
  /*
   * Allocate buffer descriptors
   */
  sccCurrRxBd[chan] =
    sccFrstRxBd[chan] = m8xx_bd_allocate(SCC_RXBD_CNT);
  sccPrepTxBd[chan] =
    sccDequTxBd[chan] =
    sccFrstTxBd[chan] = m8xx_bd_allocate(SCC_TXBD_CNT);
  switch(chan) {
  case CONS_CHN_SCC1:
    /*
     * Configure port A pins to enable TXD1 and RXD1 pins
     * FIXME: add setup for modem control lines....
     */
    m8xx.papar |=  0x03;
    m8xx.padir &= ~0x03;

    /*
     * Configure port C pins to enable RTS1 pins (static active low)
     */
    m8xx.pcpar &= ~0x01;
    m8xx.pcso  &= ~0x01;
    m8xx.pcdir |=  0x01;
    m8xx.pcdat &= ~0x01;
    break;
  case CONS_CHN_SCC2:
    /*
     * Configure port A pins to enable TXD2 and RXD2 pins
     * FIXME: add setup for modem control lines....
     */
    m8xx.papar |=  0x0C;
    m8xx.padir &= ~0x0C;

    /*
     * Configure port C pins to enable RTS2 pins (static active low)
     */
    m8xx.pcpar &= ~0x02;
    m8xx.pcso  &= ~0x02;
    m8xx.pcdir |=  0x02;
    m8xx.pcdat &= ~0x02;
    break;
  case CONS_CHN_SCC3:
    /*
     * Configure port A pins to enable TXD3 and RXD3 pins
     * FIXME: add setup for modem control lines....
     */
    m8xx.papar |=  0x30;
    m8xx.padir &= ~0x30;

    /*
     * Configure port C pins to enable RTS3 (static active low)
     */
    m8xx.pcpar &= ~0x04;
    m8xx.pcso  &= ~0x04;
    m8xx.pcdir |=  0x04;
    m8xx.pcdat &= ~0x04;
    break;
  case CONS_CHN_SCC4:
    /*
     * Configure port A pins to enable TXD4 and RXD4 pins
     * FIXME: add setup for modem control lines....
     */
    m8xx.papar |=  0xC0;
    m8xx.padir &= ~0xC0;

    /*
     * Configure port C pins to enable RTS4 pins (static active low)
     */
    m8xx.pcpar &= ~0x08;
    m8xx.pcso  &= ~0x08;
    m8xx.pcdir |=  0x08;
    m8xx.pcdat &= ~0x08;
    break;
  case CONS_CHN_SMC1:
    /*
     * Configure port B pins to enable SMTXD1 and SMRXD1 pins
     */
    m8xx.pbpar |=  0xC0;
    m8xx.pbdir &= ~0xC0;
    break;
  case CONS_CHN_SMC2:
    /*
     * Configure port B pins to enable SMTXD2 and SMRXD2 pins
     */
    m8xx.pbpar |=  0xC00;
    m8xx.pbdir &= ~0xC00;
    break;
  }
  /*
   * allocate and connect BRG
   */
  sccBRGalloc(chan,9600);


  /*
   * Set up SCCx parameter RAM common to all protocols
   */
  CHN_PARAM_SET(chan,rbase,(char *)sccFrstRxBd[chan] - (char *)&m8xx);
  CHN_PARAM_SET(chan,tbase,(char *)sccFrstTxBd[chan] - (char *)&m8xx);
  CHN_PARAM_SET(chan,rfcr ,M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0));
  CHN_PARAM_SET(chan,tfcr ,M8xx_TFCR_MOT | M8xx_TFCR_DMA_SPACE(0));
  if (m8xx_scc_mode[chan] != TERMIOS_POLLED)
    CHN_PARAM_SET(chan,mrblr,RXBUFSIZE);
  else
    CHN_PARAM_SET(chan,mrblr,1);

  /*
   * Set up SCCx parameter RAM UART-specific parameters
   */
  CHN_PARAM_SET(chan,un.uart.max_idl ,MAX_IDL_DEFAULT);
  CHN_PARAM_SET(chan,un.uart.brkln   ,0);
  CHN_PARAM_SET(chan,un.uart.brkec   ,0);
  CHN_PARAM_SET(chan,un.uart.brkcr   ,0);
  if (m8xx_console_chan_desc[chan].is_scc) {
    m8xx_console_chan_desc[chan].parms.sccp->un.uart.character[0]=0x8000; /* no char filter */
    m8xx_console_chan_desc[chan].parms.sccp->un.uart.rccm=0x80FF; /* control character mask */
  }

  /*
   * Set up the Receive Buffer Descriptors
   */
  for (i = 0;i < SCC_RXBD_CNT;i++) {
    sccFrstRxBd[chan][i].status = M8xx_BD_EMPTY | M8xx_BD_INTERRUPT;
    if (i == SCC_RXBD_CNT-1) {
      sccFrstRxBd[chan][i].status |= M8xx_BD_WRAP;
    }
    sccFrstRxBd[chan][i].length = 0;
    sccFrstRxBd[chan][i].buffer = (*rxBuf[chan])[i];
  }
  /*
   * Setup the Transmit Buffer Descriptor
   */
  for (i = 0;i < SCC_TXBD_CNT;i++) {
    sccFrstTxBd[chan][i].status = M8xx_BD_INTERRUPT;
    if (i == SCC_TXBD_CNT-1) {
      sccFrstTxBd[chan][i].status |= M8xx_BD_WRAP;
    }
    sccFrstTxBd[chan][i].length = 0;
    sccFrstTxBd[chan][i].buffer = NULL;
  }

  /*
   * Set up SCC general and protocol-specific mode registers
   */
  CHN_EVENT_CLR(chan,~0);	/* Clear any pending events */
  CHN_MASK_SET(chan,0);	        /* Mask all interrupt/event sources */

  if (m8xx_console_chan_desc[chan].is_scc) {
    m8xx_console_chan_desc[chan].regs.sccr->psmr = 0xb000; /* 8N1, CTS flow control */
    m8xx_console_chan_desc[chan].regs.sccr->gsmr_h = 0x00000000;
    m8xx_console_chan_desc[chan].regs.sccr->gsmr_l = 0x00028004; /* UART mode */
  }
  else {
    m8xx_console_chan_desc[chan].regs.smcr->smcmr = 0x4820;
  }
  /*
   * Send "Init parameters" command
   */
  m8xx_cp_execute_cmd(M8xx_CR_OP_INIT_RX_TX
		      | m8xx_console_chan_desc[chan].cr_chan_code);

  /*
   * Enable receiver and transmitter
   */
  if (m8xx_console_chan_desc[chan].is_scc) {
    m8xx_console_chan_desc[chan].regs.sccr->gsmr_l |= 0x00000030;
  }
  else {
    m8xx_console_chan_desc[chan].regs.smcr->smcmr |= 0x0003;
  }

  if (m8xx_scc_mode[chan] != TERMIOS_POLLED) {

    rtems_irq_connect_data irq_conn_data = {
      m8xx_console_chan_desc[chan].ivec_src,
      sccInterruptHandler,         /* rtems_irq_hdl           */
      (rtems_irq_hdl_param)chan,   /* (rtems_irq_hdl_param)   */
      mpc8xx_console_irq_on,       /* (rtems_irq_enable)      */
      mpc8xx_console_irq_off,      /* (rtems_irq_disable)     */
      mpc8xx_console_irq_isOn      /* (rtems_irq_is_enabled)  */
    };
    if (!BSP_install_rtems_irq_handler (&irq_conn_data)) {
      rtems_panic("console: cannot install IRQ handler");
    }
  }
}

/*
 * polled scc read function
 */
static int
sccPollRead (int minor)
{
  int c = -1;
  int chan = minor;

  while(1) {
    if ((sccCurrRxBd[chan]->status & M8xx_BD_EMPTY) != 0) {
      return -1;
    }

    if (0 == (sccCurrRxBd[chan]->status & (M8xx_BD_OVERRUN
					   | M8xx_BD_PARITY_ERROR
					   | M8xx_BD_FRAMING_ERROR
					   | M8xx_BD_BREAK
					   | M8xx_BD_IDLE))) {
      /* character received and no error detected */
      rtems_cache_invalidate_multiple_data_lines((void *)sccCurrRxBd[chan]->buffer,
						 sccCurrRxBd[chan]->length);
      c = (unsigned)*((char *)sccCurrRxBd[chan]->buffer);
      /*
       * clear status
       */
    }
    sccCurrRxBd[chan]->status =
      (sccCurrRxBd[chan]->status
       & (M8xx_BD_WRAP | M8xx_BD_INTERRUPT))
      | M8xx_BD_EMPTY;
    /*
     * advance to next BD
     */
    if ((sccCurrRxBd[chan]->status & M8xx_BD_WRAP) != 0) {
      sccCurrRxBd[chan] = sccFrstRxBd[chan];
    }
    else {
      sccCurrRxBd[chan]++;
    }
    if (c >= 0) {
      return c;
    }
  }
}


/*
 * Device-dependent write routine
 * Interrupt-driven devices:
 *	Begin transmission of as many characters as possible (minimum is 1).
 * Polling devices:
 *	Transmit all characters.
 */
static ssize_t
sccInterruptWrite (int minor, const char *buf, size_t len)
{
  int chan = minor;

  if ((sccPrepTxBd[chan]->status & M8xx_BD_READY) == 0) {
    sccPrepTxBd[chan]->buffer = (char *)buf;
    sccPrepTxBd[chan]->length = len;
    rtems_cache_flush_multiple_data_lines((const void *)buf,len);
    /*
     * clear status, set ready bit
     */
    sccPrepTxBd[chan]->status =
      (sccPrepTxBd[chan]->status
       & M8xx_BD_WRAP)
      | M8xx_BD_READY | M8xx_BD_INTERRUPT;
    if ((sccPrepTxBd[chan]->status & M8xx_BD_WRAP) != 0) {
      sccPrepTxBd[chan] = sccFrstTxBd[chan];
    }
    else {
      sccPrepTxBd[chan]++;
    }
  }
  return 0;
}

static ssize_t
sccPollWrite (int minor, const char *buf, size_t len)
{
  static char txBuf[CONS_CHN_CNT][SCC_TXBD_CNT];
  int chan = minor;
  int bd_used;
  size_t retval = len;

  while (len--) {
    while (sccPrepTxBd[chan]->status & M8xx_BD_READY)
      continue;
    bd_used = sccPrepTxBd[chan]-sccFrstTxBd[chan];
    txBuf[chan][bd_used] = *buf++;
      rtems_cache_flush_multiple_data_lines((const void *)&txBuf[chan][bd_used],
					    sizeof(txBuf[chan][bd_used]));
    sccPrepTxBd[chan]->buffer = &(txBuf[chan][bd_used]);
    sccPrepTxBd[chan]->length = 1;
    sccPrepTxBd[chan]->status =
      (sccPrepTxBd[chan]->status
       & M8xx_BD_WRAP)
      | M8xx_BD_READY;
    if ((sccPrepTxBd[chan]->status & M8xx_BD_WRAP) != 0) {
      sccPrepTxBd[chan] = sccFrstTxBd[chan];
    }
    else {
      sccPrepTxBd[chan]++;
    }
  }
  return retval;
}

/*
 * printk basic support
 */
int BSP_output_chan = CONS_CHN_NONE; /* channel used for printk operation */

static void console_debug_putc_onlcr(const char c)
{
  rtems_interrupt_level irq_level;
  static char cr_chr = '\r';

  if (BSP_output_chan != CONS_CHN_NONE) {
    rtems_interrupt_disable(irq_level);

    if (c == '\n') {
      sccPollWrite (BSP_output_chan,&cr_chr,1);
    }
    sccPollWrite (BSP_output_chan,&c,1);
    rtems_interrupt_enable(irq_level);
  }
}

BSP_output_char_function_type     BSP_output_char = console_debug_putc_onlcr;
BSP_polling_getchar_function_type BSP_poll_char = NULL;


/*
***************
* BOILERPLATE *
***************
*/

struct {
  rtems_device_minor_number minor;
  int driver_mode;
} channel_list[] = {
  {CONS_CHN_SMC1,CONS_SMC1_MODE},
  {CONS_CHN_SMC2,CONS_SMC2_MODE},
  {CONS_CHN_SCC1,CONS_SCC1_MODE},
  {CONS_CHN_SCC2,CONS_SCC2_MODE},
  {CONS_CHN_SCC3,CONS_SCC3_MODE},
  {CONS_CHN_SCC4,CONS_SCC4_MODE}
};


/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(rtems_device_major_number  major,
				       rtems_device_minor_number  minor,/* ignored */
				       void                      *arg
				       )
{
  rtems_status_code status = RTEMS_SUCCESSFUL;
  int chan,entry,ttynum;
  char tty_name[] = "/dev/tty00";

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();
  /*
   * init BRG allocataion
   */
  sccBRGinit();
  ttynum = 0;
  for (entry = 0;
       (entry < sizeof(channel_list)/sizeof(channel_list[0]))
	 && (status == RTEMS_SUCCESSFUL);
       entry++) {
    if (channel_list[entry].driver_mode != CONS_MODE_UNUSED) {
      /*
       * Do device-specific initialization
       */
      chan = channel_list[entry].minor;
      m8xx_scc_mode[chan] = channel_list[entry].driver_mode;
      sccInitialize (chan);

      /*
       * build device name
       */
      tty_name[sizeof(tty_name)-2] = '0'+ttynum;
      ttynum++;
      /*
       * Register the device
       */
      status = rtems_io_register_name (tty_name,
				       major,
				       channel_list[entry].minor);
      if (status != RTEMS_SUCCESSFUL) {
	rtems_fatal_error_occurred (status);
      }
    }
  }
  /*
   * register /dev/console
   */
  status = rtems_io_register_name ("/dev/console",
				   major,
				   CONSOLE_CHN);
  if (status != RTEMS_SUCCESSFUL) {
    rtems_fatal_error_occurred (status);
  }
  /*
   * enable printk support
   */
  BSP_output_chan = PRINTK_CHN;

  return RTEMS_SUCCESSFUL;
}

/*
 * Open the device
 */
rtems_device_driver console_open(
				 rtems_device_major_number major,
				 rtems_device_minor_number minor,
				 void                    * arg
				 )
{
  rtems_status_code status;
  int chan = minor;
  rtems_libio_open_close_args_t *args = (rtems_libio_open_close_args_t *)arg;
  static const rtems_termios_callbacks interruptCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    NULL,	        /* pollRead */
    sccInterruptWrite,	/* write */
    sccSetAttributes,	/* setAttributes */
    NULL,		/* stopRemoteTx */
    NULL,		/* startRemoteTx */
    TERMIOS_IRQ_DRIVEN	/* outputUsesInterrupts */
  };
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    sccPollRead,	/* pollRead */
    sccPollWrite,	/* write */
    sccSetAttributes,	/* setAttributes */
    NULL,		/* stopRemoteTx */
    NULL,		/* startRemoteTx */
    0			/* outputUsesInterrupts */
  };

  if (m8xx_scc_mode[chan] == TERMIOS_IRQ_DRIVEN) {
    status = rtems_termios_open (major, minor, arg, &interruptCallbacks);
    sccttyp[chan] = args->iop->data1;
  }
  else {
    status = rtems_termios_open (major, minor, arg, &pollCallbacks);
    sccttyp[chan] = args->iop->data1;
  }
  return status;
}

/*
 * Close the device
 */
rtems_device_driver console_close(
				  rtems_device_major_number major,
				  rtems_device_minor_number minor,
				  void                    * arg
				  )
{
  rtems_status_code rc;

  rc = rtems_termios_close (arg);
  sccttyp[minor] = NULL;

  return rc;

}

/*
 * Read from the device
 */
rtems_device_driver console_read(
				 rtems_device_major_number major,
				 rtems_device_minor_number minor,
				 void                    * arg
				 )
{
  return rtems_termios_read (arg);
}

/*
 * Write to the device
 */
rtems_device_driver console_write(
				  rtems_device_major_number major,
				  rtems_device_minor_number minor,
				  void                    * arg
				  )
{
  return rtems_termios_write (arg);
}

#if 0
static int scc_io_set_trm_char(rtems_device_minor_number minor,
			       rtems_libio_ioctl_args_t *ioa)
{
  rtems_status_code rc                = RTEMS_SUCCESSFUL;
  con360_io_trm_char_t *trm_char_info = ioa->buffer;

  /*
   * check, that parameter is non-NULL
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      (trm_char_info == NULL)) {
    rc = RTEMS_INVALID_ADDRESS;
  }
  /*
   * transfer max_idl
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (trm_char_info->max_idl >= 0x10000) {
      rc = RTEMS_INVALID_NUMBER;
    }
    else if (trm_char_info->max_idl > 0) {
      CHN_PARAM_SET(minor,un.uart.max_idl ,trm_char_info->max_idl);
    }
    else if (trm_char_info->max_idl == 0) {
      CHN_PARAM_SET(minor,un.uart.max_idl ,MAX_IDL_DEFAULT);
    }
  }
  /*
   * transfer characters
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if (trm_char_info->char_cnt > CON8XX_TRM_CHAR_CNT) {
      rc = RTEMS_TOO_MANY;
    }
    else if (trm_char_info->char_cnt >= 0) {
      /*
       * check, whether device is a SCC
       */
      if ((rc == RTEMS_SUCCESSFUL) &&
	  !m8xx_console_chan_desc[minor].is_scc) {
	rc = RTEMS_UNSATISFIED;
      }
      else {
	int idx = 0;
	for(idx = 0;idx < trm_char_info->char_cnt;idx++) {
	  m8xx_console_chan_desc[minor].parms.sccp->un.uart.character[idx] =
	    trm_char_info->character[idx] & 0x00ff;
	}
	if (trm_char_info->char_cnt < CON8XX_TRM_CHAR_CNT) {
	  m8xx_console_chan_desc[minor].parms.sccp
	    ->un.uart.character[trm_char_info->char_cnt] = 0x8000;
	}
      }
    }
  }

  return rc;
}
#endif

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
				    rtems_device_major_number major,
				    rtems_device_minor_number minor,
				    void                    * arg
				    )
{
  rtems_libio_ioctl_args_t *ioa=arg;

  switch (ioa->command) {
#if 0
  case CON8XX_IO_SET_TRM_CHAR:
    return scc_io_set_trm_char(minor, ioa);
#endif
  default:
    return rtems_termios_ioctl (arg);
    break;
  }
}

