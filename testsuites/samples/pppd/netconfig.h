#ifndef NETCONFIG_H_
#define NETCONFIG_H_

#include <bsp.h>
#include <rtems/rtemspppd.h>

/* Default network interface */
static struct rtems_bsdnet_ifconfig netdriver_config = {
  "ppp0",                    /* name */
  rtems_ppp_driver_attach,   /* attach function */
  NULL,                      /* No more interfaces */
  NULL,                      /* IP address */
  NULL,                      /* IP net mask */
  NULL,                      /* Driver supplies hardware address */
  0                          /* Use default driver parameters */
};

/* Network configuration */
struct rtems_bsdnet_config rtems_bsdnet_config = {
  &netdriver_config,
  NULL,
  30,                 /* Default network task priority */
  (256UL *1024UL),    /* Default mbuf capacity */
  (512UL *1024UL),    /* Default mbuf cluster capacity */
  0,                  /* Host name */
  0,                  /* Domain name */
  0,                  /* Gateway */
  0,                  /* Log host */
  { 0 },              /* Name server(s) */
  { 0 },              /* NTP server(s) */
};

#endif
