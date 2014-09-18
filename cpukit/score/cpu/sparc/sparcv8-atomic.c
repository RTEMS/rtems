/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpuopts.h>
#include <rtems/score/isrlevel.h>

/*
 * This file is a dirty hack.  A proper solution would be to add RTEMS support
 * for libatomic in GCC (see also libatomic/configure.tgt).
 */

#if defined(RTEMS_SMP)
static volatile uint32_t _SPARCV8_The_one_lock;

static inline uint32_t _SPARCV8_Atomic_swap(
  volatile uint32_t *address,
  uint32_t value
)
{
  asm volatile (
    "swap [%2], %0"
    : "=r" (value)
    : "0" (value), "r" (address)
    : "memory"
  );

  return value;
}
#endif

static ISR_Level _SPARCV8_Acquire_the_one_lock( void )
{
  ISR_Level level;

  _ISR_Disable_without_giant( level );

#if defined(RTEMS_SMP)
  do {
    while ( _SPARCV8_The_one_lock ) {
      /* Wait */
    }
  } while ( _SPARCV8_Atomic_swap( &_SPARCV8_The_one_lock, 1 ) );
#endif

  return level;
}

static void _SPARCV8_Release_the_one_lock( ISR_Level level )
{
#if defined(RTEMS_SMP)
  _SPARCV8_The_one_lock = 0;
#endif
  _ISR_Enable_without_giant( level );
}

uint8_t __atomic_exchange_1( uint8_t *mem, uint8_t val, int model )
{
  uint8_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

/*
 * In order to get rid of the warning "conflicting types for built-in function
 * '__atomic_compare_exchange_4' [enabled by default]", you must port libatomic
 * to SPARC/RTEMS.  The libatomic is provided by GCC.
 */
uint32_t __atomic_exchange_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

/*
 * You get probably a warning here which can be ignored: "warning: conflicting
 * types for built-in function '__atomic_compare_exchange_4' [enabled by
 * default]"
 */
bool __atomic_compare_exchange_4(
  uint32_t *mem,
  uint32_t *expected,
  uint32_t desired,
  int success,
  int failure
)
{
  bool equal;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  equal = *mem == *expected;
  if ( equal ) {
    *mem = desired;
  }

  _SPARCV8_Release_the_one_lock( level );

  return equal;
}

uint32_t __atomic_fetch_add_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = prev + val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

uint32_t __atomic_fetch_sub_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = prev - val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

uint32_t __atomic_fetch_and_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = prev & val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

uint32_t __atomic_fetch_or_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = prev | val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}

uint32_t __atomic_fetch_xor_4( uint32_t *mem, uint32_t val, int model )
{
  uint32_t prev;
  ISR_Level level;

  level = _SPARCV8_Acquire_the_one_lock();

  prev = *mem;
  *mem = prev ^ val;

  _SPARCV8_Release_the_one_lock( level );

  return prev;
}
