/*
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
#include <inttypes.h>
#include <rtems/bspIo.h>

#include <bsp.h>
#include "PCI.h"

/********************************************************************
 ********************************************************************
 *********                                                  *********
 *********                  Prototypes                      *********
 *********                                                  *********
 ********************************************************************
 ********************************************************************/

/********************************************************************
 ********************************************************************
 *********                                                  *********
 *********                                                  *********
 *********                                                  *********
 ********************************************************************
 ********************************************************************/

typedef struct {
  uint32_t         PCI_ID;                 /* 0x80030000 */
  uint32_t         PCI_CSR;                /* 0x80030004 */
  uint32_t         PCI_CLASS;              /* 0x80030008 */
  uint32_t         PCI_MISC0;              /* 0x8003000C */
  uint32_t         PCI_BS;                 /* 0x80030010 */
  uint32_t         Buf_0x80030014[ 0x0A ]; /* 0x80030014 */
  uint32_t         PCI_MISC1;              /* 0x8003003C */
  uint32_t         Buf_0x80030040[ 0x30 ]; /* 0x80030040 */
  uint32_t         LSI0_CTL;               /* 0x80030100 */
  uint32_t         LSI0_BS;                /* 0x80030104 */
  uint32_t         LSI0_BD;                /* 0x80030108 */
  uint32_t         LSI0_TO;                /* 0x8003010C */
  uint32_t         Buf_0x80030110;         /* 0x80030110 */
  uint32_t         LSI1_CTL;               /* 0x80030114 */
  uint32_t         LSI1_BS;                /* 0x80030118 */
  uint32_t         LSI1_BD;                /* 0x8003011C */
  uint32_t         LSI1_TO;                /* 0x80030120 */
  uint32_t         Buf_0x80030124;         /* 0x80030124 */
  uint32_t         LSI2_CTL;               /* 0x80030128 */
  uint32_t         LSI2_BS;                /* 0x8003012C */
  uint32_t         LSI2_BD;                /* 0x80030130 */
  uint32_t         LSI2_TO;                /* 0x80030134 */
  uint32_t         Buf_0x80030138;         /* 0x80030138 */
  uint32_t         LSI3_CTL;               /* 0x8003013C */
  uint32_t         LSI3_BS;                /* 0x80030140 */
  uint32_t         LSI3_BD;                /* 0x80030144 */
  uint32_t         LSI3_TO;                /* 0x80030148 */
  uint32_t         Buf_0x8003014C[ 0x09 ]; /* 0x8003014C */
  uint32_t         SCYC_CTL;               /* 0x80030170 */
  uint32_t         SCYC_ADDR;              /* 0x80030174 */
  uint32_t         SCYC_EN;                /* 0x80030178 */
  uint32_t         SCYC_CMP;               /* 0x8003017C */
  uint32_t         SCYC_SWP;               /* 0x80030180 */
  uint32_t         LMISC;                  /* 0x80030184 */
  uint32_t         SLSI;                   /* 0x80030188 */
  uint32_t         L_CMDERR;               /* 0x8003018C */
  uint32_t         LAERR;                  /* 0x80030190 */
  uint32_t         Buf_0x80030194[ 0x1B ]; /* 0x80030194 */
  uint32_t         DCTL;                   /* 0x80030200 */
  uint32_t         DTBC;                   /* 0x80030204 */
  uint32_t         DLA;                    /* 0x80030208 */
  uint32_t         Buf_0x8003020C;         /* 0x8003020C */
  uint32_t         DVA;                    /* 0x80030210 */
  uint32_t         Buf_0x80030214;         /* 0x80030214 */
  uint32_t         DCPP;                   /* 0x80030218 */
  uint32_t         Buf_0x8003021C;         /* 0x8003021C */
  uint32_t         DGCS;                   /* 0x80030220 */
  uint32_t         D_LLUE;                 /* 0x80030224 */
  uint32_t         Buf_0x80030228[ 0x36 ]; /* 0x80030228 */
  uint32_t         LINT_EN;                /* 0x80030300 */
  uint32_t         LINT_STAT;              /* 0x80030304 */
  uint32_t         LINT_MAP0;              /* 0x80030308 */
  uint32_t         LINT_MAP1;              /* 0x8003030C */
  uint32_t         VINT_EN;                /* 0x80030310 */
  uint32_t         VINT_STAT;              /* 0x80030314 */
  uint32_t         VINT_MAP0;              /* 0x80030318 */
  uint32_t         VINT_MAP1;              /* 0x8003031C */
  uint32_t         STATID;                 /* 0x80030320 */
  uint32_t         V1_STATID;              /* 0x80030324 */
  uint32_t         V2_STATID;              /* 0x80030328 */
  uint32_t         V3_STATID;              /* 0x8003032C */
  uint32_t         V4_STATID;              /* 0x80030330 */
  uint32_t         V5_STATID;              /* 0x80030334 */
  uint32_t         V6_STATID;              /* 0x80030338 */
  uint32_t         V7_STATID;              /* 0x8003033C */
  uint32_t         Buf_0x80030340[ 0x30 ]; /* 0x80030340 */
  uint32_t         MAST_CTL;               /* 0x80030400 */
  uint32_t         MISC_CTL;               /* 0x80030404 */
  uint32_t         MISC_STAT;              /* 0x80030408 */
  uint32_t         USER_AM;                /* 0x8003040C */
  uint32_t         Buf_0x80030410[ 0x2bc ];/* 0x80030410 */
  uint32_t         VSI0_CTL;               /* 0x80030F00 */
  uint32_t         VSI0_BS;                /* 0x80030F04 */
  uint32_t         VSI0_BD;                /* 0x80030F08 */
  uint32_t         VSI0_TO;                /* 0x80030F0C */
  uint32_t         Buf_0x80030f10;         /* 0x80030F10 */
  uint32_t         VSI1_CTL;               /* 0x80030F14 */
  uint32_t         VSI1_BS;                /* 0x80030F18 */
  uint32_t         VSI1_BD;                /* 0x80030F1C */
  uint32_t         VSI1_TO;                /* 0x80030F20 */
  uint32_t         Buf_0x80030F24;         /* 0x80030F24 */
  uint32_t         VSI2_CTL;               /* 0x80030F28 */
  uint32_t         VSI2_BS;                /* 0x80030F2C */
  uint32_t         VSI2_BD;                /* 0x80030F30 */
  uint32_t         VSI2_TO;                /* 0x80030F34 */
  uint32_t         Buf_0x80030F38;         /* 0x80030F38 */
  uint32_t         VSI3_CTL;               /* 0x80030F3C */
  uint32_t         VSI3_BS;                /* 0x80030F40 */
  uint32_t         VSI3_BD;                /* 0x80030F44 */
  uint32_t         VSI3_TO;                /* 0x80030F48 */
  uint32_t         Buf_0x80030F4C[ 0x9 ];  /* 0x80030F4C */
  uint32_t         VRAI_CTL;               /* 0x80030F70 */
  uint32_t         VRAI_BS;                /* 0x80030F74 */
  uint32_t         Buf_0x80030F78[ 0x2 ];  /* 0x80030F78 */
  uint32_t         VCSR_CTL;               /* 0x80030F80 */
  uint32_t         VCSR_TO;                /* 0x80030F84 */
  uint32_t         V_AMERR;                /* 0x80030F88 */
  uint32_t         VAERR;                  /* 0x80030F8C */
  uint32_t         Buf_0x80030F90[ 0x19 ]; /* 0x80030F90 */
  uint32_t         VCSR_CLR;               /* 0x80030FF4 */
  uint32_t         VCSR_SET;               /* 0x80030FF8 */
  uint32_t         VCSR_BS;                /* 0x80030FFC */
} Universe_Memory;

volatile Universe_Memory *UNIVERSE =
                         (volatile Universe_Memory *)SCORE603E_UNIVERSE_BASE;

/********************************************************************
 ********************************************************************
 *********                                                  *********
 *********                                                  *********
 *********                                                  *********
 ********************************************************************
 ********************************************************************/

/*
 * Initializes the UNIVERSE chip.  This routine is called automatically
 * by the boot code.  This routine should be called by user code only if
 * a complete SCORE603e VME initialization is required.
 */

void initialize_universe(void)
{
  uint32_t         jumper_selection;
  uint32_t         pci_id;

  /*
   * Read the VME jumper location to determine the VME base address
   */
  jumper_selection = PCI_bus_read(
                     (volatile uint32_t*)SCORE603E_VME_JUMPER_ADDR );
  printk("initialize_universe: Read 0x%x = 0x%x\n",
          SCORE603E_VME_JUMPER_ADDR, jumper_selection);
  jumper_selection = (jumper_selection >> 3) & 0x1f;

  /*
   * Verify the UNIVERSE CHIP ID
   */
   pci_id = Read_pci_device_register( SCORE603E_IO_VME_UNIVERSE_BASE );

   /*
    * compare to known ID
    */
   if (pci_id !=  SCORE603E_UNIVERSE_CHIP_ID ){
     printk ("Invalid SCORE603E_UNIVERSE_CHIP_ID: 0x08%" PRId32 "\n", pci_id);
     rtems_fatal_error_occurred( 0x603e0bad );
   } else {
     printk("initialize_universe: Reg 0x%x read 0x%x\n",
     SCORE603E_IO_VME_UNIVERSE_BASE, pci_id );
   }

   /*
    * Do not modify the DINK setup of the universe chip.
    */
}

/*
 * Set the slave VME base address to the specified base address.
 * Note: Lower 12 bits[11:0] will be masked out prior to setting the VMEbus
 *       Slave Image 0 registers.
 */
void set_vme_base_address (
  uint32_t         base_address
)
{
  volatile uint32_t         temp;

  /*
   * Calculate the current size of the Slave VME image 0
   */
  temp = ( PCI_bus_read( &UNIVERSE->VSI0_BD) & 0xFFFFF000) -
          ( PCI_bus_read( &UNIVERSE->VSI0_BS) & 0xFFFFF000);

  /*
   * Set the VMEbus Slave Image 0 Base Address to be
   * the specifed base address on VSI0_BS register.
   */
   PCI_bus_write( &UNIVERSE->VSI0_BS, (base_address & 0xFFFFF000) );

  /*
   * Update the VMEbus Slave Image 0 Bound Address.
   */
  PCI_bus_write( &UNIVERSE->VSI0_BD, temp );

  /*
   * Update the VMEbus Slave Image 0 Translation Offset
   */
  temp = 0xFFFFFFFF - (base_address & 0xFFFFF000) + 1 + 0x80000000;
  PCI_bus_write( &UNIVERSE->VSI0_TO, temp );
}

/*
 * Gets the VME base address
 */
uint32_t         get_vme_base_address (void)
{
  volatile uint32_t         temp;

  temp = PCI_bus_read( &UNIVERSE->VSI0_BS );
  temp &= 0xFFFFF000;
  return (temp);
}

uint32_t         get_vme_slave_size(void)
{
  volatile uint32_t         temp;
  temp  =  PCI_bus_read( &UNIVERSE->VSI0_BD);
  temp &= 0xFFFFF000;
  temp  = temp - get_vme_base_address ();
  return temp;
}

/*
 * Set the size of the VME slave image
 * Note: The maximum size is up to 24 M bytes. (00000000 - 017FFFFF)
 */
void set_vme_slave_size (uint32_t         size)
{
  volatile uint32_t         temp;

  if (size<0)
    size = 0;

  if (size > 0x17FFFFF)
    size = 0x17FFFFF;

  /*
   * Read the VME slave image base address
   */
  temp = get_vme_base_address ();

  /*
   * Update the VMEbus Slave Image 0 Bound Address.
   */
  temp = temp + (size & 0xFFFFF000);
  PCI_bus_write( &UNIVERSE->VSI0_BD, temp );
}
