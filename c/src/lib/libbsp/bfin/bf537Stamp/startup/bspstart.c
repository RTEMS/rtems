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
 */


#include <bsp.h>
#include <libcpu/bf537.h>
#include <libcpu/ebiuRegs.h>
#include <libcpu/gpioRegs.h>
#include <libcpu/mmu.h>
#include <libcpu/mmuRegs.h>
#include <libcpu/interrupt.h>


static bfin_mmu_config_t mmuRegions = {
    /* instruction */
    {
        {(void *) 0x00000000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x00400000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x00800000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x00c00000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x01000000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x01400000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x01800000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x01c00000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x02000000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x02400000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x02800000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x02c00000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x03000000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0x20000000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_CACHEABLE},
        {(void *) 0xff800000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_NOCACHE},
        {(void *) 0xffc00000, ICPLB_DATA_PAGE_SIZE_4MB | INSTR_NOCACHE}
    },
    /* data */
    {
        {(void *) 0x00000000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x00400000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x00800000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x00c00000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x01000000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x01400000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x01800000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x01c00000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x02000000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x02400000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x02800000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x02c00000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x03000000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0x20000000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_WRITEBACK},
        {(void *) 0xff800000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_NOCACHE},
        {(void *) 0xffc00000, DCPLB_DATA_PAGE_SIZE_4MB | DATA_NOCACHE}
    }
};

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

  *(uint32_t volatile *) DMEM_CONTROL |= DMEM_CONTROL_PORT_PREF0;
  *(uint32_t volatile *) DMEM_CONTROL &= ~DMEM_CONTROL_PORT_PREF1;
  bfin_mmu_init(&mmuRegions);
  rtems_cache_enable_instruction();
  rtems_cache_enable_data();

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
  __asm__ ("cli r0;");
  __asm__ ("idle;");
  __asm__ ("sti r0;");

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

  /* by default the processor has priority over dma channels for access to
     external memory.  this has been seen to result in dma unerruns on
     ethernet transmit; it seems likely it could cause dma overruns on
     ethernet receive as well.  setting the following bit gives the dma
     channels priority over the cpu, fixing that problem.  unfortunately
     we don't have finer grain control than that; all dma channels now
     have priority over the cpu. */
  *(uint16_t volatile *) EBIU_AMGCTL |= EBIU_AMGCTL_CDPRIO;

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
#if (!BFIN_ON_SKYEYE)
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
#endif
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


