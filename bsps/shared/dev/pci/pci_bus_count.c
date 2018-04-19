/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <rtems.h>
#include <rtems/pci.h>
#include <rtems/bspIo.h>

static uint8_t pci_number_of_buses = 0xff;

unsigned char pci_bus_count(void)
{
  if ( pci_number_of_buses != 0xff ) {
    return pci_number_of_buses;
  }

  uint8_t  bus;
  uint8_t  device;
  uint8_t  function;
  uint8_t  number_of_functions;
  uint8_t  header = 0;
  uint8_t  buses = 0;
  uint32_t vendor = 0;
  uint32_t class_rev = 0;

  pci_number_of_buses = 0;

  for (bus=0; bus < 0xff; bus++) {
    for (device=0; device < PCI_MAX_DEVICES; device++) {

       pci_read_config_dword(bus, device, 0, PCI_VENDOR_ID, &vendor);
       if ( vendor == -1 ) {
         continue;
       }

       pci_read_config_byte(bus, device, 0, PCI_HEADER_TYPE, &header);
       number_of_functions = (header & 0x80) ? PCI_MAX_FUNCTIONS : 1;

       for ( function=0; function < number_of_functions; function++ ) {
         pci_read_config_dword(bus, device, function, PCI_VENDOR_ID, &vendor);
         if ( vendor == -1 ) {
           continue;
         }

         pci_read_config_dword(bus, device, function, PCI_CLASS_REVISION, &class_rev);
         if ( (class_rev >> 16) == PCI_CLASS_BRIDGE_PCI ) {
            pci_read_config_byte(bus, device, function, PCI_SUBORDINATE_BUS, &buses);
            if ( buses > pci_number_of_buses ) {
              pci_number_of_buses = buses;
            }
         }
       }
    }
  }

  if ( pci_number_of_buses == 0 ) {
    printk("pci_bus_count() found 0 busses, assuming 1\n");
    pci_number_of_buses = 1;
  } else if ( pci_number_of_buses == 0xff ) {
    printk("pci_bus_count() found 0xff busses, assuming 1\n");
    pci_number_of_buses = 1;
  }

  return pci_number_of_buses;
}
