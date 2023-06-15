/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBAPBUART
 *
 * @brief This source file contains the implementation of
 *   apbuart_outbyte_wait(), apbuart_outbyte_polled(), and
 *   apbuart_inbyte_nonblocking().
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

#include <grlib/apbuart.h>
#include <grlib/io.h>

#include <rtems/score/io.h>

void apbuart_outbyte_wait( const apbuart *regs )
{
  while ( ( grlib_load_32( &regs->status ) & APBUART_STATUS_TE ) == 0 ) {
    _IO_Relax();
  }
}

void apbuart_outbyte_polled( apbuart *regs, char ch)
{
  apbuart_outbyte_wait( regs );
  grlib_store_32( &regs->data, (uint8_t) ch );
}

int apbuart_inbyte_nonblocking( apbuart *regs )
{
  uint32_t status;

  status = grlib_load_32( &regs->status );

  /* Clear errors, writes to non-error flags are ignored */
  status &= ~( APBUART_STATUS_FE | APBUART_STATUS_PE | APBUART_STATUS_OV |
    APBUART_STATUS_BR );
  grlib_store_32( &regs->status, status );

  if ( ( status & APBUART_STATUS_DR ) == 0 ) {
    return -1;
  }

  return (int) APBUART_DATA_DATA_GET( grlib_load_32( &regs->data ) );
}
