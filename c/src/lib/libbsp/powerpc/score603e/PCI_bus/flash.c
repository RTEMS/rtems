/*
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 * $Id$
 */

#include <rtems.h>
#include <assert.h>
#include <stdio.h>

#include <bsp.h>
#include "PCI.h"
/*PAGE
 *
 *  SCORE603e_FLASH_Disable
 */

unsigned int SCORE603e_FLASH_Disable(
  rtems_unsigned32               area                           /* IN  */
)
{
  rtems_unsigned8 value;
 
  value = *SCORE603E_BOARD_CTRL_REG;
  value = value | (~SCORE603E_BRD_FLASH_DISABLE_MASK);
  *SCORE603E_BOARD_CTRL_REG = value;

  return RTEMS_SUCCESSFUL;
}

unsigned int SCORE603e_FLASH_verify_enable()
{
  volatile rtems_unsigned8 *Ctrl_Status_Register = 
           (void *)SCORE603E_BOARD_CTRL_REG;
  rtems_unsigned8  ctrl_value;
  rtems_unsigned32 pci_value;

  ctrl_value = *Ctrl_Status_Register;
  if ( ctrl_value & SCORE603E_BRD_FLASH_DISABLE_MASK ) {
    printf ("Flash Writes Disabled by board control register %x\n",
            ctrl_value );
    assert( 0x0 );
  }

  pci_value = Read_pci_device_register( 0x800000A8 );
  if (( pci_value & 0x00001000 ) == 0) {
    printf("Error PCI A8 \n");
    assert( 0x0 );
  }

  pci_value = Read_pci_device_register( 0x800000AC );
  if ( pci_value & 0x02000000) {
    printf("Error PCI AC \n");
    assert( 0x0 );
  }
  return RTEMS_SUCCESSFUL;
}

unsigned int SCORE603e_FLASH_pci_reset_reg(
  rtems_unsigned8  reg,
  rtems_unsigned32 cmask,
  rtems_unsigned32 mask
)
{
  rtems_unsigned32 pci_value;
  rtems_unsigned32 value;

  pci_value = Read_pci_device_register( reg );
  pci_value &= cmask;
  pci_value |= mask;
  Write_pci_device_register( reg, pci_value );
  value = Read_pci_device_register(  reg );
  if (value != pci_value) {
    printf("Error PCI %x wrote %x read %x\n", reg, pci_value, value);
  }
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  SCORE603e_FLASH_Enable_writes
 */
unsigned int SCORE603e_FLASH_Enable_writes(
  rtems_unsigned32               area                           /* IN  */
)
{
  rtems_unsigned8  ctrl_value;
  rtems_unsigned32 pci_value;

  ctrl_value = *SCORE603E_BOARD_CTRL_REG;
  ctrl_value = ctrl_value & 0xbf;
  *SCORE603E_BOARD_CTRL_REG = ctrl_value;

  pci_value = Read_pci_device_register( 0x800000A8 );
  pci_value |= 0x00001000;
  Write_pci_device_register( 0x800000A8, pci_value );

  pci_value = Read_pci_device_register( 0x800000AC );
  pci_value &=  (~0x02000000);
  Write_pci_device_register( 0x000000AC, pci_value );

  return RTEMS_SUCCESSFUL;
}





