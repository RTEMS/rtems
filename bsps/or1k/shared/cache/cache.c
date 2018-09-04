/*
 * COPYRIGHT (c) 2014, 2016 ÅAC Microtec AB <www.aacmicrotec.com>
 * Contributor(s):
 *  Karol Gugala <kgugala@antmicro.com>
 *  Martin Werner <martin.werner@aacmicrotec.com>
 *
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * COPYRIGHT (c) 1989-2006
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>
#include <rtems/score/or1k-utility.h>
#include <rtems/score/percpu.h>

#define CPU_DATA_CACHE_ALIGNMENT        32
#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS 1
#define CPU_CACHE_SUPPORT_PROVIDES_CACHE_SIZE_FUNCTIONS 1

static inline size_t
_CPU_cache_get_data_cache_size( const uint32_t level )
{
  return (level == 0 || level == 1)? 8192 : 0;
}

static inline size_t
_CPU_cache_get_instruction_cache_size( const uint32_t level )
{
  return (level == 0 || level == 1)? 8192 : 0;
}

static inline void _CPU_OR1K_Cache_data_block_prefetch(const void *d_addr)
{
  ISR_Level level;

  _ISR_Local_disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBPR, (uintptr_t) d_addr);

  _ISR_Local_enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_writeback(const void *d_addr)
{
  ISR_Level level;

  _ISR_Local_disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBWR, (uintptr_t) d_addr);

  _ISR_Local_enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_lock(const void *d_addr)
{
  ISR_Level level;

  _ISR_Local_disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBLR, (uintptr_t) d_addr);

  _ISR_Local_enable(level);
}

static inline void _CPU_OR1K_Cache_instruction_block_prefetch
(const void *d_addr)
{
  ISR_Level level;

  _ISR_Local_disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_ICBPR, (uintptr_t) d_addr);

  _ISR_Local_enable(level);
}

static inline void _CPU_OR1K_Cache_instruction_block_lock
(const void *d_addr)
{
  ISR_Level level;

  _ISR_Local_disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_ICBLR, (uintptr_t) d_addr);

  _ISR_Local_enable(level);
}

/* Implement RTEMS cache manager functions */

static void _CPU_cache_freeze_data(void)
{
  /* Do nothing */
}

static void _CPU_cache_unfreeze_data(void)
{
  /* Do nothing */
}

static void _CPU_cache_freeze_instruction(void)
{
  /* Do nothing */
}

static void _CPU_cache_unfreeze_instruction(void)
{
  /* Do nothing */
}

static void _CPU_cache_flush_entire_data(void)
{
  size_t addr;
  ISR_Level level;

  _ISR_Local_disable (level);

  /* We have only 0 level cache so we do not need to invalidate others */
  for (
      addr = _CPU_cache_get_data_cache_size(0);
      addr > 0;
      addr -= CPU_DATA_CACHE_ALIGNMENT
  ) {
    _OR1K_mtspr(CPU_OR1K_SPR_DCBFR, (uintptr_t) addr);
  }

  _ISR_Local_enable (level);
}

static void _CPU_cache_invalidate_entire_data(void)
{
  size_t addr;
  ISR_Level level;

  _ISR_Local_disable (level);

  /* We have only 0 level cache so we do not need to invalidate others */
  for (
      addr = _CPU_cache_get_data_cache_size(0);
      addr > 0;
      addr -= CPU_DATA_CACHE_ALIGNMENT
  ) {
    _OR1K_mtspr(CPU_OR1K_SPR_DCBIR, (uintptr_t) addr);
  }

  _ISR_Local_enable (level);
}

static void _CPU_cache_invalidate_entire_instruction(void)
{
  size_t addr;
  ISR_Level level;

  _ISR_Local_disable (level);

  /* We have only 0 level cache so we do not need to invalidate others */
  for (
      addr = _CPU_cache_get_instruction_cache_size(0);
      addr > 0;
      addr -= CPU_INSTRUCTION_CACHE_ALIGNMENT
  ) {
    _OR1K_mtspr(CPU_OR1K_SPR_ICBIR, (uintptr_t) addr);
  }

  /* Flush instructions out of instruction buffer */
  __asm__ volatile("l.nop");
  __asm__ volatile("l.nop");
  __asm__ volatile("l.nop");
  __asm__ volatile("l.nop");
  __asm__ volatile("l.nop");

  _ISR_Local_enable (level);
}

/*
 * The range functions are copied almost verbatim from the generic
 * implementations in c/src/lib/libcpu/shared/src/cache_manager.c. The main
 * modification here is avoiding reapeated off/on toggling of the ISR for each
 * cache line operation.
 */

static void _CPU_cache_flush_data_range(const void *d_addr, size_t n_bytes)
{
  const void * final_address;
  ISR_Level level;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment d_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));

  if( final_address - d_addr > _CPU_cache_get_data_cache_size(0) ) {
    /*
     * Avoid iterating over the whole cache multiple times if the range is
     * larger than the cache size.
     */
    _CPU_cache_flush_entire_data();
    return;
  }

  _ISR_Local_disable (level);

  while( d_addr <= final_address )  {
    _OR1K_mtspr(CPU_OR1K_SPR_DCBFR, (uintptr_t) d_addr);
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }

  _ISR_Local_enable (level);
}

static void _CPU_cache_invalidate_data_range(const void *d_addr, size_t n_bytes)
{
  const void * final_address;
  ISR_Level level;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment d_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));

  if( final_address - d_addr > _CPU_cache_get_data_cache_size(0) ) {
    /*
     * Avoid iterating over the whole cache multiple times if the range is
     * larger than the cache size.
     */
    _CPU_cache_invalidate_entire_data();
    return;
  }

  _ISR_Local_disable (level);

  while( d_addr <= final_address )  {
    _OR1K_mtspr(CPU_OR1K_SPR_DCBIR, (uintptr_t) d_addr);
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }

  _ISR_Local_enable (level);
}

static void _CPU_cache_invalidate_instruction_range(const void *i_addr, size_t n_bytes)
{
  const void * final_address;
  ISR_Level level;

 /*
  * Set i_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment i_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  final_address = (void *)((size_t)i_addr + n_bytes - 1);
  i_addr = (void *)((size_t)i_addr & ~(CPU_INSTRUCTION_CACHE_ALIGNMENT - 1));

  if( final_address - i_addr > _CPU_cache_get_data_cache_size(0) ) {
    /*
     * Avoid iterating over the whole cache multiple times if the range is
     * larger than the cache size.
     */
    _CPU_cache_invalidate_entire_instruction();
    return;
  }

  _ISR_Local_disable (level);

  while( i_addr <= final_address )  {
    _OR1K_mtspr(CPU_OR1K_SPR_ICBIR, (uintptr_t) i_addr);
    i_addr = (void *)((size_t)i_addr + CPU_DATA_CACHE_ALIGNMENT);
  }

  _ISR_Local_enable (level);
}

static void _CPU_cache_enable_data(void)
{
  uint32_t sr;
  ISR_Level level;

  _ISR_Local_disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr | CPU_OR1K_SPR_SR_DCE);

  _ISR_Local_enable(level);
}

static void _CPU_cache_disable_data(void)
{
  uint32_t sr;
  ISR_Level level;

  _ISR_Local_disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_DCE));

  _ISR_Local_enable(level);
}

static void _CPU_cache_enable_instruction(void)
{
  uint32_t sr;
  ISR_Level level;

  _ISR_Local_disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr | CPU_OR1K_SPR_SR_ICE);

  _ISR_Local_enable(level);
}

static void _CPU_cache_disable_instruction(void)
{
  uint32_t sr;
  ISR_Level level;

  _ISR_Local_disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_ICE));

  _ISR_Local_enable(level);
}

#include "../../../shared/cache/cacheimpl.h"
