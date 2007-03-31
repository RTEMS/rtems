/* pcifinddevice.c
 *
 *
 * find a particular PCI device
 * (we assume, the firmware configured the PCI bus[es] for us)
 *
 */ 

/* 
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2001,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 
/*
 * Kate Feng <feng1@bnl.gov>, modified it to support the mvme5500 board.
 * 
 */

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION			0x80


#include <bsp/pci.h>
#include <rtems/bspIo.h>
#include <bsp.h>

int BSP_pciDebug=0;

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
  uint32_t	d;
  uint16_t	s;
  unsigned char bus,dev;
  uint8_t	fun, hd;

  for (bus=0; bus<BSP_MAX_PCI_BUS;  bus++) {
      for (dev=0; dev<PCI_MAX_DEVICES; dev++) {
	  pci_read_config_byte(bus, dev, 0, PCI_HEADER_TYPE, &hd);
	  hd = (hd & PCI_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
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
