/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBIRQAMP
 *
 * @brief This header file defines the IRQAMP register block interface.
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

/* Generated from spec:/dev/grlib/if/irqamp-header */

#ifndef _GRLIB_IRQAMP_REGS_H
#define _GRLIB_IRQAMP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/irqamp-timestamp */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPTimestamp IRQ(A)MP Timestamp
 *
 * @ingroup RTEMSDeviceGRLIBIRQAMP
 *
 * @brief This group contains the IRQ(A)MP Timestamp interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPTimestampITCNT \
 *   Interrupt timestamp counter n register (ITCNT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ITCNT_TCNT_SHIFT 0
#define IRQAMP_ITCNT_TCNT_MASK 0xffffffffU
#define IRQAMP_ITCNT_TCNT_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ITCNT_TCNT_MASK ) >> \
    IRQAMP_ITCNT_TCNT_SHIFT )
#define IRQAMP_ITCNT_TCNT_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ITCNT_TCNT_MASK ) | \
    ( ( ( _val ) << IRQAMP_ITCNT_TCNT_SHIFT ) & \
      IRQAMP_ITCNT_TCNT_MASK ) )
#define IRQAMP_ITCNT_TCNT( _val ) \
  ( ( ( _val ) << IRQAMP_ITCNT_TCNT_SHIFT ) & \
    IRQAMP_ITCNT_TCNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPTimestampITSTMPC \
 *   Interrupt timestamp n control register (ITSTMPC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ITSTMPC_TSTAMP_SHIFT 27
#define IRQAMP_ITSTMPC_TSTAMP_MASK 0xf8000000U
#define IRQAMP_ITSTMPC_TSTAMP_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ITSTMPC_TSTAMP_MASK ) >> \
    IRQAMP_ITSTMPC_TSTAMP_SHIFT )
#define IRQAMP_ITSTMPC_TSTAMP_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ITSTMPC_TSTAMP_MASK ) | \
    ( ( ( _val ) << IRQAMP_ITSTMPC_TSTAMP_SHIFT ) & \
      IRQAMP_ITSTMPC_TSTAMP_MASK ) )
#define IRQAMP_ITSTMPC_TSTAMP( _val ) \
  ( ( ( _val ) << IRQAMP_ITSTMPC_TSTAMP_SHIFT ) & \
    IRQAMP_ITSTMPC_TSTAMP_MASK )

#define IRQAMP_ITSTMPC_S1 0x4000000U

#define IRQAMP_ITSTMPC_S2 0x2000000U

#define IRQAMP_ITSTMPC_KS 0x20U

#define IRQAMP_ITSTMPC_TSISEL_SHIFT 0
#define IRQAMP_ITSTMPC_TSISEL_MASK 0x1fU
#define IRQAMP_ITSTMPC_TSISEL_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ITSTMPC_TSISEL_MASK ) >> \
    IRQAMP_ITSTMPC_TSISEL_SHIFT )
#define IRQAMP_ITSTMPC_TSISEL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ITSTMPC_TSISEL_MASK ) | \
    ( ( ( _val ) << IRQAMP_ITSTMPC_TSISEL_SHIFT ) & \
      IRQAMP_ITSTMPC_TSISEL_MASK ) )
#define IRQAMP_ITSTMPC_TSISEL( _val ) \
  ( ( ( _val ) << IRQAMP_ITSTMPC_TSISEL_SHIFT ) & \
    IRQAMP_ITSTMPC_TSISEL_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPTimestampITSTMPAS \
 *   Interrupt Assertion Timestamp n register (ITSTMPAS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ITSTMPAS_TASSERTION_SHIFT 0
#define IRQAMP_ITSTMPAS_TASSERTION_MASK 0xffffffffU
#define IRQAMP_ITSTMPAS_TASSERTION_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ITSTMPAS_TASSERTION_MASK ) >> \
    IRQAMP_ITSTMPAS_TASSERTION_SHIFT )
#define IRQAMP_ITSTMPAS_TASSERTION_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ITSTMPAS_TASSERTION_MASK ) | \
    ( ( ( _val ) << IRQAMP_ITSTMPAS_TASSERTION_SHIFT ) & \
      IRQAMP_ITSTMPAS_TASSERTION_MASK ) )
#define IRQAMP_ITSTMPAS_TASSERTION( _val ) \
  ( ( ( _val ) << IRQAMP_ITSTMPAS_TASSERTION_SHIFT ) & \
    IRQAMP_ITSTMPAS_TASSERTION_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPTimestampITSTMPAC \
 *   Interrupt Acknowledge Timestamp n register (ITSTMPAC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ITSTMPAC_TACKNOWLEDGE_SHIFT 0
#define IRQAMP_ITSTMPAC_TACKNOWLEDGE_MASK 0xffffffffU
#define IRQAMP_ITSTMPAC_TACKNOWLEDGE_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ITSTMPAC_TACKNOWLEDGE_MASK ) >> \
    IRQAMP_ITSTMPAC_TACKNOWLEDGE_SHIFT )
#define IRQAMP_ITSTMPAC_TACKNOWLEDGE_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ITSTMPAC_TACKNOWLEDGE_MASK ) | \
    ( ( ( _val ) << IRQAMP_ITSTMPAC_TACKNOWLEDGE_SHIFT ) & \
      IRQAMP_ITSTMPAC_TACKNOWLEDGE_MASK ) )
#define IRQAMP_ITSTMPAC_TACKNOWLEDGE( _val ) \
  ( ( ( _val ) << IRQAMP_ITSTMPAC_TACKNOWLEDGE_SHIFT ) & \
    IRQAMP_ITSTMPAC_TACKNOWLEDGE_MASK )

/** @} */

/**
 * @brief This structure defines the IRQ(A)MP Timestamp register block memory
 *   map.
 */
typedef struct irqamp_timestamp {
  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPTimestampITCNT.
   */
  uint32_t itcnt;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPTimestampITSTMPC.
   */
  uint32_t itstmpc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPTimestampITSTMPAS.
   */
  uint32_t itstmpas;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPTimestampITSTMPAC.
   */
  uint32_t itstmpac;
} irqamp_timestamp;

/** @} */

/* Generated from spec:/dev/grlib/if/irqamp */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMP IRQ(A)MP
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the IRQ(A)MP interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPILEVEL Interrupt level register (ILEVEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ILEVEL_IL_15_1_SHIFT 1
#define IRQAMP_ILEVEL_IL_15_1_MASK 0xfffeU
#define IRQAMP_ILEVEL_IL_15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ILEVEL_IL_15_1_MASK ) >> \
    IRQAMP_ILEVEL_IL_15_1_SHIFT )
#define IRQAMP_ILEVEL_IL_15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ILEVEL_IL_15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_ILEVEL_IL_15_1_SHIFT ) & \
      IRQAMP_ILEVEL_IL_15_1_MASK ) )
#define IRQAMP_ILEVEL_IL_15_1( _val ) \
  ( ( ( _val ) << IRQAMP_ILEVEL_IL_15_1_SHIFT ) & \
    IRQAMP_ILEVEL_IL_15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPIPEND Interrupt pending register (IPEND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_IPEND_EIP_31_16_SHIFT 16
#define IRQAMP_IPEND_EIP_31_16_MASK 0xffff0000U
#define IRQAMP_IPEND_EIP_31_16_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IPEND_EIP_31_16_MASK ) >> \
    IRQAMP_IPEND_EIP_31_16_SHIFT )
#define IRQAMP_IPEND_EIP_31_16_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IPEND_EIP_31_16_MASK ) | \
    ( ( ( _val ) << IRQAMP_IPEND_EIP_31_16_SHIFT ) & \
      IRQAMP_IPEND_EIP_31_16_MASK ) )
#define IRQAMP_IPEND_EIP_31_16( _val ) \
  ( ( ( _val ) << IRQAMP_IPEND_EIP_31_16_SHIFT ) & \
    IRQAMP_IPEND_EIP_31_16_MASK )

#define IRQAMP_IPEND_IP_15_1_SHIFT 1
#define IRQAMP_IPEND_IP_15_1_MASK 0xfffeU
#define IRQAMP_IPEND_IP_15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IPEND_IP_15_1_MASK ) >> \
    IRQAMP_IPEND_IP_15_1_SHIFT )
#define IRQAMP_IPEND_IP_15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IPEND_IP_15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_IPEND_IP_15_1_SHIFT ) & \
      IRQAMP_IPEND_IP_15_1_MASK ) )
#define IRQAMP_IPEND_IP_15_1( _val ) \
  ( ( ( _val ) << IRQAMP_IPEND_IP_15_1_SHIFT ) & \
    IRQAMP_IPEND_IP_15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPIFORCE0 \
 *   Interrupt force register for processor 0 (IFORCE0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_IFORCE0_IF_15_1_SHIFT 1
#define IRQAMP_IFORCE0_IF_15_1_MASK 0xfffeU
#define IRQAMP_IFORCE0_IF_15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IFORCE0_IF_15_1_MASK ) >> \
    IRQAMP_IFORCE0_IF_15_1_SHIFT )
#define IRQAMP_IFORCE0_IF_15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IFORCE0_IF_15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_IFORCE0_IF_15_1_SHIFT ) & \
      IRQAMP_IFORCE0_IF_15_1_MASK ) )
#define IRQAMP_IFORCE0_IF_15_1( _val ) \
  ( ( ( _val ) << IRQAMP_IFORCE0_IF_15_1_SHIFT ) & \
    IRQAMP_IFORCE0_IF_15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPICLEAR Interrupt clear register (ICLEAR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ICLEAR_EIC_31_16_SHIFT 16
#define IRQAMP_ICLEAR_EIC_31_16_MASK 0xffff0000U
#define IRQAMP_ICLEAR_EIC_31_16_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICLEAR_EIC_31_16_MASK ) >> \
    IRQAMP_ICLEAR_EIC_31_16_SHIFT )
#define IRQAMP_ICLEAR_EIC_31_16_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICLEAR_EIC_31_16_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICLEAR_EIC_31_16_SHIFT ) & \
      IRQAMP_ICLEAR_EIC_31_16_MASK ) )
#define IRQAMP_ICLEAR_EIC_31_16( _val ) \
  ( ( ( _val ) << IRQAMP_ICLEAR_EIC_31_16_SHIFT ) & \
    IRQAMP_ICLEAR_EIC_31_16_MASK )

#define IRQAMP_ICLEAR_IC_15_1_SHIFT 1
#define IRQAMP_ICLEAR_IC_15_1_MASK 0xfffeU
#define IRQAMP_ICLEAR_IC_15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICLEAR_IC_15_1_MASK ) >> \
    IRQAMP_ICLEAR_IC_15_1_SHIFT )
#define IRQAMP_ICLEAR_IC_15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICLEAR_IC_15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICLEAR_IC_15_1_SHIFT ) & \
      IRQAMP_ICLEAR_IC_15_1_MASK ) )
#define IRQAMP_ICLEAR_IC_15_1( _val ) \
  ( ( ( _val ) << IRQAMP_ICLEAR_IC_15_1_SHIFT ) & \
    IRQAMP_ICLEAR_IC_15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPMPSTAT \
 *   Multiprocessor status register (MPSTAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_MPSTAT_NCPU_SHIFT 28
#define IRQAMP_MPSTAT_NCPU_MASK 0xf0000000U
#define IRQAMP_MPSTAT_NCPU_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_MPSTAT_NCPU_MASK ) >> \
    IRQAMP_MPSTAT_NCPU_SHIFT )
#define IRQAMP_MPSTAT_NCPU_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_MPSTAT_NCPU_MASK ) | \
    ( ( ( _val ) << IRQAMP_MPSTAT_NCPU_SHIFT ) & \
      IRQAMP_MPSTAT_NCPU_MASK ) )
#define IRQAMP_MPSTAT_NCPU( _val ) \
  ( ( ( _val ) << IRQAMP_MPSTAT_NCPU_SHIFT ) & \
    IRQAMP_MPSTAT_NCPU_MASK )

#define IRQAMP_MPSTAT_BA 0x8000000U

#define IRQAMP_MPSTAT_ER 0x4000000U

#define IRQAMP_MPSTAT_EIRQ_SHIFT 16
#define IRQAMP_MPSTAT_EIRQ_MASK 0xf0000U
#define IRQAMP_MPSTAT_EIRQ_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_MPSTAT_EIRQ_MASK ) >> \
    IRQAMP_MPSTAT_EIRQ_SHIFT )
#define IRQAMP_MPSTAT_EIRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_MPSTAT_EIRQ_MASK ) | \
    ( ( ( _val ) << IRQAMP_MPSTAT_EIRQ_SHIFT ) & \
      IRQAMP_MPSTAT_EIRQ_MASK ) )
#define IRQAMP_MPSTAT_EIRQ( _val ) \
  ( ( ( _val ) << IRQAMP_MPSTAT_EIRQ_SHIFT ) & \
    IRQAMP_MPSTAT_EIRQ_MASK )

#define IRQAMP_MPSTAT_STATUS_SHIFT 0
#define IRQAMP_MPSTAT_STATUS_MASK 0xfU
#define IRQAMP_MPSTAT_STATUS_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_MPSTAT_STATUS_MASK ) >> \
    IRQAMP_MPSTAT_STATUS_SHIFT )
#define IRQAMP_MPSTAT_STATUS_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_MPSTAT_STATUS_MASK ) | \
    ( ( ( _val ) << IRQAMP_MPSTAT_STATUS_SHIFT ) & \
      IRQAMP_MPSTAT_STATUS_MASK ) )
#define IRQAMP_MPSTAT_STATUS( _val ) \
  ( ( ( _val ) << IRQAMP_MPSTAT_STATUS_SHIFT ) & \
    IRQAMP_MPSTAT_STATUS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPBRDCST Broadcast register (BRDCST)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_BRDCST_BM15_1_SHIFT 1
#define IRQAMP_BRDCST_BM15_1_MASK 0xfffeU
#define IRQAMP_BRDCST_BM15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_BRDCST_BM15_1_MASK ) >> \
    IRQAMP_BRDCST_BM15_1_SHIFT )
#define IRQAMP_BRDCST_BM15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_BRDCST_BM15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_BRDCST_BM15_1_SHIFT ) & \
      IRQAMP_BRDCST_BM15_1_MASK ) )
#define IRQAMP_BRDCST_BM15_1( _val ) \
  ( ( ( _val ) << IRQAMP_BRDCST_BM15_1_SHIFT ) & \
    IRQAMP_BRDCST_BM15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPERRSTAT Error Mode Status Register (ERRSTAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ERRSTAT_ERRMODE_3_0_SHIFT 0
#define IRQAMP_ERRSTAT_ERRMODE_3_0_MASK 0xfU
#define IRQAMP_ERRSTAT_ERRMODE_3_0_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ERRSTAT_ERRMODE_3_0_MASK ) >> \
    IRQAMP_ERRSTAT_ERRMODE_3_0_SHIFT )
#define IRQAMP_ERRSTAT_ERRMODE_3_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ERRSTAT_ERRMODE_3_0_MASK ) | \
    ( ( ( _val ) << IRQAMP_ERRSTAT_ERRMODE_3_0_SHIFT ) & \
      IRQAMP_ERRSTAT_ERRMODE_3_0_MASK ) )
#define IRQAMP_ERRSTAT_ERRMODE_3_0( _val ) \
  ( ( ( _val ) << IRQAMP_ERRSTAT_ERRMODE_3_0_SHIFT ) & \
    IRQAMP_ERRSTAT_ERRMODE_3_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPWDOGCTRL \
 *   Watchdog control register (WDOGCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_WDOGCTRL_NWDOG_SHIFT 27
#define IRQAMP_WDOGCTRL_NWDOG_MASK 0xf8000000U
#define IRQAMP_WDOGCTRL_NWDOG_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_WDOGCTRL_NWDOG_MASK ) >> \
    IRQAMP_WDOGCTRL_NWDOG_SHIFT )
#define IRQAMP_WDOGCTRL_NWDOG_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_WDOGCTRL_NWDOG_MASK ) | \
    ( ( ( _val ) << IRQAMP_WDOGCTRL_NWDOG_SHIFT ) & \
      IRQAMP_WDOGCTRL_NWDOG_MASK ) )
#define IRQAMP_WDOGCTRL_NWDOG( _val ) \
  ( ( ( _val ) << IRQAMP_WDOGCTRL_NWDOG_SHIFT ) & \
    IRQAMP_WDOGCTRL_NWDOG_MASK )

#define IRQAMP_WDOGCTRL_WDOGIRQ_SHIFT 16
#define IRQAMP_WDOGCTRL_WDOGIRQ_MASK 0xf0000U
#define IRQAMP_WDOGCTRL_WDOGIRQ_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_WDOGCTRL_WDOGIRQ_MASK ) >> \
    IRQAMP_WDOGCTRL_WDOGIRQ_SHIFT )
#define IRQAMP_WDOGCTRL_WDOGIRQ_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_WDOGCTRL_WDOGIRQ_MASK ) | \
    ( ( ( _val ) << IRQAMP_WDOGCTRL_WDOGIRQ_SHIFT ) & \
      IRQAMP_WDOGCTRL_WDOGIRQ_MASK ) )
#define IRQAMP_WDOGCTRL_WDOGIRQ( _val ) \
  ( ( ( _val ) << IRQAMP_WDOGCTRL_WDOGIRQ_SHIFT ) & \
    IRQAMP_WDOGCTRL_WDOGIRQ_MASK )

#define IRQAMP_WDOGCTRL_WDOGMSK_SHIFT 0
#define IRQAMP_WDOGCTRL_WDOGMSK_MASK 0xfU
#define IRQAMP_WDOGCTRL_WDOGMSK_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_WDOGCTRL_WDOGMSK_MASK ) >> \
    IRQAMP_WDOGCTRL_WDOGMSK_SHIFT )
#define IRQAMP_WDOGCTRL_WDOGMSK_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_WDOGCTRL_WDOGMSK_MASK ) | \
    ( ( ( _val ) << IRQAMP_WDOGCTRL_WDOGMSK_SHIFT ) & \
      IRQAMP_WDOGCTRL_WDOGMSK_MASK ) )
#define IRQAMP_WDOGCTRL_WDOGMSK( _val ) \
  ( ( ( _val ) << IRQAMP_WDOGCTRL_WDOGMSK_SHIFT ) & \
    IRQAMP_WDOGCTRL_WDOGMSK_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPASMPCTRL \
 *   Asymmetric multiprocessing control register (ASMPCTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ASMPCTRL_NCTRL_SHIFT 28
#define IRQAMP_ASMPCTRL_NCTRL_MASK 0xf0000000U
#define IRQAMP_ASMPCTRL_NCTRL_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ASMPCTRL_NCTRL_MASK ) >> \
    IRQAMP_ASMPCTRL_NCTRL_SHIFT )
#define IRQAMP_ASMPCTRL_NCTRL_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ASMPCTRL_NCTRL_MASK ) | \
    ( ( ( _val ) << IRQAMP_ASMPCTRL_NCTRL_SHIFT ) & \
      IRQAMP_ASMPCTRL_NCTRL_MASK ) )
#define IRQAMP_ASMPCTRL_NCTRL( _val ) \
  ( ( ( _val ) << IRQAMP_ASMPCTRL_NCTRL_SHIFT ) & \
    IRQAMP_ASMPCTRL_NCTRL_MASK )

#define IRQAMP_ASMPCTRL_ICF 0x2U

#define IRQAMP_ASMPCTRL_L 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPICSELR \
 *   Interrupt controller select register (ICSELR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_ICSELR_ICSEL0_SHIFT 28
#define IRQAMP_ICSELR_ICSEL0_MASK 0xf0000000U
#define IRQAMP_ICSELR_ICSEL0_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICSELR_ICSEL0_MASK ) >> \
    IRQAMP_ICSELR_ICSEL0_SHIFT )
#define IRQAMP_ICSELR_ICSEL0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICSELR_ICSEL0_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICSELR_ICSEL0_SHIFT ) & \
      IRQAMP_ICSELR_ICSEL0_MASK ) )
#define IRQAMP_ICSELR_ICSEL0( _val ) \
  ( ( ( _val ) << IRQAMP_ICSELR_ICSEL0_SHIFT ) & \
    IRQAMP_ICSELR_ICSEL0_MASK )

#define IRQAMP_ICSELR_ICSEL1_SHIFT 24
#define IRQAMP_ICSELR_ICSEL1_MASK 0xf000000U
#define IRQAMP_ICSELR_ICSEL1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICSELR_ICSEL1_MASK ) >> \
    IRQAMP_ICSELR_ICSEL1_SHIFT )
#define IRQAMP_ICSELR_ICSEL1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICSELR_ICSEL1_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICSELR_ICSEL1_SHIFT ) & \
      IRQAMP_ICSELR_ICSEL1_MASK ) )
#define IRQAMP_ICSELR_ICSEL1( _val ) \
  ( ( ( _val ) << IRQAMP_ICSELR_ICSEL1_SHIFT ) & \
    IRQAMP_ICSELR_ICSEL1_MASK )

#define IRQAMP_ICSELR_ICSEL2_SHIFT 20
#define IRQAMP_ICSELR_ICSEL2_MASK 0xf00000U
#define IRQAMP_ICSELR_ICSEL2_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICSELR_ICSEL2_MASK ) >> \
    IRQAMP_ICSELR_ICSEL2_SHIFT )
#define IRQAMP_ICSELR_ICSEL2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICSELR_ICSEL2_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICSELR_ICSEL2_SHIFT ) & \
      IRQAMP_ICSELR_ICSEL2_MASK ) )
#define IRQAMP_ICSELR_ICSEL2( _val ) \
  ( ( ( _val ) << IRQAMP_ICSELR_ICSEL2_SHIFT ) & \
    IRQAMP_ICSELR_ICSEL2_MASK )

#define IRQAMP_ICSELR_ICSEL3_SHIFT 16
#define IRQAMP_ICSELR_ICSEL3_MASK 0xf0000U
#define IRQAMP_ICSELR_ICSEL3_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_ICSELR_ICSEL3_MASK ) >> \
    IRQAMP_ICSELR_ICSEL3_SHIFT )
#define IRQAMP_ICSELR_ICSEL3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_ICSELR_ICSEL3_MASK ) | \
    ( ( ( _val ) << IRQAMP_ICSELR_ICSEL3_SHIFT ) & \
      IRQAMP_ICSELR_ICSEL3_MASK ) )
#define IRQAMP_ICSELR_ICSEL3( _val ) \
  ( ( ( _val ) << IRQAMP_ICSELR_ICSEL3_SHIFT ) & \
    IRQAMP_ICSELR_ICSEL3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPPIMASK \
 *   Processor n interrupt mask register (PIMASK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_PIMASK_EIM_31_16_SHIFT 16
#define IRQAMP_PIMASK_EIM_31_16_MASK 0xffff0000U
#define IRQAMP_PIMASK_EIM_31_16_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_PIMASK_EIM_31_16_MASK ) >> \
    IRQAMP_PIMASK_EIM_31_16_SHIFT )
#define IRQAMP_PIMASK_EIM_31_16_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_PIMASK_EIM_31_16_MASK ) | \
    ( ( ( _val ) << IRQAMP_PIMASK_EIM_31_16_SHIFT ) & \
      IRQAMP_PIMASK_EIM_31_16_MASK ) )
#define IRQAMP_PIMASK_EIM_31_16( _val ) \
  ( ( ( _val ) << IRQAMP_PIMASK_EIM_31_16_SHIFT ) & \
    IRQAMP_PIMASK_EIM_31_16_MASK )

#define IRQAMP_PIMASK_IM15_1_SHIFT 1
#define IRQAMP_PIMASK_IM15_1_MASK 0xfffeU
#define IRQAMP_PIMASK_IM15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_PIMASK_IM15_1_MASK ) >> \
    IRQAMP_PIMASK_IM15_1_SHIFT )
#define IRQAMP_PIMASK_IM15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_PIMASK_IM15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_PIMASK_IM15_1_SHIFT ) & \
      IRQAMP_PIMASK_IM15_1_MASK ) )
#define IRQAMP_PIMASK_IM15_1( _val ) \
  ( ( ( _val ) << IRQAMP_PIMASK_IM15_1_SHIFT ) & \
    IRQAMP_PIMASK_IM15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPPIFORCE \
 *   Processor n interrupt force register (PIFORCE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_PIFORCE_FC_15_1_SHIFT 17
#define IRQAMP_PIFORCE_FC_15_1_MASK 0xfffe0000U
#define IRQAMP_PIFORCE_FC_15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_PIFORCE_FC_15_1_MASK ) >> \
    IRQAMP_PIFORCE_FC_15_1_SHIFT )
#define IRQAMP_PIFORCE_FC_15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_PIFORCE_FC_15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_PIFORCE_FC_15_1_SHIFT ) & \
      IRQAMP_PIFORCE_FC_15_1_MASK ) )
#define IRQAMP_PIFORCE_FC_15_1( _val ) \
  ( ( ( _val ) << IRQAMP_PIFORCE_FC_15_1_SHIFT ) & \
    IRQAMP_PIFORCE_FC_15_1_MASK )

#define IRQAMP_PIFORCE_IF15_1_SHIFT 1
#define IRQAMP_PIFORCE_IF15_1_MASK 0xfffeU
#define IRQAMP_PIFORCE_IF15_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_PIFORCE_IF15_1_MASK ) >> \
    IRQAMP_PIFORCE_IF15_1_SHIFT )
#define IRQAMP_PIFORCE_IF15_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_PIFORCE_IF15_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_PIFORCE_IF15_1_SHIFT ) & \
      IRQAMP_PIFORCE_IF15_1_MASK ) )
#define IRQAMP_PIFORCE_IF15_1( _val ) \
  ( ( ( _val ) << IRQAMP_PIFORCE_IF15_1_SHIFT ) & \
    IRQAMP_PIFORCE_IF15_1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPPEXTACK \
 *   Processor n extended interrupt acknowledge register (PEXTACK)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_PEXTACK_EID_4_0_SHIFT 0
#define IRQAMP_PEXTACK_EID_4_0_MASK 0x1fU
#define IRQAMP_PEXTACK_EID_4_0_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_PEXTACK_EID_4_0_MASK ) >> \
    IRQAMP_PEXTACK_EID_4_0_SHIFT )
#define IRQAMP_PEXTACK_EID_4_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_PEXTACK_EID_4_0_MASK ) | \
    ( ( ( _val ) << IRQAMP_PEXTACK_EID_4_0_SHIFT ) & \
      IRQAMP_PEXTACK_EID_4_0_MASK ) )
#define IRQAMP_PEXTACK_EID_4_0( _val ) \
  ( ( ( _val ) << IRQAMP_PEXTACK_EID_4_0_SHIFT ) & \
    IRQAMP_PEXTACK_EID_4_0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPBADDR \
 *   Processor n Boot Address register (BADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_BADDR_BOOTADDR_31_3_SHIFT 3
#define IRQAMP_BADDR_BOOTADDR_31_3_MASK 0xfffffff8U
#define IRQAMP_BADDR_BOOTADDR_31_3_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_BADDR_BOOTADDR_31_3_MASK ) >> \
    IRQAMP_BADDR_BOOTADDR_31_3_SHIFT )
#define IRQAMP_BADDR_BOOTADDR_31_3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_BADDR_BOOTADDR_31_3_MASK ) | \
    ( ( ( _val ) << IRQAMP_BADDR_BOOTADDR_31_3_SHIFT ) & \
      IRQAMP_BADDR_BOOTADDR_31_3_MASK ) )
#define IRQAMP_BADDR_BOOTADDR_31_3( _val ) \
  ( ( ( _val ) << IRQAMP_BADDR_BOOTADDR_31_3_SHIFT ) & \
    IRQAMP_BADDR_BOOTADDR_31_3_MASK )

#define IRQAMP_BADDR_AS 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBIRQAMPIRQMAP Interrupt map register n (IRQMAP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define IRQAMP_IRQMAP_IRQMAP_4_N_0_SHIFT 24
#define IRQAMP_IRQMAP_IRQMAP_4_N_0_MASK 0xff000000U
#define IRQAMP_IRQMAP_IRQMAP_4_N_0_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IRQMAP_IRQMAP_4_N_0_MASK ) >> \
    IRQAMP_IRQMAP_IRQMAP_4_N_0_SHIFT )
#define IRQAMP_IRQMAP_IRQMAP_4_N_0_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IRQMAP_IRQMAP_4_N_0_MASK ) | \
    ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_0_SHIFT ) & \
      IRQAMP_IRQMAP_IRQMAP_4_N_0_MASK ) )
#define IRQAMP_IRQMAP_IRQMAP_4_N_0( _val ) \
  ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_0_SHIFT ) & \
    IRQAMP_IRQMAP_IRQMAP_4_N_0_MASK )

#define IRQAMP_IRQMAP_IRQMAP_4_N_1_SHIFT 16
#define IRQAMP_IRQMAP_IRQMAP_4_N_1_MASK 0xff0000U
#define IRQAMP_IRQMAP_IRQMAP_4_N_1_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IRQMAP_IRQMAP_4_N_1_MASK ) >> \
    IRQAMP_IRQMAP_IRQMAP_4_N_1_SHIFT )
#define IRQAMP_IRQMAP_IRQMAP_4_N_1_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IRQMAP_IRQMAP_4_N_1_MASK ) | \
    ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_1_SHIFT ) & \
      IRQAMP_IRQMAP_IRQMAP_4_N_1_MASK ) )
#define IRQAMP_IRQMAP_IRQMAP_4_N_1( _val ) \
  ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_1_SHIFT ) & \
    IRQAMP_IRQMAP_IRQMAP_4_N_1_MASK )

#define IRQAMP_IRQMAP_IRQMAP_4_N_2_SHIFT 8
#define IRQAMP_IRQMAP_IRQMAP_4_N_2_MASK 0xff00U
#define IRQAMP_IRQMAP_IRQMAP_4_N_2_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IRQMAP_IRQMAP_4_N_2_MASK ) >> \
    IRQAMP_IRQMAP_IRQMAP_4_N_2_SHIFT )
#define IRQAMP_IRQMAP_IRQMAP_4_N_2_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IRQMAP_IRQMAP_4_N_2_MASK ) | \
    ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_2_SHIFT ) & \
      IRQAMP_IRQMAP_IRQMAP_4_N_2_MASK ) )
#define IRQAMP_IRQMAP_IRQMAP_4_N_2( _val ) \
  ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_2_SHIFT ) & \
    IRQAMP_IRQMAP_IRQMAP_4_N_2_MASK )

#define IRQAMP_IRQMAP_IRQMAP_4_N_3_SHIFT 0
#define IRQAMP_IRQMAP_IRQMAP_4_N_3_MASK 0xffU
#define IRQAMP_IRQMAP_IRQMAP_4_N_3_GET( _reg ) \
  ( ( ( _reg ) & IRQAMP_IRQMAP_IRQMAP_4_N_3_MASK ) >> \
    IRQAMP_IRQMAP_IRQMAP_4_N_3_SHIFT )
#define IRQAMP_IRQMAP_IRQMAP_4_N_3_SET( _reg, _val ) \
  ( ( ( _reg ) & ~IRQAMP_IRQMAP_IRQMAP_4_N_3_MASK ) | \
    ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_3_SHIFT ) & \
      IRQAMP_IRQMAP_IRQMAP_4_N_3_MASK ) )
#define IRQAMP_IRQMAP_IRQMAP_4_N_3( _val ) \
  ( ( ( _val ) << IRQAMP_IRQMAP_IRQMAP_4_N_3_SHIFT ) & \
    IRQAMP_IRQMAP_IRQMAP_4_N_3_MASK )

/** @} */

/**
 * @brief This structure defines the IRQ(A)MP register block memory map.
 */
typedef struct irqamp {
  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPILEVEL.
   */
  uint32_t ilevel;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPIPEND.
   */
  uint32_t ipend;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPIFORCE0.
   */
  uint32_t iforce0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPICLEAR.
   */
  uint32_t iclear;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPMPSTAT.
   */
  uint32_t mpstat;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPBRDCST.
   */
  uint32_t brdcst;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPERRSTAT.
   */
  uint32_t errstat;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPWDOGCTRL.
   */
  uint32_t wdogctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPASMPCTRL.
   */
  uint32_t asmpctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPICSELR.
   */
  uint32_t icselr[ 2 ];

  uint32_t reserved_2c_40[ 5 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPPIMASK.
   */
  uint32_t pimask[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPPIFORCE.
   */
  uint32_t piforce[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPPEXTACK.
   */
  uint32_t pextack[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPTimestamp.
   */
  irqamp_timestamp itstmp[ 16 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPBADDR.
   */
  uint32_t baddr[ 16 ];

  uint32_t reserved_240_300[ 48 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBIRQAMPIRQMAP.
   */
  uint32_t irqmap[ 16 ];

  uint32_t reserved_340_400[ 48 ];
} irqamp;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_IRQAMP_REGS_H */
