/*
 *  Generic UART Serial driver for Motorola Coldfire processors definitions
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russian Fed.
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MCFUART_H__
#define __MCFUART_H__

#include <termios.h>
#include "bsp.h"
#include "mcf5206e.h"

/*
 * The MCF5206e System Clock Frequency; 54MHz default
 */
#ifndef SYSTEM_CLOCK_FREQUENCY
#define SYSTEM_CLOCK_FREQUENCY BSP_SYSTEM_FREQUENCY
#endif

/*
 * The following structure is a descriptor of single UART channel.
 * It contains the initialization information about channel and
 * current operating values
 */
typedef struct mcfuart {
   uint32_t            chn;              /* UART channel number */
   uint8_t             intvec;           /* UART interrupt vector number, or
                                                0 if polled I/O */
   void               *tty;              /* termios channel descriptor */

   volatile const uint8_t  *tx_buf;      /* Transmit buffer from termios */
   volatile uint32_t        tx_buf_len;  /* Transmit buffer length */
   volatile uint32_t        tx_ptr;      /* Index of next char to transmit*/
   rtems_isr_entry          old_handler; /* Saved interrupt handler */

   tcflag_t                 c_iflag;    /* termios input mode flags */
   bool                     parerr_mark_flag; /* Parity error processing
                                                  state */
} mcfuart;

/* mcfuart_init --
 *     This function verifies the input parameters and perform initialization
 *     of the Motorola Coldfire on-chip UART descriptor structure.
 *
 */
rtems_status_code
mcfuart_init(mcfuart *uart, void *tty, uint8_t   intvec,
             uint32_t   chn);

/* mcfuart_reset --
 *     This function perform the hardware initialization of Motorola
 *     Coldfire processor on-chip UART controller using parameters
 *     filled by the mcfuart_init function.
 */
rtems_status_code
mcfuart_reset(mcfuart *uart);

/* mcfuart_disable --
 *     This function disable the operations on Motorola Coldfire UART
 *     controller
 */
rtems_status_code
mcfuart_disable(mcfuart *uart);

/* mcfuart_set_attributes --
 *     This function parse the termios attributes structure and perform
 *     the appropriate settings in hardware.
 */
int
mcfuart_set_attributes(mcfuart *mcf, const struct termios *t);

/* mcfuart_poll_read --
 *     This function tried to read character from MCF UART and perform
 *     error handling.
 */
int
mcfuart_poll_read(mcfuart *uart);

/* mcfuart_interrupt_write --
 *     This function initiate transmitting of the buffer in interrupt mode.
 */
ssize_t
mcfuart_interrupt_write(mcfuart *uart, const char *buf, size_t len);

/* mcfuart_poll_write --
 *     This function transmit buffer byte-by-byte in polling mode.
 */
ssize_t
mcfuart_poll_write(mcfuart *uart, const char *buf, size_t len);

/* mcfuart_stop_remote_tx --
 *     This function stop data flow from remote device.
 */
int
mcfuart_stop_remote_tx(mcfuart *uart);

/* mcfuart_start_remote_tx --
 *     This function resume data flow from remote device.
 */
int
mcfuart_start_remote_tx(mcfuart *uart);

#endif
