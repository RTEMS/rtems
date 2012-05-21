/*
 * detect_host_bridge.c
 *
 * This code is inspired by detect_grackle_bridge.c of SVGM BSP
 * written by Till Straumann
 * Copyright (C) 2001, 2003 Till Straumann <strauman@slac.stanford.edu>
 *
 * Copyright (C) 2004 S. Kate Feng, <feng1@bnl.gov>
 * wrote it to support the MVME5500 board.
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

#define HOSTBRIDGET_ERROR               0xf0000000

unsigned long _BSP_clear_hostbridge_errors(int enableMCP, int quiet)
{
  uint32_t pcidata, pcidata1;
  int PciLocal, busNumber=0;

  /* On the mvme5500 board, the GT64260B system controller had the MCP
   * signal pulled up high.  Thus, the MCP signal is not used as it is
   * on other boards such as mvme2307.
   */
  if (enableMCP) return(-1);
  for (PciLocal=0; PciLocal<1; PciLocal++ ) {
     pci_read_config_dword(busNumber,
			0,
			0,
		        PCI_COMMAND,
                        &pcidata);

    if (!quiet)
    printk("Before _BSP_clear_hostbridge_errors(): 0x%x, cause 0x%x\n",
	   pcidata, inl(0x1d58));

    outl(0,0x1d58);

    /* Clear the error on the host bridge */
    pcidata1= pcidata;
    pcidata1 |= PCI_STATUS_CLRERR_MASK;
    pcidata1 |= 0x140;
    pci_write_config_dword(busNumber,
 			       0,
			       0,
			  PCI_COMMAND,
                          pcidata1);

    pci_read_config_dword(busNumber,
			       0,
			       0,
			  PCI_COMMAND,
                          &pcidata1);

    if (!quiet) printk("After _BSP_clear_hostbridge_errors(): sts 0x%x\n",
                        pcidata1);
    if (pcidata1 & HOSTBRIDGET_ERROR) printk("BSP_clear_hostbridge_errors(): unable to clear pending hostbridge errors\n");
    busNumber += BSP_MAX_PCI_BUS_ON_PCI0;
  }
  return(pcidata &  HOSTBRIDGET_ERROR);
}
