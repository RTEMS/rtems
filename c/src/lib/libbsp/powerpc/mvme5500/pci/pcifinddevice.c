/* pcifinddevice.c
 *
 * Copyright 2001,  Till Straumann <strauman@slac.stanford.edu> 
 *
 * find a particular PCI device
 * (we assume, the firmware configured the PCI bus[es] for us)
 *
 * 
 * Kate Feng <feng1@bnl.gov>, modified it to support
 * the mvme5500 board and provided glues to Till's vmeUniverse.c.
 *
 */

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION			0x80

/*#define PCI_DEBUG*/

#include <bsp/pci.h>
#include <rtems/bspIo.h>

int BSP_PCIxFindDevice(unsigned short vendorid, unsigned short deviceid,
     int instance, int pciNum, int *pbus, int *pdev, int *pfun )
{
  unsigned int d;
  unsigned short s;
  unsigned char bus,dev,fun,hd;

  for (bus=0; bus<2; bus++) {
      for (dev=0; dev<PCI_MAX_DEVICES; dev++) {
	  PCIx_read_config_byte(pciNum, bus, dev, 0, PCI0_HEADER_TYPE, &hd);
	  hd = (hd & PCI_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
     	  for (fun=0; fun<hd; fun++) {
	      /* 
	       * The last devfn id/slot is special; must skip it
	       */
	      if (PCI_MAX_DEVICES-1==dev && PCI_MAX_FUNCTIONS-1 == fun)
		 break;
	      (void)PCIx_read_config_dword(pciNum, bus,dev,fun,PCI0_VENDOR_ID,&d);
     	      if (PCI_INVALID_VENDORDEVICEID == d)
		 continue;
#ifdef PCI_DEBUG
	      printk("BSP_pciFindDevice: found 0x%08x at %d/%d/%d\n",d,bus,dev,fun);
#endif
	      (void)PCIx_read_config_word(pciNum, bus,dev,fun,PCI0_VENDOR_ID,&s);
      	      if (vendorid != s)
           	 continue;
	      (void)PCIx_read_config_word(pciNum, bus,dev,fun,PCI0_DEVICE_ID,&s);
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

int BSP_pciFindDevice( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun )
{
  return(BSP_PCIxFindDevice(vendorid,deviceid,instance,0,pbus,pdev,pfun));
}

/* eof */
