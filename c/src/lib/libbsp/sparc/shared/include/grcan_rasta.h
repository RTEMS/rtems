#ifndef __GRCAN_RASTA_H__
#define __GRCAN_RASTA_H__

#include <grcan.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Registers the GRCAN for RASTA
 *
 * rambase is address of the first GRCAN core has it's TX buffer, followed by
 * it's RX buffer
 */
int grcan_rasta_ram_register(struct ambapp_bus *abus, int rambase);

extern void (*grcan_rasta_int_reg)(void *handler, int irq, void *arg);

#ifdef __cplusplus
}
#endif

#endif
