#ifndef RTEMS_BSDNET_IF_GFE_PUBLIC_SYMBOLS_H
#define RTEMS_BSDNET_IF_GFE_PUBLIC_SYMBOLS_H

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <bsp/early_enet_link_status.h>
#include <net/ethernet.h>

#ifdef __cplusplus
  extern "C" {
#endif

extern int
rtems_gfe_attach(struct rtems_bsdnet_ifconfig *, int);


/* enet_addr must be 6 bytes long */
int
rtems_gfe_setup(int unit, char *enet_addr, uint32_t base_addr);

extern rtems_bsdnet_early_link_check_ops
rtems_gfe_early_link_check_ops;

#ifdef __cplusplus
  }
#endif

#endif


