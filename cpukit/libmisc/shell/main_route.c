/*
 *  ROUTE Command Implmentation
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/route.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_route(
  int   argc,
  char *argv[]
)
{
  int                cmd;
  struct sockaddr_in dst;
  struct sockaddr_in gw;
  struct sockaddr_in netmask;
  int                f_host;
  int                f_gw       = 0;
  int                cur_idx;
  int                flags;
  int                rc;

  memset(&dst, 0, sizeof(dst));
  memset(&gw, 0, sizeof(gw));
  memset(&netmask, 0, sizeof(netmask));

  dst.sin_len = sizeof(dst);
  dst.sin_family = AF_INET;
  dst.sin_addr.s_addr = inet_addr("0.0.0.0");

  gw.sin_len = sizeof(gw);
  gw.sin_family = AF_INET;
  gw.sin_addr.s_addr = inet_addr("0.0.0.0");

  netmask.sin_len = sizeof(netmask);
  netmask.sin_family = AF_INET;
  netmask.sin_addr.s_addr = inet_addr("255.255.255.0");

  if (argc < 2) {
    rtems_bsdnet_show_inet_routes();
    return 0;
  }

  if (strcmp(argv[1], "add") == 0) {
    cmd = RTM_ADD;
  } else if (strcmp(argv[1], "del") == 0) {
    cmd = RTM_DELETE;
  } else {
    printf("invalid command: %s\n", argv[1]);
    printf("\tit should be 'add' or 'del'\n");
    return -1;
  }

  if (argc < 3) {
    printf("not enough arguments\n");
    return -1;
  }

  if (strcmp(argv[2], "-host") == 0) {
    f_host = 1;
  } else if (strcmp(argv[2], "-net") == 0) {
    f_host = 0;
  } else {
    printf("Invalid type: %s\n", argv[1]);
    printf("\tit should be '-host' or '-net'\n");
    return -1;
  }

  if (argc < 4) {
    printf("not enough arguments\n");
    return -1;
  }

  inet_pton(AF_INET, argv[3], &dst.sin_addr);

  cur_idx = 4;
  while(cur_idx < argc) {
    if (strcmp(argv[cur_idx], "gw") == 0) {
      if ((cur_idx +1) >= argc) {
        printf("no gateway address\n");
        return -1;
      }
      f_gw = 1;
      inet_pton(AF_INET, argv[cur_idx + 1], &gw.sin_addr);
      cur_idx += 1;
    } else if(strcmp(argv[cur_idx], "netmask") == 0) {
      if ((cur_idx +1) >= argc) {
        printf("no netmask address\n");
        return -1;
      }
      f_gw = 1;
      inet_pton(AF_INET, argv[cur_idx + 1], &netmask.sin_addr);
      cur_idx += 1;
    } else {
      printf("Unknown argument\n");
      return -1;
    }
    cur_idx += 1;
  }

  flags = RTF_STATIC;
  if (f_gw != 0) {
    flags |= RTF_GATEWAY;
  }
  if (f_host != 0) {
    flags |= RTF_HOST;
  }

  rc = rtems_bsdnet_rtrequest(
    cmd,
    (struct sockaddr *)&dst,
    (struct sockaddr *)&gw,
    (struct sockaddr *)&netmask,
    flags,
    NULL
  );
  if (rc < 0) {
    printf("Error adding route\n");
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_ROUTE_Command = {
  "route",                                         /* name */
  "TBD",                                           /* usage */
  "network",                                       /* topic */
  rtems_shell_main_route,                          /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};
