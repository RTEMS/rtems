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
  rtems_unsigned32 PCI_ID;                 /* 0x80030000 */
  rtems_unsigned32 PCI_CSR;                /* 0x80030004 */
  rtems_unsigned32 PCI_CLASS;              /* 0x80030008 */
  rtems_unsigned32 PCI_MISC0;              /* 0x8003000C */
  rtems_unsigned32 PCI_BS;                 /* 0x80030010 */
  rtems_unsigned32 Buf_0x80030014[ 0x0A ]; /* 0x80030014 */
  rtems_unsigned32 PCI_MISC1;              /* 0x8003003C */
  rtems_unsigned32 Buf_0x80030040[ 0x30 ]; /* 0x80030040 */
  rtems_unsigned32 LSI0_CTL;               /* 0x80030100 */
  rtems_unsigned32 LSI0_BS;                /* 0x80030104 */
  rtems_unsigned32 LSI0_BD;                /* 0x80030108 */
  rtems_unsigned32 LSI0_TO;                /* 0x8003010C */
  rtems_unsigned32 Buf_0x80030110;         /* 0x80030110 */
  rtems_unsigned32 LSI1_CTL;               /* 0x80030114 */
  rtems_unsigned32 LSI1_BS;                /* 0x80030118 */
  rtems_unsigned32 LSI1_BD;                /* 0x8003011C */
  rtems_unsigned32 LSI1_TO;                /* 0x80030120 */
  rtems_unsigned32 Buf_0x80030124;         /* 0x80030124 */
  rtems_unsigned32 LSI2_CTL;               /* 0x80030128 */
  rtems_unsigned32 LSI2_BS;                /* 0x8003012C */
  rtems_unsigned32 LSI2_BD;                /* 0x80030130 */
  rtems_unsigned32 LSI2_TO;                /* 0x80030134 */
  rtems_unsigned32 Buf_0x80030138;         /* 0x80030138 */
  rtems_unsigned32 LSI3_CTL;               /* 0x8003013C */
  rtems_unsigned32 LSI3_BS;                /* 0x80030140 */
  rtems_unsigned32 LSI3_BD;                /* 0x80030144 */
  rtems_unsigned32 LSI3_TO;                /* 0x80030148 */
  rtems_unsigned32 Buf_0x8003014C[ 0x09 ]; /* 0x8003014C */
  rtems_unsigned32 SCYC_CTL;               /* 0x80030170 */
  rtems_unsigned32 SCYC_ADDR;              /* 0x80030174 */
  rtems_unsigned32 SCYC_EN;                /* 0x80030178 */
  rtems_unsigned32 SCYC_CMP;               /* 0x8003017C */
  rtems_unsigned32 SCYC_SWP;               /* 0x80030180 */
  rtems_unsigned32 LMISC;                  /* 0x80030184 */
  rtems_unsigned32 SLSI;                   /* 0x80030188 */
  rtems_unsigned32 L_CMDERR;               /* 0x8003018C */
  rtems_unsigned32 LAERR;                  /* 0x80030190 */
  rtems_unsigned32 Buf_0x80030194[ 0x1B ]; /* 0x80030194 */
  rtems_unsigned32 DCTL;                   /* 0x80030200 */
  rtems_unsigned32 DTBC;                   /* 0x80030204 */
  rtems_unsigned32 DLA;                    /* 0x80030208 */
  rtems_unsigned32 Buf_0x8003020C;         /* 0x8003020C */
  rtems_unsigned32 DVA;                    /* 0x80030210 */
  rtems_unsigned32 Buf_0x80030214;         /* 0x80030214 */
  rtems_unsigned32 DCPP;                   /* 0x80030218 */
  rtems_unsigned32 Buf_0x8003021C;         /* 0x8003021C */
  rtems_unsigned32 DGCS;                   /* 0x80030220 */
  rtems_unsigned32 D_LLUE;                 /* 0x80030224 */
  rtems_unsigned32 Buf_0x80030228[ 0x36 ]; /* 0x80030228 */
  rtems_unsigned32 LINT_EN;                /* 0x80030300 */
  rtems_unsigned32 LINT_STAT;              /* 0x80030304 */
  rtems_unsigned32 LINT_MAP0;              /* 0x80030308 */
  rtems_unsigned32 LINT_MAP1;              /* 0x8003030C */
  rtems_unsigned32 VINT_EN;                /* 0x80030310 */
  rtems_unsigned32 VINT_STAT;              /* 0x80030314 */
  rtems_unsigned32 VINT_MAP0;              /* 0x80030318 */
  rtems_unsigned32 VINT_MAP1;              /* 0x8003031C */
  rtems_unsigned32 STATID;                 /* 0x80030320 */
  rtems_unsigned32 V1_STATID;              /* 0x80030324 */
  rtems_unsigned32 V2_STATID;              /* 0x80030328 */
  rtems_unsigned32 V3_STATID;              /* 0x8003032C */
  rtems_unsigned32 V4_STATID;              /* 0x80030330 */
  rtems_unsigned32 V5_STATID;              /* 0x80030334 */
  rtems_unsigned32 V6_STATID;              /* 0x80030338 */
  rtems_unsigned32 V7_STATID;              /* 0x8003033C */
  rtems_unsigned32 Buf_0x80030340[ 0x30 ]; /* 0x80030340 */
  rtems_unsigned32 MAST_CTL;               /* 0x80030400 */ 
  rtems_unsigned32 MISC_CTL;               /* 0x80030404 */ 
  rtems_unsigned32 MISC_STAT;              /* 0x80030408 */ 
  rtems_unsigned32 USER_AM;                /* 0x8003040C */
  rtems_unsigned32 Buf_0x80030410[ 0x2bc ];/* 0x80030410 */
  rtems_unsigned32 VSI0_CTL;               /* 0x80030F00 */
  rtems_unsigned32 VSI0_BS;                /* 0x80030F04 */
  rtems_unsigned32 VSI0_BD;                /* 0x80030F08 */
  rtems_unsigned32 VSI0_TO;                /* 0x80030F0C */
  rtems_unsigned32 Buf_0x80030f10;         /* 0x80030F10 */
  rtems_unsigned32 VSI1_CTL;               /* 0x80030F14 */
  rtems_unsigned32 VSI1_BS;                /* 0x80030F18 */
  rtems_unsigned32 VSI1_BD;                /* 0x80030F1C */
  rtems_unsigned32 VSI1_TO;                /* 0x80030F20 */
  rtems_unsigned32 Buf_0x80030F24;         /* 0x80030F24 */
  rtems_unsigned32 VSI2_CTL;               /* 0x80030F28 */
  rtems_unsigned32 VSI2_BS;                /* 0x80030F2C */
  rtems_unsigned32 VSI2_BD;                /* 0x80030F30 */
  rtems_unsigned32 VSI2_TO;                /* 0x80030F34 */
  rtems_unsigned32 Buf_0x80030F38;         /* 0x80030F38 */
  rtems_unsigned32 VSI3_CTL;               /* 0x80030F3C */
  rtems_unsigned32 VSI3_BS;                /* 0x80030F40 */
  rtems_unsigned32 VSI3_BD;                /* 0x80030F44 */
  rtems_unsigned32 VSI3_TO;                /* 0x80030F48 */
  rtems_unsigned32 Buf_0x80030F4C[ 0x9 ];  /* 0x80030F4C */
  rtems_unsigned32 VRAI_CTL;               /* 0x80030F70 */
  rtems_unsigned32 VRAI_BS;                /* 0x80030F74 */
  rtems_unsigned32 Buf_0x80030F78[ 0x2 ];  /* 0x80030F78 */
  rtems_unsigned32 VCSR_CTL;               /* 0x80030F80 */
  rtems_unsigned32 VCSR_TO;                /* 0x80030F84 */
  rtems_unsigned32 V_AMERR;                /* 0x80030F88 */
  rtems_unsigned32 VAERR;                  /* 0x80030F8C */
  rtems_unsigned32 Buf_0x80030F90[ 0x19 ]; /* 0x80030F90 */
  rtems_unsigned32 VCSR_CLR;               /* 0x80030FF4 */
  rtems_unsigned32 VCSR_SET;               /* 0x80030FF8 */
  rtems_unsigned32 VCSR_BS;                /* 0x80030FFC */
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

void initialize_universe()
{
  rtems_unsigned32 jumper_selection;
  rtems_unsigned32 pci_id;
#if (SCORE603E_USE_SDS) | (SCORE603E_USE_OPEN_FIRMWARE) | (SCORE603E_USE_NONE)
  volatile rtems_unsigned32 universe_temp_value;
#endif
 
  /*
   * Read the VME jumper location to determine the VME base address
   */
  jumper_selection = PCI_bus_read( 
                     (volatile rtems_unsigned32 *)SCORE603E_VME_JUMPER_ADDR );
  jumper_selection = (jumper_selection >> 3) & 0x1f;

  /*
   * Verify the UNIVERSE CHIP ID 
   */
   pci_id = Read_pci_device_register( SCORE603E_IO_VME_UNIVERSE_BASE );

   /* 
    * compare to known ID 
    */
   if (pci_id !=  SCORE603E_UNIVERSE_CHIP_ID ){
     DEBUG_puts ("Invalid SCORE603E_UNIVERSE_CHIP_ID: ");
     rtems_fatal_error_occurred( 0x603e0bad );
   }

#if (SCORE603E_USE_SDS) | (SCORE603E_USE_OPEN_FIRMWARE) | (SCORE603E_USE_NONE)

   /*
    * Set the UNIVERSE PCI Configuration Base Address Register with 0x30001
    * to specifies the 64 Kbyte aligned base address of the UNIVERSE register
    * space on PCI to be 0x30000 + 0x80000000 (IO_BASE)
    */
   Write_pci_device_register( SCORE603E_IO_VME_UNIVERSE_BASE+0x10,0x30001 );

   /*
    * Set the UNIVERSE PCI Configuration Space Control and Status Register to
    * medium speed device, Target Back to Back Capable, Master Enable, Target
    * Memory Enable and Target IO Enable
    */
   Write_pci_device_register( SCORE603E_IO_VME_UNIVERSE_BASE+0x4, 0x2800007 );

   /* 
    * Turn off the sysfail by setting SYSFAIL bit to 1 on the VCSR_CLR register
    */
   PCI_bus_write( &UNIVERSE->VCSR_CLR, 0x40000000 );   

   /*
    * Set the VMEbus Master Control register with retry forever, 256 bytes
    * posted write transfer count, VMEbus request level 3, RWD, PCI 32 bytes 
    * aligned burst size and PCI bus number to be zero
    */
   PCI_bus_write( &UNIVERSE->MAST_CTL, 0x01C00000 );

   /*
    * VMEbus DMA Transfer Control register with 32 bit VMEbus Maximum Data
    * width, A32 VMEbus Address Space, AM code to be data, none-privilleged,
    * single and BLT cycles on VME bus and 64-bit PCI Bus Transactions enable
    PCI_bus_write( &UNIVERSE->DCTL, 0x00820180 );   
    */
   
   PCI_bus_write( &UNIVERSE->LSI0_CTL, 0x80700040 );
   PCI_bus_write( &UNIVERSE->LSI0_BS,  0x04000000 );
   PCI_bus_write( &UNIVERSE->LSI0_BD,  0x05000000 );
   PCI_bus_write( &UNIVERSE->LSI0_TO,  0x7C000000 );

   /* 
    * Remove the Universe from VMEbus BI-Mode (bus-isolation).  Once out of
    * BI-Mode VMEbus accesses can be made. 
    */

   universe_temp_value = PCI_bus_read( &UNIVERSE->MISC_CTL );

   if (universe_temp_value & 0x100000)
     PCI_bus_write( &UNIVERSE->MISC_CTL,(universe_temp_value | ~0xFF0FFFFF));

#elif (SCORE603E_USE_DINK)
   /* 
    * Do not modify the DINK setup of the universe chip.
    */

#else
#error "SCORE603E BSPSTART.C -- what ROM monitor are you using"
#endif

}


/*
 * Set the slave VME base address to the specified base address.
 * Note: Lower 12 bits[11:0] will be masked out prior to setting the VMEbus
 *       Slave Image 0 registers.
 */
void set_vme_base_address (
  rtems_unsigned32 base_address 
)
{  
  volatile rtems_unsigned32 temp;

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
rtems_unsigned32 get_vme_base_address ()
{  
  volatile rtems_unsigned32 temp;

  temp = PCI_bus_read( &UNIVERSE->VSI0_BS );
  temp &= 0xFFFFF000;
  return (temp);
}

rtems_unsigned32 get_vme_slave_size()
{
  volatile rtems_unsigned32 temp;
  temp  =  PCI_bus_read( &UNIVERSE->VSI0_BD);
  temp &= 0xFFFFF000;
  temp  = temp - get_vme_base_address ();
  return temp;
}

/*
 * Set the size of the VME slave image
 * Note: The maximum size is up to 24 M bytes. (00000000 - 017FFFFF)
 */
void set_vme_slave_size (rtems_unsigned32 size)
{  
  volatile rtems_unsigned32 temp;

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

