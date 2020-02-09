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
#include <libchip/ns16550.h>

#include <bspopts.h>
#include <bsp/usart.h>
#include <bsp/raspberrypi.h>
#include <bsp/fbcons.h>
#include <bsp.h>
#include <bsp/arm-pl011.h>
#include <bsp/console-termios.h>
#include <bsp/fdt.h>
#include <bsp/fatal.h>
#include <bsp/gpio.h>
#include <bsp/rpi-gpio.h>

/**
 * UART0 - PL011
 * UART1 - Mini UART
 */
#define PL011     "/dev/ttyAMA0"
#define MINIUART  "/dev/ttyS0"
#define FBCONS    "/dev/fbcons"

arm_pl011_context pl011_context;
ns16550_context mini_uart_context;

rpi_fb_context fb_context;

static void output_char_pl011(char c)
{
  arm_pl011_write_polled(&pl011_context.base, c);
}

static void output_char_mini_uart(char c)
{
  ns16550_polled_putchar(&mini_uart_context.base, c);
}

void output_char_fb(char c)
{
  fbcons_write_polled(&fb_context.base, c);
}

static uint8_t mini_uart_get_reg(uintptr_t port, uint8_t index)
{
  volatile uint32_t *val = (volatile uint32_t *)port + index;
  return (uint8_t) *val;
}

static void mini_uart_set_reg(uintptr_t port, uint8_t index, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *)port + index;
  *reg = val;
}

static void init_ctx_arm_pl011(
  const void *fdt,
  int node
)
{
  arm_pl011_context *ctx = &pl011_context;
  rtems_termios_device_context_initialize(&ctx->base, "PL011UART");
  ctx->regs = raspberrypi_get_reg_of_node(fdt, node);
}

static uint32_t calculate_baud_divisor(
  ns16550_context *ctx,
  uint32_t baud
)
{
  uint32_t baudDivisor = (ctx->clock / (8 * baud)) - 1;
  return baudDivisor;
}

static void init_ctx_mini_uart(
  const void *fdt,
  int node
)
{
  const char *status;
  int len;
  ns16550_context *ctx;

  memset(&mini_uart_context, 0, sizeof(mini_uart_context));
  ctx = &mini_uart_context;

  rtems_termios_device_context_initialize(&ctx->base, "MiniUART");

  status = fdt_getprop(fdt, node, "status", &len);
  if ( status == NULL || strcmp(status, "disabled" ) == 0){
    return ;
  }

  ctx->port = (uintptr_t) raspberrypi_get_reg_of_node(fdt, node);
  ctx->initial_baud = MINI_UART_DEFAULT_BAUD;
  ctx->clock = BCM2835_CLOCK_FREQ;
  ctx->calculate_baud_divisor = calculate_baud_divisor;
  ctx->get_reg = mini_uart_get_reg;
  ctx->set_reg = mini_uart_set_reg;

  rtems_gpio_bsp_select_specific_io(0, 14, RPI_ALT_FUNC_5, NULL);
  rtems_gpio_bsp_select_specific_io(0, 15, RPI_ALT_FUNC_5, NULL);
  rtems_gpio_bsp_set_resistor_mode(0, 14, NO_PULL_RESISTOR);
  rtems_gpio_bsp_set_resistor_mode(0, 15, NO_PULL_RESISTOR);

  BCM2835_REG(AUX_ENABLES) |= 0x1;
  ns16550_probe(&ctx->base);
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
    } else if ( strncmp( opt, MINIUART, sizeof(MINIUART) - 1 ) == 0) {
      BSP_output_char = output_char_mini_uart;
      link(MINIUART, CONSOLE_DEVICE_NAME);
    } else if ( strncmp( opt, PL011, sizeof(PL011) - 1 ) == 0) {
      BSP_output_char = output_char_pl011;
      link(PL011, CONSOLE_DEVICE_NAME);
    }
  }else {
    /**
     * If no command line option was given, default to PL011.
     */
    BSP_output_char = output_char_pl011;
    link(PL011, CONSOLE_DEVICE_NAME);
  }
}

static void uart_probe(void)
{
  static bool initialized = false;
  const void *fdt;
  const char *console;
  int len;
  int node;

  if ( initialized ) {
    return ;
  }

  fdt = bsp_fdt_get();

  node = fdt_node_offset_by_compatible(fdt, -1, "brcm,bcm2835-pl011");
  init_ctx_arm_pl011(fdt, node);

  node = fdt_node_offset_by_compatible(fdt, -1, "brcm,bcm2835-aux-uart");
  init_ctx_mini_uart(fdt, node);

  node = fdt_path_offset(fdt, "/aliases");
  console = fdt_getprop(fdt, node, "serial0", &len);

  if ( strcmp(console, "/soc/serial@7e215040" ) == 0) {
    BSP_output_char = output_char_mini_uart;
  }else {
    BSP_output_char = output_char_pl011;
  }

  initialized = true;
}

static void output_char(char c)
{
  uart_probe();
  (*BSP_output_char)(c);
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
    PL011,
    &arm_pl011_fns,
    NULL,
    &pl011_context.base
  );

  rtems_termios_device_install(
    MINIUART,
    &ns16550_handler_polled,
    NULL,
    &mini_uart_context.base
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
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);