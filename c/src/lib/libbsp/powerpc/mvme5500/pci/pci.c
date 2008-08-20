/*
 * pci.c :  this file contains basic PCI Io functions.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavilly inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/rtems/license.html.
 *
 *  pci.c,v 1.2 2002/05/14 17:10:16 joel Exp
 *
 *  Copyright 2004, Brookhaven National Laboratory and
 *                  Shuchen K. Feng, <feng1@bnl.gov>, 2004
 *   - modified and added support for MVME5500 board
 *   - added 2nd PCI support for the mvme5500/GT64260 PCI bridge
 *   - added bus support for the expansion of PMCSpan, thanks to 
 *     Peter Dufault (dufault@hda.com) for inputs.
 *
 * $Id$
 */
#define PCI_MAIN

#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gtpcireg.h> 

#include <stdio.h>
#include <string.h>

#define PCI_DEBUG 0
#define PCI_PRINT 0

/* allow for overriding these definitions */
#ifndef PCI_CONFIG_ADDR
#define PCI_CONFIG_ADDR			0xcf8
#endif
#ifndef PCI_CONFIG_DATA
#define PCI_CONFIG_DATA			0xcfc
#endif

#ifndef PCI1_CONFIG_ADDR
#define PCI1_CONFIG_ADDR		0xc78
#endif
#ifndef PCI1_CONFIG_DATA
#define PCI1_CONFIG_DATA	        0xc7c
#endif

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION		0x80
#define HOSTBRIDGET_ERROR               0xf0000000

/* define a shortcut */
#define pci	BSP_pci_configuration

static int		  numPCIDevs=0;
extern void pci_interface();

/* Pack RegNum,FuncNum,DevNum,BusNum,and ConfigEnable for
 * PCI Configuration Address Register
 */
#define pciConfigPack(bus,dev,func,offset)\
(((func&7)<<8)|((dev&0x1f )<<11)|(( bus&0xff)<<16)|(offset&0xfc))|0x80000000

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
unsigned char ucMaxPCIBus=0;

/* Please note that PCI0 and PCI1 does not correlate with the busNum 0 and 1.
 */
static int direct_pci_read_config_byte(unsigned char bus,unsigned char dev,unsigned char func,
unsigned char offset,unsigned char *val)
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char*) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char*) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = pci.pci_config_addr;
     config_data = pci.pci_config_data;
  }
  *val = 0xff;
  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif
  outl(pciConfigPack(bus,dev,func,offset),config_addr);
  *val = inb(config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

static int direct_pci_read_config_word(unsigned char bus, unsigned char dev,
unsigned char func, unsigned char offset, unsigned short *val)
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char*) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char*) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = (volatile unsigned char*) pci.pci_config_addr;
     config_data = (volatile unsigned char*) pci.pci_config_data;
  }

  *val = 0xffff; 
  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif
  outl(pciConfigPack(bus,dev,func,offset),config_addr);
  *val = inw(config_data + (offset&2));
  return PCIBIOS_SUCCESSFUL;
}

static int direct_pci_read_config_dword(unsigned char bus, unsigned char dev,
unsigned char func, unsigned char offset, unsigned int *val) 
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char*) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char*) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = (volatile unsigned char*) pci.pci_config_addr;
     config_data = (volatile unsigned char*) pci.pci_config_data;
  }

  *val = 0xffffffff; 
  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    pci.pci_config_data,pciConfigPack(bus,dev,func,offset)); 
#endif
  outl(pciConfigPack(bus,dev,func,offset),config_addr);
  *val = inl(config_data);
  return PCIBIOS_SUCCESSFUL;
}

static int direct_pci_write_config_byte(unsigned char bus, unsigned char dev,unsigned char func, unsigned char offset, unsigned char val) 
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char*) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char*) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = pci.pci_config_addr;
     config_data = pci.pci_config_data;
  }

  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif

  outl(pciConfigPack(bus,dev,func,offset), config_addr);
  outb(val, config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

static int direct_pci_write_config_word(unsigned char bus, unsigned char dev,unsigned char func, unsigned char offset, unsigned short val) 
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char*) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char*) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = (volatile unsigned char*) pci.pci_config_addr;
     config_data = (volatile unsigned char*) pci.pci_config_data;
  }

  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif
  outl(pciConfigPack(bus,dev,func,offset),config_addr);
  outw(val, config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

static int direct_pci_write_config_dword(unsigned char bus,unsigned char dev,unsigned char func, unsigned char offset, unsigned int val) 
{
  volatile unsigned char *config_addr, *config_data;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     config_addr = (volatile unsigned char *) PCI1_CONFIG_ADDR;
     config_data = (volatile unsigned char *) PCI1_CONFIG_DATA;
  }
  else {
     config_addr = (volatile unsigned char*) pci.pci_config_addr;
     config_data = (volatile unsigned char*) pci.pci_config_data;
  }

  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif
  outl(pciConfigPack(bus,dev,func,offset),config_addr);
  outl(val,config_data);
  return PCIBIOS_SUCCESSFUL;
}

const pci_config_access_functions pci_direct_functions = {
  	direct_pci_read_config_byte,
  	direct_pci_read_config_word,
  	direct_pci_read_config_dword,
  	direct_pci_write_config_byte,
  	direct_pci_write_config_word,
  	direct_pci_write_config_dword
};


pci_config BSP_pci_configuration = {(volatile unsigned char*) PCI_CONFIG_ADDR,
			 (volatile unsigned char*)PCI_CONFIG_DATA,
				    &pci_direct_functions};

/*
 * This routine determines the maximum bus number in the system.
 * The PCI_SUBORDINATE_BUS is not supported in GT6426xAB. Thus,
 * it's not used.
 *
 */
int pci_initialize()
{
  int deviceFound;
  unsigned char ucBusNumber, ucSlotNumber, ucFnNumber, ucNumFuncs;
  unsigned int ulHeader;
  unsigned int pcidata, ulClass, ulDeviceID;

  pci_interface();
  
  /*
   * Scan PCI0 and PCI1 buses
   */
  for (ucBusNumber=0; ucBusNumber<BSP_MAX_PCI_BUS; ucBusNumber++) {
    deviceFound=0;
    for (ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++) {
      ucFnNumber = 0;
      pci_read_config_dword(ucBusNumber,
				ucSlotNumber,
				0,
				PCI_VENDOR_ID,
				&ulDeviceID);

      if( ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
        /* This slot is empty */
        continue;
      }

      if (++numPCIDevs > PCI_MAX_DEVICES) {
	 BSP_panic("Too many PCI devices found; increase PCI_MAX_DEVICES in pci.h\n");
      }

      if (!deviceFound) deviceFound=1;
      switch(ulDeviceID) { 
        case (PCI_VENDOR_ID_MARVELL+(PCI_DEVICE_ID_MARVELL_GT6426xAB<<16)):
#if PCI_PRINT
	  printk("Marvell GT6426xA/B hostbridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
	  break;
        case (PCI_VENDOR_ID_PLX2+(PCI_DEVICE_ID_PLX2_PCI6154_HB2<<16)):
#if PCI_PRINT
          printk("PLX PCI6154 PCI-PCI bridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
          break;
        case PCI_VENDOR_ID_TUNDRA:
#if PCI_PRINT
          printk("TUNDRA PCI-VME bridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
          break;
      case (PCI_VENDOR_ID_INTEL+(PCI_DEVICE_INTEL_82544EI_COPPER<<16)):
#if PCI_PRINT
          printk("INTEL 82544EI COPPER network controller detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
          break;
      case (PCI_VENDOR_ID_DEC+(PCI_DEVICE_ID_DEC_21150<<16)):
 #if PCI_PRINT
          printk("DEC21150 PCI-PCI bridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
	  break;
       default : 
          printk("BSP unlisted vendor, Bus%d Slot%d DeviceID 0x%x \n",
             ucBusNumber,ucSlotNumber, ulDeviceID);
          break;
      }

#if PCI_DEBUG
      pci_read_config_dword(ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI_BASE_ADDRESS_0,
                          &data);
      printk("Bus%d BASE_ADDRESS_0 0x%x \n",ucBusNumber, data);  
      pci_read_config_dword(ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI_BASE_ADDRESS_1,
                          &data);
      printk("Bus%d BASE_ADDRESS_1 0x%x \n",ucBusNumber, data);
      pci_read_config_dword(ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI_BASE_ADDRESS_2,
                          &data);
      printk("Bus%d BASE_ADDRESS_2 0x%x \n", ucBusNumber, data);

      pci_read_config_dword(ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI_BASE_ADDRESS_3,
                          &data);
      printk("Bus%d BASE_ADDRESS_3 0x%x \n", ucBusNumber, data);  

      pci_read_config_word(ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI_INTERRUPT_LINE,
                          &sdata);
      printk("Bus%d INTERRUPT_LINE 0x%x \n", ucBusNumber, sdata);  

      /* We always enable internal memory. */
      pci_read_config_dword(ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI_MEM_BASE_ADDR,
			  &pcidata);
      printk("Bus%d MEM_BASE_ADDR 0x%x \n", ucBusNumber,pcidata);

      /* We always enable internal IO. */
      pci_read_config_dword(ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI_IO_BASE_ADDR,
			  &pcidata);
      printk("Bus%d IO_BASE_ADDR 0x%x \n", ucBusNumber,pcidata);
#endif

      pci_read_config_dword(ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI_CACHE_LINE_SIZE,
			  &ulHeader);
      if ((ulHeader>>16)&PCI_MULTI_FUNCTION)
         ucNumFuncs=PCI_MAX_FUNCTIONS;
      else
         ucNumFuncs=1;

#if PCI_DEBUG
      printk("Bus%d Slot 0x%x HEADER/LAT/CACHE 0x%x \n",
             ucBusNumber, ucSlotNumber, ulHeader);    
#endif

      for (ucFnNumber=1;ucFnNumber<ucNumFuncs;ucFnNumber++) {
          pci_read_config_dword(ucBusNumber,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI_VENDOR_ID,
				  &ulDeviceID);
          if (ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
	     /* This slot/function is empty */
	     continue;
          }

         /* This slot/function has a device fitted.*/
         pci_read_config_dword(ucBusNumber,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI_CLASS_REVISION,
				  &ulClass); 
#if PCI_DEBUG    
         printk("Bus%d Slot 0x%x Func %d classID 0x%x \n",ucBusNumber,ucSlotNumber,
             ucFnNumber, ulClass);
#endif

      }

      pci_read_config_dword(ucBusNumber,
			       ucSlotNumber,
			       0,
			  PCI_COMMAND,
                          &pcidata); 
#if PCI_DEBUG
      printk("MOTLoad command staus 0x%x, ", pcidata);
#endif
      /* Clear the error on the host bridge */
      if ( (ucBusNumber==0) && (ucSlotNumber==0))
	pcidata |= PCI_STATUS_CLRERR_MASK;
      /* Enable bus,I/O and memory master access. */
      pcidata |= (PCI_COMMAND_MASTER|PCI_COMMAND_IO|PCI_COMMAND_MEMORY);
      pci_write_config_dword(ucBusNumber,
 			       ucSlotNumber,
			       0,
			  PCI_COMMAND,
                          pcidata);

      pci_read_config_dword(ucBusNumber,
			       ucSlotNumber,
			       0,
			  PCI_COMMAND,
                          &pcidata); 
#if PCI_DEBUG      
      printk("Now command/staus 0x%x\n", pcidata);
#endif
    }
    if (deviceFound) ucMaxPCIBus++;
  } /* for (ucBusNumber=0; ucBusNumber<BSP_MAX_PCI_BUS; ... */
#if PCI_DEBUG
  printk("number of PCI buses: %d, numPCIDevs %d\n", 
	 pci_bus_count(), numPCIDevs);
#endif
  return(0);
}

void FixupPCI( struct _int_map *bspmap, int (*swizzler)(int,int) )
{
}

/*
 * Return the number of PCI buses in the system
 */
unsigned char pci_bus_count()
{
  return(ucMaxPCIBus);
}

