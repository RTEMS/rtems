/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief This header file provides interfaces of the ARM CP15 cache controller
 *   suppport.
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_ARM_SHARED_CACHE_L1_H
#define LIBBSP_ARM_SHARED_CACHE_L1_H

#include <bsp.h>
#include <libcpu/arm-cp15.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* These two defines also ensure that the rtems_cache_* functions have bodies */
#define ARM_CACHE_L1_CPU_DATA_ALIGNMENT 32
#define ARM_CACHE_L1_CPU_INSTRUCTION_ALIGNMENT 32

#define ARM_CACHE_L1_CSS_ID_DATA \
          (ARM_CP15_CACHE_CSS_ID_DATA | ARM_CP15_CACHE_CSS_LEVEL(0))
#define ARM_CACHE_L1_CSS_ID_INSTRUCTION \
          (ARM_CP15_CACHE_CSS_ID_INSTRUCTION | ARM_CP15_CACHE_CSS_LEVEL(0))
#define ARM_CACHE_L1_DATA_LINE_MASK ( ARM_CACHE_L1_CPU_DATA_ALIGNMENT - 1 )
#define ARM_CACHE_L1_INSTRUCTION_LINE_MASK \
  ( ARM_CACHE_L1_CPU_INSTRUCTION_ALIGNMENT \
    - 1 )

/* Errata Handlers */
static void arm_cache_l1_errata_764369_handler( void )
{
#ifdef RTEMS_SMP
  _ARM_Data_synchronization_barrier();
#endif
}

/*
 * @param l1LineSize      Number of bytes in cache line expressed as power of 
 *                        2 value
 * @param l1Associativity Associativity of cache. The associativity does not 
 *                        have to be a power of 2.
 * qparam liNumSets       Number of sets in cache
 * */

static inline void arm_cache_l1_properties_for_level(
  uint32_t *l1LineSize,
  uint32_t *l1Associativity,
  uint32_t *l1NumSets,
  uint32_t level_and_inst_dat
)
{
  uint32_t ccsidr;

  ccsidr = arm_cp15_get_cache_size_id_for_level(level_and_inst_dat);

  /* Cache line size in words + 2 -> bytes) */
  *l1LineSize      = arm_ccsidr_get_line_power(ccsidr);
  /* Number of Ways */
  *l1Associativity = arm_ccsidr_get_associativity(ccsidr);
  /* Number of Sets */
  *l1NumSets       = arm_ccsidr_get_num_sets(ccsidr);
}

/*
 * @param log_2_line_bytes The number of bytes per cache line expressed in log2
 * @param associativity    The associativity of the cache beeing operated
 * @param cache_level_idx  The level of the cache beeing operated minus 1 e.g 0
 *                         for cache level 1
 * @param set              Number of the set to operate on
 * @param way              Number of the way to operate on
 * */

static inline uint32_t arm_cache_l1_get_set_way_param(
  const uint32_t log_2_line_bytes,
  const uint32_t associativity,
  const uint32_t cache_level_idx,
  const uint32_t set,
  const uint32_t way )
{
  uint32_t way_shift = __builtin_clz( associativity - 1 );


  return ( 0
           | ( way
    << way_shift ) | ( set << log_2_line_bytes ) | ( cache_level_idx << 1 ) );
}

static inline void arm_cache_l1_flush_1_data_line( const void *d_addr )
{
  /* Flush the Data cache */
  arm_cp15_data_cache_clean_and_invalidate_line( d_addr );

  /* Wait for L1 flush to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_flush_entire_data( void )
{
  uint32_t l1LineSize, l1Associativity, l1NumSets;
  uint32_t s, w;
  uint32_t set_way_param;

  /* ensure ordering with previous memory accesses */
  _ARM_Data_memory_barrier();

  /* Get the L1 cache properties */
  arm_cache_l1_properties_for_level( &l1LineSize,
                    &l1Associativity, &l1NumSets,
                    ARM_CACHE_L1_CSS_ID_DATA);

  for ( w = 0; w < l1Associativity; ++w ) {
    for ( s = 0; s < l1NumSets; ++s ) {
      set_way_param = arm_cache_l1_get_set_way_param(
        l1LineSize,
        l1Associativity,
        0,
        s,
        w
        );
      arm_cp15_data_cache_clean_line_by_set_and_way( set_way_param );
    }
  }

  /* Wait for L1 flush to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_invalidate_entire_data( void )
{
  uint32_t l1LineSize, l1Associativity, l1NumSets;
  uint32_t s, w;
  uint32_t set_way_param;

  /* ensure ordering with previous memory accesses */
  _ARM_Data_memory_barrier();

  /* Get the L1 cache properties */
  arm_cache_l1_properties_for_level( &l1LineSize,
                    &l1Associativity, &l1NumSets,
                    ARM_CACHE_L1_CSS_ID_DATA);

  for ( w = 0; w < l1Associativity; ++w ) {
    for ( s = 0; s < l1NumSets; ++s ) {
      set_way_param = arm_cache_l1_get_set_way_param(
        l1LineSize,
        l1Associativity,
        0,
        s,
        w
        );
      arm_cp15_data_cache_invalidate_line_by_set_and_way( set_way_param );
    }
  }

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_clean_and_invalidate_entire_data( void )
{
  uint32_t l1LineSize, l1Associativity, l1NumSets;
  uint32_t s, w;
  uint32_t set_way_param;

  /* ensure ordering with previous memory accesses */
  _ARM_Data_memory_barrier();


  /* Get the L1 cache properties */
  arm_cache_l1_properties_for_level( &l1LineSize,
                    &l1Associativity, &l1NumSets,
                    ARM_CACHE_L1_CSS_ID_DATA);

  for ( w = 0; w < l1Associativity; ++w ) {
    for ( s = 0; s < l1NumSets; ++s ) {
      set_way_param = arm_cache_l1_get_set_way_param(
        l1LineSize,
        l1Associativity,
        0,
        s,
        w
        );
      arm_cp15_data_cache_clean_and_invalidate_line_by_set_and_way(
        set_way_param );
    }
  }

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_flush_data_range( 
  const void *d_addr,
  size_t      n_bytes
)
{
  if ( n_bytes != 0 ) {
    uint32_t       adx       = (uint32_t) d_addr
                               & ~ARM_CACHE_L1_DATA_LINE_MASK;
    const uint32_t ADDR_LAST =
      (uint32_t)( (size_t) d_addr + n_bytes - 1 );

    arm_cache_l1_errata_764369_handler();

    for (; adx <= ADDR_LAST; adx += ARM_CACHE_L1_CPU_DATA_ALIGNMENT ) {
      /* Store and invalidate the Data cache line */
      arm_cp15_data_cache_clean_and_invalidate_line( (void*)adx );
    }
    /* Wait for L1 store to complete */
    _ARM_Data_synchronization_barrier();
  }
}


static inline void arm_cache_l1_invalidate_1_data_line(
  const void *d_addr )
{
  /* Invalidate the data cache line */
  arm_cp15_data_cache_invalidate_line( d_addr );

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_freeze_data( void )
{
  /* To be implemented as needed, if supported by hardware at all */
}

static inline void arm_cache_l1_unfreeze_data( void )
{
  /* To be implemented as needed, if supported by hardware at all */
}

static inline void arm_cache_l1_invalidate_1_instruction_line(
  const void *i_addr )
{
  /* Invalidate the Instruction cache line */
  arm_cp15_instruction_cache_invalidate_line( i_addr );

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void arm_cache_l1_invalidate_data_range(
  const void *d_addr,
  size_t      n_bytes
)
{
  if ( n_bytes != 0 ) {
    uint32_t       adx = (uint32_t) d_addr
                         & ~ARM_CACHE_L1_DATA_LINE_MASK;
    const uint32_t end =
      (uint32_t)( (size_t)d_addr + n_bytes -1);

    arm_cache_l1_errata_764369_handler();
    
    /* Back starting address up to start of a line and invalidate until end */
    for (;
         adx <= end;
         adx += ARM_CACHE_L1_CPU_DATA_ALIGNMENT ) {
        /* Invalidate the Instruction cache line */
        arm_cp15_data_cache_invalidate_line( (void*)adx );
    }
    /* Wait for L1 invalidate to complete */
    _ARM_Data_synchronization_barrier();
  }
}

static inline void arm_cache_l1_invalidate_instruction_range(
  const void *i_addr,
  size_t      n_bytes
)
{
  if ( n_bytes != 0 ) {
    uint32_t       adx = (uint32_t) i_addr
                         & ~ARM_CACHE_L1_INSTRUCTION_LINE_MASK;
    const uint32_t end =
      (uint32_t)( (size_t)i_addr + n_bytes -1);

    arm_cache_l1_errata_764369_handler();

    /* Back starting address up to start of a line and invalidate until end */
    for (;
         adx <= end;
         adx += ARM_CACHE_L1_CPU_INSTRUCTION_ALIGNMENT ) {
        /* Invalidate the Instruction cache line */
        arm_cp15_instruction_cache_invalidate_line( (void*)adx );
    }
    /* Wait for L1 invalidate to complete */
    _ARM_Data_synchronization_barrier();
  }
}

static inline void arm_cache_l1_invalidate_entire_instruction( void )
{
  uint32_t ctrl = arm_cp15_get_control();


  #ifdef RTEMS_SMP

  /* invalidate I-cache inner shareable */
  arm_cp15_instruction_cache_inner_shareable_invalidate_all();

  /* I+BTB cache invalidate */
  arm_cp15_instruction_cache_invalidate();
  #else /* RTEMS_SMP */
  /* I+BTB cache invalidate */
  arm_cp15_instruction_cache_invalidate();
  #endif /* RTEMS_SMP */

  if ( ( ctrl & ARM_CP15_CTRL_Z ) != 0 ) {
    #if defined(__ARM_ARCH_7A__)
    arm_cp15_branch_predictor_inner_shareable_invalidate_all();
    #endif
    #if defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_7A__)
    arm_cp15_branch_predictor_invalidate_all();
    #endif
  }
}

static inline void arm_cache_l1_freeze_instruction( void )
{
  /* To be implemented as needed, if supported by hardware at all */
}

static inline void arm_cache_l1_unfreeze_instruction( void )
{
  /* To be implemented as needed, if supported by hardware at all */
}

static inline void arm_cache_l1_disable_data( void )
{
  /* Clean and invalidate the Data cache */
  arm_cache_l1_flush_entire_data();

  /* Disable the Data cache */
  arm_cp15_set_control( arm_cp15_get_control() & ~ARM_CP15_CTRL_C );
}

static inline void arm_cache_l1_disable_instruction( void )
{
  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();

  /* Invalidate the Instruction cache */
  arm_cache_l1_invalidate_entire_instruction();

  /* Disable the Instruction cache */
  arm_cp15_set_control( arm_cp15_get_control() & ~ARM_CP15_CTRL_I );
}

static inline size_t arm_cache_l1_get_data_cache_size( void )
{
  size_t   size;
  uint32_t line_size     = 0;
  uint32_t associativity = 0;
  uint32_t num_sets      = 0;

  arm_cache_l1_properties_for_level( &line_size,
                    &associativity, &num_sets,
                    ARM_CACHE_L1_CSS_ID_DATA);

  size = (1 << line_size) * associativity * num_sets;

  return size;
}

static inline size_t arm_cache_l1_get_instruction_cache_size( void )
{
  size_t   size;
  uint32_t line_size     = 0;
  uint32_t associativity = 0;
  uint32_t num_sets      = 0;

  arm_cache_l1_properties_for_level( &line_size,
                    &associativity, &num_sets,
                    ARM_CACHE_L1_CSS_ID_INSTRUCTION);

  size = (1 << line_size) * associativity * num_sets;

  return size;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_CACHE_L1_H */
