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

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MPC55XX_EDMA_TCD_DEFAULT { \
	.SADDR = 0, \
	.SMOD = 0, \
	.SSIZE = 0x2, \
	.SOFF = 4, \
	.DADDR = 0, \
	.DMOD = 0, \
	.DSIZE = 0x2, \
	.DOFF = 4, \
	.NBYTES = 0, \
	.SLAST = 0, \
	.CITER = 1, \
	.BITER = 1, \
	.MAJORLINKCH = 0, \
	.CITERE_LINK = 0, \
	.BITERE_LINK = 0, \
	.MAJORE_LINK = 0, \
	.E_SG = 0, \
	.DLAST_SGA = 0, \
	.D_REQ = 0, \
	.BWC = 0, \
	.INT_HALF = 0, \
	.INT_MAJ = 0, \
	.DONE = 0, \
	.ACTIVE = 0, \
	.START = 0, \
}

#define MPC55XX_EDMA_TCD_ALT_DEFAULT { \
	.SADDR = 0, \
	.SMOD = 0, \
	.SSIZE = 2, \
	.DMOD = 0, \
	.DSIZE = 2, \
	.SOFF = 4, \
	.NBYTES = 0, \
	.SLAST = 0, \
	.DADDR = 0, \
	.CITERE_LINK = 0, \
	.CITERLINKCH = 0, \
	.CITER = 0, \
	.DOFF = 4, \
	.DLAST_SGA = 0, \
	.BITERE_LINK = 0, \
	.BITERLINKCH = 0, \
	.BITER = 0, \
	.BWC = 0, \
	.MAJORLINKCH = 0, \
	.DONE = 0, \
	.ACTIVE = 0, \
	.MAJORE_LINK = 0, \
	.E_SG = 0, \
	.D_REQ = 0, \
	.INT_HALF = 0, \
	.INT_MAJ = 0, \
	.START = 0, \
}

rtems_status_code mpc55xx_edma_init();

rtems_status_code mpc55xx_edma_obtain_channel( int channel, uint32_t *error_status, rtems_id transfer_update);

rtems_status_code mpc55xx_edma_enable_hardware_requests( int channel, bool enable);

rtems_status_code mpc55xx_edma_enable_error_interrupts( int channel, bool enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_EDMA_H */
