/*	$NetBSD: pci_map.c,v 1.12 2002/05/30 12:06:43 drochner Exp $	*/

/*-
 * Copyright (c) 2004, 2005  Brookhaven National  Laboratory
 *               S. Kate Feng <feng1@bnl.gov>
 *
 * Copyright (c) 1998, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum; by William R. Studenmund; by Jason R. Thorpe.
 *
 */

/*
 * PCI device mapping.
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <rtems/rtems/types.h>

#include <bsp/pci.h>
#include <bsp/pcireg.h>

extern int pci_get_capability(int b, int d, int f, int capid,int *offset,uint32_t *value);
extern int pci_mem_find(int b, int d, int f, int reg, unsigned *basep,unsigned *sizep);
extern int pci_io_find(int b, int d, int f, int reg,unsigned *basep,unsigned *sizep);

int pci_io_find(int b, int d, int f, int reg,unsigned *basep,unsigned *sizep)
{
  uint32_t address, mask;

  if (reg < PCI_MAPREG_START ||
#if 0
	    /*
	     * Can't do this check; some devices have mapping registers
	     * way out in left field.
	     */
	    reg >= PCI_MAPREG_END ||
#endif
	    (reg & 3))
		rtems_panic("pci_io_find: bad request");

  /*
   * Section 6.2.5.1, `Address Maps', tells us that:
   *
   * 1) The builtin software should have already mapped the device in a
   * reasonable way.
   *
   * 2) A device which wants 2^n bytes of memory will hardwire the bottom
   * n bits of the address to 0.  As recommended, we write all 1s and see
   * what we get back.
   */
  pci_read_config_dword(b,d,f,reg, &address);
  if ( !(address & PCI_MAPREG_TYPE_IO)) return(1);
  pci_write_config_dword(b,d,f,reg, 0xffffffff);
  pci_read_config_dword(b,d,f,reg,&mask);
  pci_write_config_dword(b,d,f,reg, address);

  if ( (*sizep = PCI_MAPREG_IO_SIZE(mask))== 0) {
     printk("pci_io_find: void region\n");
     return(1);
  }
  *basep = PCI_MAPREG_IO_ADDR(address);
  return(0);
}

int pci_mem_find(int b, int d, int f, int reg, unsigned *basep,unsigned *sizep)
{
  uint32_t address, mask;

  if (reg < PCI_MAPREG_START ||
#if 0
	    /*
	     * Can't do this check; some devices have mapping registers
	     * way out in left field.
	     */
	    reg >= PCI_MAPREG_END ||
#endif
	    (reg & 3))
      rtems_panic("pci_mem_find: bad request");

  pci_read_config_dword(b,d,f,reg, &address);
  if (address & PCI_MAPREG_TYPE_IO) {
     printk("pci_mem_find: expected type mem, found I/O\n");
     return(1);
  }

  /*
   * Section 6.2.5.1, `Address Maps', tells us that:
   *
   * 1) The builtin software should have already mapped the device in a
   * reasonable way.
   *
   * 2) A device which wants 2^n bytes of memory will hardwire the bottom
   * n bits of the address to 0.  As recommended, we write all 1s and see
   * what we get back.
   */
  pci_write_config_dword(b,d,f,reg, 0xffffffff);
  pci_read_config_dword(b,d,f,reg,&mask);
  pci_write_config_dword(b,d,f,reg, address);
  if ( (*sizep = PCI_MAPREG_MEM_SIZE(mask))== 0) {
     printk("pci_io_find: void region\n");
     return (1);
  }
  *basep = PCI_MAPREG_MEM_ADDR(address);
  return(0);
}

int pci_get_capability(int b, int d, int f, int capid,int *offset,uint32_t *value)
{
  uint32_t reg, ofs;

  /*  i82544EI PCI_CAPLISTPTR_REG */
  pci_read_config_dword(b,d,f,PCI_CAPLISTPTR_REG, &reg);
  ofs = PCI_CAPLIST_PTR(reg);
  while (ofs != 0) {
#ifdef DIAGNOSTIC
    if ((ofs & 3) || (ofs < 0x40))
       panic("pci_get_capability");
#endif
    pci_read_config_dword(b,d,f,ofs, &reg);
    if (PCI_CAPLIST_CAP(reg) == capid) {
       if (offset)
	   *offset = ofs;
       if (value)
	   *value = reg;
       return (1);
    }
    ofs = PCI_CAPLIST_NEXT(reg);
  }
  return (0);
}
