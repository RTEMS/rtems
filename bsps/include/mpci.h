/* SPDX-License-Identifier: BSD-2-Clause */

/*  mpci.h
 *
 *  This include file contains all the renaming necessary to
 *  have an application use the Shared Memory Driver as its
 *  sole mechanism for MPCI.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef __SHM_MPCI_h
#define __SHM_MPCI_h

#ifdef __cplusplus
extern "C" {
#endif

#include <shm_driver.h>

#define MPCI_Initialization( _configuration ) \
  Shm_Initialization( _configuration )

#define MPCI_Get_packet( _the_packet ) \
  Shm_Get_packet( _the_packet )

#define MPCI_Return_packet( _the_packet ) \
  Shm_Return_packet( _the_packet )

#define MPCI_Receive_packet( _the_packet ) \
  Shm_Receive_packet( _the_packet )

#define MPCI_Send_packet( _destination, _the_packet ) \
  Shm_Send_packet( _destination, _the_packet )

/* Unnecessary... mapped in shm_driver.h
#define MPCI_Fatal( _the_error ) \
  Shm_Fatal( _the_error )
*/

#define MPCI_Enable_statistics()

#define MPCI_Print_statistics() \
  Shm_Print_statistics()

/* no need to rename the MPCI_Table either */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
