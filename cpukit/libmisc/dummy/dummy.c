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

int main( int, char **, char **);

/* configuration information */

/* This is enough to get a basic main() up. */
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 10
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
#define CONFIGURE_INIT_TASK_ENTRY_POINT   (void *)main

/* Include basic device drivers needed to call delays */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

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
  };
#endif

