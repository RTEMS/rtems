/*  bspstart.c for eZKit533
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *  
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>


/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, uint32_t, int );
void Init_PLL (void);
void Init_EBIU (void);
void Init_Flags(void);
void Init_RTC (void);

void null_isr(void);

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_pretasking_hook(void)
{
    extern int HeapBase;
    extern int HeapSize;
    void         *heapStart = &HeapBase;
    unsigned long heapSize = (unsigned long)&HeapSize;

    bsp_libc_init(heapStart, heapSize, 0);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
    
  extern void          * _WorkspaceBase;

  /* BSP Hardware Initialization*/
  Init_RTC();   /* Blackfin Real Time Clock initialization */  
  Init_PLL();   /* PLL initialization */
  Init_EBIU();  /* EBIU initialization */
  Init_Flags(); /* GPIO initialization */

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  BSP_Configuration.work_space_start = (void *) &_WorkspaceBase;

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  int i=0;
  for (i=5;i<16;i++) {
    set_vector((rtems_isr_entry)null_isr, i, 1);
  }
  
}

 /*
  * Init_PLL
  * 
  * Routine to initialize the PLL. The Ezkit uses a 27 Mhz XTAL.
  * See "../eZKit533/include/bsp.h" for more information.
  */

void Init_PLL (void)
{
 
  unsigned int n;
  
  /* Configure PLL registers */
  *((uint16_t*)PLL_LOCKCNT) = 0x1000;
  *((uint16_t*)PLL_DIV) = PLL_CSEL|PLL_SSEL;
  *((uint16_t*)PLL_CTL) = PLL_MSEL|PLL_DF;

  /* Commands to set PLL values */
  asm("cli r0;");
  asm("idle;");
  asm("sti r0;");
  
  /* Delay for PLL stabilization */
  for (n=0; n<200; n++) {} 
  
}

 /*
  * Init_EBIU
  * 
  * Configure extern memory
  */

void Init_EBIU (void)
{
  /* Configure FLASH */
  *((uint32_t*)EBIU_AMBCTL0)  = 0x7bb07bb0L;
  *((uint32_t*)EBIU_AMBCTL1)  = 0x7bb07bb0L;
  *((uint16_t*)EBIU_AMGCTL)   = 0x000f;
  
  /* Configure SDRAM 
  *((uint32_t*)EBIU_SDGCTL) = 0x0091998d;
  *((uint16_t*)EBIU_SDBCTL) = 0x0013;
  *((uint16_t*)EBIU_SDRRC)  = 0x0817;
  */
}

 /*
  * Init_Flags
  * 
  * Enable LEDs port
  */
void Init_Flags(void)
{
  *((uint16_t*)FIO_INEN)    = 0x0100;
  *((uint16_t*)FIO_DIR)     = 0x0000;
  *((uint16_t*)FIO_EDGE)    = 0x0100;
  *((uint16_t*)FIO_MASKA_D) = 0x0100;
  
  *((uint8_t*)FlashA_PortB_Dir)  = 0x3f;
  *((uint8_t*)FlashA_PortB_Data) = 0x00;    
}

/*
 * Helper Function to use the EzKits LEDS.
 * Can be used by the Application.
 */
void setLED (uint8_t value)
{
  *((uint8_t*)FlashA_PortB_Data) = value;    
}

/*
 * Helper Function to use the EzKits LEDS
 */
uint8_t getLED (void)
{
  return *((uint8_t*)FlashA_PortB_Data);
}
