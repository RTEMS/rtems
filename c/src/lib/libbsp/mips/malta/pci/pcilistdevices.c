/**
 *  @file
 *
 *  I think this file could be made generic and put in a general pci
 *  area.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 * List all PCI Devices
 */

#define PCI_DEBUG

#include <inttypes.h>
#include <bsp/pci.h>
#include <rtems/bspIo.h>
#include <stdio.h>

/*
 * Make device signature from bus number, device number and function
 * number
 */
#define PCIB_DEVSIG_MAKE(b,d,f) ((b<<8)|(d<<3)|(f))

/*
 * Extract various parts from device signature
 */
#define PCIB_DEVSIG_BUS(x) (((x)>>8) &0xff)
#define PCIB_DEVSIG_DEV(x) (((x)>>3) & 0x1f)
#define PCIB_DEVSIG_FUNC(x) ((x) & 0x7)

static int print_device_config(
   int bus,
   int dev,
   int fun
)
{
  uint16_t vi,di;
  uint16_t cd,st;
  uint32_t b1,b2;
  uint8_t  il,ip;

  pci_read_config_word (bus, dev, fun, PCI_VENDOR_ID,      &vi);
  pci_read_config_word (bus, dev, fun, PCI_DEVICE_ID,      &di);
  pci_read_config_word (bus, dev, fun, PCI_COMMAND,        &cd);
  pci_read_config_word (bus, dev, fun, PCI_STATUS,         &st);
  pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_0, &b1);
  pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_1, &b2);
  pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_LINE, &il);
  pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_PIN,  &ip);

  printk(
    "%3d:0x%02x:%d    0x%04x-0x%04x:  0x%04x 0x%04x 0x%08" PRIx32
        " 0x%08" PRIx32 "       %d -> %3d (=0x%02x)\n",
     bus, dev, fun, vi, di, cd, st, b1, b2, ip, il, il);
  return 0;
}

void pci_list_devices( void )
{
   uint32_t d;
   unsigned char bus,dev,fun,hd;

  printk(
    "BUS:SLOT:FUN  VENDOR-DEV_ID: COMMAND STATUS BASE_ADDR0 "
    "BASE_ADDR1 IRQ_PIN -> IRQ_LINE\n"
  );
  for (bus=0 ; bus<pci_bus_count(); bus++) {
    for (dev=0 ; dev<PCI_MAX_DEVICES; dev++) {
      for (fun=0 ; fun<PCI_MAX_FUNCTIONS; fun++) {
        /*
         * The last devfn id/slot is special; must skip it
         */
        if (PCI_MAX_DEVICES-1==dev && PCI_MAX_FUNCTIONS-1 == fun)
          break;

        (void) pci_read_config_dword(bus,dev,0,PCI_VENDOR_ID,&d);
        if (PCI_INVALID_VENDORDEVICEID == d)
          continue;

        if ( 0 == fun ) {
          pci_read_config_byte(bus,dev,0, PCI_HEADER_TYPE, &hd);
          hd = (hd & PCI_HEADER_TYPE_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
        }

        (void)pci_read_config_dword(bus,dev,fun,PCI_VENDOR_ID,&d);
        if (PCI_INVALID_VENDORDEVICEID == d)
          continue;
        print_device_config( bus, dev, fun );
      }
    }
  }
}
