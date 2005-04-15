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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  Copyright 2004, Brookhaven National Laboratory and
 *                  Shuchen K. Feng, <feng1@bnl.gov>, 2004
 *   - modified and added support for MVME5500 board
 *   - added 2nd PCI support for the mvme5500/GT64260 PCI bridge
 *
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
#define PCI_PRINT 1

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION		0x80
#define HOSTBRIDGET_ERROR               0xf0000000

typedef unsigned char unchar;

#define	MAX_NUM_PCI_DEVICES	20

static int		  numPCIDevs=0;
extern void PCI_interface(), pciAccessInit();

/* Pack RegNum,FuncNum,DevNum,BusNum,and ConfigEnable for
 * PCI Configuration Address Register
 */
#define pciConfigPack(bus,dev,func,offset)\
(((func&7)<<8)|((dev&0x1f )<<11)|(( bus&0xff)<<16)|(offset&0xfc))|0x80000000

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
unchar ucMaxPCIBus=0;

/* Please note that PCI0 and PCI1 does not correlate with the busNum 0 and 1.
 */
int PCIx_read_config_byte(int npci, unchar bus, unchar dev,
unchar func, unchar offset, unchar *val)
{
  *val = 0xff;
  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;
  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  *val = inb(BSP_pci_config[npci].pci_config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

int PCIx_read_config_word(int npci, unchar bus, unchar dev,
unchar func, unchar offset, unsigned short *val)
{
  *val = 0xffff; 
  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  *val = inw(BSP_pci_config[npci].pci_config_data + (offset&2));
  return PCIBIOS_SUCCESSFUL;
}

int PCIx_read_config_dword(int npci, unchar bus, unchar dev,
unchar func, unchar offset, unsigned int *val) 
{
  *val = 0xffffffff; 
  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", BSP_pci_config[npci].pci_config_addr,
    BSP_pci_config[npci].pci_config_data,pciConfigPack(bus,dev,func,offset)); 
#endif
  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  *val = inl(BSP_pci_config[npci].pci_config_data);
  return PCIBIOS_SUCCESSFUL;
}

int PCIx_write_config_byte(int npci, unchar bus, unchar dev,
unchar func, unchar offset, unchar val) 
{
  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;

  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  outb(val, BSP_pci_config[npci].pci_config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

int PCIx_write_config_word(int npci, unchar bus, unchar dev,
unchar func, unchar offset, unsigned short val) 
{
  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  outw(val, BSP_pci_config[npci].pci_config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

int PCIx_write_config_dword(int npci,unchar bus,unchar dev,
unchar func, unchar offset, unsigned int val) 
{
  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", BSP_pci_config[npci].pci_config_addr,
    BSP_pci_config[npci].pci_config_data,pciConfigPack(bus,dev,func,offset));
#endif
  outl(pciConfigPack(bus,dev,func,offset),BSP_pci_config[npci].pci_config_addr);
  outl(val,BSP_pci_config[npci].pci_config_data);
  return PCIBIOS_SUCCESSFUL;
}

/* backwards compatible with other PPC board for the vmeUniverse.c */
int pci_read_config_byte(unchar bus, unchar dev,unchar func,unchar offset,
unchar *val)
{
  return(PCIx_read_config_byte(0, bus, dev, func, offset, val));
}

int pci_read_config_word(unchar bus, unchar dev,
unchar func, unchar offset, unsigned short *val)
{
  return(PCIx_read_config_word(0, bus, dev, func, offset, val));
}

int pci_read_config_dword(unchar bus, unchar dev,
unchar func, unchar offset, unsigned int *val) 
{
  return(PCIx_read_config_dword(0, bus, dev, func, offset, val));
}

int pci_write_config_byte(unchar bus, unchar dev,
unchar func, unchar offset, unchar val) 
{
  return(PCIx_write_config_byte(0, bus, dev, func, offset, val));
}

int pci_write_config_word(unchar bus, unchar dev,
unchar func, unchar offset, unsigned short val) 
{
  return(PCIx_write_config_word(0, bus, dev, func, offset, val));
}

int pci_write_config_dword(unchar bus,unchar dev,
unchar func, unchar offset, unsigned int val) 
{
  return(PCIx_write_config_dword(0, bus, dev, func, offset, val));
}


pci_config BSP_pci_config[2] = {
  {PCI0_CONFIG_ADDR,PCI0_CONFIG_DATA/*,&pci_functions*/},
       {PCI1_CONFIG_ADDR,PCI1_CONFIG_DATA/*,&pci_functions*/}
};

/*
 * This routine determines the maximum bus number in the system
 */
void pci_initialize()
{
  int PciNumber;
  unchar ucBusNumber, ucSlotNumber, ucFnNumber, ucNumFuncs;
  unsigned int ulHeader;
  unsigned int pcidata, ulDeviceID;
#if PCI_DEBUG
  unsigned int data, pcidata, ulClass;
  unsigned short sdata;
#endif

  PCI_interface();
  
  /*
   * Scan PCI0 and PCI1 bus0
   */
  for (PciNumber=0; PciNumber < 2; PciNumber++) {
    pciAccessInit(PciNumber);
    for (ucBusNumber=0; ucBusNumber< 2; ucBusNumber++) {
    for (ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++) {
      ucFnNumber = 0;
      PCIx_read_config_dword(PciNumber, ucBusNumber,
				ucSlotNumber,
				0,
				PCI0_VENDOR_ID,
				&ulDeviceID);

      if( ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
        /* This slot is empty */
        continue;
      }

      if (++numPCIDevs > MAX_NUM_PCI_DEVICES) {
	 BSP_panic("Too many PCI devices found; increase MAX_NUM_PCI_DEVICES in pcicache.c\n");
      }

      switch(ulDeviceID) { 
        case (PCI_VENDOR_ID_MARVELL+(PCI_DEVICE_ID_MARVELL_GT6426xAB<<16)):
#if PCI_PRINT
	  printk("Marvell GT6426xA/B hostbridge detected at PCI%d bus%d slot%d\n",
                 PciNumber,ucBusNumber,ucSlotNumber);
#endif
          ucMaxPCIBus ++;
	  break;
        case (PCI_VENDOR_ID_PLX2+(PCI_DEVICE_ID_PLX2_PCI6154_HB2<<16)):
#if PCI_PRINT
          printk("PLX PCI6154 PCI-PCI bridge detected at PCI%d bus%d slot%d\n",
                 PciNumber,ucBusNumber,ucSlotNumber);
#endif
          ucMaxPCIBus ++;
          break;
        case PCI_VENDOR_ID_TUNDRA:
#if PCI_PRINT
          printk("TUNDRA PCI-VME bridge detected at PCI%d bus%d slot%d\n",
                 PciNumber,ucBusNumber,ucSlotNumber);
#endif
          ucMaxPCIBus ++;
          break;
      case (PCI_VENDOR_ID_INTEL+(PCI_DEVICE_INTEL_82544EI_COPPER<<16)):
#if PCI_PRINT
          printk("INTEL 82544EI COPPER network controller detected at PCI%d bus%d slot%d\n",
                 PciNumber,ucBusNumber,ucSlotNumber);
#endif
          ucMaxPCIBus ++;
          break;
        default : 
#if PCI_PRINT
          printk("PCI%d Bus%d Slot%d DeviceID 0x%x \n",
             PciNumber,ucBusNumber,ucSlotNumber, ulDeviceID);
#endif
          break;
      }
#if PCI_DEBUG
      PCIx_read_config_dword(PciNumber, ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI0_BASE_ADDRESS_0,
                          &data);
      printk("PCI%d_BASE_ADDRESS_0 0x%x \n",PciNumber, data);  
      PCIx_read_config_dword(PciNumber, ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI0_BASE_ADDRESS_1,
                          &data);
      printk("PCI%d_BASE_ADDRESS_1 0x%x \n",PciNumber, data);
      PCIx_read_config_dword(PciNumber, ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI0_BASE_ADDRESS_2,
                          &data);
      printk("PCI%d_BASE_ADDRESS_2 0x%x \n",PciNumber, data);

      PCIx_read_config_dword(PciNumber, ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI0_BASE_ADDRESS_3,
                          &data);
      printk("PCI%d_BASE_ADDRESS_3 0x%x \n",PciNumber, data);  

     PCIx_read_config_word(PciNumber, ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI0_INTERRUPT_LINE,
                          &sdata);
      printk("PCI%d_INTERRUPT_LINE 0x%x \n",PciNumber, sdata);  

      /* We always enable internal memory. */
      PCIx_read_config_dword(PciNumber, ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI0_MEM_BASE_ADDR,
			  &pcidata);
      printk("PCI%d_MEM_BASE_ADDR 0x%x \n", PciNumber,pcidata);

      /* We always enable internal IO. */
      PCIx_read_config_dword(PciNumber, ucBusNumber,
 			  ucSlotNumber,
			  0,
			  PCI0_IO_BASE_ADDR,
			  &pcidata);
      printk("PCI%d_IO_BASE_ADDR 0x%x \n", PciNumber,pcidata);
#endif

      PCIx_read_config_dword(PciNumber, ucBusNumber,
			  ucSlotNumber,
			  0,
			  PCI0_CACHE_LINE_SIZE,
			  &ulHeader);
      if ((ulHeader>>16)&PCI_MULTI_FUNCTION)
         ucNumFuncs=PCI_MAX_FUNCTIONS;
      else
         ucNumFuncs=1;

#if PCI_DEBUG
      printk("PCI%d Slot 0x%x HEADER/LAT/CACHE 0x%x \n",
             PciNumber,ucSlotNumber, ulHeader);    

      for (ucFnNumber=1;ucFnNumber<ucNumFuncs;ucFnNumber++) {
          PCIx_read_config_dword(PciNumber, ucBusNumber,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI0_VENDOR_ID,
				  &ulDeviceID);
          if (ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
	     /* This slot/function is empty */
	     continue;
          }
          if (++numPCIDevs > MAX_NUM_PCI_DEVICES) {
	     BSP_panic("Too many PCI devices found; increase MAX_NUM_PCI_DEVICES in pcicache.c\n");
          }

         /* This slot/function has a device fitted.*/
         PCIx_read_config_dword(PciNumber, ucBusNumber,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI0_CLASS_REVISION,
				  &ulClass);     
         printk("PCI%d Slot 0x%x Func %d classID 0x%x \n",PciNumber,ucSlotNumber,
             ucFnNumber, ulClass);

         ulClass >>= 16;
         if (ulClass == PCI_CLASS_GT6426xAB)
	    printk("GT64260-PCI%d bridge found \n", PciNumber);
      }
#endif
      PCIx_read_config_dword(PciNumber, ucBusNumber,
			       ucSlotNumber,
			       0,
			  PCI0_COMMAND,
                          &pcidata); 
#if PCI_DEBUG
      printk("MOTLoad command staus 0x%x, ", pcidata);
#endif
      /* Clear the error on the host bridge */
      if ( (ucBusNumber==0) && (ucSlotNumber==0))
	pcidata |= PCI_STATUS_CLRERR_MASK;
      /* Enable bus,I/O and memory master access. */
      pcidata |= (PCI_COMMAND_MASTER|PCI_COMMAND_IO|PCI_COMMAND_MEMORY);
      PCIx_write_config_dword(PciNumber, ucBusNumber,
 			       ucSlotNumber,
			       0,
			  PCI0_COMMAND,
                          pcidata);

      PCIx_read_config_dword(PciNumber, ucBusNumber,
			       ucSlotNumber,
			       0,
			  PCI0_COMMAND,
                          &pcidata); 
#if PCI_DEBUG      
      printk("Now command/staus 0x%x\n", pcidata);
#endif
 
    }
    }
  } /* PCI number */
}

/*
 * Return the number of PCI buses in the system
 */
unsigned char BusCountPCI()
{
  return(ucMaxPCIBus);
}

