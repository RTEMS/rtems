/*
 *  This file contains the TTY driver for the GRLIB APBUART
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for GRLIB BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/* Define CONSOLE_USE_INTERRUPTS to enable APBUART interrupt handling instead
 * of polling mode.
 *
 * Note that it is not possible to use the interrupt mode of the driver
 * together with the "old" APBUART and -u to GRMON. However the new
 * APBUART core (from GRLIB 1.0.17-b2710) has the GRMON debug bit and can 
 * handle interrupts.
 *
 * NOTE: This can be defined in the griscv.cfg file.
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <amba.h>
#include <grlib/ambapp.h>
#include <grlib/apbuart_termios.h>
#include <rtems/console.h>
#include <string.h>

/* The GRLIB BSP UART driver can rely on the Driver Manager if the
 * DrvMgr is initialized during startup. Otherwise the classic driver
 * must be used.
 *
 * The DrvMgr APBUART driver is located in the shared/uart directory
 */
#ifndef RTEMS_DRVMGR_STARTUP

int syscon_uart_index __attribute__((weak)) = 0;

/* body is in debugputs.c */
static struct apbuart_context apbuarts[BSP_NUMBER_OF_TERMIOS_PORTS];
static int uarts = 0;

static rtems_termios_device_context *grlib_console_get_context(int index)
{
  struct apbuart_context *uart = &apbuarts[index];

  rtems_termios_device_context_initialize(&uart->base, "APBUART");

  return &uart->base;
}

/* AMBA PP find routine. Extract AMBA PnP information into data structure. */
static int find_matching_apbuart(struct ambapp_dev *dev, int index, void *arg)
{
  struct ambapp_apb_info *apb = (struct ambapp_apb_info *)dev->devinfo;

  /* Extract needed information of one APBUART */
  apbuarts[uarts].regs = (struct apbuart_regs *)apb->start;
  apbuarts[uarts].irq = apb->irq;
  /* Get APBUART core frequency, it is assumed that it is the same
   * as Bus frequency where the UART is situated
   */
  apbuarts[uarts].freq_hz = ambapp_freq_get(&ambapp_plb, dev);
  uarts++;

  if (uarts >= BSP_NUMBER_OF_TERMIOS_PORTS)
    return 1; /* Satisfied number of UARTs, stop search */
  else
    return 0; /* Continue searching for more UARTs */
}

/* Find all UARTs */
static void grlib_console_scan_uarts(void)
{
  memset(apbuarts, 0, sizeof(apbuarts));

  /* Find APBUART cores */
  ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS), VENDOR_GAISLER,
                  GAISLER_APBUART, find_matching_apbuart, NULL);
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  const rtems_termios_device_handler *handler =
#if CONSOLE_USE_INTERRUPTS
    &apbuart_handler_interrupt;
#else
    &apbuart_handler_polled;
#endif
  rtems_status_code status;
  int i;
  char console_name[16];

  rtems_termios_initialize();

  /* Find UARTs */
  grlib_console_scan_uarts();

  /* Update syscon_uart_index to index used as /dev/console
   * Let user select System console by setting syscon_uart_index. If the
   * BSP is to provide the default UART (syscon_uart_index==0):
   *   non-MP: APBUART[0] is system console
   *   MP: LEON CPU index select UART
   */
  if (syscon_uart_index == 0) {
#if defined(RTEMS_MULTIPROCESSING)
    syscon_uart_index = GRLIB_Cpu_Index;
#else
    syscon_uart_index = 0;
#endif
  } else {
    syscon_uart_index = syscon_uart_index - 1; /* User selected sys-console */
  }

  /*  Register Device Names
   *
   *  0 /dev/console   - APBUART[USER-SELECTED, DEFAULT=APBUART[0]]
   *  1 /dev/console_a - APBUART[0] (by default not present because is console)
   *  2 /dev/console_b - APBUART[1]
   *  ...
   *
   * On a MP system one should not open UARTs that other OS instances use.
   */
  if (syscon_uart_index < uarts) {
    status = rtems_termios_device_install(
      CONSOLE_DEVICE_NAME,
      handler,
      NULL,
      grlib_console_get_context(syscon_uart_index)
    );
    if (status != RTEMS_SUCCESSFUL)
      bsp_fatal(LEON3_FATAL_CONSOLE_REGISTER_DEV);
  }
  strcpy(console_name,"/dev/console_a");
  for (i = 0; i < uarts; i++) {
    if (i == syscon_uart_index)
      continue; /* skip UART that is registered as /dev/console */
    console_name[13] = 'a' + i;
    rtems_termios_device_install(
      console_name,
      handler,
      NULL,
      grlib_console_get_context(i)
    );
  }

  return RTEMS_SUCCESSFUL;
}

#endif
