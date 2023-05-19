/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic mcf548x BSP
 *
 * This file declares glue functions to the Freescale Mcdma API.
 */

/*
 * Copyright (C) 2004, 2009 embedded brains GmbH & Co. KG
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

#ifndef _MCDMA_GLUE_H
#define _MCDMA_GLUE_H

#include <rtems.h>
#include <rtems/irq.h>
#include <mcf548x/mcf548x.h>

#define MCDMA_CLR_PENDING(chan) (MCF548X_DMA_DIPR = (1 << (chan)))
#define MCDMA_GET_PENDING(chan) (MCF548X_DMA_DIPR & (1 << (chan)))

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_enable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupt for given task number                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno                           /* task number to enable    */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_disable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable interrupt for given task number                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno                           /* task number to disable   */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_install
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install given function as mcdma interrupt handler                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_taskno,                          /* task number for handler  */
 void (*the_handler)(rtems_irq_hdl_param),     /* function to call         */
 void *the_param
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the mcdma module (if not yet done):                          |
|   - load code                                                             |
|   - initialize registers                                                  |
|   - initialize bus arbiter                                                |
|   - initialize interrupt control                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void *sram_base         /* base address for SRAM, to be used for DMA task */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/

#endif /* _MCDMA_GLUE_H */
