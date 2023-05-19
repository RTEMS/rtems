/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2013 embedded brains GmbH & Co. KG
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
