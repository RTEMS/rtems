/*
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <assert.h>
#include <stdio.h>

#include <bsp.h>

/*
 * Forced delay to get around timing problems with the UNIVERSE chip.  The
 * two nops are used so that the delay works for varying clock frequencies,
 * up to 66 Mhz, with margin.  Each nop averages about 1 1/2 clock ticks,
 * and since there are 2 nops, this routine takes about 3 clock ticks,
 * which on a worst case 66 Mhz board, is 45 nanosecond. This time period
 * is sufficient to guarantee a work-around to the UNIVERSE chip timing
 * problem.  The problem is that when there are two successive accesses to
 * an UNIVERSE register, without sufficient delay, the second access will
 * not work correctly.
 */
void PCI_bus_delay (void)
{
  __asm__ ("	nop");
  __asm__ (" nop");
}

/*
 * PCI_bus_write
 */
void PCI_bus_write(
  volatile uint32_t         * _addr,                  /* IN */
  uint32_t         _data                              /* IN */
)
{
  _data = Convert_Endian_32( _data );
  *_addr = _data;
}

uint32_t         PCI_bus_read(
  volatile uint32_t         *  _addr                  /* IN */
)
{
  uint32_t         data;

  data = *_addr;
  data = Convert_Endian_32( data );
  return data;
}
/*
 * PCI Configuration Cycle Read/Write Access which is used to access all of
 * devices registers on the PCI bus.  i.e.: Universe, Ethernet & PMC.
 */

uint32_t         Read_pci_device_register(
  uint32_t         address
)
{
  uint32_t         data;

  /*
   * Write the PCI configuration address
   */
   PCI_bus_write( (volatile uint32_t*)SCORE603E_PCI_IO_CFG_ADDR, address );

  /*
   *  Delay needed when running out of DRAM
   */
   PCI_bus_delay ();

  /*
   * read data
   */
  data = PCI_bus_read( (volatile uint32_t*)SCORE603E_PCI_IO_CFG_DATA );

  return data;
}

void  Write_pci_device_register(
  uint32_t         address,
  uint32_t         data
)
{
  /*
   * Write the PCI configuration address
   */
   PCI_bus_write( (volatile uint32_t*)SCORE603E_PCI_IO_CFG_ADDR, address );

  /*
   *  Delay needed when running out of DRAM
   */
   PCI_bus_delay ();

  /*
   * write data
   */
  PCI_bus_write( (volatile uint32_t*)SCORE603E_PCI_IO_CFG_DATA, data );
}
