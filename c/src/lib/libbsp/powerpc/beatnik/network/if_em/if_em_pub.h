#ifndef RTEMS_BSDNET_IF_EM_PUBLIC_SYMBOLS_H
#define RTEMS_BSDNET_IF_EM_PUBLIC_SYMBOLS_H

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <bsp/early_enet_link_status.h>

#ifdef __cplusplus
  extern "C" {
#endif

extern int                               rtems_em_attach(struct rtems_bsdnet_ifconfig *, int);
extern int                               rtems_em_pci_setup(int);
extern rtems_bsdnet_early_link_check_ops rtems_em_early_link_check_ops;

#ifdef __cplusplus
  }
#endif

#endif


