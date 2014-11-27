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
 *  http://www.rtems.org/license/LICENSE.
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

static void MsDelay(void)
{
  printk(".");
}

static void write8( int addr, int data ){
  out_8((uint8_t *)addr, (uint8_t)data);
}

static void write16( int addr, int data ) {
  out_be16((uint16_t *)addr, (uint16_t)data );
}

static void write32( int addr, int data ) {
  out_be32((uint32_t *)addr, (uint32_t)data );
}

static void rsPMCQ1_scc_nullFunc(void) {}

/*******************************************************************************
* rsPMCQ1Int - handle a PMCQ1 interrupt
*
* This routine gets called when the QUICC or MA causes
* an interrupt.
*
* RETURNS: NONE.
*/

static void rsPMCQ1Int( void *ptr )
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
  (void) status1;  /* avoid set but not used warning */
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
    FUNCTION_PTR	routine,/* interrupt routine */
    uintptr_t		arg	/* argument to pass to interrupt routine */
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
    FUNCTION_PTR	routine,/* interrupt routine */
    uintptr_t		arg	/* argument to pass to interrupt routine */
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
  uint32_t baseaddr = 0;
  uint32_t bridgeaddr = 0;
  unsigned long pbti0_ctl;
  int i;
  unsigned char int_vector;
  int fun;
  uint32_t temp;
  PPMCQ1BoardData       boardData;
  rtems_irq_connect_data IrqData = {
    .name   = 0,
    .hdl    = rsPMCQ1Int,
    .handle = NULL,
    .on     = (rtems_irq_enable) rsPMCQ1_scc_nullFunc,
    .off    = (rtems_irq_disable) rsPMCQ1_scc_nullFunc,
    .isOn   = (rtems_irq_is_enabled) rsPMCQ1_scc_nullFunc,
  };

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

