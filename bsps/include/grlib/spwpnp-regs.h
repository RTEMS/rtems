/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBSPWPNP
 *
 * @brief This header file defines the SPWPNP register block interface.
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

/* Generated from spec:/dev/grlib/if/spwpnp-header */

#ifndef _GRLIB_SPWPNP_REGS_H
#define _GRLIB_SPWPNP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/spwpnp */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNP SpaceWire Plug-and-Play
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the SpaceWire Plug-and-Play interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPVEND \
 *   SpaceWire Plug-and-Play - Device Vendor and Product ID (PNPVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPVEND_VEND_SHIFT 16
#define SPWPNP_PNPVEND_VEND_MASK 0xffff0000U
#define SPWPNP_PNPVEND_VEND_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVEND_VEND_MASK ) >> \
    SPWPNP_PNPVEND_VEND_SHIFT )
#define SPWPNP_PNPVEND_VEND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVEND_VEND_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVEND_VEND_SHIFT ) & \
      SPWPNP_PNPVEND_VEND_MASK ) )
#define SPWPNP_PNPVEND_VEND( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVEND_VEND_SHIFT ) & \
    SPWPNP_PNPVEND_VEND_MASK )

#define SPWPNP_PNPVEND_PROD_SHIFT 0
#define SPWPNP_PNPVEND_PROD_MASK 0xffffU
#define SPWPNP_PNPVEND_PROD_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVEND_PROD_MASK ) >> \
    SPWPNP_PNPVEND_PROD_SHIFT )
#define SPWPNP_PNPVEND_PROD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVEND_PROD_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVEND_PROD_SHIFT ) & \
      SPWPNP_PNPVEND_PROD_MASK ) )
#define SPWPNP_PNPVEND_PROD( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVEND_PROD_SHIFT ) & \
    SPWPNP_PNPVEND_PROD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPVER \
 *   SpaceWire Plug-and-Play - Version (PNPVER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPVER_MAJOR_SHIFT 24
#define SPWPNP_PNPVER_MAJOR_MASK 0xff000000U
#define SPWPNP_PNPVER_MAJOR_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVER_MAJOR_MASK ) >> \
    SPWPNP_PNPVER_MAJOR_SHIFT )
#define SPWPNP_PNPVER_MAJOR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVER_MAJOR_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVER_MAJOR_SHIFT ) & \
      SPWPNP_PNPVER_MAJOR_MASK ) )
#define SPWPNP_PNPVER_MAJOR( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVER_MAJOR_SHIFT ) & \
    SPWPNP_PNPVER_MAJOR_MASK )

#define SPWPNP_PNPVER_MINOR_SHIFT 16
#define SPWPNP_PNPVER_MINOR_MASK 0xff0000U
#define SPWPNP_PNPVER_MINOR_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVER_MINOR_MASK ) >> \
    SPWPNP_PNPVER_MINOR_SHIFT )
#define SPWPNP_PNPVER_MINOR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVER_MINOR_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVER_MINOR_SHIFT ) & \
      SPWPNP_PNPVER_MINOR_MASK ) )
#define SPWPNP_PNPVER_MINOR( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVER_MINOR_SHIFT ) & \
    SPWPNP_PNPVER_MINOR_MASK )

#define SPWPNP_PNPVER_PATCH_SHIFT 8
#define SPWPNP_PNPVER_PATCH_MASK 0xff00U
#define SPWPNP_PNPVER_PATCH_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVER_PATCH_MASK ) >> \
    SPWPNP_PNPVER_PATCH_SHIFT )
#define SPWPNP_PNPVER_PATCH_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVER_PATCH_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVER_PATCH_SHIFT ) & \
      SPWPNP_PNPVER_PATCH_MASK ) )
#define SPWPNP_PNPVER_PATCH( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVER_PATCH_SHIFT ) & \
    SPWPNP_PNPVER_PATCH_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPDEVSTS \
 *   SpaceWire Plug-and-Play - Device Status (PNPDEVSTS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPDEVSTS_STATUS_SHIFT 0
#define SPWPNP_PNPDEVSTS_STATUS_MASK 0xffU
#define SPWPNP_PNPDEVSTS_STATUS_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPDEVSTS_STATUS_MASK ) >> \
    SPWPNP_PNPDEVSTS_STATUS_SHIFT )
#define SPWPNP_PNPDEVSTS_STATUS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPDEVSTS_STATUS_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPDEVSTS_STATUS_SHIFT ) & \
      SPWPNP_PNPDEVSTS_STATUS_MASK ) )
#define SPWPNP_PNPDEVSTS_STATUS( _val ) \
  ( ( ( _val ) << SPWPNP_PNPDEVSTS_STATUS_SHIFT ) & \
    SPWPNP_PNPDEVSTS_STATUS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPACTLNK \
 *   SpaceWire Plug-and-Play - Active Links (PNPACTLNK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPACTLNK_ACTIVE_SHIFT 1
#define SPWPNP_PNPACTLNK_ACTIVE_MASK 0xfffffffeU
#define SPWPNP_PNPACTLNK_ACTIVE_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPACTLNK_ACTIVE_MASK ) >> \
    SPWPNP_PNPACTLNK_ACTIVE_SHIFT )
#define SPWPNP_PNPACTLNK_ACTIVE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPACTLNK_ACTIVE_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPACTLNK_ACTIVE_SHIFT ) & \
      SPWPNP_PNPACTLNK_ACTIVE_MASK ) )
#define SPWPNP_PNPACTLNK_ACTIVE( _val ) \
  ( ( ( _val ) << SPWPNP_PNPACTLNK_ACTIVE_SHIFT ) & \
    SPWPNP_PNPACTLNK_ACTIVE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPOA0 \
 *   SpaceWire Plug-and-Play - Owner Address 0 (PNPOA0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPOA0_RA_SHIFT 0
#define SPWPNP_PNPOA0_RA_MASK 0xffffffffU
#define SPWPNP_PNPOA0_RA_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPOA0_RA_MASK ) >> \
    SPWPNP_PNPOA0_RA_SHIFT )
#define SPWPNP_PNPOA0_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPOA0_RA_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPOA0_RA_SHIFT ) & \
      SPWPNP_PNPOA0_RA_MASK ) )
#define SPWPNP_PNPOA0_RA( _val ) \
  ( ( ( _val ) << SPWPNP_PNPOA0_RA_SHIFT ) & \
    SPWPNP_PNPOA0_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPOA1 \
 *   SpaceWire Plug-and-Play - Owner Address 1 (PNPOA1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPOA1_RA_SHIFT 0
#define SPWPNP_PNPOA1_RA_MASK 0xffffffffU
#define SPWPNP_PNPOA1_RA_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPOA1_RA_MASK ) >> \
    SPWPNP_PNPOA1_RA_SHIFT )
#define SPWPNP_PNPOA1_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPOA1_RA_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPOA1_RA_SHIFT ) & \
      SPWPNP_PNPOA1_RA_MASK ) )
#define SPWPNP_PNPOA1_RA( _val ) \
  ( ( ( _val ) << SPWPNP_PNPOA1_RA_SHIFT ) & \
    SPWPNP_PNPOA1_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPOA2 \
 *   SpaceWire Plug-and-Play - Owner Address 2 (PNPOA2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPOA2_RA_SHIFT 0
#define SPWPNP_PNPOA2_RA_MASK 0xffffffffU
#define SPWPNP_PNPOA2_RA_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPOA2_RA_MASK ) >> \
    SPWPNP_PNPOA2_RA_SHIFT )
#define SPWPNP_PNPOA2_RA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPOA2_RA_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPOA2_RA_SHIFT ) & \
      SPWPNP_PNPOA2_RA_MASK ) )
#define SPWPNP_PNPOA2_RA( _val ) \
  ( ( ( _val ) << SPWPNP_PNPOA2_RA_SHIFT ) & \
    SPWPNP_PNPOA2_RA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPDEVID \
 *   SpaceWire Plug-and-Play - Device ID (PNPDEVID)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPDEVID_DID_SHIFT 0
#define SPWPNP_PNPDEVID_DID_MASK 0xffffffffU
#define SPWPNP_PNPDEVID_DID_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPDEVID_DID_MASK ) >> \
    SPWPNP_PNPDEVID_DID_SHIFT )
#define SPWPNP_PNPDEVID_DID_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPDEVID_DID_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPDEVID_DID_SHIFT ) & \
      SPWPNP_PNPDEVID_DID_MASK ) )
#define SPWPNP_PNPDEVID_DID( _val ) \
  ( ( ( _val ) << SPWPNP_PNPDEVID_DID_SHIFT ) & \
    SPWPNP_PNPDEVID_DID_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPUVEND \
 *   SpaceWire Plug-and-Play - Unit Vendor and Product ID (PNPUVEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPUVEND_VEND_SHIFT 16
#define SPWPNP_PNPUVEND_VEND_MASK 0xffff0000U
#define SPWPNP_PNPUVEND_VEND_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPUVEND_VEND_MASK ) >> \
    SPWPNP_PNPUVEND_VEND_SHIFT )
#define SPWPNP_PNPUVEND_VEND_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPUVEND_VEND_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPUVEND_VEND_SHIFT ) & \
      SPWPNP_PNPUVEND_VEND_MASK ) )
#define SPWPNP_PNPUVEND_VEND( _val ) \
  ( ( ( _val ) << SPWPNP_PNPUVEND_VEND_SHIFT ) & \
    SPWPNP_PNPUVEND_VEND_MASK )

#define SPWPNP_PNPUVEND_PROD_SHIFT 0
#define SPWPNP_PNPUVEND_PROD_MASK 0xffffU
#define SPWPNP_PNPUVEND_PROD_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPUVEND_PROD_MASK ) >> \
    SPWPNP_PNPUVEND_PROD_SHIFT )
#define SPWPNP_PNPUVEND_PROD_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPUVEND_PROD_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPUVEND_PROD_SHIFT ) & \
      SPWPNP_PNPUVEND_PROD_MASK ) )
#define SPWPNP_PNPUVEND_PROD( _val ) \
  ( ( ( _val ) << SPWPNP_PNPUVEND_PROD_SHIFT ) & \
    SPWPNP_PNPUVEND_PROD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPUSN \
 *   SpaceWire Plug-and-Play - Unit Serial Number (PNPUSN)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPUSN_USN_SHIFT 0
#define SPWPNP_PNPUSN_USN_MASK 0xffffffffU
#define SPWPNP_PNPUSN_USN_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPUSN_USN_MASK ) >> \
    SPWPNP_PNPUSN_USN_SHIFT )
#define SPWPNP_PNPUSN_USN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPUSN_USN_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPUSN_USN_SHIFT ) & \
      SPWPNP_PNPUSN_USN_MASK ) )
#define SPWPNP_PNPUSN_USN( _val ) \
  ( ( ( _val ) << SPWPNP_PNPUSN_USN_SHIFT ) & \
    SPWPNP_PNPUSN_USN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPVSTRL \
 *   SpaceWire Plug-and-Play - Vendor String Length (PNPVSTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPVSTRL_LEN_SHIFT 0
#define SPWPNP_PNPVSTRL_LEN_MASK 0x7fffU
#define SPWPNP_PNPVSTRL_LEN_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPVSTRL_LEN_MASK ) >> \
    SPWPNP_PNPVSTRL_LEN_SHIFT )
#define SPWPNP_PNPVSTRL_LEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPVSTRL_LEN_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPVSTRL_LEN_SHIFT ) & \
      SPWPNP_PNPVSTRL_LEN_MASK ) )
#define SPWPNP_PNPVSTRL_LEN( _val ) \
  ( ( ( _val ) << SPWPNP_PNPVSTRL_LEN_SHIFT ) & \
    SPWPNP_PNPVSTRL_LEN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPPSTRL \
 *   SpaceWire Plug-and-Play - Product String Length (PNPPSTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPPSTRL_LEN_SHIFT 0
#define SPWPNP_PNPPSTRL_LEN_MASK 0x7fffU
#define SPWPNP_PNPPSTRL_LEN_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPPSTRL_LEN_MASK ) >> \
    SPWPNP_PNPPSTRL_LEN_SHIFT )
#define SPWPNP_PNPPSTRL_LEN_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPPSTRL_LEN_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPPSTRL_LEN_SHIFT ) & \
      SPWPNP_PNPPSTRL_LEN_MASK ) )
#define SPWPNP_PNPPSTRL_LEN( _val ) \
  ( ( ( _val ) << SPWPNP_PNPPSTRL_LEN_SHIFT ) & \
    SPWPNP_PNPPSTRL_LEN_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPPCNT \
 *   SpaceWire Plug-and-Play - Protocol Count (PNPPCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPPCNT_PC_SHIFT 0
#define SPWPNP_PNPPCNT_PC_MASK 0x1fU
#define SPWPNP_PNPPCNT_PC_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPPCNT_PC_MASK ) >> \
    SPWPNP_PNPPCNT_PC_SHIFT )
#define SPWPNP_PNPPCNT_PC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPPCNT_PC_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPPCNT_PC_SHIFT ) & \
      SPWPNP_PNPPCNT_PC_MASK ) )
#define SPWPNP_PNPPCNT_PC( _val ) \
  ( ( ( _val ) << SPWPNP_PNPPCNT_PC_SHIFT ) & \
    SPWPNP_PNPPCNT_PC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWPNPPNPACNT \
 *   SpaceWire Plug-and-Play - Application Count (PNPACNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWPNP_PNPACNT_AC_SHIFT 0
#define SPWPNP_PNPACNT_AC_MASK 0xffU
#define SPWPNP_PNPACNT_AC_GET( _reg ) \
  ( ( ( _reg ) & SPWPNP_PNPACNT_AC_MASK ) >> \
    SPWPNP_PNPACNT_AC_SHIFT )
#define SPWPNP_PNPACNT_AC_SET( _reg, _val ) \
  ( ( ( _reg ) & ~SPWPNP_PNPACNT_AC_MASK ) | \
    ( ( ( _val ) << SPWPNP_PNPACNT_AC_SHIFT ) & \
      SPWPNP_PNPACNT_AC_MASK ) )
#define SPWPNP_PNPACNT_AC( _val ) \
  ( ( ( _val ) << SPWPNP_PNPACNT_AC_SHIFT ) & \
    SPWPNP_PNPACNT_AC_MASK )

/** @} */

/**
 * @brief This set of defines the SpaceWire Plug-and-Play address map.
 */
typedef struct spwpnp {
  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPVEND.
   */
  uint32_t pnpvend;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPVER.
   */
  uint32_t pnpver;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPDEVSTS.
   */
  uint32_t pnpdevsts;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPACTLNK.
   */
  uint32_t pnpactlnk;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPOA0.
   */
  uint32_t pnpoa0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPOA1.
   */
  uint32_t pnpoa1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPOA2.
   */
  uint32_t pnpoa2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPDEVID.
   */
  uint32_t pnpdevid;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPUVEND.
   */
  uint32_t pnpuvend;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPUSN.
   */
  uint32_t pnpusn;

  uint16_t reserved_e_4000[ 8185 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPVSTRL.
   */
  uint32_t pnpvstrl;

  uint32_t reserved_4004_6000[ 2047 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPPSTRL.
   */
  uint32_t pnppstrl;

  uint32_t reserved_6004_8000[ 2047 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPPCNT.
   */
  uint32_t pnppcnt;

  uint32_t reserved_8004_c000[ 4095 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWPNPPNPACNT.
   */
  uint32_t pnpacnt;
} spwpnp;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_SPWPNP_REGS_H */
