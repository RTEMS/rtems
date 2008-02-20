/*
 *  Network Statistics Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2007.
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

#include <stdio.h>
#include <getopt.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/shell.h>
#include "internal.h"

static void netstats_usage()
{
  printf(
    "netstats [-vAimfpcut] where:\n"
    "  -A    print All statistics\n"
    "  -i    print Inet Routes\n"
    "  -m    print MBUF Statistics\n"
    "  -f    print IF Statistics\n"
    "  -p    print IP Statistics\n"
    "  -c    print ICMP Statistics\n"
    "  -u    print UDP Statistics\n"
    "  -t    print TCP Statistics\n"
  );
}

int rtems_shell_main_netstats(                       /* command */
  int   argc,
  char *argv[]
)
{
  int   option;
  int   doAll = 0;
  int   doInetRoutes = 0;
  int   doMBUFStats = 0;
  int   doIFStats = 0;
  int   doIPStats = 0;
  int   doICMPStats = 0;
  int   doUDPStats = 0;
  int   doTCPStats = 0;

  while ( (option = getopt( argc, argv, "Aimfpcutv")) != -1 ) {
    switch (option) {
      case 'A':    doAll = 1;         break;
      case 'i':    doInetRoutes = 1;  break;
      case 'm':    doMBUFStats = 1;   break;
      case 'f':    doIFStats = 1;     break;
      case 'p':    doIPStats = 1;     break;
      case 'c':    doICMPStats = 1;   break;
      case 'u':    doUDPStats = 1;    break;
      case 't':    doTCPStats = 1;    break;
      case '?':
      default:
        netstats_usage();
        return -1;
    }
  }
  
  if ( doInetRoutes == 1 || doAll == 1 )
    rtems_bsdnet_show_inet_routes();
  if ( doMBUFStats == 1 || doAll == 1 )
    rtems_bsdnet_show_mbuf_stats();
  if ( doIFStats == 1 || doAll == 1 )
    rtems_bsdnet_show_if_stats();
  if ( doIPStats == 1 || doAll == 1 )
    rtems_bsdnet_show_ip_stats();
  if ( doICMPStats == 1 || doAll == 1 )
    rtems_bsdnet_show_icmp_stats();
  if ( doUDPStats == 1 || doAll == 1 )
    rtems_bsdnet_show_udp_stats();
  if ( doTCPStats == 1 || doAll == 1 )
    rtems_bsdnet_show_tcp_stats();

  return 0;
}

rtems_shell_cmd_t rtems_shell_NETSTATS_Command = {
  "netstats",                                      /* name */
  "netstats [-Aimfpcutv]",                         /* usage */
  "network",                                       /* topic */
  rtems_shell_main_netstats,                       /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};
