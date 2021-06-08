/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBIRQAMP
 *
 * @brief This header file defines the IRQ(A)MP interface.
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

/* Generated from spec:/dev/grlib/if/irqamp-header-2 */

#ifndef _GRLIB_IRQAMP_H
#define _GRLIB_IRQAMP_H

#include <stddef.h>
#include <grlib/io.h>
#include <grlib/irqamp-regs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/irqamp-get-timestamp */

/**
 * @ingroup RTEMSDeviceGRLIBIRQAMP
 *
 * @brief Gets the interrupt timestamping register bock.
 *
 * @param irqamp_regs is the IRQ(A)MP register block.
 *
 * @retval NULL The IRQ(A)MP does not support the interrupt timestamping
 *   feature.
 *
 * @return Returns the interrupt timestamping register block.
 */
static inline irqamp_timestamp *irqamp_get_timestamp_registers(
  irqamp *irqamp_regs
)
{
  irqamp_timestamp *timestamp_regs;
  uint32_t itstmpc;

  timestamp_regs = &irqamp_regs->itstmp[ 0 ];
  itstmpc = grlib_load_32( &timestamp_regs->itstmpc );

  if ( IRQAMP_ITSTMPC_TSTAMP_GET( itstmpc ) == 0 ) {
    return NULL;
  }

  return timestamp_regs;
}

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_IRQAMP_H */
