/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRETHGBIT
 *
 * @brief This header file defines the GRETH_GBIT register block interface.
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

/* Generated from spec:/dev/grlib/if/grethgbit-header */

#ifndef _GRLIB_GRETHGBIT_REGS_H
#define _GRLIB_GRETHGBIT_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grethgbit */

/**
 * @defgroup RTEMSDeviceGRETHGBIT GRETH_GBIT
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITCR control register (CR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_CR_EA 0x80000000U

#define GRETHGBIT_CR_BS_SHIFT 28
#define GRETHGBIT_CR_BS_MASK 0x70000000U
#define GRETHGBIT_CR_BS_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_CR_BS_MASK ) >> \
    GRETHGBIT_CR_BS_SHIFT )
#define GRETHGBIT_CR_BS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_CR_BS_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_CR_BS_SHIFT ) & \
      GRETHGBIT_CR_BS_MASK ) )
#define GRETHGBIT_CR_BS( _val ) \
  ( ( ( _val ) << GRETHGBIT_CR_BS_SHIFT ) & \
    GRETHGBIT_CR_BS_MASK )

#define GRETHGBIT_CR_GA 0x8000000U

#define GRETHGBIT_CR_MA 0x4000000U

#define GRETHGBIT_CR_MC 0x2000000U

#define GRETHGBIT_CR_ED 0x4000U

#define GRETHGBIT_CR_RD 0x2000U

#define GRETHGBIT_CR_DD 0x1000U

#define GRETHGBIT_CR_ME 0x800U

#define GRETHGBIT_CR_PI 0x400U

#define GRETHGBIT_CR_BM 0x200U

#define GRETHGBIT_CR_GB 0x100U

#define GRETHGBIT_CR_SP 0x80U

#define GRETHGBIT_CR_RS 0x40U

#define GRETHGBIT_CR_PM 0x20U

#define GRETHGBIT_CR_FD 0x10U

#define GRETHGBIT_CR_RI 0x8U

#define GRETHGBIT_CR_TI 0x4U

#define GRETHGBIT_CR_RE 0x2U

#define GRETHGBIT_CR_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITSR status register. (SR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_SR_PS 0x100U

#define GRETHGBIT_SR_IA 0x80U

#define GRETHGBIT_SR_TS 0x40U

#define GRETHGBIT_SR_TA 0x20U

#define GRETHGBIT_SR_RA 0x10U

#define GRETHGBIT_SR_TI 0x8U

#define GRETHGBIT_SR_RI 0x4U

#define GRETHGBIT_SR_TE 0x2U

#define GRETHGBIT_SR_RE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMACMSB MAC address MSB. (MACMSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MACMSB_MSB_SHIFT 0
#define GRETHGBIT_MACMSB_MSB_MASK 0xffffU
#define GRETHGBIT_MACMSB_MSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MACMSB_MSB_MASK ) >> \
    GRETHGBIT_MACMSB_MSB_SHIFT )
#define GRETHGBIT_MACMSB_MSB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_MACMSB_MSB_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_MACMSB_MSB_SHIFT ) & \
      GRETHGBIT_MACMSB_MSB_MASK ) )
#define GRETHGBIT_MACMSB_MSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_MACMSB_MSB_SHIFT ) & \
    GRETHGBIT_MACMSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMACLSB MAC address LSB. (MACLSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MACLSB_LSB_SHIFT 0
#define GRETHGBIT_MACLSB_LSB_MASK 0xffffffffU
#define GRETHGBIT_MACLSB_LSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MACLSB_LSB_MASK ) >> \
    GRETHGBIT_MACLSB_LSB_SHIFT )
#define GRETHGBIT_MACLSB_LSB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_MACLSB_LSB_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_MACLSB_LSB_SHIFT ) & \
      GRETHGBIT_MACLSB_LSB_MASK ) )
#define GRETHGBIT_MACLSB_LSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_MACLSB_LSB_SHIFT ) & \
    GRETHGBIT_MACLSB_LSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMDIO MDIO control/status register. (MDIO)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MDIO_DATA_SHIFT 16
#define GRETHGBIT_MDIO_DATA_MASK 0xffff0000U
#define GRETHGBIT_MDIO_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MDIO_DATA_MASK ) >> \
    GRETHGBIT_MDIO_DATA_SHIFT )
#define GRETHGBIT_MDIO_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_DATA_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_MDIO_DATA_SHIFT ) & \
      GRETHGBIT_MDIO_DATA_MASK ) )
#define GRETHGBIT_MDIO_DATA( _val ) \
  ( ( ( _val ) << GRETHGBIT_MDIO_DATA_SHIFT ) & \
    GRETHGBIT_MDIO_DATA_MASK )

#define GRETHGBIT_MDIO_PHYADDR_SHIFT 11
#define GRETHGBIT_MDIO_PHYADDR_MASK 0xf800U
#define GRETHGBIT_MDIO_PHYADDR_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MDIO_PHYADDR_MASK ) >> \
    GRETHGBIT_MDIO_PHYADDR_SHIFT )
#define GRETHGBIT_MDIO_PHYADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_PHYADDR_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_MDIO_PHYADDR_SHIFT ) & \
      GRETHGBIT_MDIO_PHYADDR_MASK ) )
#define GRETHGBIT_MDIO_PHYADDR( _val ) \
  ( ( ( _val ) << GRETHGBIT_MDIO_PHYADDR_SHIFT ) & \
    GRETHGBIT_MDIO_PHYADDR_MASK )

#define GRETHGBIT_MDIO_REGADDR_SHIFT 6
#define GRETHGBIT_MDIO_REGADDR_MASK 0x7c0U
#define GRETHGBIT_MDIO_REGADDR_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MDIO_REGADDR_MASK ) >> \
    GRETHGBIT_MDIO_REGADDR_SHIFT )
#define GRETHGBIT_MDIO_REGADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_REGADDR_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_MDIO_REGADDR_SHIFT ) & \
      GRETHGBIT_MDIO_REGADDR_MASK ) )
#define GRETHGBIT_MDIO_REGADDR( _val ) \
  ( ( ( _val ) << GRETHGBIT_MDIO_REGADDR_SHIFT ) & \
    GRETHGBIT_MDIO_REGADDR_MASK )

#define GRETHGBIT_MDIO_BU 0x8U

#define GRETHGBIT_MDIO_LF 0x4U

#define GRETHGBIT_MDIO_RD 0x2U

#define GRETHGBIT_MDIO_WR 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTDTBA \
 *   transmitter descriptor table base address register. (TDTBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_TDTBA_BASEADDR_MASK 0xfffffc00U
#define GRETHGBIT_TDTBA_BASEADDR_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_TDTBA_BASEADDR_MASK ) >> \
    GRETHGBIT_TDTBA_BASEADDR_SHIFT )
#define GRETHGBIT_TDTBA_BASEADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_TDTBA_BASEADDR_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_TDTBA_BASEADDR_SHIFT ) & \
      GRETHGBIT_TDTBA_BASEADDR_MASK ) )
#define GRETHGBIT_TDTBA_BASEADDR( _val ) \
  ( ( ( _val ) << GRETHGBIT_TDTBA_BASEADDR_SHIFT ) & \
    GRETHGBIT_TDTBA_BASEADDR_MASK )

#define GRETHGBIT_TDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_TDTBA_DESCPNT_MASK 0x3f8U
#define GRETHGBIT_TDTBA_DESCPNT_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_TDTBA_DESCPNT_MASK ) >> \
    GRETHGBIT_TDTBA_DESCPNT_SHIFT )
#define GRETHGBIT_TDTBA_DESCPNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_TDTBA_DESCPNT_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_TDTBA_DESCPNT_SHIFT ) & \
      GRETHGBIT_TDTBA_DESCPNT_MASK ) )
#define GRETHGBIT_TDTBA_DESCPNT( _val ) \
  ( ( ( _val ) << GRETHGBIT_TDTBA_DESCPNT_SHIFT ) & \
    GRETHGBIT_TDTBA_DESCPNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITRDTBA \
 *   receiver descriptor table base address register. (RDTBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_RDTBA_BASEADDR_MASK 0xfffffc00U
#define GRETHGBIT_RDTBA_BASEADDR_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_RDTBA_BASEADDR_MASK ) >> \
    GRETHGBIT_RDTBA_BASEADDR_SHIFT )
#define GRETHGBIT_RDTBA_BASEADDR_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_RDTBA_BASEADDR_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_RDTBA_BASEADDR_SHIFT ) & \
      GRETHGBIT_RDTBA_BASEADDR_MASK ) )
#define GRETHGBIT_RDTBA_BASEADDR( _val ) \
  ( ( ( _val ) << GRETHGBIT_RDTBA_BASEADDR_SHIFT ) & \
    GRETHGBIT_RDTBA_BASEADDR_MASK )

#define GRETHGBIT_RDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_RDTBA_DESCPNT_MASK 0x3f8U
#define GRETHGBIT_RDTBA_DESCPNT_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_RDTBA_DESCPNT_MASK ) >> \
    GRETHGBIT_RDTBA_DESCPNT_SHIFT )
#define GRETHGBIT_RDTBA_DESCPNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_RDTBA_DESCPNT_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_RDTBA_DESCPNT_SHIFT ) & \
      GRETHGBIT_RDTBA_DESCPNT_MASK ) )
#define GRETHGBIT_RDTBA_DESCPNT( _val ) \
  ( ( ( _val ) << GRETHGBIT_RDTBA_DESCPNT_SHIFT ) & \
    GRETHGBIT_RDTBA_DESCPNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACMSB EDCL MAC address MSB. (EDCLMACMSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACMSB_MSB_SHIFT 0
#define GRETHGBIT_EDCLMACMSB_MSB_MASK 0xffffU
#define GRETHGBIT_EDCLMACMSB_MSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_EDCLMACMSB_MSB_MASK ) >> \
    GRETHGBIT_EDCLMACMSB_MSB_SHIFT )
#define GRETHGBIT_EDCLMACMSB_MSB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLMACMSB_MSB_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_EDCLMACMSB_MSB_SHIFT ) & \
      GRETHGBIT_EDCLMACMSB_MSB_MASK ) )
#define GRETHGBIT_EDCLMACMSB_MSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_EDCLMACMSB_MSB_SHIFT ) & \
    GRETHGBIT_EDCLMACMSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACLSB EDCL MAC address LSB. (EDCLMACLSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACLSB_LSB_SHIFT 0
#define GRETHGBIT_EDCLMACLSB_LSB_MASK 0xffffffffU
#define GRETHGBIT_EDCLMACLSB_LSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_EDCLMACLSB_LSB_MASK ) >> \
    GRETHGBIT_EDCLMACLSB_LSB_SHIFT )
#define GRETHGBIT_EDCLMACLSB_LSB_SET( _reg, _val ) \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLMACLSB_LSB_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_EDCLMACLSB_LSB_SHIFT ) & \
      GRETHGBIT_EDCLMACLSB_LSB_MASK ) )
#define GRETHGBIT_EDCLMACLSB_LSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_EDCLMACLSB_LSB_SHIFT ) & \
    GRETHGBIT_EDCLMACLSB_LSB_MASK )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT register block memory map.
 */
typedef struct grethgbit {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITCR.
   */
  uint32_t cr;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITSR.
   */
  uint32_t sr;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMACMSB.
   */
  uint32_t macmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMACLSB.
   */
  uint32_t maclsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMDIO.
   */
  uint32_t mdio;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTDTBA.
   */
  uint32_t tdtba;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITRDTBA.
   */
  uint32_t rdtba;

  uint32_t reserved_1c_28[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACMSB.
   */
  uint32_t edclmacmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACLSB.
   */
  uint32_t edclmaclsb;
} grethgbit;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRETHGBIT_REGS_H */
