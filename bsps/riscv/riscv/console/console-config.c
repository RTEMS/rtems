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

#include <bsp.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>

#include <dev/serial/htif.h>

#include <libfdt.h>

#if RISCV_ENABLE_HTIF_SUPPORT > 0
static htif_console_context htif_console_instance;
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

  return fdt_path_offset(fdt, stdout_path);
}

static void riscv_console_probe(void)
{
  const void *fdt;
  int node;
  int console_node;

  fdt = bsp_fdt_get();
  console_node = riscv_get_console_node(fdt);

  node = fdt_next_node(fdt, -1, NULL);

  while (node >= 0) {
#if RISCV_ENABLE_HTIF_SUPPORT
    if (fdt_node_check_compatible(fdt, node, "ucb,htif0") == 0) {
      htif_console_context_init(&htif_console_instance.base, node);

      riscv_console.context = &htif_console_instance.base;
      riscv_console.putchar = htif_console_putchar;
      riscv_console.getchar = htif_console_getchar;
    };
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
  rtems_termios_device_context *base;
  char htif_path[] = "/dev/ttyShtif";

  rtems_termios_initialize();

#if RISCV_ENABLE_HTIF_SUPPORT
  base = &htif_console_instance.base;
  rtems_termios_device_install(htif_path, &htif_console_handler, NULL, base);

  if (base == riscv_console.context) {
    link(htif_path, CONSOLE_DEVICE_NAME);
  }
#endif

  return RTEMS_SUCCESSFUL;
}

RTEMS_SYSINIT_ITEM(
  riscv_console_probe,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
