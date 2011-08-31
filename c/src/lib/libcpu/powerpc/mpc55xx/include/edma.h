/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Enhanced Direct Memory Access (eDMA).
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_POWERPC_MPC55XX_EDMA_H
#define LIBCPU_POWERPC_MPC55XX_EDMA_H

#include <stdbool.h>
#include <stdint.h>

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct mpc55xx_edma_channel_entry {
  rtems_chain_node node;
  unsigned channel;
  void (*done)( struct mpc55xx_edma_channel_entry *, uint32_t);
  rtems_id id;
} mpc55xx_edma_channel_entry;

void mpc55xx_edma_init(void);

rtems_status_code mpc55xx_edma_obtain_channel( mpc55xx_edma_channel_entry *e, unsigned irq_priority);

void mpc55xx_edma_release_channel( mpc55xx_edma_channel_entry *e);

void mpc55xx_edma_enable_hardware_requests( unsigned channel, bool enable);

void mpc55xx_edma_enable_error_interrupts( unsigned channel, bool enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_EDMA_H */
