
#ifndef NETCONFIG_H_
#define NETCONFIG_H_

#include <bsp.h>

/* external function prototypes */
extern int rtems_ppp_driver_attach(struct rtems_bsdnet_ifconfig *config,
                                   int attaching);

/* Default network interface */
static struct rtems_bsdnet_ifconfig netdriver_config = {
  "ppp0",                    /* name */
  rtems_ppp_driver_attach,   /* attach function */
  NULL,                      /* No more interfaces */
  "192.168.2.123",           /* IP address */
  "255.255.255.0",           /* IP net mask */
  NULL,                      /* Driver supplies hardware address */
  0                          /* Use default driver parameters */
};

/* Network configuration */
struct rtems_bsdnet_config rtems_bsdnet_config = {
  &netdriver_config,
  NULL,
  0,                  /* Default network task priority */
  0,                  /* Default mbuf capacity */
  0,                  /* Default mbuf cluster capacity */
  "rtems",            /* Host name */
  "xxxyyy.com",       /* Domain name */
  0,                  /* Gateway */
  0,                  /* Log host */
  { 0 },              /* Name server(s) */
  { 0 },              /* NTP server(s) */
};

#endif
