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
#define m68k_set_acr2(_acr2) __asm__ volatile ("movec %0,#0x0005" : : "d" (_acr2))
#define m68k_set_acr3(_acr3) __asm__ volatile ("movec %0,#0x0007" : : "d" (_acr3))

/*
 * Set initial CACR mode, mainly enables branch/instruction/data cache.  The
 * FPU must be switched on in the BSP startup code since the
 * _Thread_Start_multitasking() will restore the floating-point context of the
 * initialization task if necessary.
 */
static const uint32_t BSP_CACR_INIT = MCF548X_CACR_DEC /* enable data cache */
  | MCF548X_CACR_BEC /* enable branch cache */
  | MCF548X_CACR_IEC /* enable instruction cache */
  | MCF548X_CACR_DDCM(DCACHE_ON_WRIGHTTHROUGH)
      /* set data cache mode to write-through */
  | MCF548X_CACR_DESB /* enable data store buffer */
  | MCF548X_CACR_DDSP /* data access only in supv. mode */
  | MCF548X_CACR_IDSP; /* instr. access only in supv. mode */

/*
 * CACR maintenance functions
 */

void bsp_cacr_set_flags( uint32_t flags)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  _CPU_cacr_shadow |= flags;
  m68k_set_cacr( _CPU_cacr_shadow);
  rtems_interrupt_enable( level);
}

void bsp_cacr_set_self_clear_flags( uint32_t flags)
{
  rtems_interrupt_level level;
  uint32_t cacr = 0;

  rtems_interrupt_disable( level);
  cacr = _CPU_cacr_shadow | flags;
  m68k_set_cacr( cacr);
  rtems_interrupt_enable( level);
}

void bsp_cacr_clear_flags( uint32_t flags)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable( level);
  _CPU_cacr_shadow &= ~flags;
  m68k_set_cacr( _CPU_cacr_shadow);
  rtems_interrupt_enable( level);
}

/*
 * There is no complete cache lock (only 2 ways of 4 can be locked)
 */
void _CPU_cache_freeze_data(void)
{
  /* Do nothing */
}

void _CPU_cache_unfreeze_data(void)
{
  /* Do nothing */
}

void _CPU_cache_freeze_instruction(void)
{
  /* Do nothing */
}

void _CPU_cache_unfreeze_instruction(void)
{
  /* Do nothing */
}

void _CPU_cache_enable_instruction(void)
{
  bsp_cacr_clear_flags( MCF548X_CACR_IDCM);
}

void _CPU_cache_disable_instruction(void)
{
  bsp_cacr_set_flags( MCF548X_CACR_IDCM);
}

void _CPU_cache_invalidate_entire_instruction(void)
{
  bsp_cacr_set_self_clear_flags( MCF548X_CACR_ICINVA);
}

void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%ic,(%0)" :: "a" (a | 0x3));
}

void _CPU_cache_enable_data(void)
{
  bsp_cacr_clear_flags( MCF548X_CACR_DDCM( DCACHE_OFF_IMPRECISE));
}

void _CPU_cache_disable_data(void)
{
  bsp_cacr_set_flags( MCF548X_CACR_DDCM( DCACHE_OFF_IMPRECISE));
}

void _CPU_cache_invalidate_entire_data(void)
{
  bsp_cacr_set_self_clear_flags( MCF548X_CACR_DCINVA);
}

void _CPU_cache_invalidate_1_data_line( const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x3));
}

void _CPU_cache_flush_1_data_line( const void *addr)
{
  uint32_t a = (uint32_t) addr & ~0x3;

  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x0));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x1));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x2));
  __asm__ volatile ("cpushl %%dc,(%0)" :: "a" (a | 0x3));
}

void _CPU_cache_flush_entire_data( void)
{
  uint32_t line = 0;

  for (line = 0; line < 512; ++line) {
    _CPU_cache_flush_1_data_line( (const void *) (line * 16));
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
  /* Initialize CACR shadow register */
  _CPU_cacr_shadow = BSP_CACR_INIT;

  /*
   * Load the shadow variable of CACR with initial mode and write it to the
   * CACR.  Interrupts are still disabled, so there is no need for surrounding
   * rtems_interrupt_enable() / rtems_interrupt_disable().
   */
  m68k_set_cacr( _CPU_cacr_shadow);

  /*
   * do mapping of acr's and/or mmu
   */
  acr_mmu_mapping();
}


/*
 * Get the XLB clock speed
 */
uint32_t get_CPU_clock_speed(void)
{
    return (uint32_t)BSP_CPU_CLOCK_SPEED;
}
