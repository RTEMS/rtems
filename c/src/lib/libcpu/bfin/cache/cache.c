/*  Blackfin Cache Support
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>
#include <bsp.h>
#include <libcpu/memoryRegs.h>
#include "cache_.h"


/* There are many syncs in the following code because they should be
   harmless except for wasting time, and this is easier than figuring out
   exactly where they're needed to protect from the effects of write
   buffers and queued reads.  Many of them are likely unnecessary. */


void _CPU_cache_flush_1_data_line(const void *d_addr) {

  __asm__ __volatile__ ("ssync; flush [%0]; ssync" :: "a" (d_addr));
}

/* Blackfins can't just invalidate cache; they can only do flush +
   invalidate.  If the line isn't dirty then this is equivalent to
   just an invalidate.  Even if it is dirty, this should still be
   okay since with a pure invalidate method the caller would have no
   way to insure the dirty line hadn't been written out anyway prior
   to the invalidate. */
void _CPU_cache_invalidate_1_data_line(const void *d_addr) {

  __asm__ __volatile__ ("ssync; flushinv [%0]; ssync" :: "a" (d_addr));
}

void _CPU_cache_freeze_data(void) {
}

void _CPU_cache_unfreeze_data(void) {
}

void _CPU_cache_invalidate_1_instruction_line(const void *d_addr) {

  __asm__ __volatile__ ("ssync; iflush [%0]; ssync" :: "a" (d_addr));
}

void _CPU_cache_freeze_instruction(void) {
}

void _CPU_cache_unfreeze_instruction(void) {
}

/* incredibly inefficient...  It would be better to make use of the
   DTEST_COMMAND/DTEST_DATAx registers to find the addresses in each
   cache line and flush just those.  However the documentation I've
   seen on those is a bit sketchy, and I sure wouldn't want to get it
   wrong. */
void _CPU_cache_flush_entire_data(void) {
  uint32_t i;

  i = 0;
  __asm__ __volatile__ ("ssync");
  do {
      __asm__ __volatile__ ("flush [%0]" :: "a" (i));
      i += CPU_DATA_CACHE_ALIGNMENT;
  } while (i);
  __asm__ __volatile__ ("ssync");
}

void _CPU_cache_invalidate_entire_data(void) {
  uint32_t dmemControl;

  __asm__ __volatile__ ("ssync");
  dmemControl = *(uint32_t volatile *) DMEM_CONTROL;
  *(uint32_t volatile *) DMEM_CONTROL = dmemControl & ~DMEM_CONTROL_DMC_MASK;
  *(uint32_t volatile *) DMEM_CONTROL = dmemControl;
  __asm__ __volatile__ ("ssync");
}

/* this does not actually enable data cache unless CPLBs are also enabled.
   LIBCPU_DATA_CACHE_CONFIG contains the DMEM_CONTROL_DMC bits to set. */
void _CPU_cache_enable_data(void) {

  __asm__ __volatile__ ("ssync");
  *(uint32_t volatile *) DMEM_CONTROL |= LIBCPU_DATA_CACHE_CONFIG;
  __asm__ __volatile__ ("ssync");
}

void _CPU_cache_disable_data(void) {

  __asm__ __volatile__ ("ssync");
  *(uint32_t volatile *) DMEM_CONTROL &= ~DMEM_CONTROL_DMC_MASK;
  __asm__ __volatile__ ("ssync");
}

void _CPU_cache_invalidate_entire_instruction(void) {
  uint32_t imemControl;

  __asm__ __volatile__ ("ssync");
  imemControl = *(uint32_t volatile *) IMEM_CONTROL;
  *(uint32_t volatile *) IMEM_CONTROL = imemControl & ~IMEM_CONTROL_IMC;
  *(uint32_t volatile *) IMEM_CONTROL = imemControl;
  __asm__ __volatile__ ("ssync");
}

/* this only actually enables the instruction cache if the CPLBs are also
   enabled. */
void _CPU_cache_enable_instruction(void) {

  __asm__ __volatile__ ("ssync");
  *(uint32_t volatile *) IMEM_CONTROL |= IMEM_CONTROL_IMC;
  __asm__ __volatile__ ("ssync");
}

void _CPU_cache_disable_instruction(void) {

  __asm__ __volatile__ ("ssync");
  *(uint32_t volatile *) IMEM_CONTROL &= ~IMEM_CONTROL_IMC;
  __asm__ __volatile__ ("ssync");
}

