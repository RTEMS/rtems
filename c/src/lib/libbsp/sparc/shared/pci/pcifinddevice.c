/* Author: Till Straumann <strauman@slac.stanford.edu>, 2001 */

/* find a particular PCI device
 * (we assume, the firmware configured the PCI bus[es] for us)
 */

#include <pci.h>
#include <rtems/bspIo.h>

int
BSP_pciFindDevice( unsigned short vendorid, unsigned short deviceid,
                   int instance, int *pbus, int *pdev, int *pfun )
{
   uint32_t d;
   unsigned short s;
   unsigned char bus,dev,fun,hd;

	for (bus=0; bus<BusCountPCI(); bus++) {
      for (dev=0; dev<PCI_MAX_DEVICES; dev++) {

		pci_read_config_byte(bus,dev,0, PCI_HEADER_TYPE, &hd);
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
#ifdef PCI_DEBUG
			printk("BSP_pciFindDevice: found 0x%08x at %d/%d/%d\n",d,bus,dev,fun);
#endif
			(void) pci_read_config_word(bus,dev,fun,PCI_VENDOR_ID,&s);
			if (vendorid != s)
				continue;
			(void) pci_read_config_word(bus,dev,fun,PCI_DEVICE_ID,&s);
			if (deviceid == s) {
				if (instance--) continue;
				*pbus=bus; *pdev=dev; *pfun=fun;
				return 0;
			}
		}
      }
	}
    return -1;
}

/* eof */
