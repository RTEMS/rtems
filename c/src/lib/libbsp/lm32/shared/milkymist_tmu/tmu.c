/*  tmu.c
 *
 *  Milkymist TMU driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libio.h>
#include <rtems/status-checks.h>
#include "../include/system_conf.h"
#include "milkymist_tmu.h"

#define DEVICE_NAME "/dev/tmu"

static rtems_id done_sem;

static rtems_isr done_handler(rtems_vector_number n)
{
  rtems_semaphore_release(done_sem);
  lm32_interrupt_ack(1 << MM_IRQ_TMU);
}

rtems_device_driver tmu_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create TMU device");

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'M', 'U', ' '),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &done_sem
  );
  RTEMS_CHECK_SC(sc, "create TMU done semaphore");

  rtems_interrupt_catch(done_handler, MM_IRQ_TMU, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_TMU);

  return RTEMS_SUCCESSFUL;
}

static void invalidate_l2(void)
{
  volatile char *flushbase = (char *)FMLBRG_FLUSH_BASE;
  int i, offset;

  offset = 0;
  for (i=0;i<FMLBRG_LINE_COUNT;i++) {
    flushbase[offset] = 0;
    offset += FMLBRG_LINE_LENGTH;
  }
}

static bool invalidate_after;

static void tmu_start(struct tmu_td *td)
{
  if (td->invalidate_before)
    invalidate_l2();

  MM_WRITE(MM_TMU_HMESHLAST, td->hmeshlast);
  MM_WRITE(MM_TMU_VMESHLAST, td->vmeshlast);
  MM_WRITE(MM_TMU_BRIGHTNESS, td->brightness);
  MM_WRITE(MM_TMU_CHROMAKEY, td->chromakey);

  MM_WRITE(MM_TMU_VERTICESADR, (unsigned int)td->vertices);
  MM_WRITE(MM_TMU_TEXFBUF, (unsigned int)td->texfbuf);
  MM_WRITE(MM_TMU_TEXHRES, td->texhres);
  MM_WRITE(MM_TMU_TEXVRES, td->texvres);
  MM_WRITE(MM_TMU_TEXHMASK, td->texhmask);
  MM_WRITE(MM_TMU_TEXVMASK, td->texvmask);

  MM_WRITE(MM_TMU_DSTFBUF, (unsigned int)td->dstfbuf);
  MM_WRITE(MM_TMU_DSTHRES, td->dsthres);
  MM_WRITE(MM_TMU_DSTVRES, td->dstvres);
  MM_WRITE(MM_TMU_DSTHOFFSET, td->dsthoffset);
  MM_WRITE(MM_TMU_DSTVOFFSET, td->dstvoffset);
  MM_WRITE(MM_TMU_DSTSQUAREW, td->dstsquarew);
  MM_WRITE(MM_TMU_DSTSQUAREH, td->dstsquareh);

  MM_WRITE(MM_TMU_ALPHA, td->alpha);

  MM_WRITE(MM_TMU_CTL, td->flags|TMU_CTL_START);

  invalidate_after = td->invalidate_after;
}

static rtems_status_code tmu_finalize(void)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(done_sem, RTEMS_WAIT, 100);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (invalidate_after) {
    invalidate_l2();
    __asm__ volatile( /* Invalidate Level-1 data cache */
      "wcsr DCC, r0\n"
      "nop\n"
    );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver tmu_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;
  struct tmu_td *td = (struct tmu_td *)args->buffer;
  rtems_status_code sc;

  switch (args->command) {
    case TMU_EXECUTE:
      tmu_start(td);
      sc = tmu_finalize();
      break;
    case TMU_EXECUTE_NONBLOCK:
      tmu_start(td);
      sc = RTEMS_SUCCESSFUL;
      break;
    case TMU_EXECUTE_WAIT:
      sc = tmu_finalize();
      break;
    default:
      sc = RTEMS_UNSATISFIED;
      break;
  }

  if (sc == RTEMS_SUCCESSFUL)
    args->ioctl_return = 0;
  else
    args->ioctl_return = -1;

  return sc;
}
