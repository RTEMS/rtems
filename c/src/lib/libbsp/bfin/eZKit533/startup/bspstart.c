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
 */


#include <bsp.h>
#include <cplb.h>
#include <libcpu/interrupt.h>

const unsigned int dcplbs_table[16][2] = {
  { 0xFFA00000,   (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) },
  { 0xFF900000,   (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) }, /* L1 Data B */
  { 0xFF800000,   (PAGE_SIZE_1MB | CPLB_D_PAGE_MGMT | CPLB_WT) }, /* L1 Data A */
  { 0xFFB00000,   (PAGE_SIZE_1MB | CPLB_DNOCACHE) },
  { 0x20300000,   (PAGE_SIZE_1MB | CPLB_DNOCACHE) },      /* Async Memory Bank 3 */
  { 0x20200000,   (PAGE_SIZE_1MB | CPLB_DNOCACHE) },      /* Async Memory Bank 2 (Secnd)  */
  { 0x20100000,   (PAGE_SIZE_1MB | CPLB_DNOCACHE) },      /* Async Memory Bank 1 (Prim B) */
  { 0x20000000,   (PAGE_SIZE_1MB | CPLB_DNOCACHE) },      /* Async Memory Bank 0 (Prim A) */

  { 0x02400000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x02000000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00C00000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00800000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00400000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0x00000000,   (PAGE_SIZE_4MB | CPLB_DNOCACHE) },
  { 0xffffffff, 0xffffffff }                 /* end of section - termination */

};


const unsigned int _icplbs_table[16][2] = {
  { 0xFFA00000,   (PAGE_SIZE_1MB | CPLB_I_PAGE_MGMT | CPLB_I_PAGE_MGMT | 0x4) },  /* L1 Code */
  { 0xEF000000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },      /* AREA DE BOOT */
  { 0xFFB00000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },
  { 0x20300000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },      /* Async Memory Bank 3 */
  { 0x20200000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },      /* Async Memory Bank 2 (Secnd) */
  { 0x20100000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },      /* Async Memory Bank 1 (Prim B) */
  { 0x20000000,   (PAGE_SIZE_1MB | CPLB_INOCACHE) },      /* Async Memory Bank 0 (Prim A) */

  { 0x02400000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x02000000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00C00000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00800000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00400000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0x00000000,   (PAGE_SIZE_4MB | CPLB_INOCACHE) },
  { 0xffffffff, 0xffffffff }               /* end of section - termination */

};

/*
 *  Use the shared implementations of the following routines
 */

void Init_PLL (void);
void Init_EBIU (void);
void Init_Flags(void);
void Init_RTC (void);

void null_isr(void);

/*
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  bfin_interrupt_init();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  /* BSP Hardware Initialization*/
  Init_RTC();   /* Blackfin Real Time Clock initialization */
  Init_PLL();   /* PLL initialization */
  Init_EBIU();  /* EBIU initialization */
  Init_Flags(); /* GPIO initialization */

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
  __asm__ ("cli r0;");
  __asm__ ("idle;");
  __asm__ ("sti r0;");

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

void initCPLB(void)
{
  int i = 0;
  unsigned int *addr;
  unsigned int *data;

  addr = (unsigned int *)0xffe00100;
  data = (unsigned int *)0xffe00200;

  while ( dcplbs_table[i][0] != 0xffffffff ) {
    *addr = dcplbs_table[i][0];
    *data = dcplbs_table[i][1];
    addr++;
    data++;
  }
}
