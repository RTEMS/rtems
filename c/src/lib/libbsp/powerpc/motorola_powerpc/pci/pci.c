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
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp/consoleIo.h>
#include <libcpu/io.h>
#include <bsp/pci.h>
#include <bsp/residual.h>
#include <bsp/openpic.h>
#include <bsp.h>

#define PCI_CONFIG_ADDR			0xcf8
#define PCI_CONFIG_DATA			0xcfc
#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION		0x80
#define RAVEN_MPIC_IOSPACE_ENABLE	0x1
#define RAVEN_MPIC_MEMSPACE_ENABLE	0x2
#define RAVEN_MASTER_ENABLE		0x4
#define RAVEN_PARITY_CHECK_ENABLE	0x40
#define RAVEN_SYSTEM_ERROR_ENABLE	0x100
#define RAVEN_CLEAR_EVENTS_MASK		0xf9000000


/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
unsigned char ucMaxPCIBus;

static int
indirect_pci_read_config_byte(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, unsigned char *val) {
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	*val = in_8(pci.pci_config_data + (offset&3));
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_word(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, unsigned short *val) {
	*val = 0xffff; 
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	*val = in_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)));
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_dword(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, unsigned int *val) {
	*val = 0xffffffff; 
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|(offset<<24));
	*val = in_le32((volatile unsigned int *)pci.pci_config_data);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_byte(unsigned char bus, unsigned char slot,
			       unsigned char function, 
			       unsigned char offset, unsigned char val) {
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	out_8(pci.pci_config_data + (offset&3), val);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_word(unsigned char bus, unsigned char slot,
			       unsigned char function, 
			       unsigned char offset, unsigned short val) {
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|((offset&~3)<<24));
	out_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)), val);
	return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_dword(unsigned char bus, unsigned char slot,
				unsigned char function, 
				unsigned char offset, unsigned int val) {
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	out_be32((unsigned int*) pci.pci_config_addr, 
		 0x80|(bus<<8)|(PCI_DEVFN(slot,function)<<16)|(offset<<24));
	out_le32((volatile unsigned int *)pci.pci_config_data, val);
	return PCIBIOS_SUCCESSFUL;
}

static const pci_config_access_functions indirect_functions = {
  	indirect_pci_read_config_byte,
  	indirect_pci_read_config_word,
  	indirect_pci_read_config_dword,
  	indirect_pci_write_config_byte,
  	indirect_pci_write_config_word,
  	indirect_pci_write_config_dword
};

pci_config pci = {(volatile unsigned char*)PCI_CONFIG_ADDR,
			 (volatile unsigned char*)PCI_CONFIG_DATA,
			 &indirect_functions};

static int
direct_pci_read_config_byte(unsigned char bus, unsigned char slot,
			    unsigned char function, 
			    unsigned char offset, unsigned char *val) {
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
		*val=0xff;
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	*val=in_8(pci.pci_config_data + ((1<<slot)&~1) 
		  + (function<<8) + offset);
	return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_read_config_word(unsigned char bus, unsigned char slot,
			    unsigned char function, 
			    unsigned char offset, unsigned short *val) {
	*val = 0xffff; 
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	*val=in_le16((volatile unsigned short *)
		     (pci.pci_config_data + ((1<<slot)&~1)
		      + (function<<8) + offset));
	return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_read_config_dword(unsigned char bus, unsigned char slot,
			     unsigned char function, 
			     unsigned char offset, unsigned int *val) {
	*val = 0xffffffff; 
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	*val=in_le32((volatile unsigned int *)
		     (pci.pci_config_data + ((1<<slot)&~1)
		      + (function<<8) + offset));
	return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_byte(unsigned char bus, unsigned char slot,
			     unsigned char function, 
			     unsigned char offset, unsigned char val) {
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	out_8(pci.pci_config_data + ((1<<slot)&~1) 
	      + (function<<8) + offset, 
	      val);
	return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_word(unsigned char bus, unsigned char slot,
			     unsigned char function, 
			     unsigned char offset, unsigned short val) {
	if (offset&1) return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	out_le16((volatile unsigned short *)
		 (pci.pci_config_data + ((1<<slot)&~1)
		  + (function<<8) + offset),
		 val);
	return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_dword(unsigned char bus, unsigned char slot,
			      unsigned char function, 
			      unsigned char offset, unsigned int val) {
	if (offset&3) return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus != 0 || (1<<slot & 0xff8007fe)) {
 		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	out_le32((volatile unsigned int *)
		 (pci.pci_config_data + ((1<<slot)&~1)
		  + (function<<8) + offset),
		 val);
	return PCIBIOS_SUCCESSFUL;
}

static const pci_config_access_functions direct_functions = {
  	direct_pci_read_config_byte,
  	direct_pci_read_config_word,
  	direct_pci_read_config_dword,
  	direct_pci_write_config_byte,
  	direct_pci_write_config_word,
  	direct_pci_write_config_dword
};


void detect_host_bridge()
{
  PPC_DEVICE *hostbridge;
  unsigned int id0;
  unsigned int tmp;
  
  /*
   * This code assumes that the host bridge is located at
   * bus 0, dev 0, func 0 AND that the old pre PCI 2.1
   * standart devices detection mecahnism that was used on PC
   * (still used in BSD source code) works.
   */
  hostbridge=residual_find_device(&residualCopy, PROCESSORDEVICE, NULL, 
				  BridgeController,
				  PCIBridge, -1, 0);
  if (hostbridge) {
    if (hostbridge->DeviceId.Interface==PCIBridgeIndirect) {
      pci.pci_functions=&indirect_functions;
      /* Should be extracted from residual data, 
       * indeed MPC106 in CHRP mode is different,
       * but we should not use residual data in
       * this case anyway. 
       */
      pci.pci_config_addr = ((volatile unsigned char *) 
			      (ptr_mem_map->io_base+0xcf8));
      pci.pci_config_data = ptr_mem_map->io_base+0xcfc;
    } else if(hostbridge->DeviceId.Interface==PCIBridgeDirect) {
      pci.pci_functions=&direct_functions;
      pci.pci_config_data=(unsigned char *) 0x80800000;
    } else {
    }
  } else {
    /* Let us try by experimentation at our own risk! */
    pci.pci_functions = &direct_functions;
    /* On all direct bridges I know the host bridge itself
     * appears as device 0 function 0. 
		 */
    pci_read_config_dword(0, 0, 0, PCI_VENDOR_ID, &id0);
    if (id0==~0U) {
      pci.pci_functions = &indirect_functions;
      pci.pci_config_addr = ((volatile unsigned char*)
			      (ptr_mem_map->io_base+0xcf8));
      pci.pci_config_data = ((volatile unsigned char*)ptr_mem_map->io_base+0xcfc);
    }
    /* Here we should check that the host bridge is actually
     * present, but if it not, we are in such a desperate
     * situation, that we probably can't even tell it.
     */
  }
  pci_read_config_dword(0, 0, 0, 0, &id0);
  if(id0 == PCI_VENDOR_ID_MOTOROLA +
     (PCI_DEVICE_ID_MOTOROLA_RAVEN<<16)) {
    /*
     * We have a Raven bridge. We will get information about its settings
     */
    pci_read_config_dword(0, 0, 0, PCI_COMMAND, &id0);
    printk("RAVEN PCI command register = %x\n",id0);
    id0 |= RAVEN_CLEAR_EVENTS_MASK;
    pci_write_config_dword(0, 0, 0, PCI_COMMAND, id0);
    pci_read_config_dword(0, 0, 0, PCI_COMMAND, &id0);
    printk("After error clearing RAVEN PCI command register = %x\n",id0);
    
    if (id0 & RAVEN_MPIC_IOSPACE_ENABLE) {
      pci_read_config_dword(0, 0, 0,PCI_BASE_ADDRESS_0, &tmp);
      printk("Raven MPIC is accessed via IO Space Access at address : %x\n",(tmp & ~0x1));
    }
    if (id0 & RAVEN_MPIC_MEMSPACE_ENABLE) {
      pci_read_config_dword(0, 0, 0,PCI_BASE_ADDRESS_1, &tmp);
      printk("Raven MPIC is accessed via memory Space Access at address : %x\n", tmp);
      OpenPIC=(volatile struct OpenPIC *) (tmp + PREP_ISA_MEM_BASE);
      printk("OpenPIC found at %p.\n",
	     OpenPIC);
    }
  }
  else {
    BSP_panic("OpenPic Not found\n");
  }

}

/*
 * This routine determines the maximum bus number in the system
 */
void InitializePCI()
{
  unsigned char ucSlotNumber, ucFnNumber, ucNumFuncs;
  unsigned char ucHeader;
  unsigned char ucMaxSubordinate;
  unsigned int  ulClass, ulDeviceID;

  detect_host_bridge();
  /*
   * Scan PCI bus 0 looking for PCI-PCI bridges
   */
  for(ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++) {
    (void)pci_read_config_dword(0,
				ucSlotNumber,
				0,
				PCI_VENDOR_ID,
				&ulDeviceID);
    if(ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
      /*
       * This slot is empty
       */
      continue;
    }
    (void)pci_read_config_byte(0,
			       ucSlotNumber,
			       0,
			       PCI_HEADER_TYPE,
			       &ucHeader);
    if(ucHeader&PCI_MULTI_FUNCTION)	{
      ucNumFuncs=PCI_MAX_FUNCTIONS;
    }
    else {
      ucNumFuncs=1;
    }
    for(ucFnNumber=0;ucFnNumber<ucNumFuncs;ucFnNumber++) {
      (void)pci_read_config_dword(0,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI_VENDOR_ID,
				  &ulDeviceID);
      if(ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
				/*
				 * This slot/function is empty
				 */
	continue;
      }

      /*
       * This slot/function has a device fitted.
       */
      (void)pci_read_config_dword(0,
				  ucSlotNumber,
				  ucFnNumber,
				  PCI_CLASS_REVISION,
				  &ulClass);
      ulClass >>= 16;
      if (ulClass == PCI_CLASS_BRIDGE_PCI) {
				/*
				 * We have found a PCI-PCI bridge
				 */
	(void)pci_read_config_byte(0,
				   ucSlotNumber,
				   ucFnNumber,
				   PCI_SUBORDINATE_BUS,
				   &ucMaxSubordinate);
	if(ucMaxSubordinate>ucMaxPCIBus) {
	  ucMaxPCIBus=ucMaxSubordinate;
	}
      }
    }
  }
}

/*
 * Return the number of PCI busses in the system
 */
unsigned char BusCountPCI()
{
  return(ucMaxPCIBus+1);
}
