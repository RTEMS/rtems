/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief This header file defines the APBUART interface.
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
 * The spec:/... comments allow you to identify content which is
 * duplicated in the rtems and rtems-docs repositories.
 */

/* Find related documentation with spec:/dev/grlib/if/apbuart-header-2 */

#ifndef _GRLIB_APBUART_H
#define _GRLIB_APBUART_H

#include <grlib/apbuart-regs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/dev/grlib/if/apbuart-inbyte-nonblocking */

/**
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief Clears all errors and tries to get one character from the receiver
 *   FIFO.
 *
 * @param regs is the pointer to the APBUART register block.
 *
 * @retval -1 The receiver FIFO was empty.
 *
 * @return Returns the first character of the receiver FIFO if it was
 *   non-empty.
 */
int apbuart_inbyte_nonblocking( apbuart *regs );

/* Find related documentation with spec:/dev/grlib/if/apbuart-outbyte-polled */

/**
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief Waits until an empty transmitter FIFO was observed and then stores
 *   the character to the data register.
 *
 * @param regs is the pointer to the APBUART register block.
 *
 * @param ch is the character to output.
 */
void apbuart_outbyte_polled( apbuart *regs, char ch );

/* Find related documentation with spec:/dev/grlib/if/apbuart-outbyte-wait */

/**
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief Ensures that at least once an empty transmitter FIFO was observed.
 *
 * @param regs is the pointer to the APBUART register block.
 */
void apbuart_outbyte_wait( const apbuart *regs );

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_APBUART_H */
