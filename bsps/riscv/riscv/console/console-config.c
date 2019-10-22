/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <rtems/console.h>
#include <rtems/sysinit.h>
#include <rtems/termiostypes.h>

#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>
#include <bsp/riscv.h>

#include <dev/serial/htif.h>
#include <libchip/ns16550.h>

#include <libfdt.h>
#include <string.h>

#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
#include <bsp/fe310-uart.h>
fe310_uart_context driver_context;
#endif

#if RISCV_ENABLE_HTIF_SUPPORT != 0
static htif_console_context htif_console_instance;
#endif

#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
static ns16550_context ns16550_instances[RISCV_CONSOLE_MAX_NS16550_DEVICES];
#endif

static struct {
  rtems_termios_device_context *context;
  void (*putchar)(rtems_termios_device_context *base, char c);
  int (*getchar)(rtems_termios_device_context *base);
} riscv_console;

static void riscv_output_char(char c)
{
  (*riscv_console.putchar)(riscv_console.context, c);
}

static int riscv_get_console_node(const void *fdt)
{
  const char *stdout_path;
  int node;

  node = fdt_path_offset(fdt, "/chosen");

  stdout_path = fdt_getprop(fdt, node, "stdout-path", NULL);
  if (stdout_path == NULL) {
    stdout_path = "";
  }

#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
  int root;
  int soc;
  root = fdt_path_offset(fdt, "/");
  soc = fdt_subnode_offset(fdt, root, "soc");

  int offset=fdt_subnode_offset(fdt, soc,stdout_path);

  return offset;
#else
  return fdt_path_offset(fdt, stdout_path);
#endif
}

#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
static uint8_t riscv_console_get_reg_8(uintptr_t addr, uint8_t i)
{
  volatile uint8_t *reg;

  reg = (uint8_t *) addr;
  return reg[i];
}

static void riscv_console_set_reg_8(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg;

  reg = (uint8_t *)addr;
  reg[i] = val;
}

static uint8_t riscv_console_get_reg_32(uintptr_t addr, uint8_t i)
{
  volatile uint32_t *reg;

  reg = (uint32_t *) addr;
  return reg[i];
}

static void riscv_console_set_reg_32(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg;

  reg = (uint32_t *)addr;
  reg[i] = val;
}
#endif

#define RISCV_CONSOLE_IS_COMPATIBLE(actual, actual_len, desired) \
  (actual_len == sizeof(desired) \
     && memcmp(actual, desired, sizeof(desired) - 1) == 0)

static void riscv_console_probe(void)
{
  const void *fdt;
  int node;
  int console_node;
#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
  size_t ns16550_devices;
#endif

  fdt = bsp_fdt_get();
  console_node = riscv_get_console_node(fdt);
#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
  ns16550_devices = 0;
#endif

  node = fdt_next_node(fdt, -1, NULL);

  while (node >= 0) {
    const char *compat;
    int compat_len;

    compat = fdt_getprop(fdt, node, "compatible", &compat_len);
    if (compat == NULL) {
      compat_len = 0;
    }

#if RISCV_ENABLE_HTIF_SUPPORT != 0
    if (RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "ucb,htif0")) {
      htif_console_context_init(&htif_console_instance.base, node);

      riscv_console.context = &htif_console_instance.base;
      riscv_console.putchar = htif_console_putchar;
      riscv_console.getchar = htif_console_getchar;
    };
#endif

#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
    if (
      (RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "ns16550a")
          || RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "ns16750"))
        && ns16550_devices < RISCV_CONSOLE_MAX_NS16550_DEVICES
    ) {
      ns16550_context *ctx;
      fdt32_t *val;
      int len;

      ctx = &ns16550_instances[ns16550_devices];
      ctx->initial_baud = BSP_CONSOLE_BAUD;

      if (RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "ns16750")) {
        ctx->has_precision_clock_synthesizer = true;
        ctx->get_reg = riscv_console_get_reg_32;
        ctx->set_reg = riscv_console_set_reg_32;
      } else {
        ctx->get_reg = riscv_console_get_reg_8;
        ctx->set_reg = riscv_console_set_reg_8;
      }

      ctx->port = (uintptr_t) riscv_fdt_get_address(fdt, node);

      if (ctx->port == 0) {
        bsp_fatal(RISCV_FATAL_NO_NS16550_REG_IN_DEVICE_TREE);
      }

      val = (fdt32_t *) fdt_getprop(fdt, node, "clock-frequency", &len);

      if (val == NULL || len != 4) {
        bsp_fatal(RISCV_FATAL_NO_NS16550_CLOCK_FREQUENCY_IN_DEVICE_TREE);
      }

      ctx->clock = fdt32_to_cpu(val[0]);

      val = (fdt32_t *) fdt_getprop(fdt, node, "interrupts", &len);

      if (val == NULL || len != 4) {
        bsp_fatal(RISCV_FATAL_NO_NS16550_INTERRUPTS_IN_DEVICE_TREE);
      }

      ctx->irq = RISCV_INTERRUPT_VECTOR_EXTERNAL(fdt32_to_cpu(val[0]));

      if (node == console_node) {
        riscv_console.context = &ctx->base;
        riscv_console.putchar = ns16550_polled_putchar;
        riscv_console.getchar = ns16550_polled_getchar;
      }

      rtems_termios_device_context_initialize(&ctx->base, "NS16550");
      ns16550_probe(&ctx->base);

      ++ns16550_devices;
    }
#endif

#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
    if (RISCV_CONSOLE_IS_COMPATIBLE(compat, compat_len, "sifive,uart0")) {
      fe310_uart_context *ctx ;

      ctx=&driver_context;
      ctx->regs = (uintptr_t) riscv_fdt_get_address(fdt, node);
      if (ctx->regs == 0)
      {
        bsp_fatal(RISCV_FATAL_NO_NS16550_REG_IN_DEVICE_TREE);
      }

      if (node == console_node) {
        riscv_console.context = &ctx->base;
        riscv_console.putchar = fe310_console_putchar;
        riscv_console.getchar = fe310_uart_read;
      }

      rtems_termios_device_context_initialize(&ctx->base, "FE310UART");
    }
#endif

    node = fdt_next_node(fdt, node, NULL);
  }

  BSP_output_char = riscv_output_char;
}

static void riscv_output_char_init(char c)
{
  riscv_console_probe();
  riscv_output_char(c);
}

BSP_output_char_function_type BSP_output_char = riscv_output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;

rtems_status_code console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
#if RISCV_ENABLE_HTIF_SUPPORT != 0
  rtems_termios_device_context *base;
  char htif_path[] = "/dev/ttyShtif";
#endif
#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
  char path[] = "/dev/ttyS?";
  size_t i;
#endif

#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
  char path[] = "/dev/ttyS0";
#endif

  rtems_termios_initialize();

#if RISCV_ENABLE_HTIF_SUPPORT != 0
  base = &htif_console_instance.base;
  rtems_termios_device_install(htif_path, &htif_console_handler, NULL, base);

  if (base == riscv_console.context) {
    link(htif_path, CONSOLE_DEVICE_NAME);
  }
#endif

#if RISCV_CONSOLE_MAX_NS16550_DEVICES > 0
  for (i = 0; i < RISCV_CONSOLE_MAX_NS16550_DEVICES; ++i) {
    ns16550_context *ctx;

    ctx = &ns16550_instances[i];
    path[sizeof(path) - 2] = (char) ('0' + i);

    rtems_termios_device_install(
      path,
      &ns16550_handler_interrupt,
      NULL,
      &ctx->base
    );

    if (&ctx->base == riscv_console.context) {
      link(path, CONSOLE_DEVICE_NAME);
    }
  }
#endif

#if RISCV_ENABLE_FRDME310ARTY_SUPPORT != 0
  fe310_uart_context * ctx = &driver_context;

  rtems_termios_device_install(
    path,
    &fe310_uart_handler,
    NULL,
    &ctx->base
  );

  if (&ctx->base == riscv_console.context) {
    link(path, CONSOLE_DEVICE_NAME);
  }

#endif

  return RTEMS_SUCCESSFUL;
}

RTEMS_SYSINIT_ITEM(
  riscv_console_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
