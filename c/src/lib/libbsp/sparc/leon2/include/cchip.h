#ifndef __CCHIP_H__
#define __CCHIP_H__

#include <b1553brm_pci.h>
#include <occan_pci.h>
#include <grspw_pci.h>
#include <apbuart_pci.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CCHIP_IRQ 4

/* Register all drivers supported by the Companion Chip board */
int cchip_register(void);

#ifdef __cplusplus
}
#endif

#endif
