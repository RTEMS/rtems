/*
 * Generic UART Serial driver for SH-4 processors
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russian Fed.
 * Author: Alexandra Kossovsky <sasha@oktet.ru>
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <termios.h>
#include <rtems/libio.h>
#include <bsp.h>
#include "sh/sh4uart.h"

#ifndef SH4_UART_INTERRUPT_LEVEL
#define SH4_UART_INTERRUPT_LEVEL 4
#endif

/* Forward function declarations */
static rtems_isr
sh4uart1_interrupt_transmit(rtems_vector_number vec);
static rtems_isr
sh4uart1_interrupt_receive(rtems_vector_number vec);
static rtems_isr
sh4uart2_interrupt_transmit(rtems_vector_number vec);
static rtems_isr
sh4uart2_interrupt_receive(rtems_vector_number vec);

/*
 * sh4uart_init --
 * This function verifies the input parameters and perform initialization
 *     of the SH-4 on-chip UART descriptor structure.
 *
 * PARAMETERS:
 *     uart - pointer to the UART channel descriptor structure
 *     tty - pointer to termios structure
 *     chn - channel number (SH4_SCI/SH4_SCIF -- 1/2)
 *     int_driven - interrupt-driven (1) or polled (0) I/O mode
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if all parameters are valid, or error code
 */
rtems_status_code
sh4uart_init(sh4uart *uart, void *tty, int chn, int int_driven)
{
  if (uart == NULL)
    return RTEMS_INVALID_ADDRESS;

  if ((chn != SH4_SCI) && (chn != SH4_SCIF))
    return RTEMS_INVALID_NUMBER;

  uart->chn = chn;
  uart->tty = tty;
  uart->int_driven = int_driven;

#if 0
  sh4uart_poll_write(uart, "init", 4);
#endif
  return RTEMS_SUCCESSFUL;
}

/*
 * sh4uart_get_Pph --
 *    Get current peripheral module clock.
 *
 * PARAMETERS: none;
 *    Cpu clock is get from CPU_CLOCK_RATE_HZ marco
 *    (defined in bspopts.h, included from bsp.h)
 *
 * RETURNS:
 *    peripheral module clock in Hz.
 */
uint32_t
sh4uart_get_Pph(void)
{
  uint16_t   frqcr = *(volatile uint16_t*)SH7750_FRQCR;
  uint32_t   Pph = CPU_CLOCK_RATE_HZ;

  switch (frqcr & SH7750_FRQCR_IFC) {
    case SH7750_FRQCR_IFCDIV1: break;
    case SH7750_FRQCR_IFCDIV2: Pph *= 2; break;
    case SH7750_FRQCR_IFCDIV3: Pph *= 3; break;
    case SH7750_FRQCR_IFCDIV4: Pph *= 4; break;
    case SH7750_FRQCR_IFCDIV6: Pph *= 6; break;
    case SH7750_FRQCR_IFCDIV8: Pph *= 8; break;
    default: /* unreachable */
      break;
  }

  switch (frqcr & SH7750_FRQCR_PFC) {
    case SH7750_FRQCR_PFCDIV2: Pph /= 2; break;
    case SH7750_FRQCR_PFCDIV3: Pph /= 3; break;
    case SH7750_FRQCR_PFCDIV4: Pph /= 4; break;
    case SH7750_FRQCR_PFCDIV6: Pph /= 6; break;
    case SH7750_FRQCR_PFCDIV8: Pph /= 8; break;
    default: /* unreachable */
      break;
  }

  return Pph;
}

/*
 * sh4uart_set_baudrate --
 *     Program the UART timer to specified baudrate
 *
 * PARAMETERS:
 *     uart - pointer to UART descriptor structure
 *     baud - termios baud rate (B50, B9600, etc...)
 *
 * ALGORITHM:
 *     see SH7750 Hardware Manual.
 *
 * RETURNS:
 *     none
 */
static void
sh4uart_set_baudrate(sh4uart *uart, speed_t baud)
{
  uint32_t   rate;
  int16_t   div;
  int n;
  uint32_t   Pph = sh4uart_get_Pph();

  switch (baud) {
    case B50:     rate = 50; break;
    case B75:     rate = 75; break;
    case B110:    rate = 110; break;
    case B134:    rate = 134; break;
    case B150:    rate = 150; break;
    case B200:    rate = 200; break;
    case B300:    rate = 300; break;
    case B600:    rate = 600; break;
    case B1200:   rate = 1200; break;
    case B2400:   rate = 2400; break;
    case B4800:   rate = 4800; break;
    case B9600:   rate = 9600; break;
    case B19200:  rate = 19200; break;
    case B38400:  rate = 38400; break;
    case B57600:  rate = 57600; break;
#ifdef B115200
    case B115200: rate = 115200; break;
#endif
#ifdef B230400
    case B230400: rate = 230400; break;
#endif
    default:      rate = 9600; break;
  }

  for (n = 0; n < 4; n++) {
    div = Pph / (32 * (1 << (2 * n)) * rate) - 1;
    if (div < 0x100)
      break;
  }

  /* Set default baudrate if specified baudrate is impossible */
  if (n >= 4)
    sh4uart_set_baudrate(uart, B9600);

  if ( uart->chn == 1 ) {
    volatile uint8_t *smr1 = (volatile uint8_t *)SH7750_SCSMR1;
    *smr1 &= ~SH7750_SCSMR_CKS;
    *smr1 |= n << SH7750_SCSMR_CKS_S;
  } else {
    volatile uint16_t *smr2 = (volatile uint16_t *)SH7750_SCSMR2;
    *smr2 &= ~SH7750_SCSMR_CKS;
    *smr2 |= n << SH7750_SCSMR_CKS_S;
  }

  SCBRR(uart->chn) = div;
  /* Wait at least 1 bit interwal */
  rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(1000 / rate));
}

/*
 * sh4uart_reset --
 *     This function perform the hardware initialization of SH-4
 *     on-chip UART controller using parameters
 *     filled by the sh4uart_init function.
 *
 * PARAMETERS:
 *     uart - pointer to UART channel descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if channel is initialized successfully, error
 *     code in other case
 */
rtems_status_code
sh4uart_reset(sh4uart *uart)
{
  register int chn;
  register int int_driven;
  rtems_status_code rc;
  uint16_t tmp;

  if (uart == NULL)
    return RTEMS_INVALID_ADDRESS;

  chn = uart->chn;
  int_driven = uart->int_driven;

  if ( chn == 1 ) {
    volatile uint8_t *scr1 = (volatile uint8_t *)SH7750_SCSCR1;
    volatile uint8_t *smr1 = (volatile uint8_t *)SH7750_SCSMR1;
    *scr1 = 0x0;       /* Is set properly at the end of this function */
    *smr1 = 0x0;       /* 8-bit, non-parity, 1 stop bit, pf/1 clock */
  } else {
    volatile uint16_t *scr2 = (volatile uint16_t *)SH7750_SCSCR2;
    volatile uint16_t *smr2 = (volatile uint16_t *)SH7750_SCSMR2;
    *scr2 = 0x0;       /* Is set properly at the end of this function */
    *smr2 = 0x0;       /* 8-bit, non-parity, 1 stop bit, pf/1 clock */
  }

  if (chn == SH4_SCIF)
    SCFCR2 = SH7750_SCFCR2_TFRST | SH7750_SCFCR2_RFRST |
             SH7750_SCFCR2_RTRG_1 | SH7750_SCFCR2_TTRG_4;

  if (chn == SH4_SCI)
    SCSPTR1 = int_driven ? 0x0 : SH7750_SCSPTR1_EIO;
  else
    SCSPTR2 = SH7750_SCSPTR2_RTSDT;

  if (int_driven) {
    uint16_t   ipr;

    if (chn == SH4_SCI) {
      ipr = IPRB;
      ipr &= ~SH7750_IPRB_SCI1;
      ipr |= SH4_UART_INTERRUPT_LEVEL << SH7750_IPRB_SCI1_S;
      IPRB = ipr;

      rc = rtems_interrupt_catch(sh4uart1_interrupt_transmit,
                                 SH7750_EVT_TO_NUM(SH7750_EVT_SCI_TXI),
                                 &uart->old_handler_transmit);
      if (rc != RTEMS_SUCCESSFUL)
        return rc;

      rc = rtems_interrupt_catch(sh4uart1_interrupt_receive,
                                 SH7750_EVT_TO_NUM(SH7750_EVT_SCI_RXI),
                                 &uart->old_handler_receive);
      if (rc != RTEMS_SUCCESSFUL)
        return rc;
    } else {
      ipr = IPRC;
      ipr &= ~SH7750_IPRC_SCIF;
      ipr |= SH4_UART_INTERRUPT_LEVEL << SH7750_IPRC_SCIF_S;
      IPRC = ipr;

      rc = rtems_interrupt_catch(sh4uart2_interrupt_transmit,
                                 SH7750_EVT_TO_NUM(SH7750_EVT_SCIF_TXI),
                                 &uart->old_handler_transmit);
      if (rc != RTEMS_SUCCESSFUL)
        return rc;
      rc = rtems_interrupt_catch(sh4uart2_interrupt_receive,
                                 SH7750_EVT_TO_NUM(SH7750_EVT_SCIF_RXI),
                                 &uart->old_handler_receive);
      if (rc != RTEMS_SUCCESSFUL)
        return rc;
    }
    uart->tx_buf = NULL;
    uart->tx_ptr = uart->tx_buf_len = 0;
  }

  sh4uart_set_baudrate(uart, B38400); /* debug defaults (unfortunately,
                                         it is differ to termios default */

  tmp = SH7750_SCSCR_TE | SH7750_SCSCR_RE |
          (chn == SH4_SCI ? 0x0 : SH7750_SCSCR2_REIE) |
          (int_driven ? (SH7750_SCSCR_RIE | SH7750_SCSCR_TIE) : 0x0);

  if ( chn == 1 ) {
    volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
    *scr = tmp;
  } else {
    volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
    *scr = tmp;
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * sh4uart_disable --
 *     This function disable the operations on SH-4 UART controller
 *
 * PARAMETERS:
 *     uart - pointer to UART channel descriptor structure
 *     disable_port - disable receive and transmit on the port
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL if UART closed successfuly, or error code in
 *     other case
 */
rtems_status_code
sh4uart_disable(sh4uart *uart, int disable_port)
{
  rtems_status_code rc;

  if (disable_port) {
    if ( uart->chn == 1 ) {
      volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
      *scr &= ~(SH7750_SCSCR_TE | SH7750_SCSCR_RE);
    } else {
      volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
      *scr &= ~(SH7750_SCSCR_TE | SH7750_SCSCR_RE);
    }
  }

  if (uart->int_driven) {
    rc = rtems_interrupt_catch(uart->old_handler_transmit,
           uart->chn == SH4_SCI ?  SH7750_EVT_SCI_TXI : SH7750_EVT_SCIF_TXI,
           NULL);
    if (rc != RTEMS_SUCCESSFUL)
      return rc;
    rc = rtems_interrupt_catch(uart->old_handler_receive,
           uart->chn == SH4_SCI ?  SH7750_EVT_SCI_RXI : SH7750_EVT_SCIF_RXI,
           NULL);
    if (rc != RTEMS_SUCCESSFUL)
      return rc;
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * sh4uart_set_attributes --
 *     This function parse the termios attributes structure and perform
 *     the appropriate settings in hardware.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     t - pointer to termios parameters
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
sh4uart_set_attributes(sh4uart *uart, const struct termios *t)
{
  int level;
  speed_t baud;
  uint16_t   smr;

  smr = (uint16_t)(*(uint8_t*)SH7750_SCSMR(uart->chn));

  baud = cfgetospeed(t);

  /* Set flow control XXX*/
  if ((t->c_cflag & CRTSCTS) != 0) {
  }

  /* Set character size -- only 7 or 8 bit */
  switch (t->c_cflag & CSIZE) {
    case CS5:
    case CS6:
    case CS7: smr |= SH7750_SCSMR_CHR_7; break;
    case CS8: smr &= ~SH7750_SCSMR_CHR_7; break;
  }

    /* Set number of stop bits */
  if ((t->c_cflag & CSTOPB) != 0)
    smr |= SH7750_SCSMR_STOP_2;
  else
    smr &= ~SH7750_SCSMR_STOP_2;

  /* Set parity mode */
  if ((t->c_cflag & PARENB) != 0) {
    smr |= SH7750_SCSMR_PE;
    if ((t->c_cflag & PARODD) != 0)
       smr |= SH7750_SCSMR_PM_ODD;
    else
       smr &= ~SH7750_SCSMR_PM_ODD;
  } else
    smr &= ~SH7750_SCSMR_PE;

  rtems_interrupt_disable(level);
  /* wait untill all data is transmitted */
  /* XXX JOEL says this is broken -- interrupts are OFF so NO ticks  */
  rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(100));

  if ( uart->chn == 1 ) {
    volatile uint8_t *scrP = (volatile uint8_t *)SH7750_SCSCR1;
    volatile uint8_t *smrP = (volatile uint8_t *)SH7750_SCSMR1;

    *scrP &= ~(SH7750_SCSCR_TE | SH7750_SCSCR_RE); /* disable operations */
    sh4uart_set_baudrate(uart, baud);
    *smrP = (uint8_t)smr;
    *scrP |= SH7750_SCSCR_TE | SH7750_SCSCR_RE;    /* enable operations */
  } else {
    volatile uint16_t *scrP = (volatile uint16_t *)SH7750_SCSCR2;
    volatile uint16_t *smrP = (volatile uint16_t *)SH7750_SCSMR2;

    *scrP &= ~(SH7750_SCSCR_TE | SH7750_SCSCR_RE); /* disable operations */
    sh4uart_set_baudrate(uart, baud);
    *smrP = (uint8_t)smr;
    *scrP |= SH7750_SCSCR_TE | SH7750_SCSCR_RE;    /* enable operations */
  }

  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

/*
 * sh4uart_handle_error --
 *     Perfoms error (Overrun, Framing & Parity) handling
 *
 * PARAMETERS:
 *     uart - pointer to UART descriptor structure
 *
 * RETURNS:
 *     nothing
 */
void
sh4uart_handle_error(sh4uart *uart)
{
  if (uart->chn == SH4_SCI) {
    volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
    *scr &= ~(SH7750_SCSSR1_ORER | SH7750_SCSSR1_FER | SH7750_SCSSR1_PER);
  } else {
    volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
    *scr &= ~(SH7750_SCSSR2_ER | SH7750_SCSSR2_BRK | SH7750_SCSSR2_FER);
    *scr &= ~(SH7750_SCLSR2_ORER);
  }
}

/*
 * sh4uart_poll_read --
 *     This function tried to read character from SH-4 UART and perform
 *     error handling. When parity or framing error occured, return
 *     value dependent on termios input mode flags:
 *         - received character, if IGNPAR == 1
 *         - 0, if IGNPAR == 0 and PARMRK == 0
 *         - 0xff and 0x00 on next poll_read invocation, if IGNPAR == 0 and
 *           PARMRK == 1
 *
 * PARAMETERS:
 *     uart - pointer to UART descriptor structure
 *
 * RETURNS:
 *     code of received character or -1 if no characters received.
 */
int
sh4uart_poll_read(sh4uart *uart)
{
  int chn = uart->chn;
  int error_occured = 0;
  int parity_error = 0;
  int break_occured = 0;
  int ch;

  if (uart->parerr_mark_flag == true) {
    uart->parerr_mark_flag = false;
    return 0;
  }

  if (chn == SH4_SCI) {
    if ((SCSSR1 & (SH7750_SCSSR1_PER | SH7750_SCSSR1_FER |
                 SH7750_SCSSR1_ORER)) != 0) {
      error_occured = 1;
      if (SCSSR1 & (SH7750_SCSSR1_PER | SH7750_SCSSR1_FER))
        parity_error = 1;
      sh4uart_handle_error(uart);
    }
    if ((SCSSR1 & SH7750_SCSSR1_RDRF) == 0)
      return -1;
  } else {
    if ((SCSSR2 & (SH7750_SCSSR2_ER | SH7750_SCSSR2_DR |
                 SH7750_SCSSR2_BRK)) != 0 ||
            (SCLSR2 & SH7750_SCLSR2_ORER) != 0) {
      error_occured = 1;
      if (SCSSR2 & (SH7750_SCSSR1_PER | SH7750_SCSSR1_FER))
        parity_error = 1;
      if (SCSSR2 & SH7750_SCSSR2_BRK)
        break_occured = 1;
      sh4uart_handle_error(uart);
    }
    if ((SCSSR2 & SH7750_SCSSR2_RDF) == 0)
      return -1;
  }

  if (parity_error && !(uart->c_iflag & IGNPAR)) {
    if (uart->c_iflag & PARMRK) {
      uart->parerr_mark_flag = true;
      return 0xff;
    } else
      return 0;
  }

  if (break_occured && !(uart->c_iflag & BRKINT)) {
    if (uart->c_iflag & IGNBRK)
      return 0;
    else
      return 0;   /* XXX -- SIGINT */
  }

  ch = SCRDR(chn);

  if (uart->chn == SH4_SCI) {
    volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
    *scr &= ~SH7750_SCSSR1_RDRF;
  } else {
    volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
    *scr &= ~SH7750_SCSSR2_RDF;
  }

  return ch;
}

/*
 * sh4uart_poll_write --
 *     This function transmit buffer byte-by-byte in polling mode.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     buf - pointer to transmit buffer
 *     len - transmit buffer length
 *
 * RETURNS:
 *     0
 */
int
sh4uart_poll_write(sh4uart *uart, const char *buf, int len)
{
  volatile uint8_t *ssr1 = (volatile uint8_t *)SH7750_SCSSR1;
  volatile uint16_t *ssr2 = (volatile uint16_t *)SH7750_SCSSR2;

  while (len) {
    if (uart->chn == SH4_SCI) {
      while ((SCSSR1 & SH7750_SCSSR1_TDRE) != 0) {
          SCTDR1 = *buf++;
          len--;
          *ssr1 &= ~SH7750_SCSSR1_TDRE;
      }
    } else {
      while ((SCSSR2 & SH7750_SCSSR2_TDFE) != 0) {
        int i;
        for (i = 0;
                i < 16 - TRANSMIT_TRIGGER_VALUE(SCFCR2 &
                    SH7750_SCFCR2_TTRG);
                i++) {
            SCTDR2 = *buf++;
            len--;
        }
        while ((SCSSR2 & SH7750_SCSSR2_TDFE) == 0 ||
                (SCSSR2 & SH7750_SCSSR2_TEND) == 0);
            *ssr2 &= ~(SH7750_SCSSR1_TDRE | SH7750_SCSSR2_TEND);
      }
    }
  }
  return 0;
}

/**********************************
 * Functions to handle interrupts *
 **********************************/
/* sh4uart1_interrupt_receive --
 *     UART interrupt handler routine -- SCI
 *     Receiving data
 *
 * PARAMETERS:
 *     vec - interrupt vector number
 *
 * RETURNS:
 *     none
 */
static rtems_isr
sh4uart1_interrupt_receive(rtems_vector_number vec)
{
  register int bp = 0;
  char buf[32];
  volatile uint8_t *ssr1 = (volatile uint8_t *)SH7750_SCSSR1;


  /* Find UART descriptor from vector number */
  sh4uart *uart = &sh4_uarts[0];

  while (1) {
    if ((bp < sizeof(buf) - 1) && ((SCSSR1 & SH7750_SCSSR1_RDRF) != 0)) {
      /* Receive character and handle frame/parity errors */
      if ((SCSSR1 & (SH7750_SCSSR1_PER | SH7750_SCSSR1_FER |
                      SH7750_SCSSR1_ORER)) != 0) {
        if (SCSSR1 & (SH7750_SCSSR1_PER | SH7750_SCSSR1_FER)) {
            if (!(uart->c_iflag & IGNPAR)) {
              if (uart->c_iflag & PARMRK) {
                buf[bp++] = 0xff;
                buf[bp++] = 0x00;
              } else
                buf[bp++] = 0x00;
            } else
              buf[bp++] = SCRDR1;
          }
          sh4uart_handle_error(uart);
      } else
          buf[bp++] = SCRDR1;
      *ssr1 &= ~SH7750_SCSSR1_RDRF;
    } else {
      if (bp != 0)
        rtems_termios_enqueue_raw_characters(uart->tty, buf, bp);
      break;
    }
  }
}

/* sh4uart2_interrupt_receive --
 *     UART interrupt handler routine -- SCIF
 *     Receiving data
 *
 * PARAMETERS:
 *     vec - interrupt vector number
 *
 * RETURNS:
 *     none
 */
static rtems_isr
sh4uart2_interrupt_receive(rtems_vector_number vec)
{
  register int bp = 0;
  char buf[32];
  volatile uint16_t *ssr2 = (volatile uint16_t *)SH7750_SCSSR2;


  /* Find UART descriptor from vector number */
  sh4uart *uart = &sh4_uarts[1];

  while (1) {
    if ((bp < sizeof(buf) - 1) && ((SCSSR2 & SH7750_SCSSR2_RDF) != 0)) {
      if ((SCSSR2 & (SH7750_SCSSR2_ER | SH7750_SCSSR2_DR |
                      SH7750_SCSSR2_BRK)) != 0 ||
              (SH7750_SCLSR2 & SH7750_SCLSR2_ORER) != 0) {
        if (SCSSR2 & SH7750_SCSSR2_ER) {
          if (!(uart->c_iflag & IGNPAR)) {
            if (uart->c_iflag & PARMRK) {
              buf[bp++] = 0xff;
              buf[bp++] = 0x00;
            } else
              buf[bp++] = 0x00;
          } else
              buf[bp++] = SCRDR1;
        }

        if (SCSSR2 & SH7750_SCSSR2_BRK) {
          if (uart->c_iflag & IGNBRK)
            buf[bp++] = 0x00;
          else
            buf[bp++] = 0x00;   /* XXX -- SIGINT */
        }

        sh4uart_handle_error(uart);
      } else
        buf[bp++] = SCRDR1;
      *ssr2 &= ~SH7750_SCSSR2_RDF;
    } else {
      if (bp != 0)
        rtems_termios_enqueue_raw_characters(uart->tty, buf, bp);
      break;
    }
  }
}


/* sh4uart1_interrupt_transmit --
 *     UART interrupt handler routine -- SCI
 *     It continues transmit data when old part of data is transmitted
 *
 * PARAMETERS:
 *     vec - interrupt vector number
 *
 * RETURNS:
 *     none
 */
static rtems_isr
sh4uart1_interrupt_transmit(rtems_vector_number vec)
{
  volatile uint8_t *scr1 = (volatile uint8_t *)SH7750_SCSCR1;
  volatile uint8_t *ssr1 = (volatile uint8_t *)SH7750_SCSSR1;

  /* Find UART descriptor from vector number */
  sh4uart *uart = &sh4_uarts[0];

  if (uart->tx_buf != NULL && uart->tx_ptr < uart->tx_buf_len) {
    while ((SCSSR1 & SH7750_SCSSR1_TDRE) != 0 &&
            uart->tx_ptr < uart->tx_buf_len) {
        SCTDR1 = uart->tx_buf[uart->tx_ptr++];
        *ssr1 &= ~SH7750_SCSSR1_TDRE;
    }
  } else {
    register int dequeue = uart->tx_buf_len;

    uart->tx_buf = NULL;
    uart->tx_ptr = uart->tx_buf_len = 0;

    /* Disable interrupts while we do not have any data to transmit */
    *scr1 &= ~SH7750_SCSCR_TIE;

    rtems_termios_dequeue_characters(uart->tty, dequeue);
  }
}

/* sh4uart2_interrupt_transmit --
 *     UART interrupt handler routine -- SCI
 *     It continues transmit data when old part of data is transmitted
 *
 * PARAMETERS:
 *     vec - interrupt vector number
 *
 * RETURNS:
 *     none
 */
static rtems_isr
sh4uart2_interrupt_transmit(rtems_vector_number vec)
{
  volatile uint8_t *ssr1 = (volatile uint8_t *)SH7750_SCSSR1;
  volatile uint16_t *scr2 = (volatile uint16_t *)SH7750_SCSCR2;

  /* Find UART descriptor from vector number */
  sh4uart *uart = &sh4_uarts[1];

  if (uart->tx_buf != NULL && uart->tx_ptr < uart->tx_buf_len) {
    while ((SCSSR2 & SH7750_SCSSR2_TDFE) != 0) {
        int i;
        for (i = 0;
             i < 16 - TRANSMIT_TRIGGER_VALUE(SCFCR2 & SH7750_SCFCR2_TTRG);
             i++)
          SCTDR2 = uart->tx_buf[uart->tx_ptr++];
        while ((SCSSR1 & SH7750_SCSSR1_TDRE) == 0 ||
                (SCSSR1 & SH7750_SCSSR1_TEND) == 0);
        *ssr1 &= ~(SH7750_SCSSR1_TDRE | SH7750_SCSSR2_TEND);
    }
  } else {
    register int dequeue = uart->tx_buf_len;

    uart->tx_buf = NULL;
    uart->tx_ptr = uart->tx_buf_len = 0;

    /* Disable interrupts while we do not have any data to transmit */
    *scr2 &= ~SH7750_SCSCR_TIE;

    rtems_termios_dequeue_characters(uart->tty, dequeue);
  }
}

/* sh4uart_interrupt_write --
 *     This function initiate transmitting of the buffer in interrupt mode.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *     buf - pointer to transmit buffer
 *     len - transmit buffer length
 *
 * RETURNS:
 *     0
 */
rtems_status_code
sh4uart_interrupt_write(sh4uart *uart, const char *buf, int len)
{
  volatile uint8_t  *scr1 = (volatile uint8_t *)SH7750_SCSCR1;
  volatile uint16_t *scr2 = (volatile uint16_t *)SH7750_SCSCR2;
  int level;

  while ((SCSSR1 & SH7750_SCSSR1_TEND) == 0);

  rtems_interrupt_disable(level);

  uart->tx_buf = buf;
  uart->tx_buf_len = len;
  uart->tx_ptr = 0;

  if (uart->chn == SH4_SCI)
    *scr1 |= SH7750_SCSCR_TIE;
  else
    *scr2 |= SH7750_SCSCR_TIE;

  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

/* sh4uart_stop_remote_tx --
 *     This function stop data flow from remote device.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
sh4uart_stop_remote_tx(sh4uart *uart)
{
  if ( uart->chn == 1 ) {
    volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
    *scr &= ~(SH7750_SCSCR_RIE | SH7750_SCSCR_RE);
  } else {
    volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
    *scr &= ~(SH7750_SCSCR_RIE | SH7750_SCSCR_RE);
  }

  return RTEMS_SUCCESSFUL;
}

/* sh4uart_start_remote_tx --
 *     This function resume data flow from remote device.
 *
 * PARAMETERS:
 *     uart - pointer to the UART descriptor structure
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL
 */
rtems_status_code
sh4uart_start_remote_tx(sh4uart *uart)
{
  if ( uart->chn == 1 ) {
    volatile uint8_t *scr = (volatile uint8_t *)SH7750_SCSCR1;
    *scr |= SH7750_SCSCR_RIE | SH7750_SCSCR_RE;
  } else {
    volatile uint16_t *scr = (volatile uint16_t *)SH7750_SCSCR2;
    *scr |= SH7750_SCSCR_RIE | SH7750_SCSCR_RE;
  }

  return RTEMS_SUCCESSFUL;
}

#ifdef SH4_WITH_IPL
/*********************************
 * Functions for SH-IPL gdb stub *
 *********************************/

/*
 * ipl_finish --
 *     Says gdb that program finished to get out from it.
 */
extern void ipl_finish(void);
__asm__ (
"   .global _ipl_finish\n"
"_ipl_finish:\n"
"   mov.l   __ipl_finish_value, r0\n"
"   trapa   #0x3f\n"
"   nop\n"
"   rts\n"
"   nop\n"
"   .align 4\n"
"__ipl_finish_value:\n"
"   .long   255"
);

extern int ipl_serial_input(int poll_count);
__asm__ (
"    .global _ipl_serial_input\n"
"_ipl_serial_input:\n"
"    mov  #1,r0\n"
"    trapa #0x3f\n"
"    nop\n"
"    rts\n"
"    nop\n");

extern void ipl_serial_output(const char *buf, int len);
__asm__ (
"    .global _ipl_serial_output\n"
"_ipl_serial_output:\n"
"    mov  #0,r0\n"
"    trapa #0x3f\n"
"    nop\n"
"    rts\n"
"    nop\n");

/* ipl_console_poll_read --
 *     poll read operation for simulator console through ipl mechanism.
 *
 * PARAMETERS:
 *     minor - minor device number
 *
 * RETURNS:
 *     character code red from UART, or -1 if there is no characters
 *     available
 */
int
ipl_console_poll_read(int minor)
{
    unsigned char buf;
    buf = ipl_serial_input(0x100000);
    return buf;
}

/* ipl_console_poll_write --
 *     wrapper for polling mode write function
 *
 * PARAMETERS:
 *     minor - minor device number
 *     buf - output buffer
 *     len - output buffer length
 *
 * RETURNS:
 *     result code (0)
 */
int
ipl_console_poll_write(int minor, const char *buf, int len)
{
  int c;
  while (len > 0) {
    c = (len < 64 ? len : 64);
    ipl_serial_output(buf, c);
    len -= c;
    buf += c;
  }
  return 0;
}
#endif
