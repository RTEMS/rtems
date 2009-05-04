/*
 * detect_host_bridge.c
 *
 * This code is inspired by detect_grackle_bridge.c of SVGM BSP
 * written by Till Straumann
 * Copyright (C) 2001, 2003 Till Straumann <strauman@slac.stanford.edu>
 * 
 * Copyright (C) 2004 S. Kate Feng, <feng1@bnl.gov>
 * wrote it to support the MVME5500 board,
 * under the Deaprtment of Energy contract DE-AC02-98CH10886.
 * 
 */
#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gtpcireg.h> 

#include <stdio.h>
#include <string.h>

#define PCI_DEBUG 0

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_MULTI_FUNCTION		0x80

unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet)
{
  unsigned long pcidata=0;
  unsigned short val;
  int PciLocal, busNumber=0, loop;
  
  /* On the mvme5500 board, the GT64260B system controller had the MCP
   * signal pulled up high.  Thus, the MCP signal is not used as it is
   * on other boards such as mvme2307.
   */
  if (enableMCP) return(-1);
  for (PciLocal=0; PciLocal<2; PciLocal++ ) {
      pci_read_config_word(busNumber,
			       0,
			       0,
			  PCI_STATUS,
			   &val);
    if ( (val & PCI_STATUS_CLRERR_MASK) && (!quiet)) 
       printk("Before _BSP_clear_hostbridge_errors(): PCI %d sts was 0x%x\n", 
                       PciLocal, val);
      if (!busNumber) 
	pcidata |= val;
      else
        pcidata |= (val <<16);

      for ( loop=0; loop < 10; loop++) {
          /* Clear the error on the host bridge */
          pci_write_config_word(busNumber,
 			       0,
			       0,
			  PCI_STATUS,
                          PCI_STATUS_CLRERR_MASK);

          pci_read_config_word(busNumber,
			       0,
			       0,
			  PCI_STATUS,
                          &val); 
          if ( !(val & PCI_STATUS_CLRERR_MASK)) 
             break;
          else {
	    if (loop==9)
	      printk("BSP_clear_hostbridge_errors(): unable to clear pending hostbridge errors\n");
          }
      }
      busNumber += BSP_MAX_PCI_BUS_ON_PCI0;
  }
  return(pcidata);
}
