/*
 *  Network Statistics Shell Command Implmentation
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#define __need_getopt_newlib
#include <getopt.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/shell.h>
#include "internal.h"

static void netstats_usage(void)
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

static int rtems_shell_main_netstats(                       /* command */
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
  int   verbose = 0;
  struct getopt_data getopt_reent;

  memset(&getopt_reent, 0, sizeof(getopt_data));
  while ( (option = getopt_r( argc, argv, "Aimfpcutv", &getopt_reent)) != -1 ) {

    switch ((char)option) {
      case 'A': doAll = 1;        break;
      case 'i': doInetRoutes = 1; break;
      case 'm': doMBUFStats = 1;  break;
      case 'f': doIFStats = 1;    break;
      case 'p': doIPStats = 1;    break;
      case 'c': doICMPStats = 1;  break;
      case 'u': doUDPStats = 1;   break;
      case 't': doTCPStats = 1;   break;
      case 'v': verbose = 1;      break;
      case '?':
      default:
        netstats_usage();
        return -1;
    }
  }

  if ( verbose ) {
    printf(
      "doAll=%d\n"
      "doInetRoutes=%d\n"
      "doMBUFStats=%d\n"
      "doIFStats=%d\n"
      "doIPStats=%d\n"
      "doICMPStats=%d\n"
      "doUDPStats=%d\n"
      "doTCPStats=%d\n",
      doAll,
      doInetRoutes,
      doMBUFStats,
      doIFStats,
      doIPStats,
      doICMPStats,
      doUDPStats,
      doTCPStats
    );
  }

  if ( doInetRoutes == 1 || doAll == 1 ) {
    rtems_bsdnet_show_inet_routes();
  }

  if ( doMBUFStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_mbuf_stats();
  }

  if ( doIFStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_if_stats();
  }

  if ( doIPStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_ip_stats();
  }

  if ( doICMPStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_icmp_stats();
  }

  if ( doUDPStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_udp_stats();
  }

  if ( doTCPStats == 1 || doAll == 1 ) {
    rtems_bsdnet_show_tcp_stats();
  }

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
