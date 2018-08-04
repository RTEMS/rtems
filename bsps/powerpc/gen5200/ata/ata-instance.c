/*
 * Copyright (c) 2011-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/ata.h>
#include <bsp/fatal.h>

#include <libchip/ata.h>

#include <rtems/bdbuf.h>

#include <bsp.h>

static ata_driver_dma_pio_single ata_driver_instance;

rtems_status_code rtems_ata_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor_arg,
  void *arg
)
{
  rtems_status_code sc = rtems_bdbuf_init();

  if (sc == RTEMS_SUCCESSFUL) {
    bestcomm_glue_init();

    ata_driver_dma_pio_single_create(&ata_driver_instance, "/dev/hda", TASK_PCI_TX);
  } else {
    bsp_fatal(MPC5200_FATAL_ATA_DISK_IO_INIT);
  }

  return sc;
}
