/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRIOMMU
 *
 * @brief This header file defines the GRIOMMU register block interface.
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

/* Generated from spec:/dev/grlib/if/griommu-header */

#ifndef _GRLIB_GRIOMMU_REGS_H
#define _GRLIB_GRIOMMU_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/griommu */

/**
 * @defgroup RTEMSDeviceGRIOMMU GRIOMMU
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRIOMMU interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRIOMMUCAP0 Capability register 0 (CAP0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_CAP0_A 0x80000000U

#define GRIOMMU_CAP0_AC 0x40000000U

#define GRIOMMU_CAP0_CA 0x20000000U

#define GRIOMMU_CAP0_CP 0x10000000U

#define GRIOMMU_CAP0_NARB_SHIFT 20
#define GRIOMMU_CAP0_NARB_MASK 0xf00000U
#define GRIOMMU_CAP0_NARB_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP0_NARB_MASK ) >> \
    GRIOMMU_CAP0_NARB_SHIFT )
#define GRIOMMU_CAP0_NARB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP0_NARB_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP0_NARB_SHIFT ) & \
      GRIOMMU_CAP0_NARB_MASK ) )
#define GRIOMMU_CAP0_NARB( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP0_NARB_SHIFT ) & \
    GRIOMMU_CAP0_NARB_MASK )

#define GRIOMMU_CAP0_CS 0x80000U

#define GRIOMMU_CAP0_FT_SHIFT 17
#define GRIOMMU_CAP0_FT_MASK 0x60000U
#define GRIOMMU_CAP0_FT_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP0_FT_MASK ) >> \
    GRIOMMU_CAP0_FT_SHIFT )
#define GRIOMMU_CAP0_FT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP0_FT_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP0_FT_SHIFT ) & \
      GRIOMMU_CAP0_FT_MASK ) )
#define GRIOMMU_CAP0_FT( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP0_FT_SHIFT ) & \
    GRIOMMU_CAP0_FT_MASK )

#define GRIOMMU_CAP0_ST 0x10000U

#define GRIOMMU_CAP0_I 0x8000U

#define GRIOMMU_CAP0_IT 0x4000U

#define GRIOMMU_CAP0_IA 0x2000U

#define GRIOMMU_CAP0_IP 0x1000U

#define GRIOMMU_CAP0_MB 0x100U

#define GRIOMMU_CAP0_GRPS_SHIFT 4
#define GRIOMMU_CAP0_GRPS_MASK 0xf0U
#define GRIOMMU_CAP0_GRPS_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP0_GRPS_MASK ) >> \
    GRIOMMU_CAP0_GRPS_SHIFT )
#define GRIOMMU_CAP0_GRPS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP0_GRPS_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP0_GRPS_SHIFT ) & \
      GRIOMMU_CAP0_GRPS_MASK ) )
#define GRIOMMU_CAP0_GRPS( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP0_GRPS_SHIFT ) & \
    GRIOMMU_CAP0_GRPS_MASK )

#define GRIOMMU_CAP0_MSTS_SHIFT 0
#define GRIOMMU_CAP0_MSTS_MASK 0xfU
#define GRIOMMU_CAP0_MSTS_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP0_MSTS_MASK ) >> \
    GRIOMMU_CAP0_MSTS_SHIFT )
#define GRIOMMU_CAP0_MSTS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP0_MSTS_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP0_MSTS_SHIFT ) & \
      GRIOMMU_CAP0_MSTS_MASK ) )
#define GRIOMMU_CAP0_MSTS( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP0_MSTS_SHIFT ) & \
    GRIOMMU_CAP0_MSTS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUCAP1 Capability register 1 (CAP1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_CAP1_CADDR_SHIFT 20
#define GRIOMMU_CAP1_CADDR_MASK 0xfff00000U
#define GRIOMMU_CAP1_CADDR_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP1_CADDR_MASK ) >> \
    GRIOMMU_CAP1_CADDR_SHIFT )
#define GRIOMMU_CAP1_CADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP1_CADDR_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP1_CADDR_SHIFT ) & \
      GRIOMMU_CAP1_CADDR_MASK ) )
#define GRIOMMU_CAP1_CADDR( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP1_CADDR_SHIFT ) & \
    GRIOMMU_CAP1_CADDR_MASK )

#define GRIOMMU_CAP1_CMASK_SHIFT 16
#define GRIOMMU_CAP1_CMASK_MASK 0xf0000U
#define GRIOMMU_CAP1_CMASK_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP1_CMASK_MASK ) >> \
    GRIOMMU_CAP1_CMASK_SHIFT )
#define GRIOMMU_CAP1_CMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP1_CMASK_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP1_CMASK_SHIFT ) & \
      GRIOMMU_CAP1_CMASK_MASK ) )
#define GRIOMMU_CAP1_CMASK( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP1_CMASK_SHIFT ) & \
    GRIOMMU_CAP1_CMASK_MASK )

#define GRIOMMU_CAP1_CTAGBITS_SHIFT 8
#define GRIOMMU_CAP1_CTAGBITS_MASK 0xff00U
#define GRIOMMU_CAP1_CTAGBITS_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP1_CTAGBITS_MASK ) >> \
    GRIOMMU_CAP1_CTAGBITS_SHIFT )
#define GRIOMMU_CAP1_CTAGBITS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP1_CTAGBITS_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP1_CTAGBITS_SHIFT ) & \
      GRIOMMU_CAP1_CTAGBITS_MASK ) )
#define GRIOMMU_CAP1_CTAGBITS( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP1_CTAGBITS_SHIFT ) & \
    GRIOMMU_CAP1_CTAGBITS_MASK )

#define GRIOMMU_CAP1_CISIZE_SHIFT 5
#define GRIOMMU_CAP1_CISIZE_MASK 0xe0U
#define GRIOMMU_CAP1_CISIZE_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP1_CISIZE_MASK ) >> \
    GRIOMMU_CAP1_CISIZE_SHIFT )
#define GRIOMMU_CAP1_CISIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP1_CISIZE_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP1_CISIZE_SHIFT ) & \
      GRIOMMU_CAP1_CISIZE_MASK ) )
#define GRIOMMU_CAP1_CISIZE( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP1_CISIZE_SHIFT ) & \
    GRIOMMU_CAP1_CISIZE_MASK )

#define GRIOMMU_CAP1_CLINES_SHIFT 0
#define GRIOMMU_CAP1_CLINES_MASK 0x1fU
#define GRIOMMU_CAP1_CLINES_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP1_CLINES_MASK ) >> \
    GRIOMMU_CAP1_CLINES_SHIFT )
#define GRIOMMU_CAP1_CLINES_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP1_CLINES_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP1_CLINES_SHIFT ) & \
      GRIOMMU_CAP1_CLINES_MASK ) )
#define GRIOMMU_CAP1_CLINES( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP1_CLINES_SHIFT ) & \
    GRIOMMU_CAP1_CLINES_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUCAP2 Capability register 2 (CAP2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_CAP2_TMASK_SHIFT 24
#define GRIOMMU_CAP2_TMASK_MASK 0xff000000U
#define GRIOMMU_CAP2_TMASK_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_TMASK_MASK ) >> \
    GRIOMMU_CAP2_TMASK_SHIFT )
#define GRIOMMU_CAP2_TMASK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_TMASK_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_TMASK_SHIFT ) & \
      GRIOMMU_CAP2_TMASK_MASK ) )
#define GRIOMMU_CAP2_TMASK( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_TMASK_SHIFT ) & \
    GRIOMMU_CAP2_TMASK_MASK )

#define GRIOMMU_CAP2_MTYPE_SHIFT 18
#define GRIOMMU_CAP2_MTYPE_MASK 0xc0000U
#define GRIOMMU_CAP2_MTYPE_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_MTYPE_MASK ) >> \
    GRIOMMU_CAP2_MTYPE_SHIFT )
#define GRIOMMU_CAP2_MTYPE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_MTYPE_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_MTYPE_SHIFT ) & \
      GRIOMMU_CAP2_MTYPE_MASK ) )
#define GRIOMMU_CAP2_MTYPE( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_MTYPE_SHIFT ) & \
    GRIOMMU_CAP2_MTYPE_MASK )

#define GRIOMMU_CAP2_TTYPE_SHIFT 16
#define GRIOMMU_CAP2_TTYPE_MASK 0x30000U
#define GRIOMMU_CAP2_TTYPE_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_TTYPE_MASK ) >> \
    GRIOMMU_CAP2_TTYPE_SHIFT )
#define GRIOMMU_CAP2_TTYPE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_TTYPE_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_TTYPE_SHIFT ) & \
      GRIOMMU_CAP2_TTYPE_MASK ) )
#define GRIOMMU_CAP2_TTYPE( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_TTYPE_SHIFT ) & \
    GRIOMMU_CAP2_TTYPE_MASK )

#define GRIOMMU_CAP2_TTAGBITS_SHIFT 8
#define GRIOMMU_CAP2_TTAGBITS_MASK 0xff00U
#define GRIOMMU_CAP2_TTAGBITS_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_TTAGBITS_MASK ) >> \
    GRIOMMU_CAP2_TTAGBITS_SHIFT )
#define GRIOMMU_CAP2_TTAGBITS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_TTAGBITS_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_TTAGBITS_SHIFT ) & \
      GRIOMMU_CAP2_TTAGBITS_MASK ) )
#define GRIOMMU_CAP2_TTAGBITS( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_TTAGBITS_SHIFT ) & \
    GRIOMMU_CAP2_TTAGBITS_MASK )

#define GRIOMMU_CAP2_ISIZE_SHIFT 5
#define GRIOMMU_CAP2_ISIZE_MASK 0xe0U
#define GRIOMMU_CAP2_ISIZE_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_ISIZE_MASK ) >> \
    GRIOMMU_CAP2_ISIZE_SHIFT )
#define GRIOMMU_CAP2_ISIZE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_ISIZE_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_ISIZE_SHIFT ) & \
      GRIOMMU_CAP2_ISIZE_MASK ) )
#define GRIOMMU_CAP2_ISIZE( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_ISIZE_SHIFT ) & \
    GRIOMMU_CAP2_ISIZE_MASK )

#define GRIOMMU_CAP2_TLBENT_SHIFT 0
#define GRIOMMU_CAP2_TLBENT_MASK 0x1fU
#define GRIOMMU_CAP2_TLBENT_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CAP2_TLBENT_MASK ) >> \
    GRIOMMU_CAP2_TLBENT_SHIFT )
#define GRIOMMU_CAP2_TLBENT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CAP2_TLBENT_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CAP2_TLBENT_SHIFT ) & \
      GRIOMMU_CAP2_TLBENT_MASK ) )
#define GRIOMMU_CAP2_TLBENT( _val ) \
  ( ( ( _val ) << GRIOMMU_CAP2_TLBENT_SHIFT ) & \
    GRIOMMU_CAP2_TLBENT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUCTRL Control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_CTRL_PGSZ_SHIFT 18
#define GRIOMMU_CTRL_PGSZ_MASK 0x1c0000U
#define GRIOMMU_CTRL_PGSZ_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CTRL_PGSZ_MASK ) >> \
    GRIOMMU_CTRL_PGSZ_SHIFT )
#define GRIOMMU_CTRL_PGSZ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CTRL_PGSZ_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CTRL_PGSZ_SHIFT ) & \
      GRIOMMU_CTRL_PGSZ_MASK ) )
#define GRIOMMU_CTRL_PGSZ( _val ) \
  ( ( ( _val ) << GRIOMMU_CTRL_PGSZ_SHIFT ) & \
    GRIOMMU_CTRL_PGSZ_MASK )

#define GRIOMMU_CTRL_LB 0x20000U

#define GRIOMMU_CTRL_SP 0x10000U

#define GRIOMMU_CTRL_ITR_SHIFT 12
#define GRIOMMU_CTRL_ITR_MASK 0xf000U
#define GRIOMMU_CTRL_ITR_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CTRL_ITR_MASK ) >> \
    GRIOMMU_CTRL_ITR_SHIFT )
#define GRIOMMU_CTRL_ITR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CTRL_ITR_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CTRL_ITR_SHIFT ) & \
      GRIOMMU_CTRL_ITR_MASK ) )
#define GRIOMMU_CTRL_ITR( _val ) \
  ( ( ( _val ) << GRIOMMU_CTRL_ITR_SHIFT ) & \
    GRIOMMU_CTRL_ITR_MASK )

#define GRIOMMU_CTRL_DP 0x800U

#define GRIOMMU_CTRL_SIV 0x400U

#define GRIOMMU_CTRL_HPROT_SHIFT 8
#define GRIOMMU_CTRL_HPROT_MASK 0x300U
#define GRIOMMU_CTRL_HPROT_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CTRL_HPROT_MASK ) >> \
    GRIOMMU_CTRL_HPROT_SHIFT )
#define GRIOMMU_CTRL_HPROT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CTRL_HPROT_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CTRL_HPROT_SHIFT ) & \
      GRIOMMU_CTRL_HPROT_MASK ) )
#define GRIOMMU_CTRL_HPROT( _val ) \
  ( ( ( _val ) << GRIOMMU_CTRL_HPROT_SHIFT ) & \
    GRIOMMU_CTRL_HPROT_MASK )

#define GRIOMMU_CTRL_AU 0x80U

#define GRIOMMU_CTRL_WP 0x40U

#define GRIOMMU_CTRL_DM 0x20U

#define GRIOMMU_CTRL_GS 0x10U

#define GRIOMMU_CTRL_CE 0x8U

#define GRIOMMU_CTRL_PM_SHIFT 1
#define GRIOMMU_CTRL_PM_MASK 0x6U
#define GRIOMMU_CTRL_PM_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_CTRL_PM_MASK ) >> \
    GRIOMMU_CTRL_PM_SHIFT )
#define GRIOMMU_CTRL_PM_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_CTRL_PM_MASK ) | \
    ( ( ( _val ) << GRIOMMU_CTRL_PM_SHIFT ) & \
      GRIOMMU_CTRL_PM_MASK ) )
#define GRIOMMU_CTRL_PM( _val ) \
  ( ( ( _val ) << GRIOMMU_CTRL_PM_SHIFT ) & \
    GRIOMMU_CTRL_PM_MASK )

#define GRIOMMU_CTRL_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUFLUSH TLB/cache flush register (FLUSH)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_FLUSH_FGRP_SHIFT 4
#define GRIOMMU_FLUSH_FGRP_MASK 0xf0U
#define GRIOMMU_FLUSH_FGRP_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_FLUSH_FGRP_MASK ) >> \
    GRIOMMU_FLUSH_FGRP_SHIFT )
#define GRIOMMU_FLUSH_FGRP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_FLUSH_FGRP_MASK ) | \
    ( ( ( _val ) << GRIOMMU_FLUSH_FGRP_SHIFT ) & \
      GRIOMMU_FLUSH_FGRP_MASK ) )
#define GRIOMMU_FLUSH_FGRP( _val ) \
  ( ( ( _val ) << GRIOMMU_FLUSH_FGRP_SHIFT ) & \
    GRIOMMU_FLUSH_FGRP_MASK )

#define GRIOMMU_FLUSH_GF 0x2U

#define GRIOMMU_FLUSH_F 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUSTATUS Status register (STATUS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_STATUS_PE 0x20U

#define GRIOMMU_STATUS_DE 0x10U

#define GRIOMMU_STATUS_FC 0x8U

#define GRIOMMU_STATUS_FL 0x4U

#define GRIOMMU_STATUS_AD 0x2U

#define GRIOMMU_STATUS_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUIMASK Interrupt mask register (IMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_IMASK_PEI 0x20U

#define GRIOMMU_IMASK_FCI 0x8U

#define GRIOMMU_IMASK_FLI 0x4U

#define GRIOMMU_IMASK_ADI 0x2U

#define GRIOMMU_IMASK_TEI 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUAHBFAS AHB failing access register (AHBFAS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_AHBFAS_FADDR_31_5_SHIFT 5
#define GRIOMMU_AHBFAS_FADDR_31_5_MASK 0xffffffe0U
#define GRIOMMU_AHBFAS_FADDR_31_5_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_AHBFAS_FADDR_31_5_MASK ) >> \
    GRIOMMU_AHBFAS_FADDR_31_5_SHIFT )
#define GRIOMMU_AHBFAS_FADDR_31_5_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_AHBFAS_FADDR_31_5_MASK ) | \
    ( ( ( _val ) << GRIOMMU_AHBFAS_FADDR_31_5_SHIFT ) & \
      GRIOMMU_AHBFAS_FADDR_31_5_MASK ) )
#define GRIOMMU_AHBFAS_FADDR_31_5( _val ) \
  ( ( ( _val ) << GRIOMMU_AHBFAS_FADDR_31_5_SHIFT ) & \
    GRIOMMU_AHBFAS_FADDR_31_5_MASK )

#define GRIOMMU_AHBFAS_FW 0x10U

#define GRIOMMU_AHBFAS_FMASTER_SHIFT 0
#define GRIOMMU_AHBFAS_FMASTER_MASK 0xfU
#define GRIOMMU_AHBFAS_FMASTER_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_AHBFAS_FMASTER_MASK ) >> \
    GRIOMMU_AHBFAS_FMASTER_SHIFT )
#define GRIOMMU_AHBFAS_FMASTER_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_AHBFAS_FMASTER_MASK ) | \
    ( ( ( _val ) << GRIOMMU_AHBFAS_FMASTER_SHIFT ) & \
      GRIOMMU_AHBFAS_FMASTER_MASK ) )
#define GRIOMMU_AHBFAS_FMASTER( _val ) \
  ( ( ( _val ) << GRIOMMU_AHBFAS_FMASTER_SHIFT ) & \
    GRIOMMU_AHBFAS_FMASTER_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUMSTCFG \
 *   Master configuration register 0 - 9 (MSTCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_MSTCFG_VENDOR_SHIFT 24
#define GRIOMMU_MSTCFG_VENDOR_MASK 0xff000000U
#define GRIOMMU_MSTCFG_VENDOR_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_MSTCFG_VENDOR_MASK ) >> \
    GRIOMMU_MSTCFG_VENDOR_SHIFT )
#define GRIOMMU_MSTCFG_VENDOR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_MSTCFG_VENDOR_MASK ) | \
    ( ( ( _val ) << GRIOMMU_MSTCFG_VENDOR_SHIFT ) & \
      GRIOMMU_MSTCFG_VENDOR_MASK ) )
#define GRIOMMU_MSTCFG_VENDOR( _val ) \
  ( ( ( _val ) << GRIOMMU_MSTCFG_VENDOR_SHIFT ) & \
    GRIOMMU_MSTCFG_VENDOR_MASK )

#define GRIOMMU_MSTCFG_DEVICE_SHIFT 12
#define GRIOMMU_MSTCFG_DEVICE_MASK 0xfff000U
#define GRIOMMU_MSTCFG_DEVICE_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_MSTCFG_DEVICE_MASK ) >> \
    GRIOMMU_MSTCFG_DEVICE_SHIFT )
#define GRIOMMU_MSTCFG_DEVICE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_MSTCFG_DEVICE_MASK ) | \
    ( ( ( _val ) << GRIOMMU_MSTCFG_DEVICE_SHIFT ) & \
      GRIOMMU_MSTCFG_DEVICE_MASK ) )
#define GRIOMMU_MSTCFG_DEVICE( _val ) \
  ( ( ( _val ) << GRIOMMU_MSTCFG_DEVICE_SHIFT ) & \
    GRIOMMU_MSTCFG_DEVICE_MASK )

#define GRIOMMU_MSTCFG_BS 0x10U

#define GRIOMMU_MSTCFG_GROUP_SHIFT 0
#define GRIOMMU_MSTCFG_GROUP_MASK 0xfU
#define GRIOMMU_MSTCFG_GROUP_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_MSTCFG_GROUP_MASK ) >> \
    GRIOMMU_MSTCFG_GROUP_SHIFT )
#define GRIOMMU_MSTCFG_GROUP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_MSTCFG_GROUP_MASK ) | \
    ( ( ( _val ) << GRIOMMU_MSTCFG_GROUP_SHIFT ) & \
      GRIOMMU_MSTCFG_GROUP_MASK ) )
#define GRIOMMU_MSTCFG_GROUP( _val ) \
  ( ( ( _val ) << GRIOMMU_MSTCFG_GROUP_SHIFT ) & \
    GRIOMMU_MSTCFG_GROUP_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUGRPCTRL Group control register 0 - 7 (GRPCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_GRPCTRL_BASE_31_4_SHIFT 4
#define GRIOMMU_GRPCTRL_BASE_31_4_MASK 0xfffffff0U
#define GRIOMMU_GRPCTRL_BASE_31_4_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_GRPCTRL_BASE_31_4_MASK ) >> \
    GRIOMMU_GRPCTRL_BASE_31_4_SHIFT )
#define GRIOMMU_GRPCTRL_BASE_31_4_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_GRPCTRL_BASE_31_4_MASK ) | \
    ( ( ( _val ) << GRIOMMU_GRPCTRL_BASE_31_4_SHIFT ) & \
      GRIOMMU_GRPCTRL_BASE_31_4_MASK ) )
#define GRIOMMU_GRPCTRL_BASE_31_4( _val ) \
  ( ( ( _val ) << GRIOMMU_GRPCTRL_BASE_31_4_SHIFT ) & \
    GRIOMMU_GRPCTRL_BASE_31_4_MASK )

#define GRIOMMU_GRPCTRL_P 0x2U

#define GRIOMMU_GRPCTRL_AG 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUDIAGCTRL \
 *   Diagnostic cache access register (DIAGCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_DIAGCTRL_DA 0x80000000U

#define GRIOMMU_DIAGCTRL_RW 0x40000000U

#define GRIOMMU_DIAGCTRL_DP 0x200000U

#define GRIOMMU_DIAGCTRL_TP 0x100000U

#define GRIOMMU_DIAGCTRL_SETADDR_SHIFT 0
#define GRIOMMU_DIAGCTRL_SETADDR_MASK 0x7ffffU
#define GRIOMMU_DIAGCTRL_SETADDR_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_DIAGCTRL_SETADDR_MASK ) >> \
    GRIOMMU_DIAGCTRL_SETADDR_SHIFT )
#define GRIOMMU_DIAGCTRL_SETADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_DIAGCTRL_SETADDR_MASK ) | \
    ( ( ( _val ) << GRIOMMU_DIAGCTRL_SETADDR_SHIFT ) & \
      GRIOMMU_DIAGCTRL_SETADDR_MASK ) )
#define GRIOMMU_DIAGCTRL_SETADDR( _val ) \
  ( ( ( _val ) << GRIOMMU_DIAGCTRL_SETADDR_SHIFT ) & \
    GRIOMMU_DIAGCTRL_SETADDR_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUDIAGD \
 *   Diagnostic cache access data register 0 - 7 (DIAGD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_DIAGD_CDATAN_SHIFT 0
#define GRIOMMU_DIAGD_CDATAN_MASK 0xffffffffU
#define GRIOMMU_DIAGD_CDATAN_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_DIAGD_CDATAN_MASK ) >> \
    GRIOMMU_DIAGD_CDATAN_SHIFT )
#define GRIOMMU_DIAGD_CDATAN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_DIAGD_CDATAN_MASK ) | \
    ( ( ( _val ) << GRIOMMU_DIAGD_CDATAN_SHIFT ) & \
      GRIOMMU_DIAGD_CDATAN_MASK ) )
#define GRIOMMU_DIAGD_CDATAN( _val ) \
  ( ( ( _val ) << GRIOMMU_DIAGD_CDATAN_SHIFT ) & \
    GRIOMMU_DIAGD_CDATAN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUDIAGT \
 *   Diagnostic cache access tag register (DIAGT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_DIAGT_TAG_SHIFT 1
#define GRIOMMU_DIAGT_TAG_MASK 0xfffffffeU
#define GRIOMMU_DIAGT_TAG_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_DIAGT_TAG_MASK ) >> \
    GRIOMMU_DIAGT_TAG_SHIFT )
#define GRIOMMU_DIAGT_TAG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_DIAGT_TAG_MASK ) | \
    ( ( ( _val ) << GRIOMMU_DIAGT_TAG_SHIFT ) & \
      GRIOMMU_DIAGT_TAG_MASK ) )
#define GRIOMMU_DIAGT_TAG( _val ) \
  ( ( ( _val ) << GRIOMMU_DIAGT_TAG_SHIFT ) & \
    GRIOMMU_DIAGT_TAG_MASK )

#define GRIOMMU_DIAGT_V 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUDERRI Data RAM error injection register (DERRI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_DERRI_DPERRINJ_SHIFT 0
#define GRIOMMU_DERRI_DPERRINJ_MASK 0xffffffffU
#define GRIOMMU_DERRI_DPERRINJ_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_DERRI_DPERRINJ_MASK ) >> \
    GRIOMMU_DERRI_DPERRINJ_SHIFT )
#define GRIOMMU_DERRI_DPERRINJ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_DERRI_DPERRINJ_MASK ) | \
    ( ( ( _val ) << GRIOMMU_DERRI_DPERRINJ_SHIFT ) & \
      GRIOMMU_DERRI_DPERRINJ_MASK ) )
#define GRIOMMU_DERRI_DPERRINJ( _val ) \
  ( ( ( _val ) << GRIOMMU_DERRI_DPERRINJ_SHIFT ) & \
    GRIOMMU_DERRI_DPERRINJ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUTERRI Tag RAM error injection register (TERRI)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_TERRI_TPERRINJ_SHIFT 0
#define GRIOMMU_TERRI_TPERRINJ_MASK 0xffffffffU
#define GRIOMMU_TERRI_TPERRINJ_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_TERRI_TPERRINJ_MASK ) >> \
    GRIOMMU_TERRI_TPERRINJ_SHIFT )
#define GRIOMMU_TERRI_TPERRINJ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_TERRI_TPERRINJ_MASK ) | \
    ( ( ( _val ) << GRIOMMU_TERRI_TPERRINJ_SHIFT ) & \
      GRIOMMU_TERRI_TPERRINJ_MASK ) )
#define GRIOMMU_TERRI_TPERRINJ( _val ) \
  ( ( ( _val ) << GRIOMMU_TERRI_TPERRINJ_SHIFT ) & \
    GRIOMMU_TERRI_TPERRINJ_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRIOMMUASMPCTRL \
 *   ASMP access control registers 0 - 3 (ASMPCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRIOMMU_ASMPCTRL_FC 0x40000U

#define GRIOMMU_ASMPCTRL_SC 0x20000U

#define GRIOMMU_ASMPCTRL_MC 0x10000U

#define GRIOMMU_ASMPCTRL_GRPACCSZCTRL_SHIFT 0
#define GRIOMMU_ASMPCTRL_GRPACCSZCTRL_MASK 0xffffU
#define GRIOMMU_ASMPCTRL_GRPACCSZCTRL_GET( _reg ) \
  ( ( ( _reg ) & GRIOMMU_ASMPCTRL_GRPACCSZCTRL_MASK ) >> \
    GRIOMMU_ASMPCTRL_GRPACCSZCTRL_SHIFT )
#define GRIOMMU_ASMPCTRL_GRPACCSZCTRL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRIOMMU_ASMPCTRL_GRPACCSZCTRL_MASK ) | \
    ( ( ( _val ) << GRIOMMU_ASMPCTRL_GRPACCSZCTRL_SHIFT ) & \
      GRIOMMU_ASMPCTRL_GRPACCSZCTRL_MASK ) )
#define GRIOMMU_ASMPCTRL_GRPACCSZCTRL( _val ) \
  ( ( ( _val ) << GRIOMMU_ASMPCTRL_GRPACCSZCTRL_SHIFT ) & \
    GRIOMMU_ASMPCTRL_GRPACCSZCTRL_MASK )

/** @} */

/**
 * @brief This structure defines the GRIOMMU register block memory map.
 */
typedef struct griommu {
  /**
   * @brief See @ref RTEMSDeviceGRIOMMUCAP0.
   */
  uint32_t cap0;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUCAP1.
   */
  uint32_t cap1;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUCAP2.
   */
  uint32_t cap2;

  uint32_t reserved_c_10;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUFLUSH.
   */
  uint32_t flush;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUSTATUS.
   */
  uint32_t status;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUIMASK.
   */
  uint32_t imask;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUAHBFAS.
   */
  uint32_t ahbfas;

  uint32_t reserved_24_40[ 7 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUMSTCFG.
   */
  uint32_t mstcfg_0;

  uint32_t reserved_44_64[ 8 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUMSTCFG.
   */
  uint32_t mstcfg_1;

  uint32_t reserved_68_80[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUGRPCTRL.
   */
  uint32_t grpctrl_0;

  uint32_t reserved_84_9c[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUGRPCTRL.
   */
  uint32_t grpctrl_1;

  uint32_t reserved_a0_c0[ 8 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUDIAGCTRL.
   */
  uint32_t diagctrl;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUDIAGD.
   */
  uint32_t diagd_0;

  uint32_t reserved_c8_e0[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUDIAGD.
   */
  uint32_t diagd_1;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUDIAGT.
   */
  uint32_t diagt;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUDERRI.
   */
  uint32_t derri;

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUTERRI.
   */
  uint32_t terri;

  uint32_t reserved_f0_100[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUASMPCTRL.
   */
  uint32_t asmpctrl_0;

  uint32_t reserved_104_10c[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRIOMMUASMPCTRL.
   */
  uint32_t asmpctrl_1;
} griommu;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRIOMMU_REGS_H */
