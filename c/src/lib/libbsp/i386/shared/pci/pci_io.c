/**
 * @file
 *
 * PCI Support when Configuration Space is in I/O
 */

/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/bspimpl.h>

static int pci_io_initialized = 0;

/*
 * Forward reference. Initialized at bottom.
 */
static const pci_config_access_functions pci_io_indirect_functions;

/*
 * Detects presense of PCI Configuration is in I/O space. If so, return
 * pointer to accessor methods.
 *
 * NOTE: TBD to determine if (a) PCI Bus exists and (b) this is the
 *       access method.
 */
const pci_config_access_functions *pci_io_initialize(void)
{
  pci_io_initialized = 1;

  printk( "PCI I/O Support Initialized\n" );

  return &pci_io_indirect_functions;
}

/*
 * Build PCI Address
 */
static inline uint32_t pci_io_build_address(
  uint16_t  bus,
  uint16_t  slot,
  uint16_t  function,
  uint16_t  offset
)
{
  uint32_t bus_u32      = (uint32_t)bus;
  uint32_t slot_u32     = (uint32_t)slot;
  uint32_t function_u32 = (uint32_t)function;
  uint32_t address;

  /*
   * create configuration address as per figure at
   *   http://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
   */
  address  = (uint32_t) 0x80000000;  /* Bit  31    - Enable Bit */
                                     /* Bits 30-24 - Reserved */
  address |= bus_u32 << 16;          /* Bits 23-16 - Bus Number */
  address |= slot_u32 << 11;         /* Bits 15-11 - Device/Slot Number */
  address |= function_u32 << 8;      /* Bits 10-8  - Function Number */
  address |= offset & 0xfc;          /* Bits 7-2   - Offset/Register Number */
                                     /* Bits 1-0   - Reserved 0 */
  return address;
}

static int BSP_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  unsigned char *value
)
{
  uint32_t address;
  uint32_t tmp;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  inport_long(0xCFC, tmp);

  /* (offset & 3) * 8) = 0 will choose the first byte of the 32 bits register */
  *value = (uint16_t)(tmp >> ((offset & 3) * 8)) & 0xff;
  return PCIBIOS_SUCCESSFUL;
}

static int BSP_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  unsigned short *value
)
{
  uint32_t address;
  uint32_t tmp;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  inport_long(0xCFC, tmp);

  /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
  *value = (uint16_t)(tmp >> ((offset & 2) * 8)) & 0xffff;
  return PCIBIOS_SUCCESSFUL;
}

static int BSP_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t     *value
)
{
  uint32_t address;
  uint32_t tmp;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  inport_long(0xCFC, tmp);

  *value = tmp;
  return PCIBIOS_SUCCESSFUL;
}

static int BSP_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  unsigned char value
)
{
  uint32_t address;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  outport_byte(0xCFC, value);

  return PCIBIOS_SUCCESSFUL;
}

static int BSP_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  unsigned short value
)
{
  uint32_t address;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  outport_word(0xCFC, value);

  return PCIBIOS_SUCCESSFUL;
}

static int BSP_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      value
)
{
  uint32_t address;

  address = pci_io_build_address( bus, slot, function, offset );

  /* write out the address */
  outport_long(0xCF8, address);

  /* read in the data */
  outport_long(0xCFC, value);

  return PCIBIOS_SUCCESSFUL;
}

static const pci_config_access_functions pci_io_indirect_functions = {
  BSP_pci_read_config_byte,
  BSP_pci_read_config_word,
  BSP_pci_read_config_dword,
  BSP_pci_write_config_byte,
  BSP_pci_write_config_word,
  BSP_pci_write_config_dword
};
