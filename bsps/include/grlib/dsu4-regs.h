/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBDSU4
 *
 * @brief This header file defines the DSU4 register block interface.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/dev/grlib/if/dsu4-header */

#ifndef _GRLIB_DSU4_REGS_H
#define _GRLIB_DSU4_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/dsu4 */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4 DSU4
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the DSU4 interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4CTRL DSU control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_CTRL_PW 0x800U

#define DSU4_CTRL_HL 0x400U

#define DSU4_CTRL_PE 0x200U

#define DSU4_CTRL_EB 0x100U

#define DSU4_CTRL_EE 0x80U

#define DSU4_CTRL_DM 0x40U

#define DSU4_CTRL_BZ 0x20U

#define DSU4_CTRL_BX 0x10U

#define DSU4_CTRL_BS 0x8U

#define DSU4_CTRL_BW 0x4U

#define DSU4_CTRL_BE 0x2U

#define DSU4_CTRL_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4DTTC DSU time tag counter register (DTTC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_DTTC_TIMETAG_SHIFT 0
#define DSU4_DTTC_TIMETAG_MASK 0xffffffffU
#define DSU4_DTTC_TIMETAG_GET( _reg ) \
  ( ( ( _reg ) & DSU4_DTTC_TIMETAG_MASK ) >> \
    DSU4_DTTC_TIMETAG_SHIFT )
#define DSU4_DTTC_TIMETAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_DTTC_TIMETAG_MASK ) | \
    ( ( ( _val ) << DSU4_DTTC_TIMETAG_SHIFT ) & \
      DSU4_DTTC_TIMETAG_MASK ) )
#define DSU4_DTTC_TIMETAG( _val ) \
  ( ( ( _val ) << DSU4_DTTC_TIMETAG_SHIFT ) & \
    DSU4_DTTC_TIMETAG_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4BRSS DSU break and single step register (BRSS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_BRSS_SS_3_0_SHIFT 16
#define DSU4_BRSS_SS_3_0_MASK 0xf0000U
#define DSU4_BRSS_SS_3_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_BRSS_SS_3_0_MASK ) >> \
    DSU4_BRSS_SS_3_0_SHIFT )
#define DSU4_BRSS_SS_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_BRSS_SS_3_0_MASK ) | \
    ( ( ( _val ) << DSU4_BRSS_SS_3_0_SHIFT ) & \
      DSU4_BRSS_SS_3_0_MASK ) )
#define DSU4_BRSS_SS_3_0( _val ) \
  ( ( ( _val ) << DSU4_BRSS_SS_3_0_SHIFT ) & \
    DSU4_BRSS_SS_3_0_MASK )

#define DSU4_BRSS_BN_3_0_SHIFT 0
#define DSU4_BRSS_BN_3_0_MASK 0xfU
#define DSU4_BRSS_BN_3_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_BRSS_BN_3_0_MASK ) >> \
    DSU4_BRSS_BN_3_0_SHIFT )
#define DSU4_BRSS_BN_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_BRSS_BN_3_0_MASK ) | \
    ( ( ( _val ) << DSU4_BRSS_BN_3_0_SHIFT ) & \
      DSU4_BRSS_BN_3_0_MASK ) )
#define DSU4_BRSS_BN_3_0( _val ) \
  ( ( ( _val ) << DSU4_BRSS_BN_3_0_SHIFT ) & \
    DSU4_BRSS_BN_3_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4DBGM DSU debug mode mask register (DBGM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_DBGM_DM_3_0_SHIFT 16
#define DSU4_DBGM_DM_3_0_MASK 0xf0000U
#define DSU4_DBGM_DM_3_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_DBGM_DM_3_0_MASK ) >> \
    DSU4_DBGM_DM_3_0_SHIFT )
#define DSU4_DBGM_DM_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_DBGM_DM_3_0_MASK ) | \
    ( ( ( _val ) << DSU4_DBGM_DM_3_0_SHIFT ) & \
      DSU4_DBGM_DM_3_0_MASK ) )
#define DSU4_DBGM_DM_3_0( _val ) \
  ( ( ( _val ) << DSU4_DBGM_DM_3_0_SHIFT ) & \
    DSU4_DBGM_DM_3_0_MASK )

#define DSU4_DBGM_ED_3_0_SHIFT 0
#define DSU4_DBGM_ED_3_0_MASK 0xfU
#define DSU4_DBGM_ED_3_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_DBGM_ED_3_0_MASK ) >> \
    DSU4_DBGM_ED_3_0_SHIFT )
#define DSU4_DBGM_ED_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_DBGM_ED_3_0_MASK ) | \
    ( ( ( _val ) << DSU4_DBGM_ED_3_0_SHIFT ) & \
      DSU4_DBGM_ED_3_0_MASK ) )
#define DSU4_DBGM_ED_3_0( _val ) \
  ( ( ( _val ) << DSU4_DBGM_ED_3_0_SHIFT ) & \
    DSU4_DBGM_ED_3_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4DTR DSU trap register (DTR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_DTR_EM 0x1000U

#define DSU4_DTR_TRAPTYPE_SHIFT 4
#define DSU4_DTR_TRAPTYPE_MASK 0xff0U
#define DSU4_DTR_TRAPTYPE_GET( _reg ) \
  ( ( ( _reg ) & DSU4_DTR_TRAPTYPE_MASK ) >> \
    DSU4_DTR_TRAPTYPE_SHIFT )
#define DSU4_DTR_TRAPTYPE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_DTR_TRAPTYPE_MASK ) | \
    ( ( ( _val ) << DSU4_DTR_TRAPTYPE_SHIFT ) & \
      DSU4_DTR_TRAPTYPE_MASK ) )
#define DSU4_DTR_TRAPTYPE( _val ) \
  ( ( ( _val ) << DSU4_DTR_TRAPTYPE_SHIFT ) & \
    DSU4_DTR_TRAPTYPE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4DASI DSU ASI diagnostic access register (DASI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_DASI_ASI_SHIFT 0
#define DSU4_DASI_ASI_MASK 0xffU
#define DSU4_DASI_ASI_GET( _reg ) \
  ( ( ( _reg ) & DSU4_DASI_ASI_MASK ) >> \
    DSU4_DASI_ASI_SHIFT )
#define DSU4_DASI_ASI_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_DASI_ASI_MASK ) | \
    ( ( ( _val ) << DSU4_DASI_ASI_SHIFT ) & \
      DSU4_DASI_ASI_MASK ) )
#define DSU4_DASI_ASI( _val ) \
  ( ( ( _val ) << DSU4_DASI_ASI_SHIFT ) & \
    DSU4_DASI_ASI_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBC AHB trace buffer control register (ATBC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBC_DCNT_SHIFT 16
#define DSU4_ATBC_DCNT_MASK 0xff0000U
#define DSU4_ATBC_DCNT_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBC_DCNT_MASK ) >> \
    DSU4_ATBC_DCNT_SHIFT )
#define DSU4_ATBC_DCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBC_DCNT_MASK ) | \
    ( ( ( _val ) << DSU4_ATBC_DCNT_SHIFT ) & \
      DSU4_ATBC_DCNT_MASK ) )
#define DSU4_ATBC_DCNT( _val ) \
  ( ( ( _val ) << DSU4_ATBC_DCNT_SHIFT ) & \
    DSU4_ATBC_DCNT_MASK )

#define DSU4_ATBC_DF 0x100U

#define DSU4_ATBC_SF 0x80U

#define DSU4_ATBC_TE 0x40U

#define DSU4_ATBC_TF 0x20U

#define DSU4_ATBC_BW_SHIFT 3
#define DSU4_ATBC_BW_MASK 0x18U
#define DSU4_ATBC_BW_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBC_BW_MASK ) >> \
    DSU4_ATBC_BW_SHIFT )
#define DSU4_ATBC_BW_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBC_BW_MASK ) | \
    ( ( ( _val ) << DSU4_ATBC_BW_SHIFT ) & \
      DSU4_ATBC_BW_MASK ) )
#define DSU4_ATBC_BW( _val ) \
  ( ( ( _val ) << DSU4_ATBC_BW_SHIFT ) & \
    DSU4_ATBC_BW_MASK )

#define DSU4_ATBC_BR 0x4U

#define DSU4_ATBC_DM 0x2U

#define DSU4_ATBC_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBI AHB trace buffer index register (ATBI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBI_INDEX_SHIFT 4
#define DSU4_ATBI_INDEX_MASK 0xff0U
#define DSU4_ATBI_INDEX_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBI_INDEX_MASK ) >> \
    DSU4_ATBI_INDEX_SHIFT )
#define DSU4_ATBI_INDEX_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBI_INDEX_MASK ) | \
    ( ( ( _val ) << DSU4_ATBI_INDEX_SHIFT ) & \
      DSU4_ATBI_INDEX_MASK ) )
#define DSU4_ATBI_INDEX( _val ) \
  ( ( ( _val ) << DSU4_ATBI_INDEX_SHIFT ) & \
    DSU4_ATBI_INDEX_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBFC \
 *   AHB trace buffer filter control register (ATBFC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBFC_WPF_SHIFT 12
#define DSU4_ATBFC_WPF_MASK 0x3000U
#define DSU4_ATBFC_WPF_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBFC_WPF_MASK ) >> \
    DSU4_ATBFC_WPF_SHIFT )
#define DSU4_ATBFC_WPF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBFC_WPF_MASK ) | \
    ( ( ( _val ) << DSU4_ATBFC_WPF_SHIFT ) & \
      DSU4_ATBFC_WPF_MASK ) )
#define DSU4_ATBFC_WPF( _val ) \
  ( ( ( _val ) << DSU4_ATBFC_WPF_SHIFT ) & \
    DSU4_ATBFC_WPF_MASK )

#define DSU4_ATBFC_BPF_SHIFT 8
#define DSU4_ATBFC_BPF_MASK 0x300U
#define DSU4_ATBFC_BPF_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBFC_BPF_MASK ) >> \
    DSU4_ATBFC_BPF_SHIFT )
#define DSU4_ATBFC_BPF_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBFC_BPF_MASK ) | \
    ( ( ( _val ) << DSU4_ATBFC_BPF_SHIFT ) & \
      DSU4_ATBFC_BPF_MASK ) )
#define DSU4_ATBFC_BPF( _val ) \
  ( ( ( _val ) << DSU4_ATBFC_BPF_SHIFT ) & \
    DSU4_ATBFC_BPF_MASK )

#define DSU4_ATBFC_PF 0x8U

#define DSU4_ATBFC_AF 0x4U

#define DSU4_ATBFC_FR 0x2U

#define DSU4_ATBFC_FW 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBFM \
 *   AHB trace buffer filter mask register (ATBFM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBFM_SMASK_15_0_SHIFT 16
#define DSU4_ATBFM_SMASK_15_0_MASK 0xffff0000U
#define DSU4_ATBFM_SMASK_15_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBFM_SMASK_15_0_MASK ) >> \
    DSU4_ATBFM_SMASK_15_0_SHIFT )
#define DSU4_ATBFM_SMASK_15_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBFM_SMASK_15_0_MASK ) | \
    ( ( ( _val ) << DSU4_ATBFM_SMASK_15_0_SHIFT ) & \
      DSU4_ATBFM_SMASK_15_0_MASK ) )
#define DSU4_ATBFM_SMASK_15_0( _val ) \
  ( ( ( _val ) << DSU4_ATBFM_SMASK_15_0_SHIFT ) & \
    DSU4_ATBFM_SMASK_15_0_MASK )

#define DSU4_ATBFM_MMASK_15_0_SHIFT 0
#define DSU4_ATBFM_MMASK_15_0_MASK 0xffffU
#define DSU4_ATBFM_MMASK_15_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBFM_MMASK_15_0_MASK ) >> \
    DSU4_ATBFM_MMASK_15_0_SHIFT )
#define DSU4_ATBFM_MMASK_15_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBFM_MMASK_15_0_MASK ) | \
    ( ( ( _val ) << DSU4_ATBFM_MMASK_15_0_SHIFT ) & \
      DSU4_ATBFM_MMASK_15_0_MASK ) )
#define DSU4_ATBFM_MMASK_15_0( _val ) \
  ( ( ( _val ) << DSU4_ATBFM_MMASK_15_0_SHIFT ) & \
    DSU4_ATBFM_MMASK_15_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBBA \
 *   AHB trace buffer break address registers (ATBBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBBA_BADDR_31_2_SHIFT 2
#define DSU4_ATBBA_BADDR_31_2_MASK 0xfffffffcU
#define DSU4_ATBBA_BADDR_31_2_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBBA_BADDR_31_2_MASK ) >> \
    DSU4_ATBBA_BADDR_31_2_SHIFT )
#define DSU4_ATBBA_BADDR_31_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBBA_BADDR_31_2_MASK ) | \
    ( ( ( _val ) << DSU4_ATBBA_BADDR_31_2_SHIFT ) & \
      DSU4_ATBBA_BADDR_31_2_MASK ) )
#define DSU4_ATBBA_BADDR_31_2( _val ) \
  ( ( ( _val ) << DSU4_ATBBA_BADDR_31_2_SHIFT ) & \
    DSU4_ATBBA_BADDR_31_2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ATBBM \
 *   AHB trace buffer break mask registers (ATBBM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ATBBM_BMASK_31_2_SHIFT 2
#define DSU4_ATBBM_BMASK_31_2_MASK 0xfffffffcU
#define DSU4_ATBBM_BMASK_31_2_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ATBBM_BMASK_31_2_MASK ) >> \
    DSU4_ATBBM_BMASK_31_2_SHIFT )
#define DSU4_ATBBM_BMASK_31_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ATBBM_BMASK_31_2_MASK ) | \
    ( ( ( _val ) << DSU4_ATBBM_BMASK_31_2_SHIFT ) & \
      DSU4_ATBBM_BMASK_31_2_MASK ) )
#define DSU4_ATBBM_BMASK_31_2( _val ) \
  ( ( ( _val ) << DSU4_ATBBM_BMASK_31_2_SHIFT ) & \
    DSU4_ATBBM_BMASK_31_2_MASK )

#define DSU4_ATBBM_LD 0x2U

#define DSU4_ATBBM_ST 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ICNT Instruction trace count register (ICNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ICNT_CE 0x80000000U

#define DSU4_ICNT_IC 0x40000000U

#define DSU4_ICNT_PE 0x20000000U

#define DSU4_ICNT_ICOUNT_28_0_SHIFT 0
#define DSU4_ICNT_ICOUNT_28_0_MASK 0x1fffffffU
#define DSU4_ICNT_ICOUNT_28_0_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ICNT_ICOUNT_28_0_MASK ) >> \
    DSU4_ICNT_ICOUNT_28_0_SHIFT )
#define DSU4_ICNT_ICOUNT_28_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ICNT_ICOUNT_28_0_MASK ) | \
    ( ( ( _val ) << DSU4_ICNT_ICOUNT_28_0_SHIFT ) & \
      DSU4_ICNT_ICOUNT_28_0_MASK ) )
#define DSU4_ICNT_ICOUNT_28_0( _val ) \
  ( ( ( _val ) << DSU4_ICNT_ICOUNT_28_0_SHIFT ) & \
    DSU4_ICNT_ICOUNT_28_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4AHBWPC \
 *   AHB watchpoint control register (AHBWPC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_AHBWPC_IN 0x40U

#define DSU4_AHBWPC_CP 0x20U

#define DSU4_AHBWPC_EN 0x10U

#define DSU4_AHBWPC_IN 0x4U

#define DSU4_AHBWPC_CP 0x2U

#define DSU4_AHBWPC_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4AHBWPD AHB watchpoint data registers (AHBWPD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_AHBWPD_DATA_SHIFT 0
#define DSU4_AHBWPD_DATA_MASK 0xffffffffU
#define DSU4_AHBWPD_DATA_GET( _reg ) \
  ( ( ( _reg ) & DSU4_AHBWPD_DATA_MASK ) >> \
    DSU4_AHBWPD_DATA_SHIFT )
#define DSU4_AHBWPD_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_AHBWPD_DATA_MASK ) | \
    ( ( ( _val ) << DSU4_AHBWPD_DATA_SHIFT ) & \
      DSU4_AHBWPD_DATA_MASK ) )
#define DSU4_AHBWPD_DATA( _val ) \
  ( ( ( _val ) << DSU4_AHBWPD_DATA_SHIFT ) & \
    DSU4_AHBWPD_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4AHBWPM AHB watchpoint mask registers (AHBWPM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_AHBWPM_MASK_SHIFT 0
#define DSU4_AHBWPM_MASK_MASK 0xffffffffU
#define DSU4_AHBWPM_MASK_GET( _reg ) \
  ( ( ( _reg ) & DSU4_AHBWPM_MASK_MASK ) >> \
    DSU4_AHBWPM_MASK_SHIFT )
#define DSU4_AHBWPM_MASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_AHBWPM_MASK_MASK ) | \
    ( ( ( _val ) << DSU4_AHBWPM_MASK_SHIFT ) & \
      DSU4_AHBWPM_MASK_MASK ) )
#define DSU4_AHBWPM_MASK( _val ) \
  ( ( ( _val ) << DSU4_AHBWPM_MASK_SHIFT ) & \
    DSU4_AHBWPM_MASK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ITBC0 \
 *   Instruction trace buffer control register 0 (ITBC0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ITBC0_TFILT_SHIFT 28
#define DSU4_ITBC0_TFILT_MASK 0xf0000000U
#define DSU4_ITBC0_TFILT_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ITBC0_TFILT_MASK ) >> \
    DSU4_ITBC0_TFILT_SHIFT )
#define DSU4_ITBC0_TFILT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ITBC0_TFILT_MASK ) | \
    ( ( ( _val ) << DSU4_ITBC0_TFILT_SHIFT ) & \
      DSU4_ITBC0_TFILT_MASK ) )
#define DSU4_ITBC0_TFILT( _val ) \
  ( ( ( _val ) << DSU4_ITBC0_TFILT_SHIFT ) & \
    DSU4_ITBC0_TFILT_MASK )

#define DSU4_ITBC0_ITPOINTER_SHIFT 0
#define DSU4_ITBC0_ITPOINTER_MASK 0xffffU
#define DSU4_ITBC0_ITPOINTER_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ITBC0_ITPOINTER_MASK ) >> \
    DSU4_ITBC0_ITPOINTER_SHIFT )
#define DSU4_ITBC0_ITPOINTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ITBC0_ITPOINTER_MASK ) | \
    ( ( ( _val ) << DSU4_ITBC0_ITPOINTER_SHIFT ) & \
      DSU4_ITBC0_ITPOINTER_MASK ) )
#define DSU4_ITBC0_ITPOINTER( _val ) \
  ( ( ( _val ) << DSU4_ITBC0_ITPOINTER_SHIFT ) & \
    DSU4_ITBC0_ITPOINTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBDSU4ITBC1 \
 *   Instruction trace buffer control register 1 (ITBC1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define DSU4_ITBC1_WO 0x8000000U

#define DSU4_ITBC1_TLIM_SHIFT 24
#define DSU4_ITBC1_TLIM_MASK 0x7000000U
#define DSU4_ITBC1_TLIM_GET( _reg ) \
  ( ( ( _reg ) & DSU4_ITBC1_TLIM_MASK ) >> \
    DSU4_ITBC1_TLIM_SHIFT )
#define DSU4_ITBC1_TLIM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~DSU4_ITBC1_TLIM_MASK ) | \
    ( ( ( _val ) << DSU4_ITBC1_TLIM_SHIFT ) & \
      DSU4_ITBC1_TLIM_MASK ) )
#define DSU4_ITBC1_TLIM( _val ) \
  ( ( ( _val ) << DSU4_ITBC1_TLIM_SHIFT ) & \
    DSU4_ITBC1_TLIM_MASK )

#define DSU4_ITBC1_TOV 0x800000U

/** @} */

/**
 * @brief This structure defines the DSU4 register block memory map.
 */
typedef struct dsu4 {
  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4CTRL.
   */
  uint32_t ctrl;

  uint32_t reserved_4_8;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4DTTC.
   */
  uint32_t dttc;

  uint32_t reserved_c_20[ 5 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4BRSS.
   */
  uint32_t brss;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4DBGM.
   */
  uint32_t dbgm;

  uint32_t reserved_28_40[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBC.
   */
  uint32_t atbc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBI.
   */
  uint32_t atbi;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBFC.
   */
  uint32_t atbfc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBFM.
   */
  uint32_t atbfm;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBBA.
   */
  uint32_t atbba_0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBBM.
   */
  uint32_t atbbm_0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBBA.
   */
  uint32_t atbba_1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ATBBM.
   */
  uint32_t atbbm_1;

  uint32_t reserved_60_70[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ICNT.
   */
  uint32_t icnt;

  uint32_t reserved_74_80[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPC.
   */
  uint32_t ahbwpc;

  uint32_t reserved_84_90[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPD.
   */
  uint32_t ahbwpd_0;

  uint32_t reserved_94_9c[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPD.
   */
  uint32_t ahbwpd_1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPM.
   */
  uint32_t ahbwpm_0;

  uint32_t reserved_a4_ac[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPM.
   */
  uint32_t ahbwpm_1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPD.
   */
  uint32_t ahbwpd_2;

  uint32_t reserved_b4_bc[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPD.
   */
  uint32_t ahbwpd_3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPM.
   */
  uint32_t ahbwpm_2;

  uint32_t reserved_c4_cc[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4AHBWPM.
   */
  uint32_t ahbwpm_3;

  uint32_t reserved_d0_110000[ 278476 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ITBC0.
   */
  uint32_t itbc0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4ITBC1.
   */
  uint32_t itbc1;

  uint32_t reserved_110008_400020[ 770054 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4DTR.
   */
  uint32_t dtr;

  /**
   * @brief See @ref RTEMSDeviceGRLIBDSU4DASI.
   */
  uint32_t dasi;
} dsu4;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_DSU4_REGS_H */
