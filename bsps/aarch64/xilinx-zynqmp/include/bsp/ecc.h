/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64XilinxZynqMP
 *
 * @brief This header file provides internal APIs for managing ECC events.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#ifndef LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_H
#define LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_H

/**
 * @addtogroup RTEMSBSPsAArch64
 *
 * @{
 */

//#include <bspopts.h>

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumeration describing the possible types of ECC events
 */
typedef enum {
  /* L1 Cache event information is delivered via Cache_Error_Info struct. */
  L1_CACHE,
  /* L2 Cache event information is delivered via Cache_Error_Info struct. */
  L2_CACHE,
  /*
   * L1 and L2 cache are on a combined interrupt on ZynqMP. They are enabled as
   * a single unit. The above individual L1 and L2 cache definitions will be
   * used for reporting. Attempting to enable L1 or L2 individually will enable
   * both.
   */
  L1_L2_CACHE,
  /* OCM RAM event information is delivered via OCM_Error_Info struct. */
  OCM_RAM,
  /* DDR RAM event information is delivered via DDR_Error_Info struct. */
  DDR_RAM,
} ECC_Event_Type;

/**
 * @brief The specific locations where a cache error can originate
 */
typedef enum {
  RAM_ID_L1I_TAG,
  RAM_ID_L1I_DATA,
  RAM_ID_L1D_TAG,
  RAM_ID_L1D_DATA,
  RAM_ID_L1D_DIRTY,
  RAM_ID_TLB,
  RAM_ID_L2_TAG,
  RAM_ID_L2_DATA,
  RAM_ID_SCU,
  RAM_ID_UNKNOWN
} Cache_Error_RAM_ID;

/**
 * @brief Structure containing information about a Cache error
 */
typedef struct {
  /* Indicates the RAM index address */
  uint64_t address;
  /* Indicates the type of RAM where the error originated */
  Cache_Error_RAM_ID ramid;
  /*
   * Indicates the segment (way or bank) of the RAM where the error originated.
   * Does not apply to L1D_DIRTY RAM ID. For SCU errors, this also indicates the
   * associated CPU.
   */
  uint8_t segment;
  /* The number of times this specific error has occurred since last reset */
  uint8_t repeats;
  /* The number of times other errors have occurred since last reset */
  uint8_t other_errors;
  /* Whether any of the errors represented have caused a data abort */
  bool abort;
} Cache_Error_Info;

/**
 * @brief Typedef for ECC handlers
 *
 * Functions matching this prototype can be registered as the handler for ECC
 * event callbacks. The data argument is a struct describing the event that
 * occurred.
 */
typedef void (*zynqmp_ecc_handler)( ECC_Event_Type event, void *data );

/**
 * @brief Enumeration describing the possible types of ECC events
 *
 * Note that the provided handler may be called from interrupt context.
 *
 * @param handler The handler to be called for all ECC error events
 */
void zynqmp_ecc_register_handler( zynqmp_ecc_handler handler );

/**
 * @brief Enable ECC error reporting
 *
 * Enables ECC error reporting for the specified subsystem.
 *
 * @param event The ECC error event type to enable
 */
int zynqmp_ecc_enable( ECC_Event_Type event );

/**
 * @brief Injects an ECC fault in the On-Chip Memory (OCM)
 */
void zynqmp_ocm_inject_fault( void );

/**
 * @brief The types of OCM ECC errors
 */
typedef enum {
  OCM_UNCORRECTABLE,
  OCM_UNCORRECTABLE_RMW,
  OCM_CORRECTABLE
} OCM_Error_Type;

/**
 * @brief Structure containing information about a OCM ECC error
 */
typedef struct {
  /* Describes the type of error being reported */
  OCM_Error_Type type;
  /* The offset into OCM where the error occurred */
  uint32_t offset;
  /* The data relevant to the error. Does not apply to RMW errors */
  uint32_t data0;
  uint32_t data1;
  uint32_t data2;
  uint32_t data3;
  /* The ECC syndrome relevant to the error. Does not apply to RMW errors */
  uint16_t syndrome;
} OCM_Error_Info;

/**
 * @brief The types of DDR ECC errors
 */
typedef enum {
  DDR_UNCORRECTABLE,
  DDR_CORRECTABLE
} DDR_Error_Type;

/**
 * @brief Structure containing information about a DDR ECC error
 */
typedef struct {
  /* Describes the type of error being reported */
  DDR_Error_Type type;
  /* The DDR Rank where the error occurred */
  uint32_t rank;
  /* The DDR Bank Group where the error occurred */
  uint32_t bank_group;
  /* The DDR Bank where the error occurred */
  uint32_t bank;
  /* The DDR Row where the error occurred */
  uint32_t row;
  /* The DDR Column where the error occurred */
  uint32_t column;
  /*
   * When mapping from SDRAM addressing back to AXI addressing, this is will
   * only be a close approximation of the source address since bits can be
   * discarded when converting from AXI to SDRAM.
   */
  uint64_t address;
} DDR_Error_Info;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

/** @} */

#endif /* LIBBSP_AARCH64_XILINX_ZYNQMP_BSP_ECC_H */
