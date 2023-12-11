/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplCRC
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSImplCRC.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_CRC_H
#define _RTEMS_CRC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSImplCRC Cyclic Redundancy Check (CRC) Support
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains functions to calculate
 *   Cyclic Redundancy Check (CRC) values.
 *
 * @{
 */


/**
 * @brief This constant represents the default CRC-24Q seed state.
 */
#define CRC24Q_SEED 0U

/**
 * @brief This constant provides a mask to get a valid CRC-24Q value from the
 *   integers returned by _CRC24Q_Update() and _CRC24Q_Sequence_update().
 */
#define CRC24Q_MASK 0xffffffU

/**
 * @brief Updates the CRC-24Q state using a byte.
 *
 * @param crc is the input CRC-24Q state.
 *
 * @param byte is the byte updating the input CRC-24Q state.
 *
 * @return Returns the updated CRC-24Q state.  Use the #CRC24Q_MASK to get a
 *   valid CRC-24Q value.
 */
uint32_t _CRC24Q_Update( uint32_t crc, uint8_t byte );

/**
 * @brief Updates the CRC-24Q state using a sequence of bytes.
 *
 * @param crc is the input CRC-24Q state.
 *
 * @param bytes[in] is the sequence of bytes updating the input CRC-24Q state.
 *
 * @param size_in_bytes is the size in bytes of the byte sequence.
 *
 * @return Returns the updated CRC-24Q state.  Use the #CRC24Q_MASK to get a
 *   valid CRC-24Q value.
 */
uint32_t _CRC24Q_Sequence_update(
  uint32_t    crc,
  const void *bytes,
  size_t      size_in_bytes
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_CRC_H */
