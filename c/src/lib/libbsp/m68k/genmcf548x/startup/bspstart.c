/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: bspstart.c                                                |
+-----------------------------------------------------------------+
| The file contains the startup code of generic MCF548x BSP       |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

#include <bsp.h>

extern uint32_t _CPU_cacr_shadow;

/*
* These labels (!) are defined in the linker command file or when the linker is
* invoked.
* NOTE: The information (size) is the address of the object, not the object
* itself.
*/

extern char _SdramBase[];
extern char _BootFlashBase[];
extern char _CodeFlashBase[];
extern char _SdramSize[];
extern char _BootFlashSize[];
extern char _CodeFlashSize[];
extern char _TopRamReserved [];
extern char WorkAreaBase [];

/*
 * CPU-space access
 */
#define m68k_set_cacr(_cacr) asm volatile ("movec %0,%%cacr\n\tnop" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) asm volatile ("movec %0,#0x0004" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) asm volatile ("movec %0,#0x0005" : : "d" (_acr1))
#define m68k_set_acr2(_acr2) asm volatile ("movec %0,#0x0005" : : "d" (_acr2))
#define m68k_set_acr3(_acr3) asm volatile ("movec %0,#0x0007" : : "d" (_acr3))

/*
 * Set initial cacr mode, mainly enables branch/intruction/data cache and switch off FPU.
 */
static uint32_t cacr_mode = (0                                          |
                             MCF548X_CACR_DEC                           | /* enable data cache */
                             MCF548X_CACR_BEC                           | /* enable branch cache */
                             MCF548X_CACR_IEC                           | /* enable instruction cache */
                             MCF548X_CACR_DDCM(DCACHE_ON_WRIGHTTHROUGH) | /* set data cache mode to write-through */
                             MCF548X_CACR_DESB                          | /* enable data store buffer */
                             MCF548X_CACR_DDSP                          | /* data access only in supv. mode */
                             MCF548X_CACR_IDSP                          | /* instr. access only in supv. mode */
                             MCF548X_CACR_DF);                            /* disable FPU */


/*
 * Coldfire cacr maintenance functions
 */
void _CPU_cacr_set_mode(uint32_t new_cacr_mode)
{
rtems_interrupt_level level;

rtems_interrupt_disable(level);
cacr_mode = new_cacr_mode;
m68k_set_cacr(new_cacr_mode);
rtems_interrupt_enable(level);
}

/*
 * There is no complete cache lock (only 2 ways of 4 can be locked)
 */
void _CPU_cache_freeze_data(void)
{
}

void _CPU_cache_unfreeze_data(void)
{
}

void _CPU_cache_freeze_instruction(void)
{
}

void _CPU_cache_unfreeze_instruction(void)
{
}

void _CPU_cache_enable_instruction(void)
{
    cacr_mode &= ~(MCF548X_CACR_IDCM);
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_disable_instruction(void)
{
    cacr_mode |= MCF548X_CACR_IDCM;
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_invalidate_entire_instruction(void)
{
	cacr_mode |= MCF548X_CACR_ICINVA;
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{

    asm volatile ("cpushl %%ic,(%0)" :: "a" (addr));
}

void _CPU_cache_enable_data(void)
{
    cacr_mode &= ~MCF548X_CACR_DDCM(DCACHE_OFF_IMPRECISE);
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_disable_data(void)
{
    cacr_mode |= MCF548X_CACR_DDCM(DCACHE_OFF_IMPRECISE);
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_invalidate_entire_data(void)
{
    cacr_mode |= MCF548X_CACR_DCINVA;
    _CPU_cacr_set_mode(cacr_mode);
}

void _CPU_cache_invalidate_1_data_line(const void *addr)
{

   asm volatile ("cpushl %%dc,(%0)" :: "a" (addr));
}

void _CPU_cache_flush_1_data_line(const void *addr)
{
   asm volatile ("cpushl %%dc,(%0)" :: "a" (addr));
}

void _CPU_cache_flush_entire_data(void)
{
register uint32_t way_cnt, set_cnt, addr;

asm volatile("nop");

for(way_cnt=0; way_cnt<4; way_cnt++)
  {
  for(addr=0,set_cnt=0; set_cnt<512; set_cnt++,addr+=0x10)
    {
    asm volatile ("cpushl %%dc,(%0)" :: "a" (addr));
    }
  addr=way_cnt;
  }
}

/*
 * Coldfire acr and mmu settings
 */
 void acr_mmu_mapping(void)
   {

  /*
   * Cache disabled for internal register area (256 kB).
   * Choose the smallest maskable size of 1MB.
   */
  m68k_set_acr0(MCF548X_ACR_BA((uint32_t)(__MBAR))                           |
                MCF548X_ACR_ADMSK_AMM((uint32_t)(0xFFFFF))                   |
                MCF548X_ACR_E                                                |
                MCF548X_ACR_SP               /* supervisor protection */     |
                MCF548X_ACR_S(S_ACCESS_SUPV) /* always in supervisor mode */ |
                MCF548X_ACR_CM(CM_OFF_PRECISE));

#ifdef M5484FIREENGINE


  /*
   * Cache enabled for entire SDRAM (64 MB)
   */
  m68k_set_acr1(MCF548X_ACR_BA((uint32_t)(_SdramBase))                       |
                MCF548X_ACR_ADMSK_AMM((uint32_t)(_SdramSize - 1))            |
                MCF548X_ACR_E                                                |
                MCF548X_ACR_SP               /* supervisor protection */     |
                MCF548X_ACR_S(S_ACCESS_SUPV) /* always in supervisor mode */ |
                MCF548X_ACR_CM(CM_ON_WRIGHTTHROUGH));

  /*
   * Cache enabled for entire boot flash (2 MB)
   */
  m68k_set_acr2(MCF548X_ACR_BA((uint32_t)(_BootFlashBase))                   |
                MCF548X_ACR_ADMSK_AMM((uint32_t)(_BootFlashSize - 1))        |
                MCF548X_ACR_E                                                |
                MCF548X_ACR_SP               /* supervisor protection */     |
                MCF548X_ACR_S(S_ACCESS_SUPV) /* always in supervisor mode */ |
                MCF548X_ACR_CM(CM_ON_COPYBACK));

  /*
   * Cache enabled for entire code flash (16 MB)
   */
  m68k_set_acr3(MCF548X_ACR_BA((uint32_t)(_CodeFlashBase))                   |
                MCF548X_ACR_ADMSK_AMM((uint32_t)(_CodeFlashSize - 1))        |
                MCF548X_ACR_E                                                |
                MCF548X_ACR_SP               /* supervisor protection */     |
                MCF548X_ACR_S(S_ACCESS_SUPV) /* always in supervisor mode */ |
                MCF548X_ACR_CM(CM_ON_COPYBACK));
#endif

   }

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialisation.
 */
void bsp_start( void )
{
  /*
   * do mapping of acr's and/or mmu
   */
  acr_mmu_mapping();

  /*
   * Load the shadow variable of cacr with initial mode and write it to the cacr.
   * Interrupts are still disabled, so there is no need for surrounding rtems_interrupt_enable()/rtems_interrupt_disable()
   */
  _CPU_cacr_shadow = cacr_mode;
  m68k_set_cacr(_CPU_cacr_shadow);

}


/*
 * Get the XLB clock speed
 */
uint32_t get_CPU_clock_speed(void)
{
    return (uint32_t)BSP_CPU_CLOCK_SPEED;
}
