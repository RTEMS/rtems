/* rsPMCQ1.c - Radstone PMCQ1 Common Initialisation Code
 *
 * Copyright 2000 Radstone Technology
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

/*
DESCRIPTION
These functions are responsible for scanning for PMCQ1's and setting up
the Motorola MC68360's if present.

USAGE
call rsPMCQ1Init() to perform ba  sic initialisation of the PMCQ1's.
*/

/* includes */
#include <libcpu/io.h>
#include <bsp/irq.h>
#include <stdlib.h>
#include <rtems/bspIo.h>
#include <bsp/pci.h>
#include <bsp.h>
#include "rsPMCQ1.h"
#include "m68360.h"

/* defines */
#if 1
#define DEBUG_360
#endif

/* Local data */
PPMCQ1BoardData		pmcq1BoardData = NULL;

static unsigned char rsPMCQ1Initialized = FALSE;

/* forward declarations */

/* local Qspan II serial eeprom table */
static unsigned char rsPMCQ1eeprom[] =
    {
    0x00,	/* Byte 0 - PCI_SID */
    0x00,	/* Byte 1 - PCI_SID */
    0x00,	/* Byte 2 - PCI_SID */
    0x00,	/* Byte 3 - PCI_SID */
    0x00,	/* Byte 4 - PBROM_CTL */
    0x00,	/* Byte 5 - PBROM_CTL */
    0x00,	/* Byte 6 - PBROM_CTL */
    0x2C,	/* Byte 7 - PBTI0_CTL */
    0xB0,	/* Byte 8 - PBTI1_CTL */
    0x00,	/* Byte 9 - QBSI0_AT */
    0x00,	/* Byte 10 - QBSI0_AT */
    0x02,	/* Byte 11 - QBSI0_AT */
    0x00,	/* Byte 12 - PCI_ID */
    0x07,	/* Byte 13 - PCI_ID */
    0x11,	/* Byte 14 - PCI_ID */
    0xB5,	/* Byte 15 - PCI_ID */
    0x06,	/* Byte 16 - PCI_CLASS */
    0x80,	/* Byte 17 - PCI_CLASS */
    0x00,	/* Byte 18 - PCI_CLASS */
    0x00,	/* Byte 19 - PCI_MISC1 */
    0x00,	/* Byte 20 - PCI_MISC1 */
    0xC0,	/* Byte 21 - PCI_PMC */
    0x00	/* Byte 22 - PCI_BST */
};

void MsDelay(void)
{
  printk(".");
}

void write8( int addr, int data ){
  out_8((void *)addr, (unsigned char)data);
}

void write16( int addr, int data ) {
  out_be16((void *)addr, (short)data );
}

void write32( int addr, int data ) {
  out_be32((unsigned int *)addr, data );
}

int read32( int addr){
  return in_be32((unsigned int *)addr);
}


void rsPMCQ1_scc_nullFunc(void) {}

/*******************************************************************************
* rsPMCQ1Int - handle a PMCQ1 interrupt
*
* This routine gets called when the QUICC or MA causes
* an interrupt.
*
* RETURNS: NONE.
*/

void rsPMCQ1Int( void *ptr )
{
  unsigned long   status;
  unsigned long   status1;
  unsigned long   mask;
  uint32_t        data;
  PPMCQ1BoardData boardData = ptr;

  status = PMCQ1_Read_EPLD(boardData->baseaddr, PMCQ1_INT_STATUS );
  mask   = PMCQ1_Read_EPLD(boardData->baseaddr, PMCQ1_INT_MASK );

  if (((mask & PMCQ1_INT_MASK_QUICC) == 0) && (status & PMCQ1_INT_STATUS_QUICC))
  {
    /* If there is a handler call it otherwise mask the interrupt */
    if (boardData->quiccInt) {
      boardData->quiccInt(boardData->quiccArg);
    } else {
      *(volatile unsigned long *)(boardData->baseaddr + PMCQ1_INT_MASK) |= PMCQ1_INT_MASK_QUICC;
    }
  }

  if (((mask & PMCQ1_INT_MASK_MA) == 0) && (status & PMCQ1_INT_STATUS_MA))
  {
    /* If there is a handler call it otherwise mask the interrupt */
    if (boardData->maInt) {
      boardData->maInt(boardData->maArg);

      data = PMCQ1_Read_EPLD(boardData->baseaddr, PMCQ1_INT_STATUS );
      data &= (~PMCQ1_INT_STATUS_MA);
      PMCQ1_Write_EPLD(boardData->baseaddr, PMCQ1_INT_STATUS, data );

    } else {
     *(volatile unsigned long *)(boardData->baseaddr + PMCQ1_INT_MASK) |= PMCQ1_INT_MASK_MA;
    }
  }

  RTEMS_COMPILER_MEMORY_BARRIER();

  /* Clear Interrupt on QSPAN */
  *(volatile unsigned long *)(boardData->bridgeaddr + 0x600) = 0x00001000;

  /* read back the status register to ensure that the pci write has completed */
  status1 = *(volatile unsigned long *)(boardData->bridgeaddr + 0x600);
  RTEMS_COMPILER_MEMORY_BARRIER();

}


/*******************************************************************************
*
* rsPMCQ1MaIntConnect - connect a MiniAce interrupt routine
*
* This routine is called to connect a MiniAce interrupt handler
* upto a PMCQ1.
*
* RETURNS: OK if PMCQ1 found, ERROR if not.
*/

unsigned int rsPMCQ1MaIntConnect (
    unsigned long	busNo,	/* Pci Bus number of PMCQ1 */
    unsigned long	slotNo,	/* Pci Slot number of PMCQ1 */
    unsigned long	funcNo,	/* Pci Function number of PMCQ1 */
    FUNCION_PTR	routine,/* interrupt routine */
    int		arg	/* argument to pass to interrupt routine */
)
{
  PPMCQ1BoardData boardData;
  uint32_t        data;
  unsigned int    status = RTEMS_IO_ERROR;

  for (boardData = pmcq1BoardData; boardData; boardData = boardData->pNext)
  {
    if ((boardData->busNo == busNo) && (boardData->slotNo == slotNo) &&
        (boardData->funcNo == funcNo))
    {
      boardData->maInt = routine;
      boardData->maArg = arg;

      data  = PMCQ1_Read_EPLD(boardData->baseaddr, PMCQ1_INT_MASK );
      data &= (~PMCQ1_INT_MASK_MA);
      PMCQ1_Write_EPLD(boardData->baseaddr, PMCQ1_INT_MASK, data );

      data = PMCQ1_Read_EPLD(boardData->baseaddr, PMCQ1_INT_STATUS );
      data &= (~PMCQ1_INT_STATUS_MA);
      PMCQ1_Write_EPLD(boardData->baseaddr, PMCQ1_INT_STATUS, data );

      status = RTEMS_SUCCESSFUL;
      break;
    }
  }

  return (status);
}

/*******************************************************************************
*
* rsPMCQ1MaIntDisconnect - disconnect a MiniAce interrupt routine
*
* This routine is called to disconnect a MiniAce interrupt handler
* from a PMCQ1. It also masks the interrupt source on the PMCQ1.
*
* RETURNS: OK if PMCQ1 found, ERROR if not.
*/

unsigned int rsPMCQ1MaIntDisconnect(
    unsigned long	busNo,	/* Pci Bus number of PMCQ1 */
    unsigned long	slotNo,	/* Pci Slot number of PMCQ1 */
    unsigned long	funcNo	/* Pci Function number of PMCQ1 */
)
{
  PPMCQ1BoardData boardData;
  unsigned int status = RTEMS_IO_ERROR;

  for (boardData = pmcq1BoardData; boardData; boardData = boardData->pNext) {
    if ((boardData->busNo == busNo) && (boardData->slotNo == slotNo) &&
        (boardData->funcNo == funcNo))
    {
      boardData->maInt = NULL;
      *(unsigned long *)(boardData->baseaddr + PMCQ1_INT_MASK) |= PMCQ1_INT_MASK_MA;
      status = RTEMS_SUCCESSFUL;
      break;
    }
  }

  return (status);
}

/*******************************************************************************
*
* rsPMCQ1QuiccIntConnect - connect a Quicc interrupt routine
*
* This routine is called to connect a Quicc interrupt handler
* upto a PMCQ1.
*
* RETURNS: OK if PMCQ1 found, ERROR if not.
*/

unsigned int rsPMCQ1QuiccIntConnect(
    unsigned long	busNo,	/* Pci Bus number of PMCQ1 */
    unsigned long	slotNo,	/* Pci Slot number of PMCQ1 */
    unsigned long	funcNo,	/* Pci Function number of PMCQ1 */
    FUNCION_PTR	routine,/* interrupt routine */
    int		arg	/* argument to pass to interrupt routine */
)
{
  PPMCQ1BoardData boardData;
  unsigned int status = RTEMS_IO_ERROR;

  for (boardData = pmcq1BoardData; boardData; boardData = boardData->pNext)
  {
    if ((boardData->busNo == busNo) && (boardData->slotNo == slotNo) &&
        (boardData->funcNo == funcNo))
    {
      boardData->quiccInt = routine;
      boardData->quiccArg = arg;
      status = RTEMS_SUCCESSFUL;
      break;
    }
  }
  return (status);
}

/*******************************************************************************
*
* rsPMCQ1QuiccIntDisconnect - disconnect a Quicc interrupt routine
*
* This routine is called to disconnect a Quicc interrupt handler
* from a PMCQ1. It also masks the interrupt source on the PMCQ1.
*
* RETURNS: OK if PMCQ1 found, ERROR if not.
*/

unsigned int rsPMCQ1QuiccIntDisconnect(
    unsigned long	busNo,	/* Pci Bus number of PMCQ1 */
    unsigned long	slotNo,	/* Pci Slot number of PMCQ1 */
    unsigned long	funcNo	/* Pci Function number of PMCQ1 */
)
{
  PPMCQ1BoardData boardData;
  unsigned int status = RTEMS_IO_ERROR;

  for (boardData = pmcq1BoardData; boardData; boardData = boardData->pNext)
  {
    if ((boardData->busNo == busNo) && (boardData->slotNo == slotNo) &&
        (boardData->funcNo == funcNo))
    {
      boardData->quiccInt = NULL;
      *(unsigned long *)(boardData->baseaddr + PMCQ1_INT_MASK) |= PMCQ1_INT_MASK_QUICC;
      status = RTEMS_SUCCESSFUL;
      break;
    }
  }

  return (status);
}


/*******************************************************************************
*
* rsPMCQ1Init - initialize the PMCQ1's
*
* This routine is called to initialize the PCI card to a quiescent state.
*
* RETURNS: OK if PMCQ1 found, ERROR if not.
*/

unsigned int rsPMCQ1Init(void)
{
  int busNo;
  int slotNo;
  unsigned int baseaddr = 0;
  unsigned int bridgeaddr = 0;
  unsigned long pbti0_ctl;
  int i;
  unsigned char int_vector;
  int fun;
  int temp;
  PPMCQ1BoardData       boardData;
  rtems_irq_connect_data IrqData = {0,
                                    rsPMCQ1Int,
                                    rsPMCQ1_scc_nullFunc,
                                    rsPMCQ1_scc_nullFunc,
                                    rsPMCQ1_scc_nullFunc,
                                    NULL};

  if (rsPMCQ1Initialized)
  {
    return RTEMS_SUCCESSFUL;
  }
  for (i=0;;i++){
    if ( pci_find_device(PCI_VEN_ID_RADSTONE, PCI_DEV_ID_PMCQ1, i, &busNo, &slotNo, &fun) != 0 )
      break;

    pci_read_config_dword(busNo, slotNo, 0, PCI_BASE_ADDRESS_2, &baseaddr);
    pci_read_config_dword(busNo, slotNo, 0, PCI_BASE_ADDRESS_0, &bridgeaddr);
#ifdef DEBUG_360
  printk("PMCQ1 baseaddr 0x%08x bridgeaddr 0x%08x\n", baseaddr, bridgeaddr );
#endif

    /* Set function code to normal mode and enable window */
    pbti0_ctl = *(unsigned long *)(bridgeaddr + 0x100) & 0xff0fffff;
    eieio();
    *(unsigned long *)(bridgeaddr + 0x100) = pbti0_ctl | 0x00500080;
    eieio();

    /* Assert QBUS reset */
    *(unsigned long *)(bridgeaddr + 0x800) |= 0x00000080;
    eieio();

    /*
     * Hold QBus in reset for 1ms
     */
    MsDelay();

    /* Take QBUS out of reset */
    *(unsigned long *)(bridgeaddr + 0x800) &= ~0x00000080;
    eieio();

    MsDelay();

    /* If a QUICC is fitted initialise it */
    if (PMCQ1_Read_EPLD(baseaddr, PMCQ1_BUILD_OPTION) & PMCQ1_QUICC_FITTED)
    {
#ifdef DEBUG_360
  printk(" Found QUICC busNo %d slotNo %d\n", busNo, slotNo);
#endif

      /* Initialise MBAR (must use function code of 7) */
      *(unsigned long *)(bridgeaddr + 0x100) = pbti0_ctl | 0x00700080;
      eieio();

      /* place internal 8K SRAM and registers at address 0x0 */
      *(unsigned long *)(baseaddr + Q1_360_MBAR) = 0x1;
      eieio();

      /* Set function code to normal mode */
      *(unsigned long *)(bridgeaddr + 0x100) = pbti0_ctl | 0x00500080;
      eieio();

      /* Disable the SWT and perform basic initialisation */
      write8(baseaddr+Q1_360_SIM_SYPCR,0);
      eieio();

      write32(baseaddr+Q1_360_SIM_MCR,0xa0001029);
      write16(baseaddr+Q1_360_SIM_PICR,0);
      write16(baseaddr+Q1_360_SIM_PITR,0);

      write16(baseaddr+Q1_360_CPM_ICCR,0x770);
      write16(baseaddr+Q1_360_CPM_SDCR,0x770);
      write32(baseaddr+Q1_360_CPM_CICR,0x00e49f00);
      write16(baseaddr+Q1_360_SIM_PEPAR,0x2080);
      eieio();

      /* Enable SRAM */
      write32(baseaddr+Q1_360_SIM_GMR,0x00001000);  /* external master wait state */
      eieio();
      write32(baseaddr+Q1_360_SIM_OR0,0x1ff00000);  /*| MEMC_OR_FC*/
      eieio();
      write32(baseaddr+Q1_360_SIM_BR0,0);
      eieio();
      write32(baseaddr+Q1_360_SIM_OR1,(0x5ff00000 | 0x00000780));  /*| MEMC_OR_FC*/
      eieio();
      write32(baseaddr+Q1_360_SIM_BR1,(0x00000040 | 0x00000001 | 0x00200280) );
      eieio();
    }

    /*
     * If a second PCI window is present then make it opposite
     * endian to simplify 1553 integration.
     */
    pci_read_config_dword(busNo, slotNo, 0, PCI_BASE_ADDRESS_3, &temp);
    if (temp) {
      *(unsigned long *)(bridgeaddr + 0x110) |= 0x00500880;
    }

    /*
     * Create descriptor structure for this card
     */
    if ((boardData = malloc(sizeof(struct _PMCQ1BoardData))) == NULL)
    {
      printk("Error Unable to allocate memory for _PMCQ1BoardData\n");
      return(RTEMS_IO_ERROR);
    }

    boardData->pNext = pmcq1BoardData;
    boardData->busNo = busNo;
    boardData->slotNo = slotNo;
    boardData->funcNo = 0;
    boardData->baseaddr = baseaddr;
    boardData->bridgeaddr = bridgeaddr;
    boardData->quiccInt = NULL;
    boardData->maInt = NULL;
    pmcq1BoardData = boardData;
    mc68360_scc_create_chip( boardData, int_vector );

    /*
     * Connect PMCQ1 interrupt handler.
     */
    pci_read_config_byte(busNo, slotNo, 0, 0x3c, &int_vector);
#ifdef DEBUG_360
    printk("PMCQ1 int_vector %d\n", int_vector);
#endif
    IrqData.name  = ((unsigned int)BSP_PCI_IRQ0 + int_vector);
    IrqData.handle = boardData;
    if (!BSP_install_rtems_shared_irq_handler (&IrqData)) {
        printk("Error installing interrupt handler!\n");
        rtems_fatal_error_occurred(1);
    }

    /*
     * Enable PMCQ1 Interrupts from QSPAN-II
     */

    *(unsigned long *)(bridgeaddr + 0x600) = 0x00001000;
    eieio();
    *(unsigned long *)(bridgeaddr + 0x604) |= 0x00001000;

    eieio();
  }

  if (i > 0)
  {
    rsPMCQ1Initialized = TRUE;
  }
  return((i > 0) ? RTEMS_SUCCESSFUL : RTEMS_IO_ERROR);
}

/*******************************************************************************
*
* rsPMCQ1Commission - initialize the serial EEPROM on the QSPAN
*
* This routine is called to initialize the EEPROM attached to the QSPAN
* on the PMCQ1 module. It will load standard settings into any QSPAN's
* found with apparently uninitialised EEPROM's or PMCQ1's (to allow
* EEPROM modifications to be performed).
*/

unsigned int rsPMCQ1Commission( unsigned long busNo, unsigned long slotNo )
{
  unsigned int status = RTEMS_IO_ERROR;
  uint32_t     bridgeaddr = 0;
  unsigned long val;
  int i;
  uint32_t venId1;
  uint32_t venId2;

  pci_read_config_dword(busNo, slotNo, 0, PCI_VENDOR_ID, &venId1);
  pci_read_config_dword(busNo, slotNo, 0, PCI_VENDOR_ID, &venId2);
  if ((venId1 == 0x086210e3) ||
      (venId2 == PCI_ID(PCI_VEN_ID_RADSTONE, PCI_DEV_ID_PMCQ1)))
  {
    pci_read_config_dword(busNo, slotNo, 0, PCI_BASE_ADDRESS_0, &bridgeaddr);
    status = RTEMS_SUCCESSFUL;

    /*
     * The On board PMCQ1 on an EP1A has a subVendor ID of 0.
     * A real PMCQ1 also has the sub vendor ID set up.
     */
    if ((busNo == 0) && (slotNo == 1)) {
      *(unsigned long *)rsPMCQ1eeprom = 0;
    } else {
      *(unsigned long *)rsPMCQ1eeprom = PCI_ID(PCI_VEN_ID_RADSTONE, PCI_DEV_ID_PMCQ1);
    }

    for (i = 0; i < 23; i++) {
      /* Wait until interface not active */
      while(read32(bridgeaddr + 0x804) & 0x80000000) {
        rtems_bsp_delay(1);
      }

      /* Write value */
      write32(bridgeaddr + 0x804, (rsPMCQ1eeprom[i] << 8) | i);

      /* delay for > 31 usec to allow active bit to become set */
      rtems_bsp_delay(100);

      /* Wait until interface not active */
      while(read32(bridgeaddr + 0x804) & 0x80000000) {
        rtems_bsp_delay(1);
      }

      /* Re-read value */
      write32(bridgeaddr + 0x804, 0x40000000 | i);

      /* delay for > 31 usec to allow active bit to become set */
      rtems_bsp_delay(100);

      /* Wait until interface not active */
      while((val = read32(bridgeaddr + 0x804)) & 0x80000000) {
        rtems_bsp_delay(1);
      }

      if (((val >> 8) & 0xff) != rsPMCQ1eeprom[i]) {
        printk("Error writing byte %d expected 0x%02x got 0x%02x\n",
               i, rsPMCQ1eeprom[i], (unsigned char)(val >> 8));
        status = RTEMS_IO_ERROR;
        break;
      }
    }
  }
  return(status);
}

uint32_t PMCQ1_Read_EPLD( uint32_t base, uint32_t reg )
{
  uint32_t data;

  data = ( *((unsigned long *) (base + reg)) );
#ifdef DEBUG_360
  printk("EPLD Read 0x%x: 0x%08x\n", reg + base, data );
#endif
  return data;
}

void PMCQ1_Write_EPLD( uint32_t base, uint32_t reg, uint32_t data )
{
  *((unsigned long *) (base + reg)) = data;
#ifdef DEBUG_360
  printk("EPLD Write 0x%x: 0x%08x\n", reg+base, data );
#endif
}

