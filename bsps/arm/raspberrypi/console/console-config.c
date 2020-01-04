/**
 * @file
 *
 * @ingroup raspberrypi_usart
 *
 * @brief Console Configuration.
 */

/*
 * Copyright (c) 2015 Yang Qiao
 * based on work by:
 * Copyright (c) 2013 Alan Cudmore
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>

#include <libchip/serial.h>
#include <libfdt.h>

#include <bspopts.h>
#include <bsp/usart.h>
#include <bsp/raspberrypi.h>
#include <bsp/fbcons.h>
#include <bsp.h>
#include <bsp/arm-pl011.h>
#include <bsp/console-termios.h>
#include <bsp/fdt.h>
#include <bsp/fatal.h>


#define UART0     "/dev/ttyS0"
#define FBCONS    "/dev/fbcons"

arm_pl011_context pl011_context;

rpi_fb_context fb_context;

static void output_char_serial(char c)
{
  arm_pl011_write_polled(&pl011_context.base, c);
}

void output_char_fb(char c)
{
  fbcons_write_polled(&fb_context.base, c);
}

static void init_ctx_arm_pl011(
  const void *fdt,
  int node
)
{
  arm_pl011_context *ctx = &pl011_context;
  rtems_termios_device_context_initialize(&ctx->base, "UART");
  ctx->regs = raspberrypi_get_reg_of_node(fdt, node);
}

static void register_fb( void )
{
  if (fbcons_probe(&fb_context.base) == true) {
    rtems_termios_device_install(
      FBCONS,
      &fbcons_fns,
      NULL,
      &fb_context.base);
  }
}

static void console_select( void )
{
  const char *opt;

  opt = rpi_cmdline_get_arg("--console=");

  if ( opt ) {
    if ( strncmp( opt, "fbcons", sizeof( "fbcons" ) - 1 ) == 0 ) {
      if ( rpi_video_is_initialized() > 0 ) {
        BSP_output_char = output_char_fb;
        link(FBCONS, CONSOLE_DEVICE_NAME);
        return ;
      }
    }
  }
  BSP_output_char = output_char_serial;
  link(UART0, CONSOLE_DEVICE_NAME);
}

static void uart_probe(void)
{
  static bool initialized = false;
  const void *fdt;
  int node;

  if ( initialized ) {
    return ;
  }

  fdt = bsp_fdt_get();
  node = fdt_node_offset_by_compatible(fdt, -1, "brcm,bcm2835-pl011");

  init_ctx_arm_pl011(fdt, node);

  initialized = true;
}

static void output_char(char c)
{
  uart_probe();
  output_char_serial(c);
}

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_termios_initialize();

  uart_probe();
  rtems_termios_device_install(
    UART0,
    &arm_pl011_fns,
    NULL,
    &pl011_context.base
  );

  register_fb();

  console_select();

  return RTEMS_SUCCESSFUL;
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

RTEMS_SYSINIT_ITEM(
  uart_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);