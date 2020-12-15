/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARMPMSAv8
 *
 * @brief This source file contains the implementation of
 *   _AArch32_PMSA_Initialize().
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/aarch32-pmsa.h>

#if __ARM_ARCH >= 8 && __ARM_ARCH_PROFILE == 'R'

#include <rtems/score/aarch32-system-registers.h>
#include <rtems/score/cpu.h>

#define AARCH32_PSMA_REGION_MAX \
  ( ( AARCH32_MPUIR_REGION_MASK >> AARCH32_MPUIR_REGION_SHIFT ) + 1 )

typedef struct {
  uint32_t base;
  uint32_t limit;
  uint32_t attributes;
} AArch32_PMSA_Region;

static void _AArch32_PMSA_Configure(
  const AArch32_PMSA_Region *regions,
  size_t                     region_used,
  size_t                     region_max
)
{
  size_t   ri;
  uint32_t sctlr;

  for ( ri = 0 ; ri < region_used; ++ri ) {
    uint32_t prbar;
    uint32_t prlar;
    uint32_t attr;

    prbar = regions[ ri ].base;
    prlar = regions[ ri ].limit;
    attr = regions[ ri ].attributes;

    prbar |= ( attr >> 6 ) & 0x3fU;
    prlar |= attr & 0x3fU;

    _AArch32_Write_prselr( ri );
    _ARM_Instruction_synchronization_barrier();
    _AArch32_Write_prbar( prbar );
    _AArch32_Write_prlar( prlar );
  }

  for ( ri = region_used ; ri < region_max; ++ri ) {
    _AArch32_Write_prselr( ri );
    _ARM_Instruction_synchronization_barrier();
    _AArch32_Write_prbar( 0 );
    _AArch32_Write_prlar( 0 );
  }

  _ARM_Data_synchronization_barrier();
  sctlr = _AArch32_Read_sctlr();
  sctlr |= AARCH32_SCTLR_M | AARCH32_SCTLR_I | AARCH32_SCTLR_C;
  sctlr &= ~AARCH32_SCTLR_BR;
  _AArch32_Write_sctlr( sctlr );
  _ARM_Instruction_synchronization_barrier();
}

void _AArch32_PMSA_Initialize(
  uint32_t                    memory_attributes_0,
  uint32_t                    memory_attributes_1,
  const AArch32_PMSA_Section *sections,
  size_t                      section_count
)
{
  AArch32_PMSA_Region regions[ AARCH32_PSMA_REGION_MAX ];
  size_t ri;
  size_t si;
  size_t region_used;
  size_t region_max;

  _AArch32_Write_mair0( memory_attributes_0 );
  _AArch32_Write_mair1( memory_attributes_1 );

  region_max = ( _AArch32_Read_mpuir() & AARCH32_MPUIR_REGION_MASK ) >>
    AARCH32_MPUIR_REGION_SHIFT;
  region_used = 0;

  for ( si = 0; si < section_count; ++si ) {
    uint32_t base;
    uint32_t limit;
    uint32_t attr;

    base = sections[ si ].begin;
    limit = sections[ si ].end;
    attr = sections[ si ].attributes;

    if ( base == limit ) {
      continue;
    }

    base = RTEMS_ALIGN_DOWN( base, AARCH32_PMSA_MIN_REGION_ALIGN );
    limit = RTEMS_ALIGN_DOWN( limit - 1, AARCH32_PMSA_MIN_REGION_ALIGN );

    for ( ri = 0; ri < region_used; ++ri ) {
      uint32_t region_base;
      uint32_t region_limit;
      uint32_t region_attr;

      region_base = regions[ ri ].base;
      region_limit = regions[ ri ].limit;
      region_attr = regions[ ri ].attributes;

      if (
        limit + AARCH32_PMSA_MIN_REGION_ALIGN == region_base &&
          attr == region_attr
      ) {
        /* Merge section with existing region */
        regions[ ri ].base = base;
        break;
      } else if (
        base == region_limit + AARCH32_PMSA_MIN_REGION_ALIGN &&
          attr == region_attr
      ) {
        /* Merge section with existing region */
        regions[ ri ].limit = limit;
        break;
      } else if ( limit < region_base ) {
        size_t i;

        if ( region_used >= region_max ) {
          return;
        }

        for ( i = ri; i < region_used; ++i ) {
          regions[ i + 1 ] = regions[ i ];
        }

        /* New first region */
        ++region_used;
        regions[ ri ].base = base;
        regions[ ri ].limit = limit;
        regions[ ri ].attributes = attr;
        break;
      }
    }

    if ( ri == region_used ) {
      if ( region_used >= region_max ) {
        return;
      }

      /* New last region */
      regions[ ri ].base = base;
      regions[ ri ].limit = limit;
      regions[ ri ].attributes = attr;
      ++region_used;
    }
  }

  _AArch32_PMSA_Configure( regions, region_used, region_max );
}

#endif
