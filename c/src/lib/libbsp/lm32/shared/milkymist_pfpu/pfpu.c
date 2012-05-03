/*  pfpu.c
 *
 *  Milkymist PFPU driver for RTEMS
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
#include "milkymist_pfpu.h"

#define DEVICE_NAME "/dev/pfpu"

static rtems_id done_sem;

static rtems_isr done_handler(rtems_vector_number n)
{
  rtems_semaphore_release(done_sem);
  lm32_interrupt_ack(1 << MM_IRQ_PFPU);
}

rtems_device_driver pfpu_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  rtems_isr_entry dummy;

  sc = rtems_io_register_name(DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create PFPU device");

  sc = rtems_semaphore_create(
    rtems_build_name('P', 'F', 'P', 'U'),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &done_sem
  );
  RTEMS_CHECK_SC(sc, "create PFPU done semaphore");

  rtems_interrupt_catch(done_handler, MM_IRQ_PFPU, &dummy);
  bsp_interrupt_vector_enable(MM_IRQ_PFPU);

  return RTEMS_SUCCESSFUL;
}

static void load_program(unsigned int *program, int size)
{
  int page;
  int word;
  volatile unsigned int *pfpu_prog = (unsigned int *)MM_PFPU_CODEBASE;

  for (page=0;page<(PFPU_PROGSIZE/PFPU_PAGESIZE);page++) {
    MM_WRITE(MM_PFPU_CODEPAGE, page);
    for (word=0;word<PFPU_PAGESIZE;word++) {
      if (size == 0) return;
      pfpu_prog[word] = *program;
      program++;
      size--;
    }
  }
}

static void load_registers(float *registers)
{
  volatile float *pfpu_regs = (float *)MM_PFPU_DREGBASE;
  int i;

  for (i=PFPU_SPREG_COUNT;i<PFPU_REG_COUNT;i++)
    pfpu_regs[i] = registers[i];
}

static void update_registers(float *registers)
{
  volatile float *pfpu_regs = (float *)MM_PFPU_DREGBASE;
  int i;

  for (i=PFPU_SPREG_COUNT;i<PFPU_REG_COUNT;i++)
    registers[i] = pfpu_regs[i];
}

static rtems_status_code pfpu_execute(struct pfpu_td *td)
{
  rtems_status_code sc;

  load_program(td->program, td->progsize);
  load_registers(td->registers);
  MM_WRITE(MM_PFPU_MESHBASE, (unsigned int)td->output);
  MM_WRITE(MM_PFPU_HMESHLAST, td->hmeshlast);
  MM_WRITE(MM_PFPU_VMESHLAST, td->vmeshlast);
  MM_WRITE(MM_PFPU_CTL, PFPU_CTL_START);

  sc = rtems_semaphore_obtain(done_sem, RTEMS_WAIT, 10);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  if (td->update)
    update_registers(td->registers);
    if (td->invalidate) {
      __asm__ volatile( /* Invalidate Level-1 data cache */
      "wcsr DCC, r0\n"
      "nop\n"
    );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver pfpu_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  args->ioctl_return = -1;
  if (args->command != PFPU_EXECUTE)
    return RTEMS_UNSATISFIED;

  if (pfpu_execute((struct pfpu_td *)args->buffer) != RTEMS_SUCCESSFUL)
    return RTEMS_UNSATISFIED;

  args->ioctl_return = 0;
  return RTEMS_SUCCESSFUL;
}
