/**
 * @file
 *
 * #ingroup powerpc_shared
 *
 * @brief Header file for the Cache Manager PowerPC support.
 */

/*
 *  Cache Management Support Routines for the MC68040
 * Modified for MPC8260 Andy Dachs <a.dachs@sstl.co.uk>
 * Surrey Satellite Technology Limited (SSTL), 2001
 */

#ifndef LIBCPU_POWERPC_CACHE_H
#define LIBCPU_POWERPC_CACHE_H

#include <rtems.h>
#include <rtems/powerpc/powerpc.h>
#include <rtems/powerpc/registers.h>

/* Provide the CPU defines only if we have a cache */
#if PPC_CACHE_ALIGNMENT != PPC_NO_CACHE_ALIGNMENT
  #define CPU_DATA_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
  #define CPU_INSTRUCTION_CACHE_ALIGNMENT PPC_CACHE_ALIGNMENT
#endif

/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 *
 * FIXME: Some functions simply have not been implemented.
 */

#if defined(ppc603) || defined(ppc603e) || defined(mpc8260) /* And possibly others */

/* Helpful macros */
#define PPC_Get_HID0( _value ) \
  do { \
      _value = 0;        /* to avoid warnings */ \
      __asm__ volatile( \
          "mfspr %0, 0x3f0;"     /* get HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)

#define PPC_Set_HID0( _value ) \
  do { \
      __asm__ volatile( \
          "isync;" \
          "mtspr 0x3f0, %0;"     /* load HID0 */ \
          "isync" \
          : "=r" (_value) \
          : "0" (_value) \
      ); \
  } while (0)

static inline void _CPU_cache_enable_data(void)
{
  uint32_t   value;
  PPC_Get_HID0( value );
  value |= HID0_DCE;        /* set DCE bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_disable_data(void)
{
  uint32_t   value;
  PPC_Get_HID0( value );
  value &= ~HID0_DCE;        /* clear DCE bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value |= HID0_DCI;        /* set data flash invalidate bit */
  PPC_Set_HID0( value );
  value &= ~HID0_DCI;        /* clear data flash invalidate bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_freeze_data(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value |= HID0_DLOCK;        /* set data cache lock bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_unfreeze_data(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value &= ~HID0_DLOCK;        /* set data cache lock bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_flush_entire_data(void)
{
  /*
   * FIXME: how can we do this?
   */
}

static inline void _CPU_cache_enable_instruction(void)
{
  uint32_t   value;
  PPC_Get_HID0( value );
  value |= 0x00008000;       /* Set ICE bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_disable_instruction(void)
{
  uint32_t   value;
  PPC_Get_HID0( value );
  value &= 0xFFFF7FFF;       /* Clear ICE bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value |= HID0_ICFI;        /* set data flash invalidate bit */
  PPC_Set_HID0( value );
  value &= ~HID0_ICFI;        /* clear data flash invalidate bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_freeze_instruction(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value |= HID0_ILOCK;        /* set instruction cache lock bit */
  PPC_Set_HID0( value );
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
  uint32_t  value;
  PPC_Get_HID0( value );
  value &= ~HID0_ILOCK;        /* set instruction cache lock bit */
  PPC_Set_HID0( value );
}

#elif ( defined(mpx8xx) || defined(mpc860) || defined(mpc821) )

#define mtspr(_spr,_reg) \
  __asm__ volatile ( "mtspr %0, %1\n" : : "i" ((_spr)), "r" ((_reg)) )
#define isync \
  __asm__ volatile ("isync\n"::)

static inline void _CPU_cache_flush_entire_data(void) {}
static inline void _CPU_cache_invalidate_entire_data(void) {}
static inline void _CPU_cache_freeze_data(void) {}
static inline void _CPU_cache_unfreeze_data(void) {}

static inline void _CPU_cache_enable_data(void)
{
  uint32_t   r1;
  r1 = (0x2<<24);
  mtspr( 568, r1 );
  isync;
}

static inline void _CPU_cache_disable_data(void)
{
  uint32_t   r1;
  r1 = (0x4<<24);
  mtspr( 568, r1 );
  isync;
}

static inline void _CPU_cache_invalidate_entire_instruction(void) {}
static inline void _CPU_cache_freeze_instruction(void) {}
static inline void _CPU_cache_unfreeze_instruction(void) {}

static inline void _CPU_cache_enable_instruction(void)
{
  uint32_t   r1;
  r1 = (0x2<<24);
  mtspr( 560, r1 );
  isync;
}

static inline void _CPU_cache_disable_instruction(void)
{
  uint32_t   r1;
  r1 = (0x4<<24);
  mtspr( 560, r1 );
  isync;
}

#else

#warning Most cache functions are not implemented

static inline void _CPU_cache_flush_entire_data(void)
{
	/* Void */
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
	/* Void */
}

static inline void _CPU_cache_freeze_data(void)
{
	/* Void */
}

static inline void _CPU_cache_unfreeze_data(void)
{
	/* Void */
}

static inline void _CPU_cache_enable_data(void)
{
	/* Void */
}

static inline void _CPU_cache_disable_data(void)
{
	/* Void */
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
	/* Void */
}

static inline void _CPU_cache_freeze_instruction(void)
{
	/* Void */
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
	/* Void */
}

static inline void _CPU_cache_enable_instruction(void)
{
	/* Void */
}

static inline void _CPU_cache_disable_instruction(void)
{
	/* Void */
}

#endif

static inline void _CPU_cache_invalidate_1_data_line(const void *addr)
{
  __asm__ volatile ( "dcbi 0,%0" :: "r" (addr) : "memory" );
}

static inline void _CPU_cache_flush_1_data_line(const void *addr)
{
  __asm__ volatile ( "dcbf 0,%0" :: "r" (addr) : "memory" );
}


static inline void _CPU_cache_invalidate_1_instruction_line(const void *addr)
{
  __asm__ volatile ( "icbi 0,%0" :: "r" (addr) : "memory");
}

#endif /* LIBCPU_POWERPC_CACHE_H */
