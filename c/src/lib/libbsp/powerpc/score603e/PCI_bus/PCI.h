/**
 *  @file
 *
 *  This include file contains prototypes for chips attached to the
 *  PCI bus.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __PCI_h
#define __PCI_h

/*
 * PCI.c
 */

void PCI_bus_write(
  volatile uint32_t         * _addr,
  uint32_t         _data
);

uint32_t         PCI_bus_read(
  volatile uint32_t         *  _addr
);

uint32_t         Read_pci_device_register(
  uint32_t         address
);

void  Write_pci_device_register(
  uint32_t         address,
  uint32_t         data
);

#endif
