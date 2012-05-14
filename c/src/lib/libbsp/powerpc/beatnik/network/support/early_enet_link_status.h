#ifndef BSP_EARLY_ENET_LINK_STATUS_H
#define BSP_EARLY_ENET_LINK_STATUS_H

/* Determine link status of ethernet device before network is initialized */

/* T. Straumann, 2005; see ../../LICENSE */

#include <rtems.h>

#ifdef __cplusplus
  extern "C" {
#endif

typedef struct {
	int				(*init)(int idx);	/* perform enough initialization to access (default) phy */
	int 			(*read_phy)(int idx, unsigned reg);
	int 			(*write_phy)(int idx, unsigned reg, unsigned val);
	const char 		*name;				/* driver name */
	unsigned char 	num_slots;			/* max number of supported devices */
	unsigned char	initialized;		/* must be initialized to 0;       */
} rtems_bsdnet_early_link_check_ops;

int
BSP_early_check_link_status(int unit, rtems_bsdnet_early_link_check_ops *ops);

#ifdef __cplusplus
  }
#endif

#endif
