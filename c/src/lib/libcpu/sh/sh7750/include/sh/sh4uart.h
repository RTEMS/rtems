/*
 *  Generic UART Serial driver for SH-4 processors definitions
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russian Fed.
 *  Author: Alexandra Kossovsky <sasha@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __SH4UART_H__
#define __SH4UART_H__

#include <rtems/score/sh7750_regs.h>


/*
 * Define this to work from gdb stub
 */

/* FIXME: This is BSP-specific */
#define SH4_WITH_IPL

#define SH4_SCI     1   /* Serial Communication Interface - SCI */
#define SH4_SCIF    2   /* Serial Communication Interface with FIFO - SCIF */
#define TRANSMIT_TRIGGER_VALUE(ttrg) ((ttrg) == SH7750_SCFCR2_RTRG_1 ? 1 : \
        (ttrg) == SH7750_SCFCR2_RTRG_4 ? 4 : \
        (ttrg) == SH7750_SCFCR2_RTRG_8 ? 8 : 14)

/*
 * Macros to call UART registers
 */
#define SCRDR(n) (*(volatile uint8_t*)SH7750_SCRDR(n))
#define SCRDR1 SCRDR(1)
#define SCRDR2 SCRDR(2)
#define SCTDR(n) (*(volatile uint8_t*)SH7750_SCTDR(n))
#define SCTDR1 SCTDR(1)
#define SCTDR2 SCTDR(2)
#define SCSMR(n) ((n) == 1 ? *(volatile uint8_t*)SH7750_SCSMR1 : \
        *(volatile uint16_t*)SH7750_SCSMR2)
#define SCSMR1 SCSMR(1)
#define SCSMR2 SCSMR(2)
#define SCSCR(n) ((n) == 1 ? *(volatile uint8_t*)SH7750_SCSCR1 : \
        *(volatile uint16_t*)SH7750_SCSCR2)
#define SCSCR1 SCSCR(1)
#define SCSCR2 SCSCR(2)
#define SCSSR(n) ((n) == 1 ? *(volatile uint8_t*)SH7750_SCSSR1 : \
        *(volatile uint16_t*)SH7750_SCSSR2)
#define SCSSR1 SCSSR(1)
#define SCSSR2 SCSSR(2)
#define SCSPTR1 (*(volatile uint8_t*)SH7750_SCSPTR1)
#define SCSPTR2 (*(volatile uint16_t*)SH7750_SCSPTR2)
#define SCBRR(n) (*(volatile uint8_t*)SH7750_SCBRR(n))
#define SCBRR1 SCBRR(1)
#define SCBRR2 SCBRR(2)
#define SCFCR2 (*(volatile uint16_t*)SH7750_SCFCR2)
#define SCFDR2 (*(volatile uint16_t*)SH7750_SCFDR2)
#define SCLSR2 (*(volatile uint16_t*)SH7750_SCLSR2)

#define IPRB (*(volatile uint16_t*)SH7750_IPRB)
#define IPRC (*(volatile uint16_t*)SH7750_IPRC)

/*
 * The following structure is a descriptor of single UART channel.
 * It contains the initialization information about channel and
 * current operating values
 */
typedef struct sh4uart {
   uint8_t        chn;        /* UART channel number */
   uint8_t        int_driven; /* UART interrupt vector number, or
                                       0 if polled I/O */
   void                *tty;        /* termios channel descriptor */

   volatile const char *tx_buf;     /* Transmit buffer from termios */
   volatile uint32_t    tx_buf_len; /* Transmit buffer length */
   volatile uint32_t    tx_ptr;     /* Index of next char to transmit*/

   rtems_isr_entry      old_handler_transmit;   /* Saved interrupt handlers */
   rtems_isr_entry      old_handler_receive;

   tcflag_t             c_iflag;            /* termios input mode flags */
   bool                 parerr_mark_flag;   /* Parity error processing state */
} sh4uart;

/*
 * Functions from sh4uart.c
 */

/* sh4uart_init --
 *     This function verifies the input parameters and perform initialization
 *     of the Motorola Coldfire on-chip UART descriptor structure.
 *
 */
rtems_status_code
sh4uart_init(sh4uart *uart, void *tty, int chn, int int_driven);

/* sh4uart_reset --
 *     This function perform the hardware initialization of Motorola
 *     Coldfire processor on-chip UART controller using parameters
 *     filled by the sh4uart_init function.
 */
rtems_status_code
sh4uart_reset(sh4uart *uart);

/* sh4uart_disable --
 *     This function disable the operations on Motorola Coldfire UART
 *     controller
 */
rtems_status_code
sh4uart_disable(sh4uart *uart, int disable_port);

/* sh4uart_set_attributes --
 *     This function parse the termios attributes structure and perform
 *     the appropriate settings in hardware.
 */
rtems_status_code
sh4uart_set_attributes(sh4uart *mcf, const struct termios *t);

/* sh4uart_poll_read --
 *     This function tried to read character from MCF UART and perform
 *     error handling.
 */
int
sh4uart_poll_read(sh4uart *uart);

#ifdef SH4_WITH_IPL
/* ipl_console_poll_read --
 *     This function tried to read character from MCF UART over SH-IPL.
 */
int
ipl_console_poll_read(int minor);

/* sh4uart_interrupt_write --
 *     This function initiate transmitting of the buffer in interrupt mode.
 */
rtems_status_code
sh4uart_interrupt_write(sh4uart *uart, const char *buf, int len);

/* sh4uart_poll_write --
 *     This function transmit buffer byte-by-byte in polling mode.
 */
int
sh4uart_poll_write(sh4uart *uart, const char *buf, int len);

/* ipl_console_poll_write --
 *     This function transmit buffer byte-by-byte in polling mode over SH-IPL.
 */
int
ipl_console_poll_write(int minor, const char *buf, int len);

/*
 * ipl_finish --
 *     Says gdb that program finished to get out from it.
 */
extern void ipl_finish(void);
#endif

/* sh4uart_stop_remote_tx --
 *     This function stop data flow from remote device.
 */
rtems_status_code
sh4uart_stop_remote_tx(sh4uart *uart);

/* sh4uart_start_remote_tx --
 *     This function resume data flow from remote device.
 */
rtems_status_code
sh4uart_start_remote_tx(sh4uart *uart);

/* Descriptor structures for two on-chip UART channels */
extern sh4uart sh4_uarts[2];

#endif
