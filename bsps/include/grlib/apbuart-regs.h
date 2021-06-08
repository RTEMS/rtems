/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief This header file defines the APBUART register block interface.
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

/* Generated from spec:/dev/grlib/if/apbuart-header */

#ifndef _GRLIB_APBUART_REGS_H
#define _GRLIB_APBUART_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/apbuart */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUART APBUART
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the APBUART interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUARTDATA UART data register (DATA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define APBUART_DATA_DATA_SHIFT 0
#define APBUART_DATA_DATA_MASK 0xffU
#define APBUART_DATA_DATA_GET( _reg ) \
  ( ( ( _reg ) & APBUART_DATA_DATA_MASK ) >> \
    APBUART_DATA_DATA_SHIFT )
#define APBUART_DATA_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~APBUART_DATA_DATA_MASK ) | \
    ( ( ( _val ) << APBUART_DATA_DATA_SHIFT ) & \
      APBUART_DATA_DATA_MASK ) )
#define APBUART_DATA_DATA( _val ) \
  ( ( ( _val ) << APBUART_DATA_DATA_SHIFT ) & \
    APBUART_DATA_DATA_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUARTSTATUS UART status register (STATUS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define APBUART_STATUS_RCNT_SHIFT 26
#define APBUART_STATUS_RCNT_MASK 0xfc000000U
#define APBUART_STATUS_RCNT_GET( _reg ) \
  ( ( ( _reg ) & APBUART_STATUS_RCNT_MASK ) >> \
    APBUART_STATUS_RCNT_SHIFT )
#define APBUART_STATUS_RCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~APBUART_STATUS_RCNT_MASK ) | \
    ( ( ( _val ) << APBUART_STATUS_RCNT_SHIFT ) & \
      APBUART_STATUS_RCNT_MASK ) )
#define APBUART_STATUS_RCNT( _val ) \
  ( ( ( _val ) << APBUART_STATUS_RCNT_SHIFT ) & \
    APBUART_STATUS_RCNT_MASK )

#define APBUART_STATUS_TCNT_SHIFT 20
#define APBUART_STATUS_TCNT_MASK 0x3f00000U
#define APBUART_STATUS_TCNT_GET( _reg ) \
  ( ( ( _reg ) & APBUART_STATUS_TCNT_MASK ) >> \
    APBUART_STATUS_TCNT_SHIFT )
#define APBUART_STATUS_TCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~APBUART_STATUS_TCNT_MASK ) | \
    ( ( ( _val ) << APBUART_STATUS_TCNT_SHIFT ) & \
      APBUART_STATUS_TCNT_MASK ) )
#define APBUART_STATUS_TCNT( _val ) \
  ( ( ( _val ) << APBUART_STATUS_TCNT_SHIFT ) & \
    APBUART_STATUS_TCNT_MASK )

#define APBUART_STATUS_RF 0x400U

#define APBUART_STATUS_TF 0x200U

#define APBUART_STATUS_RH 0x100U

#define APBUART_STATUS_TH 0x80U

#define APBUART_STATUS_FE 0x40U

#define APBUART_STATUS_PE 0x20U

#define APBUART_STATUS_OV 0x10U

#define APBUART_STATUS_BR 0x8U

#define APBUART_STATUS_TE 0x4U

#define APBUART_STATUS_TS 0x2U

#define APBUART_STATUS_DR 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUARTCTRL UART control register (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define APBUART_CTRL_FA 0x80000000U

#define APBUART_CTRL_SI 0x4000U

#define APBUART_CTRL_DI 0x2000U

#define APBUART_CTRL_BI 0x1000U

#define APBUART_CTRL_DB 0x800U

#define APBUART_CTRL_RF 0x400U

#define APBUART_CTRL_TF 0x200U

#define APBUART_CTRL_EC 0x100U

#define APBUART_CTRL_LB 0x80U

#define APBUART_CTRL_FL 0x40U

#define APBUART_CTRL_PE 0x20U

#define APBUART_CTRL_PS 0x10U

#define APBUART_CTRL_TI 0x8U

#define APBUART_CTRL_RI 0x4U

#define APBUART_CTRL_TE 0x2U

#define APBUART_CTRL_RE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUARTSCALER UART scaler reload register (SCALER)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define APBUART_SCALER_SCALER_RELOAD_VALUE_SHIFT 0
#define APBUART_SCALER_SCALER_RELOAD_VALUE_MASK 0xfffffU
#define APBUART_SCALER_SCALER_RELOAD_VALUE_GET( _reg ) \
  ( ( ( _reg ) & APBUART_SCALER_SCALER_RELOAD_VALUE_MASK ) >> \
    APBUART_SCALER_SCALER_RELOAD_VALUE_SHIFT )
#define APBUART_SCALER_SCALER_RELOAD_VALUE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~APBUART_SCALER_SCALER_RELOAD_VALUE_MASK ) | \
    ( ( ( _val ) << APBUART_SCALER_SCALER_RELOAD_VALUE_SHIFT ) & \
      APBUART_SCALER_SCALER_RELOAD_VALUE_MASK ) )
#define APBUART_SCALER_SCALER_RELOAD_VALUE( _val ) \
  ( ( ( _val ) << APBUART_SCALER_SCALER_RELOAD_VALUE_SHIFT ) & \
    APBUART_SCALER_SCALER_RELOAD_VALUE_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBAPBUARTFIFO UART FIFO debug register (FIFO)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define APBUART_FIFO_DATA_SHIFT 0
#define APBUART_FIFO_DATA_MASK 0xffU
#define APBUART_FIFO_DATA_GET( _reg ) \
  ( ( ( _reg ) & APBUART_FIFO_DATA_MASK ) >> \
    APBUART_FIFO_DATA_SHIFT )
#define APBUART_FIFO_DATA_SET( _reg, _val ) \
  ( ( ( _reg ) & ~APBUART_FIFO_DATA_MASK ) | \
    ( ( ( _val ) << APBUART_FIFO_DATA_SHIFT ) & \
      APBUART_FIFO_DATA_MASK ) )
#define APBUART_FIFO_DATA( _val ) \
  ( ( ( _val ) << APBUART_FIFO_DATA_SHIFT ) & \
    APBUART_FIFO_DATA_MASK )

/** @} */

/**
 * @brief This structure defines the APBUART register block memory map.
 */
typedef struct apbuart {
  /**
   * @brief See @ref RTEMSDeviceGRLIBAPBUARTDATA.
   */
  uint32_t data;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAPBUARTSTATUS.
   */
  uint32_t status;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAPBUARTCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAPBUARTSCALER.
   */
  uint32_t scaler;

  /**
   * @brief See @ref RTEMSDeviceGRLIBAPBUARTFIFO.
   */
  uint32_t fifo;
} apbuart;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_APBUART_REGS_H */
