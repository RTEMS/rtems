/*  PCI.h
 *
 *  This include file contains prototypes for chips attached to the
 *  PCI bus.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: 
 */
#ifndef __PCI_h
#define __PCI_h

/*
 * PCI.c
 */

void PCI_bus_write(
  volatile rtems_unsigned32 * _addr, 
  rtems_unsigned32 _data 
); 

rtems_unsigned32 PCI_bus_read(
  volatile rtems_unsigned32 *  _addr 
);

rtems_unsigned32 Read_pci_device_register(
  rtems_unsigned32 address
);

void  Write_pci_device_register(
  rtems_unsigned32 address,
  rtems_unsigned32 data 
);

#endif

