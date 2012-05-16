/* pcifinddevice.c
 *
 * Copyright 2001,  Till Straumann <strauman@slac.stanford.edu>
 *
 * find a particular PCI device
 * (we assume, the firmware configured the PCI bus[es] for us)
 *
 *
 * Kate Feng <feng1@bnl.gov>, modified it to support the mvme5500 board.
 *
 */

#include <bsp/pci.h>
#include <rtems/bspIo.h>
#include <bsp.h>

static int BSP_pciDebug=0;

int BSP_pciFindDevicePrint(unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun )
{
  int x;

  BSP_pciDebug = 1;
  x=pci_find_device(vendorid, deviceid, instance, pbus, pdev, pfun );
  BSP_pciDebug = 0;

  return 0;
}

int pci_find_device( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun )
{
  uint32_t d;
  unsigned short s;
  unsigned char bus,dev,fun,hd;

  for (bus=0; bus<BSP_MAX_PCI_BUS;  bus++) {
      for (dev=0; dev<PCI_MAX_DEVICES; dev++) {
	  pci_read_config_byte(bus, dev, 0, PCI_HEADER_TYPE, &hd);
	  hd = (hd & PCI_HEADER_TYPE_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
     	  for (fun=0; fun<hd; fun++) {
	      /*
	       * The last devfn id/slot is special; must skip it
	       */
	      if (PCI_MAX_DEVICES-1==dev && PCI_MAX_FUNCTIONS-1 == fun)
		 break;
	      (void)pci_read_config_dword(bus,dev,fun,PCI_VENDOR_ID,&d);
     	      if (PCI_INVALID_VENDORDEVICEID == d)
		 continue;
              if (BSP_pciDebug) {
	         printk("pci_find_device: found 0x%08x at %2d/%2d/%2d ",d,bus,dev,fun);
	         printk("(Physically: PCI%d  %2d/%2d/%2d)\n",
		     (bus>= BSP_MAX_PCI_BUS_ON_PCI0)? 1:0,
		     (bus>= BSP_MAX_PCI_BUS_ON_PCI0)? bus-BSP_MAX_PCI_BUS_ON_PCI0:bus,
		     dev, fun);
              }

	      (void)pci_read_config_word(bus,dev,fun,PCI_VENDOR_ID,&s);
      	      if (vendorid != s)
           	 continue;
	      (void)pci_read_config_word(bus,dev,fun,PCI_DEVICE_ID,&s);
	      if (deviceid == s) {
		 if (instance--) continue;
		 *pbus=bus; *pdev=dev; *pfun=fun;
		 return 0;
	      }
	  }
      }
  }  /* end for bus */
  return -1;
}

/* eof */
