/*
 *  IFCONFIG Command Implmentation
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
#include <errno.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>


#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_ifconfig(
  int   argc,
  char *argv[]
)
{
  struct sockaddr_in  ipaddr;
  struct sockaddr_in  dstaddr;
  struct sockaddr_in  netmask;
  struct sockaddr_in  broadcast;
  char               *iface;
  int                 f_ip        = 0;
  int                 f_ptp       = 0;
  int                 f_netmask   = 0;
  int                 f_up        = 0;
  int                 f_down      = 0;
  int                 f_bcast     = 0;
  int                 cur_idx;
  int                 rc;
  int                 flags;

  memset(&ipaddr, 0, sizeof(ipaddr));
  memset(&dstaddr, 0, sizeof(dstaddr));
  memset(&netmask, 0, sizeof(netmask));
  memset(&broadcast, 0, sizeof(broadcast));

  ipaddr.sin_len = sizeof(ipaddr);
  ipaddr.sin_family = AF_INET;

  dstaddr.sin_len = sizeof(dstaddr);
  dstaddr.sin_family = AF_INET;

  netmask.sin_len = sizeof(netmask);
  netmask.sin_family = AF_INET;

  broadcast.sin_len = sizeof(broadcast);
  broadcast.sin_family = AF_INET;

  cur_idx = 0;
  if (argc <= 1) {
    /* display all interfaces */
    iface = NULL;
    cur_idx += 1;
  } else {
    iface = argv[1];
    if (isdigit((unsigned char)*argv[2])) {
      if (inet_pton(AF_INET, argv[2], &ipaddr.sin_addr) < 0) {
        printf("bad ip address: %s\n", argv[2]);
        return 0;
      }
      f_ip = 1;
      cur_idx += 3;
    } else {
      cur_idx += 2;
    }
  }

  if ((f_down !=0) && (f_ip != 0)) {
    f_up = 1;
  }

  while(argc > cur_idx) {
    if (strcmp(argv[cur_idx], "up") == 0) {
      f_up = 1;
      if (f_down != 0) {
        printf("Can't make interface up and down\n");
      }
    } else if(strcmp(argv[cur_idx], "down") == 0) {
      f_down = 1;
      if (f_up != 0) {
        printf("Can't make interface up and down\n");
        }
    } else if(strcmp(argv[cur_idx], "netmask") == 0) {
      if ((cur_idx + 1) >= argc) {
        printf("No netmask address\n");
        return -1;
      }
      if (inet_pton(AF_INET, argv[cur_idx+1], &netmask.sin_addr) < 0) {
        printf("bad netmask: %s\n", argv[cur_idx]);
        return -1;
      }
      f_netmask = 1;
      cur_idx += 1;
    } else if(strcmp(argv[cur_idx], "broadcast") == 0) {
      if ((cur_idx + 1) >= argc) {
        printf("No broadcast address\n");
        return -1;
      }
      if (inet_pton(AF_INET, argv[cur_idx+1], &broadcast.sin_addr) < 0) {
        printf("bad broadcast: %s\n", argv[cur_idx]);
        return -1;
      }
      f_bcast = 1;
      cur_idx += 1;
    } else if(strcmp(argv[cur_idx], "pointopoint") == 0) {
      if ((cur_idx + 1) >= argc) {
        printf("No pointopoint address\n");
        return -1;
      }
      if (inet_pton(AF_INET, argv[cur_idx+1], &dstaddr.sin_addr) < 0) {
        printf("bad pointopoint: %s\n", argv[cur_idx]);
        return -1;
      }
      f_ptp = 1;
      cur_idx += 1;
    } else {
      printf("Bad parameter: %s\n", argv[cur_idx]);
      return -1;
    }
    cur_idx += 1;
  }

  printf("ifconfig ");
  if (iface != NULL) {
    printf("%s ", iface);
    if (f_ip != 0) {
      char str[256];
      inet_ntop(AF_INET, &ipaddr.sin_addr, str, 256);
      printf("%s ", str);
    }

    if (f_netmask != 0) {
      char str[256];
      inet_ntop(AF_INET, &netmask.sin_addr, str, 256);
      printf("netmask %s ", str);
    }

    if (f_bcast != 0) {
      char str[256];
      inet_ntop(AF_INET, &broadcast.sin_addr, str, 256);
      printf("broadcast %s ", str);
    }

    if (f_ptp != 0) {
      char str[256];
      inet_ntop(AF_INET, &dstaddr.sin_addr, str, 256);
      printf("pointopoint %s ", str);
    }

    if (f_up != 0) {
      printf("up\n");
    } else if (f_down != 0) {
      printf("down\n");
    } else {
      printf("\n");
    }
  }

  if ((iface == NULL) || ((f_ip == 0) && (f_down == 0) && (f_up == 0))) {
    rtems_bsdnet_show_if_stats();
    return 0;
  }

  flags = 0;
  if (f_netmask) {
    rc = rtems_bsdnet_ifconfig(iface, SIOCSIFNETMASK, &netmask);
    if (rc < 0) {
      printf("Could not set netmask: %s\n", strerror(errno));
      return -1;
    }
  }

  if (f_bcast) {
    rc = rtems_bsdnet_ifconfig(iface, SIOCSIFBRDADDR, &broadcast);
    if (rc < 0) {
      printf("Could not set broadcast: %s\n", strerror(errno));
      return -1;
    }
  }

  if (f_ptp) {
    rc = rtems_bsdnet_ifconfig(iface, SIOCSIFDSTADDR, &dstaddr);
    if (rc < 0) {
      printf("Could not set destination address: %s\n", strerror(errno));
      return -1;
    }
    flags |= IFF_POINTOPOINT;
  }

  /* This must come _after_ setting the netmask, broadcast addresses */
  if (f_ip) {
    rc = rtems_bsdnet_ifconfig(iface, SIOCSIFADDR, &ipaddr);
    if (rc < 0) {
      printf("Could not set IP address: %s\n", strerror(errno));
      return -1;
    }
  }

  if (f_up != 0) {
    flags |= IFF_UP;
  }

  if (f_down != 0) {
    printf("Warning: taking interfaces down is not supported\n");
  }

  rc = rtems_bsdnet_ifconfig(iface, SIOCSIFFLAGS, &flags);
  if (rc < 0) {
    printf("Could not set interface flags: %s\n", strerror(errno));
    return -1;
  }

  return 0;
}

rtems_shell_cmd_t rtems_shell_IFCONFIG_Command = {
  "ifconfig",                                      /* name */
  "TBD",                                           /* usage */
  "network",                                       /* topic */
  rtems_shell_main_ifconfig,                       /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};
