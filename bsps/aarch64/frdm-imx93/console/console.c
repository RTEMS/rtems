/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRTEMSBSPsAArch64FRDMIMX93
 *
 * @brief Console implementation.
 */

/*
 * Copyright (C) 2026 Rongjie Lee
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>
#include <bsp/fatal.h>
#include <imx93/imx93_uartreg.h>
#include <bsp/frdm-imx93.h>

/* My low-level driver specialization of Termios device context */
typedef struct {
  rtems_termios_device_context base;
  const char                  *device_name;
  volatile imx93_uart         *regs;
} imx93_console_context;

static imx93_console_context imx93_uart_instances[ 2 ] = {
  { .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "ttyS1" ),
    .device_name = "ttyS1",
    .regs = (volatile imx93_uart *) IMX93_LPUART1_BASE },

  { .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "ttyS2" ),
    .device_name = "ttyS2",
    .regs = (volatile imx93_uart *) IMX93_LPUART2_BASE }
};
static imx93_console_context *imx93_uart_console = &imx93_uart_instances[ 0 ];

static volatile imx93_uart *imx93_uart_get_regs(
  rtems_termios_device_context *base
)
{
  imx93_console_context *ctx;

  ctx = (imx93_console_context *) base;
  return ctx->regs;
}

static bool im93_setup_watermark(
  rtems_termios_tty             *tty,
  rtems_termios_device_context  *base,
  struct termios                *term,
  rtems_libio_open_close_args_t *args
)
{
  (void) term;
  (void) args;

  volatile imx93_uart *regs;
  uint32_t             crtl_default;

  regs = imx93_uart_get_regs( base );

  crtl_default = regs->CTRL;

  regs->CTRL &= ~(
    UARTCTRL_TIE | UARTCTRL_TCIE | UARTCTRL_TE | UARTCTRL_RIE | UARTCTRL_RE |
    UARTCTRL_ILIE
  );

  regs->FIFO |= UARTFIFO_TXFE | UARTFIFO_RXFE | UARTFIFO_TXFLUSH |
                UARTFIFO_RXFLUSH;
  regs->FIFO = IMX93_UARTFIFO_RXIDEN( regs->FIFO, 0x3 );

  regs->WATER = ( 1 << UARTWATER_RXWATER_OFF ) |
                ( 0 << UARTWATER_TXWATER_OFF );

  regs->MODIR = IMX93_UARTMODIR_RTSWATER(
    regs->MODIR,
    IMX93_UARTFIFO_RXFIFOSIZE_GET( regs->FIFO ) >> 1
  );

  regs->CTRL = crtl_default;

  regs->CTRL |= UARTCTRL_RE | UARTCTRL_TE;
  regs->CTRL = IMX93_UARTCTRL_IDLECFG( regs->CTRL, 0x7 );

  regs->CTRL |= UARTCTRL_TIE | UARTCTRL_RIE | UARTCTRL_ILIE;

  rtems_termios_set_initial_baud( tty, 115200 );

  return true;
}

static void imx93_poll_write( rtems_termios_device_context *base, char c )
{
  volatile imx93_uart *regs;

  regs = imx93_uart_get_regs( base );

  while ( ( regs->STAT & UARTSTAT_TDRE ) == 0 ) {
  }

  regs->DATA = IMX93_UART_DATA( c );
}

static void imx93_uart_write(
  rtems_termios_device_context *base,
  const char                   *buf,
  size_t                        len
)
{
  size_t i;

  for ( i = 0; i < len; ++i ) {
    imx93_poll_write( base, buf[ i ] );
  }
}

static int imx93_uart_read_polled( rtems_termios_device_context *base )
{
  volatile imx93_uart *regs;

  regs = imx93_uart_get_regs( base );

  if ( !( regs->WATER >> UARTWARER_RXCNT_OFF ) ) {
    return -1;
  } else {
    return IMX93_UART_DATA_GET( regs->DATA );
  }
}

static void imx93_output_char( char c )
{
  imx93_poll_write( &imx93_uart_console->base, c );
}

static int imx93_poll_char( void )
{
  return imx93_uart_read_polled( &imx93_uart_console->base );
}

const rtems_termios_device_handler imx93_handler_polled = {
  .first_open = im93_setup_watermark,
  .last_close = NULL,
  .poll_read = imx93_uart_read_polled,
  .write = imx93_uart_write,
  .set_attributes = NULL,
  .ioctl = NULL, /* optional, may be NULL */
  .mode = TERMIOS_POLLED
};

rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;
  char path[] = "/dev/ttyS?";
  ;
  rtems_status_code sc;
  size_t            i;

  rtems_termios_initialize();

  for ( i = 0; i < RTEMS_ARRAY_SIZE( imx93_uart_instances ); ++i ) {
    imx93_console_context *ctx;

    ctx = &imx93_uart_instances[ i ];
    path[ sizeof( path ) - 2 ] = (char) ( '0' + i );
    sc = rtems_termios_device_install(
      path,
      &imx93_handler_polled,
      NULL,
      &ctx->base
    );

    if ( ctx == imx93_uart_console ) {
      link( path, CONSOLE_DEVICE_NAME );
    }
  }

  if ( sc != RTEMS_SUCCESSFUL ) {
    bsp_fatal( BSP_FATAL_CONSOLE_INSTALL_1 );
  }

  return RTEMS_SUCCESSFUL;
}

BSP_output_char_function_type BSP_output_char = imx93_output_char;

BSP_polling_getchar_function_type BSP_poll_char = imx93_poll_char;
