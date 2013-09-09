/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#include <bsp/irq.h>
#include <bsp/arm-pl050.h>
#include <bsp/arm-pl050-regs.h>

#include <libchip/sersupp.h>

static volatile pl050 *pl050_get_regs(int minor)
{
  const console_tbl *ct = Console_Port_Tbl[minor];

  return (volatile pl050 *) ct->ulCtrlPort1;
}

static void pl050_interrupt(void *arg)
{
  int minor = (int) arg;
  const console_data *cd = &Console_Port_Data[minor];
  volatile pl050 *regs = pl050_get_regs(minor);
  uint32_t kmiir_rx = PL050_KMIIR_KMIRXINTR;
  uint32_t kmiir_tx = (regs->kmicr & PL050_KMICR_KMITXINTREN) != 0 ?
    PL050_KMIIR_KMITXINTR : 0;
  uint32_t kmiir = regs->kmiir;

  if ((kmiir & kmiir_rx) != 0) {
    char c = (char) PL050_KMIDATA_KMIDATA_GET(regs->kmidata);

    rtems_termios_enqueue_raw_characters(cd->termios_data, &c, 1);
  }

  if ((kmiir & kmiir_tx) != 0) {
    rtems_termios_dequeue_characters(cd->termios_data, 1);
  }
}

static void pl050_initialize(int minor)
{
  /* Nothing to do */
}

static int pl050_first_open(int major, int minor, void *arg)
{
  rtems_libio_open_close_args_t *oc = (rtems_libio_open_close_args_t *) arg;
  struct rtems_termios_tty *tty = (struct rtems_termios_tty *) oc->iop->data1;
  console_data *cd = &Console_Port_Data[minor];
  const console_tbl *ct = Console_Port_Tbl[minor];
  volatile pl050 *regs = pl050_get_regs(minor);
  rtems_status_code sc;

  cd->termios_data = tty;
  rtems_termios_set_initial_baud(tty, (rtems_termios_baud_t) ct->pDeviceParams);

  regs->kmicr = PL050_KMICR_KMIEN | PL050_KMICR_KMIRXINTREN;

  sc = rtems_interrupt_handler_install(
    ct->ulIntVector,
    ct->sDeviceName,
    RTEMS_INTERRUPT_UNIQUE,
    pl050_interrupt,
    (void *) minor
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return 0;
}

static int pl050_last_close(int major, int minor, void *arg)
{
  const console_tbl *ct = Console_Port_Tbl[minor];
  volatile pl050 *regs = pl050_get_regs(minor);
  rtems_status_code sc;

  regs->kmicr = 0;

  sc = rtems_interrupt_handler_remove(
    ct->ulIntVector,
    pl050_interrupt,
    (void *) minor
  );
  assert(sc == RTEMS_SUCCESSFUL);

  return 0;
}

static ssize_t pl050_write_support(
  int minor,
  const char *s,
  size_t n
)
{
  volatile pl050 *regs = pl050_get_regs(minor);

  if (n > 0) {
    regs->kmidata = PL050_KMIDATA_KMIDATA(s[0]);
    regs->kmicr |= PL050_KMICR_KMITXINTREN;
  } else {
    regs->kmicr &= ~PL050_KMICR_KMITXINTREN;
  }

  return 0;
}

static int pl050_set_attribues(int minor, const struct termios *term)
{
  return -1;
}

const console_fns arm_pl050_fns = {
  .deviceProbe = libchip_serial_default_probe,
  .deviceFirstOpen = pl050_first_open,
  .deviceLastClose = pl050_last_close,
  .deviceRead = NULL,
  .deviceWrite = pl050_write_support,
  .deviceInitialize = pl050_initialize,
  .deviceWritePolled = NULL,
  .deviceSetAttributes = pl050_set_attribues,
  .deviceOutputUsesInterrupts = true
};
