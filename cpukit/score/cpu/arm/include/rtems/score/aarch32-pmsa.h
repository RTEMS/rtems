/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARMPMSAv8
 *
 * @brief This header file provides the API to manage an Arm PMSAv8-32 based
 *   Memory Protection Unit (MPU).
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

#ifndef _RTEMS_SCORE_AARCH32_PMSA_H
#define _RTEMS_SCORE_AARCH32_PMSA_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreCPUARMPMSAv8 PMSAv8-32 Support
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This group provides support functions to manage an Arm PMSAv8-32
 *   (Protected Memory System Architecture) based Memory Protection Unit (MPU).
 *
 * @{
 */

#define AARCH32_PMSA_MIN_REGION_ALIGN 64

#define AARCH32_PMSA_ATTR_EN 0x1U

#define AARCH32_PMSA_ATTR_IDX_SHIFT 1
#define AARCH32_PMSA_ATTR_IDX_MASK 0xeU
#define AARCH32_PMSA_ATTR_IDX( _idx ) \
  ( ( _idx ) << AARCH32_PMSA_ATTR_IDX_SHIFT )

#define AARCH32_PMSA_ATTR_XN 0x6U

#define AARCH32_PMSA_ATTR_AP_SHIFT 7
#define AARCH32_PMSA_ATTR_AP_MASK 0x18U
#define AARCH32_PMSA_ATTR_AP( _ap ) \
  ( ( _ap ) << AARCH32_PMSA_ATTR_AP_SHIFT )
#define AARCH32_PMSA_ATTR_AP_EL1_RW_EL0_NO 0x0U
#define AARCH32_PMSA_ATTR_AP_EL1_RW_EL0_RW 0x1U
#define AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_NO 0x2U
#define AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_RO 0x3U

#define AARCH32_PMSA_ATTR_SH_SHIFT 9
#define AARCH32_PMSA_ATTR_SH_MASK 0x600U
#define AARCH32_PMSA_ATTR_SH( _sh ) \
  ( ( _sh ) << AARCH32_PMSA_ATTR_SH_SHIFT )
#define AARCH32_PMSA_ATTR_SH_NO 0x0U
#define AARCH32_PMSA_ATTR_SH_RES 0x1U
#define AARCH32_PMSA_ATTR_SH_OUTER 0x2U
#define AARCH32_PMSA_ATTR_SH_INNER 0x3U

#define AARCH32_PMSA_MEM_DEVICE_NG_NR_NE 0x00U
#define AARCH32_PMSA_MEM_DEVICE_NG_NR_E  0x04U
#define AARCH32_PMSA_MEM_DEVICE_NG_R_E   0x08U
#define AARCH32_PMSA_MEM_DEVICE_G_R_E    0x0cU

#define AARCH32_PMSA_MEM_OUTER_WTT  0x00U
#define AARCH32_PMSA_MEM_OUTER_NC   0x40U
#define AARCH32_PMSA_MEM_OUTER_WBT  0x40U
#define AARCH32_PMSA_MEM_OUTER_WTNT 0x80U
#define AARCH32_PMSA_MEM_OUTER_WBNT 0xc0U

#define AARCH32_PMSA_MEM_OUTER_RA 0x20U
#define AARCH32_PMSA_MEM_OUTER_WA 0x10U

#define AARCH32_PMSA_MEM_INNER_WTT  0x00U
#define AARCH32_PMSA_MEM_INNER_NC   0x40U
#define AARCH32_PMSA_MEM_INNER_WBT  0x40U
#define AARCH32_PMSA_MEM_INNER_WTNT 0x80U
#define AARCH32_PMSA_MEM_INNER_WBNT 0xc0U

#define AARCH32_PMSA_MEM_INNER_RA 0x02U
#define AARCH32_PMSA_MEM_INNER_WA 0x01U

#define AARCH32_PMSA_MEM_ATTR( _ma0, _ma1, _ma2, _ma3 ) \
  ( ( _ma0 ) | ( ( _ma1 ) << 8 ) | ( ( _ma1 ) << 16 ) | ( ( _ma1 ) << 24 ) )

#define AARCH32_PMSA_MEM_ATTR_DEFAULT_CACHED \
  ( AARCH32_PMSA_MEM_OUTER_WBNT | \
    AARCH32_PMSA_MEM_OUTER_RA | \
    AARCH32_PMSA_MEM_OUTER_WA | \
    AARCH32_PMSA_MEM_INNER_WBNT | \
    AARCH32_PMSA_MEM_INNER_RA | \
    AARCH32_PMSA_MEM_INNER_WA )

#define AARCH32_PMSA_MEM_ATTR_DEFAULT_UNCACHED \
  ( AARCH32_PMSA_MEM_OUTER_NC | \
    AARCH32_PMSA_MEM_INNER_NC )

#define AARCH32_PMSA_MEM_ATTR_DEFAULT_DEVICE \
  AARCH32_PMSA_MEM_DEVICE_NG_NR_NE

#define AARCH32_PMSA_CODE_CACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_NO ) | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_NO ) | \
    AARCH32_PMSA_ATTR_IDX( 0U ) )

#define AARCH32_PMSA_CODE_UNCACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_NO ) | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_NO ) | \
    AARCH32_PMSA_ATTR_IDX( 1U ) )

#define AARCH32_PMSA_DATA_READ_ONLY_CACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_XN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_NO ) |  \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_OUTER ) | \
    AARCH32_PMSA_ATTR_IDX( 0U ) )

#define AARCH32_PMSA_DATA_READ_ONLY_UNCACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_XN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RO_EL0_NO ) | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_NO ) | \
    AARCH32_PMSA_ATTR_IDX( 1U ) )

#define AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_XN | \
    AARCH32_PMSA_ATTR_AP_EL1_RW_EL0_NO | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_OUTER ) | \
    AARCH32_PMSA_ATTR_IDX( 0U ) )

#define AARCH32_PMSA_DATA_READ_WRITE_UNCACHED \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_XN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RW_EL0_NO ) | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_NO ) | \
    AARCH32_PMSA_ATTR_IDX( 1U ) )

#define AARCH32_PMSA_DEVICE \
  ( AARCH32_PMSA_ATTR_EN | \
    AARCH32_PMSA_ATTR_XN | \
    AARCH32_PMSA_ATTR_AP( AARCH32_PMSA_ATTR_AP_EL1_RW_EL0_NO ) | \
    AARCH32_PMSA_ATTR_SH( AARCH32_PMSA_ATTR_SH_NO ) | \
    AARCH32_PMSA_ATTR_IDX( 2U ) )

/**
 * @brief The default section definitions shall be used by the BSP to define
 *   ::_AArch32_PMSA_Sections.
 *
 * In addition to the default section definitions, the BSP should provide
 * section definitions for the memory-mapped devices and other memory areas.
 */
#define AARCH32_PMSA_DEFAULT_SECTIONS \
  { \
    .begin = (uint32_t) bsp_section_fast_text_begin, \
    .end = (uint32_t) bsp_section_fast_text_end, \
    .attributes = AARCH32_PMSA_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_fast_data_begin, \
    .end = (uint32_t) bsp_section_fast_data_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_start_begin, \
    .end = (uint32_t) bsp_section_start_end, \
    .attributes = AARCH32_PMSA_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_vector_begin, \
    .end = (uint32_t) bsp_section_vector_end, \
    .attributes = AARCH32_PMSA_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_text_begin, \
    .end = (uint32_t) bsp_section_text_end, \
    .attributes = AARCH32_PMSA_CODE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_rodata_begin, \
    .end = (uint32_t) bsp_section_rodata_end, \
    .attributes = AARCH32_PMSA_DATA_READ_ONLY_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_data_begin, \
    .end = (uint32_t) bsp_section_data_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_bss_begin, \
    .end = (uint32_t) bsp_section_bss_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_rtemsstack_begin, \
    .end = (uint32_t) bsp_section_rtemsstack_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_work_begin, \
    .end = (uint32_t) bsp_section_work_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_stack_begin, \
    .end = (uint32_t) bsp_section_stack_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_CACHED \
  }, { \
    .begin = (uint32_t) bsp_section_nocache_begin, \
    .end = (uint32_t) bsp_section_nocache_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_UNCACHED \
  }, { \
    .begin = (uint32_t) bsp_section_nocachenoload_begin, \
    .end = (uint32_t) bsp_section_nocachenoload_end, \
    .attributes = AARCH32_PMSA_DATA_READ_WRITE_UNCACHED \
  }

/**
 * @brief The section definition is used to initialize the Memory Protection
 *   Unit (MPU).
 *
 * A section is empty if the begin address is equal to the end address.
 */
typedef struct {
  /**
   * @brief This member defines the begin address of the section.
   */
  uint32_t begin;

  /**
   * @brief This member defines the end address of the section.
   */
  uint32_t end;

  /**
   * @brief This member defines the attributes of the section.
   */
  uint32_t attributes;
} AArch32_PMSA_Section;

/**
 * @brief Initializes the Memory Protection Unit (MPU).
 *
 * The section definitions are used to define the regions of the MPU.  Sections
 * are merged if possible to reduce the count of used regions.  If too many
 * regions are used, then the MPU is not enabled.  Overlapping section
 * definitions result in undefined system behaviour.
 *
 * @param memory_attributes_0 are the memory attributes for MAIR0.
 *
 * @param memory_attributes_1 are the memory attributes for MAIR1.
 *
 * @param sections is the array with section definitions.
 *
 * @param section_count is the count of section definitions.
 */
void _AArch32_PMSA_Initialize(
  uint32_t                    memory_attributes_0,
  uint32_t                    memory_attributes_1,
  const AArch32_PMSA_Section *sections,
  size_t                      section_count
);

/**
 * @brief This array provides section definitions to initialize the memory
 *   protection unit (MPU).
 *
 * The BSP shall provide the section definitions with the help of
 * ::AARCH32_PMSA_DEFAULT_SECTIONS.  The section count is provided by
 * ::_AArch32_PMSA_Section_count.
 */
extern const AArch32_PMSA_Section _AArch32_PMSA_Sections[];

/**
 * @brief This constant provides the count of elements in
 * ::_AArch32_PMSA_Sections.
 */
extern const size_t _AArch32_PMSA_Section_count;

/**
 * @brief Initializes the Memory Protection Unit (MPU) using the section
 *   definitions with default memory attributes.
 *
 * Calls _AArch32_PMSA_Initialize() using ::_AArch32_PMSA_Sections and
 * ::_AArch32_PMSA_Section_count and the default memory attributes.
 */
void _AArch32_PMSA_Initialize_default( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_AARCH32_PMSA_H */
