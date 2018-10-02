#include <machine/rtems-bsd-kernel-space.h>

/*
  Description: Wrapper around DHCP client to restart it when the interface
               moves to another network.

  Authors: Arnout Vandecappelle <arnout@mind.be>, Essensium/Mind
           Maarten Van Es <maarten@mind.be>, Essensium/Mind
  (C) Septentrio 2008

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.org/license/LICENSE.


  To use this functionality, call rtems_bsdnet_do_dhcp_failsafe() or set it
  as the bootp member of the rtems_bsdnet_config structure.

  The rtems_bsdnet_do_dhcp_failsafe() function provides the following
  functionalities:

  * It starts DHCP on the first non-loopback, non-point-to-point interface.
    Before DHCP is started, any existing IP address on that interface is
    removed, as well as the default route.

  * If DHCP fails to acquire an address for whatever reason, the interface
    is reconfigured with the static address provided in its
    rtems_bsdnet_ifconfig structure, and the default route from
    rtems_bsdnet_config is restored.
    It is possible to change the address in the rtems_bsdnet_ifconfig structure
    while the system is running.

  * Optionally, after the interface is configured (either with DHCP or
    statically), a task is started to monitor it.  When the interface remains
    disconnected (i.e. its IFF_RUNNING flag is off) for network_fail_timeout
    seconds, the dhcp lease renewal is stopped.  As soon as the interface is
    connected again, DHCP is started again as above.
    If network_fail_timeout is set to 0, the monitor task is not started.

  * Optionally, when the interface is disconnected, it is also brought down
    for network_down_time seconds.  Since this possibly makes the IFF_RUNNING
    flag unuseable, the interface is brought up again before the flag is
    polled.
    If network_down_time is set to 0, the interface is not brought down.

  * Optionally, before DHCP is started, we wait for broadcast_delay seconds.
    This is necessary to allow some routers to perform spanning tree discovery.

  Note that DHCP doesn't work well with multiple interfaces: only one of them
  is configured using DHCP, and we can't guarantee it's the same one that is
  monitored by this function.

*/

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_bsdnet_internal.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <rtems/dhcp.h>
#include <rtems/rtems_dhcp_failsafe.h>

struct  proc;                   /* Unused parameter of some functions. */
#include <sys/sockio.h>
#include <sys/socket.h>
#include <net/route.h>
#include <netinet/in.h>         /* for sockaddr_in */
#include <net/if.h>             /* for if.h */
#include <net/if_var.h>         /* for in_var.h */
#include <netinet/in_var.h>     /* for in_aliasreq */
#include <sys/sockio.h>         /* for ioctl definitions */

static int network_fail_timeout = RTEMS_DHCP_FAILSAFE_NETWORK_FAIL_TIMEOUT;
static int network_down_time = RTEMS_DHCP_FAILSAFE_NETWORK_DOWN_TIME;
static int broadcast_delay = RTEMS_DHCP_FAILSAFE_BROADCAST_DELAY;
static int dhcp_monitor_priority = RTEMS_DHCP_FAILSAFE_DHCP_MONITOR_PRIORITY;

void rtems_bsdnet_dhcp_failsafe_config(
  int network_fail_timeout_,
  int network_down_time_,
  int broadcast_delay_,
  int dhcp_monitor_priority_
)
{
  network_fail_timeout = network_fail_timeout_;
  network_down_time = network_down_time_;
  broadcast_delay = broadcast_delay_;
  dhcp_monitor_priority = dhcp_monitor_priority_;
}

/*
 *  returns 0 when successful, negative value for failure
 */
static int remove_address(const char *if_name)
{
  struct sockaddr_in address;
  struct in_aliasreq ifra;
  int retval = 0;

  memset (&address, '\0', sizeof (address));
  address.sin_len = sizeof (address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;

  /* Remove old default route to 0.0.0.0 */
  if (rtems_bsdnet_rtrequest (RTM_DELETE, (struct sockaddr *)&address, NULL,
                                          (struct sockaddr *)&address,
                              (RTF_UP | RTF_STATIC), NULL) < 0 ) {
    printf ("Failed to delete default route: %s (%d)\n", strerror (errno), errno);
    retval = -1;
  }

  /* Remove old ip-address */
  if (rtems_bsdnet_ifconfig(if_name, SIOCGIFADDR, &address) < 0)  {
    printf ("Failed to get if address: %s (%d)\n", strerror (errno), errno);
    return -1;
  }

  strncpy (ifra.ifra_name, if_name, IFNAMSIZ);
  memcpy (&ifra.ifra_addr, &address, sizeof(address));
  if (rtems_bsdnet_ifconfig(if_name, SIOCDIFADDR, &ifra)) {
    printf ("Can't delete if address: %s (%d)\n", strerror (errno), errno);
    return -1;
  }

  return retval;
}


static int
dhcp_if_down (const char* ifname)
{
  int16_t flags;
  if (rtems_bsdnet_ifconfig (ifname, SIOCGIFFLAGS, &flags) < 0) {
    printf ("Can't get flags for %s: %s\n", ifname, strerror (errno));
    return -1;
  }
  if (flags & IFF_UP) {
    flags &= ~IFF_UP;
    if (rtems_bsdnet_ifconfig (ifname, SIOCSIFFLAGS, &flags) < 0) {
      printf ("Can't bring %s down: %s\n", ifname, strerror (errno));
      return -1;
    }
  }

  return 0;
}

static int
dhcp_if_up (const char* ifname)
{
  int16_t flags;
  if (rtems_bsdnet_ifconfig (ifname, SIOCGIFFLAGS, &flags) < 0) {
    printf ("Can't get flags for %s: %s\n", ifname, strerror (errno));
    return -1;
  }
  if (!(flags & IFF_UP)) {
    flags |= IFF_UP;
    if (rtems_bsdnet_ifconfig (ifname, SIOCSIFFLAGS, &flags) < 0) {
      printf ("Can't bring %s up: %s\n", ifname, strerror (errno));
      return -1;
    }
  }

  return 0;
}


static int
set_static_address (struct rtems_bsdnet_ifconfig *ifp)
{
  int16_t flags;
  struct sockaddr_in address;
  struct sockaddr_in netmask;
  struct sockaddr_in broadcast;
  struct sockaddr_in gateway;

  if (ifp->ip_address != NULL) {
    printf("Setting static address for interface %s.\n", ifp->name);

    /*
     * Bring interface up
     */
    if (dhcp_if_up (ifp->name) < 0)
      return -1;

    /*
     * Set interface netmask
     */
    memset (&netmask, '\0', sizeof netmask);
    netmask.sin_len = sizeof netmask;
    netmask.sin_family = AF_INET;
    netmask.sin_addr.s_addr = inet_addr (ifp->ip_netmask);
    if (rtems_bsdnet_ifconfig (ifp->name, SIOCSIFNETMASK, &netmask) < 0) {
      printf ("Can't set %s netmask: %s\n", ifp->name, strerror (errno));
      return -1;
    }

    /*
     * Set interface address
     */
    memset (&address, '\0', sizeof address);
    address.sin_len = sizeof address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr (ifp->ip_address);
    if (rtems_bsdnet_ifconfig (ifp->name, SIOCSIFADDR, &address) < 0) {
      printf ("Can't set %s address: %s\n", ifp->name, strerror (errno));
      return -1;
    }

    /*
     * Set interface broadcast address if the interface has the
     * broadcast flag set.
     */
    if (rtems_bsdnet_ifconfig (ifp->name, SIOCGIFFLAGS, &flags) < 0) {
      printf ("Can't read %s flags: %s\n", ifp->name, strerror (errno));
      return -1;
    }
    if (flags & IFF_BROADCAST) {
      memset (&broadcast, '\0', sizeof broadcast);
      broadcast.sin_len = sizeof broadcast;
      broadcast.sin_family = AF_INET;
      broadcast.sin_addr.s_addr = address.sin_addr.s_addr | ~netmask.sin_addr.s_addr;

      if (rtems_bsdnet_ifconfig (ifp->name, SIOCSIFBRDADDR, &broadcast) < 0) {
        struct in_addr in_addr;
        char buf[20];

        in_addr.s_addr = broadcast.sin_addr.s_addr;
        if (!inet_ntop (AF_INET, &in_addr, buf, sizeof (buf)))
            strcpy (buf, "?.?.?.?");
        printf ("Can't set %s broadcast address %s: %s\n", ifp->name, buf, strerror (errno));
      }
    }
  }

  /*
   * Set default route
   */
  if (rtems_bsdnet_config.gateway) {
    address.sin_addr.s_addr = INADDR_ANY;
    netmask.sin_addr.s_addr = INADDR_ANY;
    memset (&gateway, '\0', sizeof gateway);
    gateway.sin_len = sizeof gateway;
    gateway.sin_family = AF_INET;
    gateway.sin_addr.s_addr = inet_addr (rtems_bsdnet_config.gateway);

    if (rtems_bsdnet_rtrequest (RTM_ADD,
                                (struct sockaddr *) &address,
                                (struct sockaddr *) &gateway,
                                (struct sockaddr *) &netmask,
                                (RTF_UP | RTF_GATEWAY | RTF_STATIC),
                                NULL
                              )  < 0) {
      printf ("Can't set default route: %s\n", strerror (errno));
      return -1;
    }
  }

  return 0;
}

static void
do_dhcp_init (struct rtems_bsdnet_ifconfig *ifp)
{
  if (broadcast_delay) {
    /* Wait before sending broadcast. */
    rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(broadcast_delay * 1000));
  }

  printf ("starting dhcp client...\n");

  remove_address(ifp->name);
  if (rtems_bsdnet_do_dhcp_timeout () != 0) {
    remove_address(ifp->name);
    set_static_address (ifp);      /* use static ip-address if dhcp failed */
  }

}

/*
 * Main dhcp monitor thread
 */
static void dhcp_monitor_task (rtems_task_argument ifp_arg)
{
  struct rtems_bsdnet_ifconfig *ifp = (struct rtems_bsdnet_ifconfig *)ifp_arg;
  char                         *ifname = ifp->name;
  unsigned int                  downcount = 0;
  int16_t                       ifflags;
  int                           must_renew = FALSE;

  while (TRUE) {
    if (rtems_bsdnet_ifconfig(ifname, SIOCGIFFLAGS, &ifflags) < 0)  {
      printf ("Failed to get if flags: %s (%d)\n", strerror (errno), errno);
      goto error_out;
    }

    if ((ifflags & IFF_RUNNING) != 0) {
      if(must_renew) {
        must_renew = FALSE;
        do_dhcp_init(ifp);
      }
      downcount = 0;
    } else {
      if (downcount < network_fail_timeout) {
        downcount++;

        if (downcount == network_fail_timeout) {
          printf ("lost network connection...\n");
          rtems_bsdnet_dhcp_down ();
          must_renew = TRUE;
          dhcp_if_down(ifname);
          rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(network_down_time * 1000));
          dhcp_if_up(ifname);
          downcount = 0;
        }
      }
    }

    rtems_task_wake_after(RTEMS_MILLISECONDS_TO_TICKS(1000));
  }

error_out:
  printf("Stopping dhcp monitoring application.\n");
  rtems_task_exit();
}

/*
* initialize dhcp monitoring application
*   start dhcp monitoring thread
*/
void rtems_bsdnet_do_dhcp_failsafe (void)
{
  rtems_status_code             sc;
  rtems_id                      id;
  struct rtems_bsdnet_ifconfig *ifp;
  int16_t                       ifflags;

  /* Find a suitable interface */
  for (ifp = rtems_bsdnet_config.ifconfig; ifp; ifp = ifp->next) {
    if (rtems_bsdnet_ifconfig (ifp->name, SIOCGIFFLAGS, &ifflags) < 0)
      continue;
    if ((ifflags & (IFF_LOOPBACK | IFF_POINTOPOINT)) == 0)
      break;
  }
  if (ifp == NULL){
    printf ("dhcpc_failsafe: no suitable interface\n");
    return;
  }
  printf("starting dhcp on interface %s\n", ifp->name);
  do_dhcp_init(ifp);

  if (network_fail_timeout) {
    sc = rtems_task_create (rtems_build_name ('d','h','c','m'),
                            dhcp_monitor_priority,
                            2048,
                            RTEMS_PREEMPT |
                            RTEMS_NO_TIMESLICE |
                            RTEMS_NO_ASR |
                            RTEMS_INTERRUPT_LEVEL (0),
                            RTEMS_LOCAL,
                            &id);

    if (sc != RTEMS_SUCCESSFUL) {
      printf ("Failed to create dhcp monitor task, code %d\n", sc);
      return;
    }

    sc = rtems_task_start (id, dhcp_monitor_task, (rtems_task_argument) ifp);

    if (sc != RTEMS_SUCCESSFUL) {
      printf ("Failed to start dhcp monitor task, code %d\n", sc);
    }
  }
}

