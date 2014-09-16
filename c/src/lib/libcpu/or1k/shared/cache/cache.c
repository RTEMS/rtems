/*
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
#include <libcpu/cache.h>

static inline void _CPU_OR1K_Cache_enable_data(void)
{
  uint32_t sr;
   ISR_Level level;

  _ISR_Disable (level);
  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr | CPU_OR1K_SPR_SR_DCE);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_disable_data(void)
{
  uint32_t sr;
   ISR_Level level;

  _ISR_Disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_DCE));

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_enable_instruction(void)
{
  uint32_t sr;
   ISR_Level level;

  _ISR_Disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, sr | CPU_OR1K_SPR_SR_ICE);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_disable_instruction(void)
{
  uint32_t sr;
  ISR_Level level;

  _ISR_Disable (level);

  sr = _OR1K_mfspr(CPU_OR1K_SPR_SR);
  _OR1K_mtspr(CPU_OR1K_SPR_SR, (sr & ~CPU_OR1K_SPR_SR_ICE));

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_prefetch(const void *d_addr)
{
  ISR_Level level;

  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBPR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_flush(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBFR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_invalidate(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBIR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_writeback(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBWR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_data_block_lock(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_DCBLR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_instruction_block_prefetch
(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_ICBPR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_instruction_block_invalidate
(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_ICBIR, d_addr);

  _ISR_Enable(level);
}

static inline void _CPU_OR1K_Cache_instruction_block_lock
(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _OR1K_mtspr(CPU_OR1K_SPR_ICBLR, d_addr);

  _ISR_Enable(level);
}

/* Implement RTEMS cache manager functions */

void _CPU_cache_flush_1_data_line(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _CPU_OR1K_Cache_data_block_flush(d_addr);

  //asm volatile("l.csync");

  _ISR_Enable(level);
}

void _CPU_cache_invalidate_1_data_line(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _CPU_OR1K_Cache_data_block_invalidate(d_addr);

  _ISR_Enable(level);
}

void _CPU_cache_freeze_data(void)
{
  /* Do nothing */
}

void _CPU_cache_unfreeze_data(void)
{
  /* Do nothing */
}

void _CPU_cache_invalidate_1_instruction_line(const void *d_addr)
{
   ISR_Level level;
  _ISR_Disable (level);

  _CPU_OR1K_Cache_instruction_block_invalidate(d_addr);

  _ISR_Enable(level);
}

void _CPU_cache_freeze_instruction(void)
{
  /* Do nothing */
}

void _CPU_cache_unfreeze_instruction(void)
{
  /* Do nothing */
}

void _CPU_cache_flush_entire_data(void)
{

}

void _CPU_cache_invalidate_entire_data(void)
{

}

void _CPU_cache_invalidate_entire_instruction(void)
{

}

void _CPU_cache_enable_data(void)
{
  _CPU_OR1K_Cache_enable_data();
}

void _CPU_cache_disable_data(void)
{
  _CPU_OR1K_Cache_disable_data();

}

void _CPU_cache_enable_instruction(void)
{

  _CPU_OR1K_Cache_enable_instruction();
}

void _CPU_cache_disable_instruction(void)
{
  _CPU_OR1K_Cache_disable_instruction();
}
