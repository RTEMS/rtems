/* placeholder (just a shell) */

/*  SPI driver for Blackfin
 * 
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <stdlib.h>
#include <rtems.h>
#include <rtems/libi2c.h>

#include <libcpu/spiRegs.h>
#include "spi.h"


static rtems_status_code spiInit(rtems_libi2c_bus_t *bus) {
  bfin_spi_softc_t *softc;
  rtems_status_code status;

  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  status = rtems_semaphore_create(rtems_build_name('s','p','i','s'),
                                  0,
                                  RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
                                  0,
                                  &softc->irq_sema_id);

  return status;
}

static rtems_status_code spiSendStart(rtems_libi2c_bus_t *bus) {
  bfin_spi_softc_t *softc;
  rtems_status_code status;

  status = RTEMS_SUCCESSFUL;
  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  return status;
}

static rtems_status_code spiSendStop(rtems_libi2c_bus_t *bus) {
  bfin_spi_softc_t *softc;
  rtems_status_code status;

  status = RTEMS_SUCCESSFUL;
  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  return status;
}

static rtems_status_code spiSendAddr(rtems_libi2c_bus_t *bus,
                                     uint32_t addr, int rw) {
  bfin_spi_softc_t *softc;
  rtems_status_code status;

  status = RTEMS_SUCCESSFUL;
  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  return status;
}

static int spiReadBytes(rtems_libi2c_bus_t *bus,
                        unsigned char *buf, int len) {
  bfin_spi_softc_t *softc;

  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  return 0;
}

static int spiWriteBytes(rtems_libi2c_bus_t *bus,
                         unsigned char *buf, int len) {
  bfin_spi_softc_t *softc;

  softc = &(((bfin_spi_desc_t *)(bus))->softc);

  return 0;
}

static int spiIoctl(rtems_libi2c_bus_t *bus, int cmd, void *arg) {
  bfin_spi_softc_t *softc;

  softc = &(((bfin_spi_desc_t *)(bus))->softc);


  return 0;
}

void bfin_spi_isr(int source) {
}

rtems_libi2c_bus_ops_t bfin_spi_libi2c_bus_ops = {
  init: spiInit,
  send_start: spiSendStart,
  send_stop: spiSendStop,
  send_addr: spiSendAddr,
  read_bytes: spiReadBytes,
  write_bytes: spiWriteBytes,
  ioctl: spiIoctl
};

