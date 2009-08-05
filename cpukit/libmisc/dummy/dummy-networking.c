/*
 *  Dummy configuration file
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

/* Loopback Network Configuration */
#if defined(RTEMS_NETWORKING)
  #include <rtems/rtems_bsdnet.h>
  #include <sys/socket.h>
  #include <netinet/in.h>

  extern int rtems_bsdnet_loopattach(struct rtems_bsdnet_ifconfig *, int);

  static struct rtems_bsdnet_ifconfig loopback_config = {
      "lo0",                     /* name */
      rtems_bsdnet_loopattach,   /* attach function */
      NULL,                      /* link to next interface */
      "127.0.0.1",               /* IP address */
      "255.0.0.0",               /* IP net mask */
      0,                         /* hardware_address */
      0, 0, 0, 0,
      0, 0, 0,
      0
  };

  struct rtems_bsdnet_config rtems_bsdnet_config = {
      &loopback_config,       /* Network interface */
      NULL,                   /* Use fixed network configuration */
      0,                      /* Default network task priority */
      0,                      /* Default mbuf capacity */
      0,                      /* Default mbuf cluster capacity */
      "testSystem",           /* Host name */
      "nowhere.com",          /* Domain name */
      "127.0.0.1",            /* Gateway */
      "127.0.0.1",            /* Log host */
      {"127.0.0.1" },         /* Name server(s) */
      {"127.0.0.1" },         /* NTP server(s) */
      1,                      /* sb_efficiency */
      0,                      /* udp_tx_buf_size */
      0,                      /* udp_rx_buf_size */
      0,                      /* tcp_tx_buf_size */
      0                       /* tcp_rx_buf_size */
  };
#endif
