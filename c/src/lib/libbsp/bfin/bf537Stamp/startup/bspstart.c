/*  bspstart.c for bf537Stamp
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

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <libcpu/bf537.h>
#include <libcpu/gpioRegs.h>
#include <libcpu/mmu.h>
#include <libcpu/interrupt.h>
#include <bsp.h>


#if 0
static bfin_mmu_region_t mmuRegions[] = {
};
#endif

void bsp_libc_init(void *, uint32_t, int);

void Init_RTC(void);

static void initPLL(void);
static void initEBIU(void);
static void initGPIO(void);

/*
 *  BSP pretasking hook.
 */
void bsp_pretasking_hook(void)
{
  bfin_interrupt_init();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the BSP initialization.
 */
void bsp_start(void)
{
  /* BSP Hardware Initialization*/

  /*bfin_mmu_init(sizeof(mmuRegions) / sizeof(mmuRegions[0]), mmuRegions);*/

  Init_RTC();   /* Blackfin Real Time Clock initialization */

  initPLL();   /* PLL initialization */
  initEBIU();  /* EBIU initialization */
  initGPIO();  /* GPIO initialization */
}

 /*
  * initPLL
  * 
  * Routine to initialize the PLL. The BF537 Stamp uses a 27 Mhz XTAL. BISON
  * See "../bf537Stamp/include/bsp.h" for more information.
  */

static void initPLL(void) {
 
#ifdef BISON
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
#endif
  
}

 /*
  * initEBIU
  * 
  * Configure extern memory
  */

static void initEBIU(void) {
#ifdef BISON
  /* Configure FLASH */
  *((uint32_t*)EBIU_AMBCTL0)  = 0x7bb07bb0L;
  *((uint32_t*)EBIU_AMBCTL1)  = 0x7bb07bb0L;
  *((uint16_t*)EBIU_AMGCTL)   = 0x000f;
  
  /* Configure SDRAM 
  *((uint32_t*)EBIU_SDGCTL) = 0x0091998d;
  *((uint16_t*)EBIU_SDBCTL) = 0x0013;
  *((uint16_t*)EBIU_SDRRC)  = 0x0817;
  */
#endif
}

 /*
  * initGPIO
  * 
  * Enable LEDs port
  */
static void initGPIO(void) {

  *(uint16_t volatile *) PORT_MUX = 0;

  /* port f bits 0, 1: uart0 tx, rx */
  /*        bits 2 - 5: buttons */
  /*        bits 6 - 11: leds */
  *(uint16_t volatile *) PORTF_FER = 0x0003;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_INEN_OFFSET) = 0x003c;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_POLAR_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_EDGE_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_BOTH_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_MASKA_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_MASKB_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_DIR_OFFSET) = 0x0fc0;

  *(uint16_t volatile *) PORTG_FER = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_INEN_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_POLAR_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_EDGE_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_BOTH_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_MASKA_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_MASKB_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTGIO_BASE_ADDRESS + PORTIO_DIR_OFFSET) = 0x0000;

  /* port h bits 0 - 15: ethernet */
  *(uint16_t volatile *) PORTH_FER = 0xffff;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_INEN_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_POLAR_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_EDGE_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_BOTH_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_MASKA_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_MASKB_OFFSET) = 0x0000;
  *(uint16_t volatile *) (PORTHIO_BASE_ADDRESS + PORTIO_DIR_OFFSET) = 0x0000;
}

/*
 * Helper Function to use the EzKits LEDS.
 * Can be used by the Application.
 */
void setLEDs(uint8_t value) {

  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_CLEAR_OFFSET) =
      (uint16_t) (~value & 0x3f) << 6;
  *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_SET_OFFSET) =
      (uint16_t) (value & 0x3f) << 6;
}

/*
 * Helper Function to use the EzKits LEDS
 */
uint8_t getLEDs(void) {
  uint16_t r;

  r = *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_OFFSET);
  return (uint8_t) ((r >> 6) & 0x3f);
}

uint8_t getButtons(void) {
  uint16_t r;

  r = *(uint16_t volatile *) (PORTFIO_BASE_ADDRESS + PORTIO_OFFSET);

  return (uint8_t) ((r >> 2) & 0x0f);
}


