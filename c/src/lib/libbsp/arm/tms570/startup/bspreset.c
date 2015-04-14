/**
 * @file bspreset.c
 *
 * @ingroup tms570
 *
 * @brief Reset code.
 */

/*
 * Copyright (c) 2015 Taller Technologies.
 *
 * @author Martin Galvan <martin.galvan@tallertechnologies.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/tms570.h>
#include <bsp/start.h>

static void handle_esm_errors(uint32_t esm_irq_channel)
{
   /* ESMR3 errors don't generate interrupts. */
   if (esm_irq_channel < 0x20u) {
     ESMSR1 = 1 << esm_irq_channel;
   } else if (esm_irq_channel < 0x40u) {
     ESMSR2 = 1 << (esm_irq_channel - 32u);
   } else if (esm_irq_channel < 0x60u) {
     ESMSR4 = 1 << (esm_irq_channel - 64u);
   }
}

void bsp_reset(void)
{
   uint32_t esm_irq_channel = ESMIOFFHR - 1;

   if (esm_irq_channel) {
     handle_esm_errors(esm_irq_channel);
   }

   /* Reset the board */
   SYSECR = SYSECR_RESET;
}
