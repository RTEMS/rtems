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
| http://www.rtems.org/license/LICENSE.                           |
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

#include <unistd.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>
#include <rtems/error.h>

#include <bsp.h>
#include <mpc8xx.h>
#include <bsp/irq.h>

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
 * I/O buffers and pointers to buffer descriptors
 */
#define SCC_RXBD_CNT 4
#define SCC_TXBD_CNT 4
typedef char sccRxBuf_t[SCC_RXBD_CNT][RXBUFSIZE];

/*
 * Interrupt-driven callback
 */
typedef struct m8xx_console_chan_desc_s {
  rtems_termios_device_context base;
  volatile m8xxBufferDescriptor_t *sccFrstRxBd;
  volatile m8xxBufferDescriptor_t *sccCurrRxBd;
  volatile m8xxBufferDescriptor_t *sccFrstTxBd;
  volatile m8xxBufferDescriptor_t *sccPrepTxBd;
  volatile m8xxBufferDescriptor_t *sccDequTxBd;
  bool is_scc;                  /* true for SCC */
  struct {
    volatile m8xxSCCparms_t *sccp;
    volatile m8xxSMCparms_t *smcp;
  } parms;
  struct {
    volatile m8xxSCCRegisters_t *sccr;
    volatile m8xxSMCRegisters_t *smcr;
  } regs;
  int chan;
  rtems_termios_device_mode mode;
  rtems_vector_number ivec_src;
  int cr_chan_code;
  int brg_used;
  sccRxBuf_t *rxBuf;
} m8xx_console_chan_desc_t;

m8xx_console_chan_desc_t m8xx_console_chan_desc[CONS_CHN_CNT] = {
  /* SCC1 */
  { .is_scc = true,
   .parms = {(m8xxSCCparms_t *)&(m8xx.scc1p),NULL},
   .regs = {&(m8xx.scc1),NULL},
   .chan = CONS_CHN_SCC1,
   .ivec_src = BSP_CPM_IRQ_SCC1,
   .cr_chan_code = M8xx_CR_CHAN_SCC1,
   .brg_used = -1},
  /* SCC2 */
  { .is_scc = true,
   .parms = {&(m8xx.scc2p),NULL},
   .regs = {&(m8xx.scc2),NULL},
   .chan = CONS_CHN_SCC2,
   .ivec_src = BSP_CPM_IRQ_SCC2,
   .cr_chan_code = M8xx_CR_CHAN_SCC2,
   .brg_used = -1},
  /* SCC3 */
  { .is_scc = true,
   .parms = {&(m8xx.scc3p),NULL},
   .regs = {&(m8xx.scc3),NULL},
   .chan = CONS_CHN_SCC3,
   .ivec_src = BSP_CPM_IRQ_SCC3,
   .cr_chan_code = M8xx_CR_CHAN_SCC3,
   .brg_used = -1},
  /* SCC4 */
  { .is_scc = true,
   .parms = {&(m8xx.scc4p),NULL},
   .regs = {&(m8xx.scc4),NULL},
   .chan = CONS_CHN_SCC4,
   .ivec_src = BSP_CPM_IRQ_SCC4,
   .cr_chan_code = M8xx_CR_CHAN_SCC4,
   .brg_used = -1},
  /* SMC1 */
  { .is_scc = false,
   .parms = {NULL,&(m8xx.smc1p)},
   .regs = {NULL,&(m8xx.smc1)},
   .chan = CONS_CHN_SMC1,
   .ivec_src = BSP_CPM_IRQ_SMC1,
   .cr_chan_code = M8xx_CR_CHAN_SMC1,
   .brg_used = -1},
  /* SMC2 */
  { .is_scc = false,
   .parms = {NULL,&(m8xx.smc2p)},
   .regs = {NULL,&(m8xx.smc2)},
   .chan = CONS_CHN_SMC2,
   .ivec_src = BSP_CPM_IRQ_SMC2_OR_PIP,
   .cr_chan_code = M8xx_CR_CHAN_SMC2,
   .brg_used = -1}};

#define CHN_PARAM_GET(cd,param)	\
  (cd->is_scc			\
   ? cd->parms.sccp->param	\
   : cd->parms.smcp->param)

#define CHN_PARAM_SET(cd,param,value)	\
  do {if (cd->is_scc)			\
      cd->parms.sccp->param = value;	\
    else				\
      cd->parms.smcp->param = value;	\
  } while (0)

#define CHN_EVENT_GET(cd)	\
  (cd->is_scc			\
   ? cd->regs.sccr->scce	\
   : cd->regs.smcr->smce)

#define CHN_EVENT_CLR(cd,mask)		\
  do {					\
    if (cd->is_scc) 			\
      cd->regs.sccr->scce = (mask);	\
    else				\
      cd->regs.smcr->smce = (mask);	\
  }while (0)

#define CHN_MASK_GET(cd)	\
  (cd->is_scc			\
   ? cd->regs.sccr->sccm	\
   : cd->regs.smcr->smcm)

#define CHN_MASK_SET(cd,mask) 		\
  do {					\
    if (cd->is_scc) 			\
      cd->regs.sccr->sccm = (mask);	\
    else				\
      cd->regs.smcr->smcm = (mask);	\
  }while (0)

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

static bool sccBRGalloc(m8xx_console_chan_desc_t *cd,int baud)
{
  rtems_interrupt_level level;
  uint32_t reg_val;
  int old_brg;
  int new_brg = -1;
  int brg_idx;
#if CONS_USE_EXT_CLK
  int clk_group;
  int clk_sel;
#endif

  old_brg = cd->brg_used;
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
    cd->brg_used = new_brg;
    /*
     * Put SCC in NMSI mode, connect SCC to BRG or CLKx
     */
    if (cd->is_scc) {
      m8xx.sicr = ((m8xx.sicr & ~(M8xx_SICR_SCCRX_MSK(cd->chan) |
				  M8xx_SICR_SCCTX_MSK(cd->chan))) |
		   M8xx_SICR_SCCRX(cd->chan,new_brg)|
		   M8xx_SICR_SCCTX(cd->chan,new_brg));
    }
    else {
      /* connect SMC to BRGx or CLKx... */
      m8xx.simode = ((m8xx.simode & ~(M8xx_SIMODE_SMCCS_MSK(cd->chan - CONS_CHN_SMC1)))|
		     M8xx_SIMODE_SMCCS(cd->chan - CONS_CHN_SMC1,new_brg));
    }

    return true;
  } else {
    return false;
  }
}


/*
 * Hardware-dependent portion of tcsetattr().
 */
static bool
sccSetAttributes (rtems_termios_device_context *base, const struct termios *t)
{
  m8xx_console_chan_desc_t *cd = (m8xx_console_chan_desc_t *)base;
  speed_t speed;
  rtems_termios_baud_t baud;

  speed = cfgetispeed(t);
  if (speed == B0) {
        speed = cfgetospeed(t);
  }

  baud = rtems_termios_baud_to_number(speed);
  if (baud == 0) {
    return false;
  }

  return sccBRGalloc(cd,baud);
}

/*
 * Interrupt handler
 */
static rtems_isr
sccInterruptHandler (void *arg)
{
  rtems_termios_tty *tty = arg;
  m8xx_console_chan_desc_t *cd = rtems_termios_get_device_context(tty);
  uint16_t status;
  uint16_t length;
  void *buffer;

  /*
   * Buffer received?
   */
  if (CHN_EVENT_GET(cd) & 0x1) {
    /*
     * clear SCC event flag
     */
    CHN_EVENT_CLR(cd,0x01);
    /*
     * process event
     */
    while (true) {
      status = cd->sccCurrRxBd->status;

      if ((cd->sccCurrRxBd->status & M8xx_BD_EMPTY) != 0) {
        break;
      }

      buffer = cd->sccCurrRxBd->buffer;
      length = cd->sccCurrRxBd->length;
      rtems_cache_invalidate_multiple_data_lines(buffer, length);
      rtems_termios_enqueue_raw_characters (tty, buffer, length);

      /*
       * clear status
       */
      cd->sccCurrRxBd->status = (status & (M8xx_BD_WRAP | M8xx_BD_INTERRUPT))
        | M8xx_BD_EMPTY;
      /*
       * advance to next BD
       */
      if ((status & M8xx_BD_WRAP) != 0) {
        cd->sccCurrRxBd = cd->sccFrstRxBd;
      } else {
        cd->sccCurrRxBd++;
      }
    }
  }
  /*
   * Buffer transmitted?
   */
  if (CHN_EVENT_GET(cd) & 0x2) {
    /*
     * then clear interrupt event bit
     */
    CHN_EVENT_CLR(cd,0x2);
    /*
     * and signal successful transmit to termios
     */
    /*
     * FIXME: multiple dequeue calls for multiple buffers
     */
    while (cd->sccDequTxBd != cd->sccPrepTxBd) {
      status = cd->sccDequTxBd->status;

      if ((status & M8xx_BD_READY) != 0) {
        break;
      }

      if ((status & M8xx_BD_INTERRUPT) != 0) {
        rtems_termios_dequeue_characters (tty, cd->sccDequTxBd->length);
      }

      /*
       * advance to next BD
       */
      if ((status & M8xx_BD_WRAP) != 0) {
        cd->sccDequTxBd = cd->sccFrstTxBd;
      } else {
        cd->sccDequTxBd++;
      }
    }
  }
}

static void
mpc8xx_console_irq_on(m8xx_console_chan_desc_t *cd)
{
    CHN_MASK_SET(cd, 3);	/* Enable TX and RX interrupts */
}

static void
sccInitialize (m8xx_console_chan_desc_t *cd)
{
  int i;
  /*
   * allocate buffers
   * FIXME: use a cache-line size boundary alloc here
   */
  cd->rxBuf = rtems_cache_aligned_malloc(sizeof(*cd->rxBuf));
  if (cd->rxBuf == NULL) {
    rtems_panic("Cannot allocate console rx buffer\n");
  }

  /*
   * Allocate buffer descriptors
   */
  cd->sccCurrRxBd =
    cd->sccFrstRxBd = m8xx_bd_allocate(SCC_RXBD_CNT);
  cd->sccPrepTxBd =
    cd->sccDequTxBd =
    cd->sccFrstTxBd = m8xx_bd_allocate(SCC_TXBD_CNT);
  switch(cd->chan) {
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
  sccBRGalloc(cd,9600);


  /*
   * Set up SCCx parameter RAM common to all protocols
   */
  CHN_PARAM_SET(cd,rbase,(char *)cd->sccFrstRxBd - (char *)&m8xx);
  CHN_PARAM_SET(cd,tbase,(char *)cd->sccFrstTxBd - (char *)&m8xx);
  CHN_PARAM_SET(cd,rfcr ,M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0));
  CHN_PARAM_SET(cd,tfcr ,M8xx_TFCR_MOT | M8xx_TFCR_DMA_SPACE(0));
  if (cd->mode != TERMIOS_POLLED)
    CHN_PARAM_SET(cd,mrblr,RXBUFSIZE);
  else
    CHN_PARAM_SET(cd,mrblr,1);

  /*
   * Set up SCCx parameter RAM UART-specific parameters
   */
  CHN_PARAM_SET(cd,un.uart.max_idl ,MAX_IDL_DEFAULT);
  CHN_PARAM_SET(cd,un.uart.brkln   ,0);
  CHN_PARAM_SET(cd,un.uart.brkec   ,0);
  CHN_PARAM_SET(cd,un.uart.brkcr   ,0);
  if (cd->is_scc) {
    cd->parms.sccp->un.uart.character[0]=0x8000; /* no char filter */
    cd->parms.sccp->un.uart.rccm=0x80FF; /* control character mask */
  }

  /*
   * Set up the Receive Buffer Descriptors
   */
  for (i = 0;i < SCC_RXBD_CNT;i++) {
    cd->sccFrstRxBd[i].status = M8xx_BD_EMPTY | M8xx_BD_INTERRUPT;
    if (i == SCC_RXBD_CNT-1) {
      cd->sccFrstRxBd[i].status |= M8xx_BD_WRAP;
    }
    cd->sccFrstRxBd[i].length = 0;
    cd->sccFrstRxBd[i].buffer = (*cd->rxBuf)[i];
  }
  /*
   * Setup the Transmit Buffer Descriptor
   */
  for (i = 0;i < SCC_TXBD_CNT;i++) {
    cd->sccFrstTxBd[i].status = M8xx_BD_INTERRUPT;
    if (i == SCC_TXBD_CNT-1) {
      cd->sccFrstTxBd[i].status |= M8xx_BD_WRAP;
    }
    cd->sccFrstTxBd[i].length = 0;
    cd->sccFrstTxBd[i].buffer = NULL;
  }

  /*
   * Set up SCC general and protocol-specific mode registers
   */
  CHN_EVENT_CLR(cd,~0);	/* Clear any pending events */
  CHN_MASK_SET(cd,0);	        /* Mask all interrupt/event sources */

  if (cd->is_scc) {
    cd->regs.sccr->psmr = 0xb000; /* 8N1, CTS flow control */
    cd->regs.sccr->gsmr_h = 0x00000000;
    cd->regs.sccr->gsmr_l = 0x00028004; /* UART mode */
  }
  else {
    cd->regs.smcr->smcmr = 0x4820;
  }
  /*
   * Send "Init parameters" command
   */
  m8xx_cp_execute_cmd(M8xx_CR_OP_INIT_RX_TX | cd->cr_chan_code);

  /*
   * Enable receiver and transmitter
   */
  if (cd->is_scc) {
    cd->regs.sccr->gsmr_l |= 0x00000030;
  }
  else {
    cd->regs.smcr->smcmr |= 0x0003;
  }
}

/*
 * polled scc read function
 */
static int
sccPollRead (rtems_termios_device_context *base)
{
  m8xx_console_chan_desc_t *cd = (m8xx_console_chan_desc_t *)base;
  int c = -1;

  while(1) {
    if ((cd->sccCurrRxBd->status & M8xx_BD_EMPTY) != 0) {
      return -1;
    }

    if (0 == (cd->sccCurrRxBd->status & (M8xx_BD_OVERRUN
					   | M8xx_BD_PARITY_ERROR
					   | M8xx_BD_FRAMING_ERROR
					   | M8xx_BD_BREAK
					   | M8xx_BD_IDLE))) {
      /* character received and no error detected */
      rtems_cache_invalidate_multiple_data_lines((void *)cd->sccCurrRxBd->buffer,
						 cd->sccCurrRxBd->length);
      c = (unsigned)*((char *)cd->sccCurrRxBd->buffer);
      /*
       * clear status
       */
    }
    cd->sccCurrRxBd->status =
      (cd->sccCurrRxBd->status
       & (M8xx_BD_WRAP | M8xx_BD_INTERRUPT))
      | M8xx_BD_EMPTY;
    /*
     * advance to next BD
     */
    if ((cd->sccCurrRxBd->status & M8xx_BD_WRAP) != 0) {
      cd->sccCurrRxBd = cd->sccFrstRxBd;
    }
    else {
      cd->sccCurrRxBd++;
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
static void
sccInterruptWrite (rtems_termios_device_context *base, const char *buf, size_t len)
{
  m8xx_console_chan_desc_t *cd = (m8xx_console_chan_desc_t *)base;

  if (len > 0) {
    uint16_t status = cd->sccPrepTxBd->status;

    if ((status & M8xx_BD_READY) == 0) {
      cd->sccPrepTxBd->buffer = RTEMS_DECONST(char*, buf);
      cd->sccPrepTxBd->length = len;
      rtems_cache_flush_multiple_data_lines(buf, len);

      /*
       * clear status, set ready bit
       */
      cd->sccPrepTxBd->status = (status & M8xx_BD_WRAP)
        | M8xx_BD_READY | M8xx_BD_INTERRUPT;

      if ((status & M8xx_BD_WRAP) != 0) {
        cd->sccPrepTxBd = cd->sccFrstTxBd;
      } else {
        cd->sccPrepTxBd++;
      }
    }
  }
}

static void
sccPollWrite (rtems_termios_device_context *base, const char *buf, size_t len)
{
  m8xx_console_chan_desc_t *cd = (m8xx_console_chan_desc_t *)base;
  volatile m8xxBufferDescriptor_t *bd = NULL;

  rtems_cache_flush_multiple_data_lines (buf, len);

  while (bd == NULL) {
    rtems_interrupt_level level;
    uint16_t status;

    rtems_interrupt_disable(level);

    bd = cd->sccPrepTxBd;
    status = bd->status;

    if ((status & M8xx_BD_READY) == 0) {
      bd->buffer = RTEMS_DECONST (char *, buf);
      bd->length = len;
      bd->status = (status & M8xx_BD_WRAP) | M8xx_BD_READY;

      if ((status & M8xx_BD_WRAP) != 0) {
        cd->sccPrepTxBd = cd->sccFrstTxBd;
      } else {
        cd->sccPrepTxBd++;
      }
    } else {
      bd = NULL;
    }

    rtems_interrupt_enable(level);
  }

  while ((bd->status & M8xx_BD_READY) != 0) {
    /* Wait */
  }
}

/*
 * printk basic support
 */
int BSP_output_chan = CONS_CHN_NONE; /* channel used for printk operation */

static void console_debug_putc_onlcr(const char c)
{
  rtems_interrupt_level irq_level;

  if (BSP_output_chan != CONS_CHN_NONE) {
    rtems_interrupt_disable(irq_level);

    sccPollWrite (&m8xx_console_chan_desc[BSP_output_chan].base,&c,1);
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

static bool m8xx_console_first_open(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  m8xx_console_chan_desc_t *cd = (m8xx_console_chan_desc_t *)base;

  if (cd->mode == TERMIOS_IRQ_DRIVEN) {
    rtems_status_code sc;

    sc = rtems_interrupt_handler_install(
      cd->ivec_src,
      "SCC",
      RTEMS_INTERRUPT_UNIQUE,
      sccInterruptHandler,
      tty
    );
    if (sc != RTEMS_SUCCESSFUL) {
      return false;
    }

    mpc8xx_console_irq_on(cd);
  }

  return true;
}

static const rtems_termios_device_handler m8xx_console_handler_polled = {
  .first_open = m8xx_console_first_open,
  .set_attributes = sccSetAttributes,
  .write = sccPollWrite,
  .poll_read = sccPollRead,
  .mode = TERMIOS_POLLED
};

static const rtems_termios_device_handler m8xx_console_handler_irq_driven = {
  .first_open = m8xx_console_first_open,
  .set_attributes = sccSetAttributes,
  .write = sccInterruptWrite,
  .mode = TERMIOS_IRQ_DRIVEN
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
  int entry,ttynum;
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
      m8xx_console_chan_desc_t *cd =
	&m8xx_console_chan_desc[channel_list[entry].minor];
      /*
       * Do device-specific initialization
       */
      cd->mode = channel_list[entry].driver_mode;
      sccInitialize (cd);

      /*
       * build device name
       */
      tty_name[sizeof(tty_name)-2] = '0'+ttynum;
      ttynum++;
      /*
       * Register the device
       */
      status = rtems_termios_device_install(
	tty_name,
	cd->mode == TERMIOS_IRQ_DRIVEN ?
	  &m8xx_console_handler_irq_driven : &m8xx_console_handler_polled,
	NULL,
	&cd->base
      );
      if (status != RTEMS_SUCCESSFUL) {
	rtems_fatal_error_occurred (status);
      }

      if (cd->chan == CONSOLE_CHN) {
	int rv;

	rv = link(tty_name, CONSOLE_DEVICE_NAME);
	if (rv != 0) {
	  rtems_fatal_error_occurred (RTEMS_IO_ERROR);
	}
      }
    }
  }

  /*
   * enable printk support
   */
  BSP_output_chan = PRINTK_CHN;

  return RTEMS_SUCCESSFUL;
}
