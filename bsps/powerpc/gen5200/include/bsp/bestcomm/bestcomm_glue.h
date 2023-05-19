/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file declares glue functions to the Freescale BestComm API.
 */

/*
 * Copyright (C) 2004, 2005 embedded brains GmbH & Co. KG
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

#ifndef _BESTCOMM_GLUE_H
#define _BESTCOMM_GLUE_H

#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_enable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupt for given task number                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno                           /* task number to enable    */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_disable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable interrupt for given task number                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno                           /* task number to disable   */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_install
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install given function as bestcomm interrupt handler                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno,                          /* task number for handler  */
 rtems_interrupt_handler handler,              /* function to call         */
 void *arg
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the bestcomm module (if not yet done):                       |
|   - load code                                                             |
|   - initialize registers                                                  |
|   - initialize bus arbiter                                                |
|   - initialize interrupt control                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void /* none */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

void *bestcomm_malloc(size_t size);

void bestcomm_free(void *ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BESTCOMM_GLUE_H */
