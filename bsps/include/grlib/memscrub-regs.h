/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBMEMSCRUB
 *
 * @brief This header file defines the MEMSCRUB register block interface.
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

/* Generated from spec:/dev/grlib/if/memscrub-header */

#ifndef _GRLIB_MEMSCRUB_REGS_H
#define _GRLIB_MEMSCRUB_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/memscrub */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUB MEMSCRUB
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the MEMSCRUB interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBS AHB Status register (AHBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBS_CECNT_SHIFT 22
#define MEMSCRUB_AHBS_CECNT_MASK 0xffc00000U
#define MEMSCRUB_AHBS_CECNT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBS_CECNT_MASK ) >> \
    MEMSCRUB_AHBS_CECNT_SHIFT )
#define MEMSCRUB_AHBS_CECNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBS_CECNT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBS_CECNT_SHIFT ) & \
      MEMSCRUB_AHBS_CECNT_MASK ) )
#define MEMSCRUB_AHBS_CECNT( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBS_CECNT_SHIFT ) & \
    MEMSCRUB_AHBS_CECNT_MASK )

#define MEMSCRUB_AHBS_UECNT_SHIFT 14
#define MEMSCRUB_AHBS_UECNT_MASK 0x3fc000U
#define MEMSCRUB_AHBS_UECNT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBS_UECNT_MASK ) >> \
    MEMSCRUB_AHBS_UECNT_SHIFT )
#define MEMSCRUB_AHBS_UECNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBS_UECNT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBS_UECNT_SHIFT ) & \
      MEMSCRUB_AHBS_UECNT_MASK ) )
#define MEMSCRUB_AHBS_UECNT( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBS_UECNT_SHIFT ) & \
    MEMSCRUB_AHBS_UECNT_MASK )

#define MEMSCRUB_AHBS_DONE 0x2000U

#define MEMSCRUB_AHBS_SEC 0x800U

#define MEMSCRUB_AHBS_SBC 0x400U

#define MEMSCRUB_AHBS_CE 0x200U

#define MEMSCRUB_AHBS_NE 0x100U

#define MEMSCRUB_AHBS_HWRITE 0x80U

#define MEMSCRUB_AHBS_HMASTER_SHIFT 3
#define MEMSCRUB_AHBS_HMASTER_MASK 0x78U
#define MEMSCRUB_AHBS_HMASTER_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBS_HMASTER_MASK ) >> \
    MEMSCRUB_AHBS_HMASTER_SHIFT )
#define MEMSCRUB_AHBS_HMASTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBS_HMASTER_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBS_HMASTER_SHIFT ) & \
      MEMSCRUB_AHBS_HMASTER_MASK ) )
#define MEMSCRUB_AHBS_HMASTER( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBS_HMASTER_SHIFT ) & \
    MEMSCRUB_AHBS_HMASTER_MASK )

#define MEMSCRUB_AHBS_HSIZE_SHIFT 0
#define MEMSCRUB_AHBS_HSIZE_MASK 0x7U
#define MEMSCRUB_AHBS_HSIZE_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBS_HSIZE_MASK ) >> \
    MEMSCRUB_AHBS_HSIZE_SHIFT )
#define MEMSCRUB_AHBS_HSIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBS_HSIZE_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBS_HSIZE_SHIFT ) & \
      MEMSCRUB_AHBS_HSIZE_MASK ) )
#define MEMSCRUB_AHBS_HSIZE( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBS_HSIZE_SHIFT ) & \
    MEMSCRUB_AHBS_HSIZE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBFAR \
 *   AHB Failing Address Register (AHBFAR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SHIFT 0
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK 0xffffffffU
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK ) >> \
    MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SHIFT )
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SHIFT ) & \
      MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK ) )
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SHIFT ) & \
    MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBERC \
 *   AHB Error configuration register (AHBERC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBERC_CECNTT_SHIFT 22
#define MEMSCRUB_AHBERC_CECNTT_MASK 0xffc00000U
#define MEMSCRUB_AHBERC_CECNTT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBERC_CECNTT_MASK ) >> \
    MEMSCRUB_AHBERC_CECNTT_SHIFT )
#define MEMSCRUB_AHBERC_CECNTT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBERC_CECNTT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBERC_CECNTT_SHIFT ) & \
      MEMSCRUB_AHBERC_CECNTT_MASK ) )
#define MEMSCRUB_AHBERC_CECNTT( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBERC_CECNTT_SHIFT ) & \
    MEMSCRUB_AHBERC_CECNTT_MASK )

#define MEMSCRUB_AHBERC_UECNTT_SHIFT 14
#define MEMSCRUB_AHBERC_UECNTT_MASK 0x3fc000U
#define MEMSCRUB_AHBERC_UECNTT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_AHBERC_UECNTT_MASK ) >> \
    MEMSCRUB_AHBERC_UECNTT_SHIFT )
#define MEMSCRUB_AHBERC_UECNTT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_AHBERC_UECNTT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_AHBERC_UECNTT_SHIFT ) & \
      MEMSCRUB_AHBERC_UECNTT_MASK ) )
#define MEMSCRUB_AHBERC_UECNTT( _val ) \
  ( ( ( _val ) << MEMSCRUB_AHBERC_UECNTT_SHIFT ) & \
    MEMSCRUB_AHBERC_UECNTT_MASK )

#define MEMSCRUB_AHBERC_CECTE 0x2U

#define MEMSCRUB_AHBERC_UECTE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBSTAT Status register (STAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_STAT_RUNCOUNT_SHIFT 22
#define MEMSCRUB_STAT_RUNCOUNT_MASK 0xffc00000U
#define MEMSCRUB_STAT_RUNCOUNT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_STAT_RUNCOUNT_MASK ) >> \
    MEMSCRUB_STAT_RUNCOUNT_SHIFT )
#define MEMSCRUB_STAT_RUNCOUNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_STAT_RUNCOUNT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_STAT_RUNCOUNT_SHIFT ) & \
      MEMSCRUB_STAT_RUNCOUNT_MASK ) )
#define MEMSCRUB_STAT_RUNCOUNT( _val ) \
  ( ( ( _val ) << MEMSCRUB_STAT_RUNCOUNT_SHIFT ) & \
    MEMSCRUB_STAT_RUNCOUNT_MASK )

#define MEMSCRUB_STAT_BLKCOUNT_SHIFT 14
#define MEMSCRUB_STAT_BLKCOUNT_MASK 0x3fc000U
#define MEMSCRUB_STAT_BLKCOUNT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_STAT_BLKCOUNT_MASK ) >> \
    MEMSCRUB_STAT_BLKCOUNT_SHIFT )
#define MEMSCRUB_STAT_BLKCOUNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_STAT_BLKCOUNT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_STAT_BLKCOUNT_SHIFT ) & \
      MEMSCRUB_STAT_BLKCOUNT_MASK ) )
#define MEMSCRUB_STAT_BLKCOUNT( _val ) \
  ( ( ( _val ) << MEMSCRUB_STAT_BLKCOUNT_SHIFT ) & \
    MEMSCRUB_STAT_BLKCOUNT_MASK )

#define MEMSCRUB_STAT_DONE 0x2000U

#define MEMSCRUB_STAT_BURSTLEN_SHIFT 1
#define MEMSCRUB_STAT_BURSTLEN_MASK 0x1eU
#define MEMSCRUB_STAT_BURSTLEN_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_STAT_BURSTLEN_MASK ) >> \
    MEMSCRUB_STAT_BURSTLEN_SHIFT )
#define MEMSCRUB_STAT_BURSTLEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_STAT_BURSTLEN_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_STAT_BURSTLEN_SHIFT ) & \
      MEMSCRUB_STAT_BURSTLEN_MASK ) )
#define MEMSCRUB_STAT_BURSTLEN( _val ) \
  ( ( ( _val ) << MEMSCRUB_STAT_BURSTLEN_SHIFT ) & \
    MEMSCRUB_STAT_BURSTLEN_MASK )

#define MEMSCRUB_STAT_ACTIVE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBCONFIG Configuration register (CONFIG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_CONFIG_DELAY_SHIFT 8
#define MEMSCRUB_CONFIG_DELAY_MASK 0xff00U
#define MEMSCRUB_CONFIG_DELAY_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_CONFIG_DELAY_MASK ) >> \
    MEMSCRUB_CONFIG_DELAY_SHIFT )
#define MEMSCRUB_CONFIG_DELAY_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_CONFIG_DELAY_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_CONFIG_DELAY_SHIFT ) & \
      MEMSCRUB_CONFIG_DELAY_MASK ) )
#define MEMSCRUB_CONFIG_DELAY( _val ) \
  ( ( ( _val ) << MEMSCRUB_CONFIG_DELAY_SHIFT ) & \
    MEMSCRUB_CONFIG_DELAY_MASK )

#define MEMSCRUB_CONFIG_IRQD 0x80U

#define MEMSCRUB_CONFIG_SERA 0x20U

#define MEMSCRUB_CONFIG_LOOP 0x10U

#define MEMSCRUB_CONFIG_MODE_SHIFT 2
#define MEMSCRUB_CONFIG_MODE_MASK 0xcU
#define MEMSCRUB_CONFIG_MODE_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_CONFIG_MODE_MASK ) >> \
    MEMSCRUB_CONFIG_MODE_SHIFT )
#define MEMSCRUB_CONFIG_MODE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_CONFIG_MODE_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_CONFIG_MODE_SHIFT ) & \
      MEMSCRUB_CONFIG_MODE_MASK ) )
#define MEMSCRUB_CONFIG_MODE( _val ) \
  ( ( ( _val ) << MEMSCRUB_CONFIG_MODE_SHIFT ) & \
    MEMSCRUB_CONFIG_MODE_MASK )

#define MEMSCRUB_CONFIG_ES 0x2U

#define MEMSCRUB_CONFIG_SCEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEL Range low address register (RANGEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEL_RLADDR_SHIFT 0
#define MEMSCRUB_RANGEL_RLADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEL_RLADDR_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_RANGEL_RLADDR_MASK ) >> \
    MEMSCRUB_RANGEL_RLADDR_SHIFT )
#define MEMSCRUB_RANGEL_RLADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_RANGEL_RLADDR_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_RANGEL_RLADDR_SHIFT ) & \
      MEMSCRUB_RANGEL_RLADDR_MASK ) )
#define MEMSCRUB_RANGEL_RLADDR( _val ) \
  ( ( ( _val ) << MEMSCRUB_RANGEL_RLADDR_SHIFT ) & \
    MEMSCRUB_RANGEL_RLADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEH \
 *   Range high address register (RANGEH)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEH_RHADDR_SHIFT 0
#define MEMSCRUB_RANGEH_RHADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEH_RHADDR_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_RANGEH_RHADDR_MASK ) >> \
    MEMSCRUB_RANGEH_RHADDR_SHIFT )
#define MEMSCRUB_RANGEH_RHADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_RANGEH_RHADDR_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_RANGEH_RHADDR_SHIFT ) & \
      MEMSCRUB_RANGEH_RHADDR_MASK ) )
#define MEMSCRUB_RANGEH_RHADDR( _val ) \
  ( ( ( _val ) << MEMSCRUB_RANGEH_RHADDR_SHIFT ) & \
    MEMSCRUB_RANGEH_RHADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBPOS Position register (POS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_POS_POSITION_SHIFT 0
#define MEMSCRUB_POS_POSITION_MASK 0xffffffffU
#define MEMSCRUB_POS_POSITION_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_POS_POSITION_MASK ) >> \
    MEMSCRUB_POS_POSITION_SHIFT )
#define MEMSCRUB_POS_POSITION_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_POS_POSITION_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_POS_POSITION_SHIFT ) & \
      MEMSCRUB_POS_POSITION_MASK ) )
#define MEMSCRUB_POS_POSITION( _val ) \
  ( ( ( _val ) << MEMSCRUB_POS_POSITION_SHIFT ) & \
    MEMSCRUB_POS_POSITION_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBETHRES Error threshold register (ETHRES)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_ETHRES_RECT_SHIFT 22
#define MEMSCRUB_ETHRES_RECT_MASK 0xffc00000U
#define MEMSCRUB_ETHRES_RECT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_ETHRES_RECT_MASK ) >> \
    MEMSCRUB_ETHRES_RECT_SHIFT )
#define MEMSCRUB_ETHRES_RECT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_ETHRES_RECT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_ETHRES_RECT_SHIFT ) & \
      MEMSCRUB_ETHRES_RECT_MASK ) )
#define MEMSCRUB_ETHRES_RECT( _val ) \
  ( ( ( _val ) << MEMSCRUB_ETHRES_RECT_SHIFT ) & \
    MEMSCRUB_ETHRES_RECT_MASK )

#define MEMSCRUB_ETHRES_BECT_SHIFT 14
#define MEMSCRUB_ETHRES_BECT_MASK 0x3fc000U
#define MEMSCRUB_ETHRES_BECT_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_ETHRES_BECT_MASK ) >> \
    MEMSCRUB_ETHRES_BECT_SHIFT )
#define MEMSCRUB_ETHRES_BECT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_ETHRES_BECT_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_ETHRES_BECT_SHIFT ) & \
      MEMSCRUB_ETHRES_BECT_MASK ) )
#define MEMSCRUB_ETHRES_BECT( _val ) \
  ( ( ( _val ) << MEMSCRUB_ETHRES_BECT_SHIFT ) & \
    MEMSCRUB_ETHRES_BECT_MASK )

#define MEMSCRUB_ETHRES_RECTE 0x2U

#define MEMSCRUB_ETHRES_BECTE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBINIT Initialisation data register (INIT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_INIT_DATA_SHIFT 0
#define MEMSCRUB_INIT_DATA_MASK 0xffffffffU
#define MEMSCRUB_INIT_DATA_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_INIT_DATA_MASK ) >> \
    MEMSCRUB_INIT_DATA_SHIFT )
#define MEMSCRUB_INIT_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_INIT_DATA_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_INIT_DATA_SHIFT ) & \
      MEMSCRUB_INIT_DATA_MASK ) )
#define MEMSCRUB_INIT_DATA( _val ) \
  ( ( ( _val ) << MEMSCRUB_INIT_DATA_SHIFT ) & \
    MEMSCRUB_INIT_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEL2 \
 *   Second range low address register (RANGEL2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEL2_RLADDR_SHIFT 0
#define MEMSCRUB_RANGEL2_RLADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEL2_RLADDR_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_RANGEL2_RLADDR_MASK ) >> \
    MEMSCRUB_RANGEL2_RLADDR_SHIFT )
#define MEMSCRUB_RANGEL2_RLADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_RANGEL2_RLADDR_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_RANGEL2_RLADDR_SHIFT ) & \
      MEMSCRUB_RANGEL2_RLADDR_MASK ) )
#define MEMSCRUB_RANGEL2_RLADDR( _val ) \
  ( ( ( _val ) << MEMSCRUB_RANGEL2_RLADDR_SHIFT ) & \
    MEMSCRUB_RANGEL2_RLADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEH2 \
 *   Second range high address register (RANGEH2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEH2_RHADDR_SHIFT 0
#define MEMSCRUB_RANGEH2_RHADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEH2_RHADDR_GET( _reg ) \
  ( ( ( _reg ) & MEMSCRUB_RANGEH2_RHADDR_MASK ) >> \
    MEMSCRUB_RANGEH2_RHADDR_SHIFT )
#define MEMSCRUB_RANGEH2_RHADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~MEMSCRUB_RANGEH2_RHADDR_MASK ) | \
    ( ( ( _val ) << MEMSCRUB_RANGEH2_RHADDR_SHIFT ) & \
      MEMSCRUB_RANGEH2_RHADDR_MASK ) )
#define MEMSCRUB_RANGEH2_RHADDR( _val ) \
  ( ( ( _val ) << MEMSCRUB_RANGEH2_RHADDR_SHIFT ) & \
    MEMSCRUB_RANGEH2_RHADDR_MASK )

/** @} */

/**
 * @brief This structure defines the MEMSCRUB register block memory map.
 */
typedef struct memscrub {
  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBS.
   */
  uint32_t ahbs;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBFAR.
   */
  uint32_t ahbfar;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBERC.
   */
  uint32_t ahberc;

  uint32_t reserved_c_10;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBSTAT.
   */
  uint32_t stat;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBCONFIG.
   */
  uint32_t config;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEL.
   */
  uint32_t rangel;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEH.
   */
  uint32_t rangeh;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBPOS.
   */
  uint32_t pos;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBETHRES.
   */
  uint32_t ethres;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBINIT.
   */
  uint32_t init;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEL2.
   */
  uint32_t rangel2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEH2.
   */
  uint32_t rangeh2;
} memscrub;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_MEMSCRUB_REGS_H */
