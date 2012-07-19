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
 *  Copyright 2004, 2008 Brookhaven National Laboratory and
 *                  Shuchen K. Feng, <feng1@bnl.gov>
 *
 *   - to be consistent with the original pci.c written by Eric Valette
 *   - added 2nd PCI support for discovery based PCI bridge (e.g. mvme5500/mvme6100)
 *   - added bus support for the expansion of PMCSpan as per request by Peter
 */
#define PCI_MAIN

#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include <bsp/irq.h>
#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gtpcireg.h>
#include <bsp.h>

#include <stdio.h>
#include <string.h>

#define PCI_DEBUG 0
#define PCI_PRINT 1

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

#define HOSTBRIDGET_ERROR               0xf0000000

#define GT64x60_PCI_CONFIG_ADDR         GT64x60_REG_BASE + PCI_CONFIG_ADDR
#define GT64x60_PCI_CONFIG_DATA         GT64x60_REG_BASE + PCI_CONFIG_DATA

#define GT64x60_PCI1_CONFIG_ADDR        GT64x60_REG_BASE + PCI1_CONFIG_ADDR
#define GT64x60_PCI1_CONFIG_DATA        GT64x60_REG_BASE + PCI1_CONFIG_DATA

static int      numPCIDevs=0;
static DiscoveryChipVersion BSP_sysControllerVersion = 0;
static BSP_VMEchipTypes BSP_VMEinterface = 0;
static rtems_pci_config_t BSP_pci[2]={
  {(volatile unsigned char*) (GT64x60_PCI_CONFIG_ADDR),
   (volatile unsigned char*) (GT64x60_PCI_CONFIG_DATA),
   0 /* defined at BSP_pci_configuration */},
  {(volatile unsigned char*) (GT64x60_PCI1_CONFIG_ADDR),
   (volatile unsigned char*) (GT64x60_PCI1_CONFIG_DATA),
   0 /* defined at BSP_pci_configuration */}
};

extern void pci_interface(void);

/* Pack RegNum,FuncNum,DevNum,BusNum,and ConfigEnable for
 * PCI Configuration Address Register
 */
#define pciConfigPack(bus,dev,func,offset)\
((offset&~3)<<24)|(PCI_DEVFN(dev,func)<<16)|(bus<<8)|0x80

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
static unsigned char ucMaxPCIBus=0;

/* Please note that PCI0 and PCI1 does not correlate with the busNum 0 and 1.
 */
static int indirect_pci_read_config_byte(unsigned char bus,unsigned char dev,unsigned char func,
unsigned char offset, uint8_t *val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  *val = 0xff;
  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", BSP_pci[n].pci_config_addr),
    BSP_pci[n].config_data,pciConfigPack(bus,dev,func,offset));
#endif

  out_be32((volatile unsigned int *) BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  *val = in_8(BSP_pci[n].pci_config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

static int indirect_pci_read_config_word(unsigned char bus, unsigned char dev,
unsigned char func, unsigned char offset, uint16_t *val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  *val = 0xffff;
  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;
#if 0
  printk("addr %x, data %x, pack %x \n", config_addr,
    config_data,pciConfigPack(bus,dev,func,offset));
#endif
  out_be32((volatile unsigned int *) BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  *val = in_le16((volatile unsigned short *) (BSP_pci[n].pci_config_data + (offset&2)));
  return PCIBIOS_SUCCESSFUL;
}

static int indirect_pci_read_config_dword(unsigned char bus, unsigned char dev,
unsigned char func, unsigned char offset, uint32_t *val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  *val = 0xffffffff;
  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;

  out_be32((volatile unsigned int *)BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  *val = in_le32((volatile unsigned int *)BSP_pci[n].pci_config_data);
  return PCIBIOS_SUCCESSFUL;
}

static int indirect_pci_write_config_byte(unsigned char bus, unsigned char dev,unsigned char func, unsigned char offset, uint8_t val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  if (offset & ~0xff) return PCIBIOS_BAD_REGISTER_NUMBER;

  out_be32((volatile unsigned int *)BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  out_8((volatile unsigned char *) (BSP_pci[n].pci_config_data + (offset&3)), val);
  return PCIBIOS_SUCCESSFUL;
}

static int indirect_pci_write_config_word(unsigned char bus, unsigned char dev,unsigned char func, unsigned char offset, uint16_t val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  if ((offset&1)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;

  out_be32((volatile unsigned int *)BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  out_le16((volatile unsigned short *)(BSP_pci[n].pci_config_data + (offset&3)), val);
  return PCIBIOS_SUCCESSFUL;
}

static int indirect_pci_write_config_dword(unsigned char bus,unsigned char dev,unsigned char func, unsigned char offset, uint32_t val)
{
  int n=0;

  if (bus>= BSP_MAX_PCI_BUS_ON_PCI0) {
     bus-=BSP_MAX_PCI_BUS_ON_PCI0;
     n=1;
  }

  if ((offset&3)|| (offset & ~0xff)) return PCIBIOS_BAD_REGISTER_NUMBER;

  out_be32((volatile unsigned int *)BSP_pci[n].pci_config_addr, pciConfigPack(bus,dev,func,offset));
  out_le32((volatile unsigned int *)BSP_pci[n].pci_config_data, val);
  return PCIBIOS_SUCCESSFUL;
}

const pci_config_access_functions pci_indirect_functions = {
  	indirect_pci_read_config_byte,
  	indirect_pci_read_config_word,
  	indirect_pci_read_config_dword,
  	indirect_pci_write_config_byte,
  	indirect_pci_write_config_word,
  	indirect_pci_write_config_dword
};


rtems_pci_config_t BSP_pci_configuration = {
           (volatile unsigned char*) (GT64x60_PCI_CONFIG_ADDR),
	   (volatile unsigned char*) (GT64x60_PCI_CONFIG_DATA),
	   &pci_indirect_functions};

DiscoveryChipVersion BSP_getDiscoveryChipVersion(void)
{
  return(BSP_sysControllerVersion);
}

BSP_VMEchipTypes BSP_getVMEchipType(void)
{
  return(BSP_VMEinterface);
}

/*
 * This routine determines the maximum bus number in the system.
 * The PCI_SUBORDINATE_BUS is not supported in GT6426xAB. Thus,
 * it's not used.
 *
 */
int pci_initialize(void)
{
  int deviceFound;
  unsigned char ucBusNumber, ucSlotNumber, ucFnNumber, ucNumFuncs, data8;
  uint32_t ulHeader, ulClass, ulDeviceID;
#if PCI_DEBUG
  uint32_t pcidata;
#endif

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
          pci_read_config_byte(0,0,0,PCI_REVISION_ID, &data8);
          switch(data8) {
	  case 0x10:
	    BSP_sysControllerVersion = GT64260A;
#if PCI_PRINT
	    printk("Marvell GT64260A (Discovery I) hostbridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
	    break;
          case 0x20:
	    BSP_sysControllerVersion = GT64260B;
#if PCI_PRINT
	    printk("Marvell GT64260B (Discovery I) hostbridge detected at bus%d slot%d\n",
                 ucBusNumber,ucSlotNumber);
#endif
	    break;
	  default:
	    printk("Undefined revsion of GT64260 chip\n");
            break;
          }
	  break;
        case PCI_VENDOR_ID_TUNDRA:
#if PCI_PRINT
          printk("TUNDRA PCI-VME bridge detected at bus%d slot%d\n",
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
#if PCI_PRINT
          printk("BSP unlisted vendor, Bus%d Slot%d DeviceID 0x%x \n",
             ucBusNumber,ucSlotNumber, ulDeviceID);
#endif
	  /* Kate Feng : device not supported by BSP needs to remap the IRQ line on mvme5500/mvme6100 */
          pci_read_config_byte(ucBusNumber,ucSlotNumber,0,PCI_INTERRUPT_LINE,&data8);
	  if (data8 < BSP_GPP_IRQ_LOWEST_OFFSET)  pci_write_config_byte(ucBusNumber,
 	     ucSlotNumber,0,PCI_INTERRUPT_LINE,BSP_GPP_IRQ_LOWEST_OFFSET+data8);

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
      if ((ulHeader>>16)&PCI_HEADER_TYPE_MULTI_FUNCTION)
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
    }
    if (deviceFound) ucMaxPCIBus++;
  } /* for (ucBusNumber=0; ucBusNumber<BSP_MAX_PCI_BUS; ... */
#if PCI_DEBUG
  printk("number of PCI buses: %d, numPCIDevs %d\n",
	 pci_bus_count(), numPCIDevs);
#endif
  pci_interface();
  return(0);
}

void FixupPCI( const struct _int_map *bspmap, int (*swizzler)(int,int) )
{
}

/*
 * Return the number of PCI buses in the system
 */
unsigned char pci_bus_count()
{
  return(ucMaxPCIBus);
}

