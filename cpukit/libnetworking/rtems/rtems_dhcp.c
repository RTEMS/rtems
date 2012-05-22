/*	
 *  DCHP client for RTEMS
 *  Andrew Bythell, <abythell@nortelnetworks.com>
 *  based on and uses subroutines from c/src/libnetworking/nfs/bootp_subr.c
 */

/*
 * DHCP task added.
 * Brendan Gannon, <bgannon@cybertec.com.au>
 */

/*
 * Added interface to terminate DHCP task, and removed panics.
 * Arnout Vandecappelle <arnout@mind.be>, Essensium/Mind
 * Maarten Van Es <maarten@mind.be>, Essensium/Mind
 */

/*
 * Copyright (c) 1995 Gordon Ross, Adam Glass
 * Copyright (c) 1992 Regents of the University of California.
 * All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * WARNING:
 *   This file should be moved into c/src/libnetworking/nfs
 *   and the following two #ifndef...#endif blocks and the #undefs at
 *   the end of the file should be removed
 */

#ifndef __INSIDE_RTEMS_BSD_TCPIP_STACK__
#define __INSIDE_RTEMS_BSD_TCPIP_STACK__
#endif

#ifndef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#endif

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/bsdnet/servers.h>

#include <string.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/param.h>		/* for MAXHOSTNAMELEN */
#include <sys/systm.h>
#include <sys/socketvar.h>	/* for socreat() soclose() */
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_var.h>
#include <netinet/in.h>		/* for NBO-HBO conversions */
#include <net/if_types.h>	/* for IFT_ETHER */
#include <net/if_dl.h>		/* for LLADDR */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <rtems/mkrootfs.h>

#include "rtems/dhcp.h"
#include "rtems/bootp.h"

#ifndef EALEN
#define EALEN 6
#endif

/*
 *DHCP flags
 */
#define DHCP_BROADCAST 0x8000
#define DHCP_UNICAST   0x0000

/*
 * DHCP Op Codes
 */
#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY   2

/*
 * DHCP Messages
 */
#define DHCP_DISCOVER 1
#define DHCP_OFFER    2
#define DHCP_REQUEST  3
#define DHCP_DECLINE  4
#define DHCP_ACK      5
#define DHCP_NACK     6
#define DHCP_RELEASE  7

/*
 * DHCP Options
 */
#define DHCP_OPTION_PAD    0
#define DHCP_SUBNET        1
#define DHCP_GATEWAY       3
#define DHCP_DNS           6
#define DHCP_HOST          12
#define DHCP_DOMAIN_NAME   15
#define DHCP_NETMASK       28
#define DHCP_REQUESTED_IP  50
#define DHCP_LEASE         51
#define DHCP_MESSAGE       53
#define DHCP_SERVER        54
#define DHCP_PARAMETERS    55
#define DHCP_OPTION_END    255

/*
 * Definitions from RFC
 */
struct dhcp_packet
{
  u_int8_t       op;
  u_int8_t       htype;
  u_int8_t       hlen;
  u_int8_t       hops;
  u_int32_t      xid;
  u_int16_t      secs;
  u_int16_t      flags;
  struct in_addr ciaddr;
  struct in_addr yiaddr;
  struct in_addr siaddr;
  struct in_addr giaddr;
  unsigned char  chaddr[16];
  char           sname[64];
  char           file[128];
  unsigned char  vend[312];
};

/*
 * Variables
 */
static int                dhcp_option_overload = 0;
static char               dhcp_gotgw = 0;
static char               dhcp_gotnetmask = 0;
static char               dhcp_gotserver = 0;
static char               dhcp_gotlogserver = 0;
static struct sockaddr_in dhcp_netmask;
static struct sockaddr_in dhcp_gw;
static char               *dhcp_hostname;
static int                dhcp_message_type = 0;
static unsigned long      dhcp_lease_time;
static unsigned long      dhcp_elapsed_time = 0;
static const char         dhcp_magic_cookie[4] = { 99, 130, 83, 99 };
static const char         dhcp_request_parameters[5] = { DHCP_SUBNET,
                                                         DHCP_GATEWAY,
                                                         DHCP_DNS,
                                                         DHCP_HOST,
                                                         DHCP_DOMAIN_NAME };
#define NUM_NAMESERVERS \
  (sizeof rtems_bsdnet_config.name_server / sizeof rtems_bsdnet_config.name_server[0])
static struct in_addr rtems_dhcpd_nameserver[NUM_NAMESERVERS];
static int rtems_dhcpd_nameserver_count = 0;

/*
 * Clean any DNS entries add by a DHCP request.
 */
static void
clean_dns_entries (void)
{
  int e;
  for (e = 0; e < rtems_dhcpd_nameserver_count; ++e)
  {
    int n;
    for (n = 0; n < rtems_bsdnet_nameserver_count; ++ n)
    {
      if (memcmp (&rtems_dhcpd_nameserver[e], &rtems_bsdnet_nameserver[n], 4) == 0)
      {
        if (n < (NUM_NAMESERVERS - 1))
          memmove (&rtems_bsdnet_nameserver[n],
                   &rtems_bsdnet_nameserver[n + 1],
                   (NUM_NAMESERVERS - n - 1) * 4);
        --rtems_bsdnet_nameserver_count;
      }
    }
  }
  rtems_dhcpd_nameserver_count = 0;
}

/*
 * Format an IP address in dotted decimal.
 */
static void
format_ip (unsigned long ip, char* buffer)
{
  sprintf (buffer,
           "%lu.%lu.%lu.%lu",
          (ip >> 24),
          (ip >> 16) & 0xff,
          (ip >>  8) & 0xff,
          (ip        & 0xff));

  return;
}

/*
 * Print the IP setup
 */
static void
printsetup (const char     *iface,
            struct in_addr ip_addr,
            struct in_addr mask_addr,
            struct in_addr srv_addr,
            struct in_addr gw_addr)
{
  unsigned long ip;
  char ip_str[15];

  printf ("dhcpc: %s: ", iface);

  ip = ntohl (ip_addr.s_addr);
  format_ip (ip, ip_str);
  printf ("inet: %-15s ", ip_str);

  ip = ntohl (mask_addr.s_addr);
  format_ip (ip, ip_str);
  printf ("mask: %-15s\n", ip_str);

  ip = ntohl (srv_addr.s_addr);
  format_ip (ip, ip_str);
  printf   ("             srv: %-15s ", ip_str);

  ip = ntohl (gw_addr.s_addr);
  format_ip (ip, ip_str);
  printf ("  gw: %-15s\n", ip_str);

  return;
}

/*
 * Process options from the DHCP packet.
 * Based on BOOTP routine.
 */
static void
process_options (unsigned char *optbuf, int optbufSize)
{
  int j = 0;
  int len;
  int code, ncode;
  char *p;

  dhcp_message_type = 0;

  ncode = optbuf[0];
  while (j < optbufSize)
  {
    code = optbuf[j] = ncode;
    if (code == 255)
      return;
    if (code == 0)
    {
      j++;
      continue;
    }
    len = optbuf[j + 1];
    j += 2;

    if ((len + j) >= optbufSize)
    {
      printf ("Truncated field for code %d", code);
      return;
    }

    ncode = optbuf[j + len];
    optbuf[j + len] = '\0';
    p = (char*) &optbuf[j];
    j += len;

    /*
     * Process the option
     */
    switch (code)
    {
      case 1:
        /* Subnet mask */
        if (len != 4) {
          printf ("dhcpc: subnet mask len is %d\n", len);
          continue;
        }
        memcpy (&dhcp_netmask.sin_addr, p, 4);
        dhcp_gotnetmask = 1;
        break;

      case 2:
        /* Time offset */
        if (len != 4) {
          printf ("dhcpc: time offset len is %d\n", len);
          continue;
        }
        memcpy (&rtems_bsdnet_timeoffset, p, 4);
        rtems_bsdnet_timeoffset = ntohl (rtems_bsdnet_timeoffset);
        break;

      case 3:
        /* Routers */
        if (len % 4) {
          printf ("dhcpc: Router Len is %d\n", len);
          continue;
        }
        if (len > 0)
        {
          memcpy (&dhcp_gw.sin_addr, p, 4);
          dhcp_gotgw = 1;
        }
        break;

      case 42:
        /* NTP servers */
        if (len % 4) {
          printf ("dhcpc: time server Len is %d\n", len);
          continue;
        }
        {
          int tlen = 0;
          while ((tlen < len) &&
                 (rtems_bsdnet_ntpserver_count <
                  sizeof rtems_bsdnet_config.ntp_server /
                  sizeof rtems_bsdnet_config.ntp_server[0]))
          {
            memcpy (&rtems_bsdnet_ntpserver[rtems_bsdnet_ntpserver_count],
                    p + tlen, 4);
            rtems_bsdnet_ntpserver_count++;
            tlen += 4;
          }
        }
        break;

      case 6:
        /* Domain Name servers */
        if (len % 4) {
          printf ("dhcpc: DNS Len is %d\n", len);
          continue;
        }
        {
          int dlen = 0;
          while ((dlen < len) &&
                 (rtems_dhcpd_nameserver_count < NUM_NAMESERVERS) &&
                 (rtems_bsdnet_nameserver_count < NUM_NAMESERVERS))
          {
            memcpy (&rtems_dhcpd_nameserver
                    [rtems_dhcpd_nameserver_count], p + dlen, 4);
            rtems_dhcpd_nameserver_count++;
            memcpy (&rtems_bsdnet_nameserver
                    [rtems_bsdnet_nameserver_count], p + dlen, 4);
            rtems_bsdnet_nameserver_count++;
            dlen += 4;
          }
        }
        break;

      case 12:
        /* Host name */
        if (len >= MAXHOSTNAMELEN) {
          printf ("dhcpc: hostname >= %d bytes\n", MAXHOSTNAMELEN);
          len = MAXHOSTNAMELEN-1;
        }
        if (sethostname (p, len) < 0)
          printf ("dhcpc: can't set host name");
        if (dhcp_hostname != NULL)
        {
          dhcp_hostname = realloc (dhcp_hostname, len);
          strncpy (dhcp_hostname, p, len);
        }
        else
          dhcp_hostname = strndup (p, len);
        break;

      case 7:
        /* Log servers */
        if (len % 4) {
          printf ("dhcpc: Log server Len is %d\n", len);
          continue;
        }
        if (len > 0)
        {
          memcpy (&rtems_bsdnet_log_host_address, p, 4);
          dhcp_gotlogserver = 1;
        }
        break;

      case 15:
        /* Domain name */
        if (p[0])
        {
          rtems_bsdnet_domain_name = strdup (p);
        }
        break;

      case 16:		/* Swap server IP address. unused */
        break;

      case 50:
        /* DHCP Requested IP Address */
        if (len != 4)
          printf ("dhcpc: DHCP option requested IP len is %d", len);
        /*
         * although nothing happens here, this case keeps the client
         * from complaining about unknown options.  The Requested IP
         * is necessary to return to the server for a DHCP REQUEST
         */
        break;

      case 51:
        /* DHCP Lease Length */
        if (len != 4) {
          printf ("dhcpc: DHCP option lease-length len is %d", len);
          continue;
        }
        memcpy (&dhcp_lease_time, &p[0], 4);
        dhcp_lease_time = ntohl (dhcp_lease_time);
        break;

      case 52:
        /* DHCP option override */
        if (len != 1) {
          printf ("dhcpc: DHCP option overload len is %d", len);
          continue;
        }
        dhcp_option_overload = p[0];
        break;

      case 53:
        /* DHCP message */
        if (len != 1) {
          printf ("dhcpc: DHCP message len is %d", len);
          continue;
        }
        dhcp_message_type = p[0];
        break;

      case 128:		/* Site-specific option for DHCP servers that
                   *   a) don't supply tag 54
                   * and
                   *   b) don't supply the server address in siaddr
                   * For example, on Solaris 2.6 in.dhcpd, include in the dhcptab:
                   *    Bootsrv s Site,128,IP,1,1
                   * and use that symbol in the macro that defines the client:
                   *    Bootsrv=<tftp-server-ip-address>
                   */
      case 54:
        /* DHCP server */
        if (len != 4) {
          printf ("dhcpc: DHCP server len is %d", len);
          continue;
        }
        memcpy (&rtems_bsdnet_bootp_server_address, p, 4);
        dhcp_gotserver = 1;
        break;

      case 66:
        /* DHCP server name option */
        if (p[0])
          rtems_bsdnet_bootp_server_name = strdup (p);
        break;

      case 67:
        /* DHCP bootfile option */
        if (p[0])
          rtems_bsdnet_bootp_boot_file_name = strdup (p);
        break;

      default:
        break;
    }
  }
}

/*
 * Generate the packet for a DHCP DISCOVER.
 */
static int
dhcp_discover_req (struct dhcp_packet* call,
                   struct sockaddr_dl *sdl,
                   unsigned long *xid)
{
  int len = 0;

  memset (call, 0, sizeof (struct dhcp_packet));

  /*
   * Send a DHCP DISCOVER Message
   */
  call->op = DHCP_BOOTREQUEST;
  call->htype = 1;		/* 10mb ethernet */
  call->hlen = sdl->sdl_alen;	/* Hardware address length */
  call->hops = 0;
  (*xid)++;
  call->xid = htonl (*xid);
  call->flags = htons (DHCP_BROADCAST);

  memcpy (&call->chaddr, LLADDR (sdl), sdl->sdl_alen);

  /*
   * Magic cookie.
   */
  memcpy (&call->vend[len], dhcp_magic_cookie, sizeof (dhcp_magic_cookie));
  len += sizeof (dhcp_magic_cookie);

  /*
   * DHCP Message type.
   */
  call->vend[len++] = DHCP_MESSAGE;
  call->vend[len++] = 1;
  call->vend[len++] = DHCP_DISCOVER;

  /*
   * DHCP Parameter request list
   */
  call->vend[len++] = DHCP_PARAMETERS;
  call->vend[len++] = sizeof (dhcp_request_parameters);
  memcpy (&call->vend[len], &dhcp_request_parameters, sizeof (dhcp_request_parameters));
  len += sizeof (dhcp_request_parameters);

  /*
   * Lease time.
   */
  call->vend[len++] = DHCP_LEASE;
  call->vend[len++] = 4;
  memset (&call->vend[len], 0xFF, 4);	/* request infinite lease time */
  len += 4;

  /*
   * End.
   */
  call->vend[len++] = DHCP_OPTION_END;
  call->secs = 0;

  return len;
}

/*
 * Generate the packet for a DHCP REQUEST.
 */
static int
dhcp_request_req (struct dhcp_packet* call,
                  struct dhcp_packet* reply,
                  struct sockaddr_dl *sdl,
                  int broadcast)
{
  int           len = 0;
  unsigned long temp;
  char          *hostname;

  memset (call, 0, sizeof (struct dhcp_packet));

  /*
   * Send a DHCP REQUEST Message
   */
  call->op = DHCP_BOOTREQUEST;
  call->htype = 1;		/* 10mb ethernet */
  call->hlen = sdl->sdl_alen;	/* Hardware address length */
  call->hops = 0;
  call->xid = reply->xid;
  if (broadcast)
    call->flags = htons (DHCP_BROADCAST);
  else
  {
    call->flags = htons (DHCP_UNICAST);
    call->ciaddr = reply->yiaddr;
  }
  memcpy (&call->chaddr, LLADDR (sdl), sdl->sdl_alen);

  /*
   * Magic cookie.
   */
  memcpy (&call->vend[len], dhcp_magic_cookie, sizeof (dhcp_magic_cookie));
  len += sizeof (dhcp_magic_cookie);

  /*
   * DHCP Message type.
   */
  call->vend[len++] = DHCP_MESSAGE;
  call->vend[len++] = 1;
  call->vend[len++] = DHCP_REQUEST;

  /*
   * DHCP server
   */
  if (broadcast)
  {
    call->vend[len++] = DHCP_SERVER;
    call->vend[len++] = sizeof (rtems_bsdnet_bootp_server_address);
    memcpy (&call->vend[len], &rtems_bsdnet_bootp_server_address,
            sizeof (rtems_bsdnet_bootp_server_address));
    len += sizeof (rtems_bsdnet_bootp_server_address);
  }

  /*
   * Requested IP
   */
  call->vend[len++] = DHCP_REQUESTED_IP;
  call->vend[len++] = sizeof (reply->yiaddr);
  memcpy (&call->vend[len], &reply->yiaddr, sizeof (reply->yiaddr));
  len += sizeof (reply->yiaddr);

  /*
   * DHCP Parameter request list
   */
  call->vend[len++] = DHCP_PARAMETERS;
  call->vend[len++] = sizeof (dhcp_request_parameters);
  memcpy (&call->vend[len], &dhcp_request_parameters, sizeof (dhcp_request_parameters));
  len += sizeof (dhcp_request_parameters);

  /*
   * Lease time.
   * For the REQUEST, return the lease time the server offered.
   */
  call->vend[len++] = DHCP_LEASE;
  call->vend[len++] = 4;
  temp = htonl (dhcp_lease_time);
  memcpy (&call->vend[len], &temp, sizeof (unsigned long));
  len += 4;

  /*
   * Host name.
   */
  hostname = malloc (MAXHOSTNAMELEN, 0, M_NOWAIT);
  if (hostname != NULL)
  {
    if (gethostname (hostname, MAXHOSTNAMELEN) == 0)
    {
      call->vend[len++] = DHCP_HOST;
      call->vend[len++] = strlen (hostname);
      strcpy ((char*) &call->vend[len], hostname);
      len += strlen (hostname);
    }
    free (hostname, 0);
  }

  /*
   * End.
   */
  call->vend[len++] = DHCP_OPTION_END;
  call->secs = 0;

  return len;
}

/*
 * Variables for the DHCP task.
 */
static struct dhcp_packet dhcp_req;
static rtems_id           dhcp_task_id;

/*
 * The DHCP task counts until half the lease time has expired.
 * When this period is up, it sends a DHCP REQUEST packet to the
 * server again to renew the lease.
 * If the lease is renewed, the task starts counting again.
 * If the lease is not renewed, the task retries until it is.
 *
 * The task will not rebind if the lease is not renewed.
 */
static void
dhcp_task (rtems_task_argument _sdl)
{
  unsigned long       count;
  struct dhcp_packet  call;
  struct sockaddr_dl  *sdl;
  rtems_event_set     event_out;
  unsigned int        timeout = 0;
  int                 error;
  struct proc *procp = NULL;
  rtems_status_code   ev_st;

  sdl = (struct sockaddr_dl *) _sdl;

  count = dhcp_elapsed_time;

  while (true)
  {
    /*
     * Sleep until the next poll
     */
    timeout = TOD_MILLISECONDS_TO_TICKS (1000);
    ev_st = rtems_event_receive (RTEMS_EVENT_0,
                                 RTEMS_WAIT | RTEMS_EVENT_ANY,
                                 timeout, &event_out);

    /*
     * Check if not a poll timeout. So when ANY event received, exit task.
     * Actually, only event RTEMS_EVENT_0 sent from rtem_dhcp_failsafe.c
     * if "failsafe" dhcp enabled when interface down.  Otherwise, no
     * event should occur, just timeout.
     */
    if(ev_st != RTEMS_TIMEOUT)
        break;

    count++;

    if (count >= (dhcp_lease_time / 2))
    {
      rtems_bsdnet_semaphore_obtain ();

      dhcp_request_req (&call, &dhcp_req, sdl, true);

      /*
       * Send the Request.
       */
      error = bootpc_call ((struct bootp_packet *)&call, (struct bootp_packet *)&dhcp_req, procp);
      if (error) {
        rtems_bsdnet_semaphore_release ();
        printf ("DHCP call failed -- error %d", error);
        continue;
      }

      /*
       * Check for DHCP ACK/NACK
       */
      if (memcmp (&dhcp_req.vend[0],
                  dhcp_magic_cookie,
                  sizeof (dhcp_magic_cookie)) != 0)
      {
        rtems_bsdnet_semaphore_release ();
        printf ("DHCP server did not send Magic Cookie.\n");
        continue;
      }

      /*
       * We have an ack. Clear the DNS entries that have been assigned by a previous
       * DHCP request.
       */
      clean_dns_entries ();

      /*
       * Process this requests options.
       */
      process_options (&dhcp_req.vend[4], sizeof (dhcp_req.vend) - 4);

      if (dhcp_message_type != DHCP_ACK)
      {
        rtems_bsdnet_semaphore_release ();
        printf ("DHCP server did not accept the DHCP request");
        continue;
      }

      rtems_bsdnet_semaphore_release ();

      count = 0;
    }
  }


  dhcp_task_id = 0;
  printf ("dhcpc: exiting lease renewal task.\n");
  rtems_task_delete( RTEMS_SELF);

}

/*
 * Start the DHCP task.
 */
static rtems_status_code
dhcp_start_task (struct sockaddr_dl *sdl,
                 struct dhcp_packet *reply,
                 int priority)
{
  rtems_status_code sc;

  memcpy (&dhcp_req, reply, sizeof (struct dhcp_packet));

  sc = rtems_task_create (rtems_build_name ('d','h','c','p'),
                          priority,
                          2048,
                          RTEMS_PREEMPT |
                          RTEMS_NO_TIMESLICE |
                          RTEMS_NO_ASR |
                          RTEMS_INTERRUPT_LEVEL (0),
                          RTEMS_LOCAL,
                          &dhcp_task_id);

  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  sc = rtems_task_start (dhcp_task_id,
                         dhcp_task,
                         (rtems_task_argument) sdl);

  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  return RTEMS_SUCCESSFUL;
}

/*
 * Check if the chosen interface already has an IP.
 */
static int
dhcp_interface_has_ip (struct ifreq *ireq, struct socket *so, struct proc *procp)
{
  struct sockaddr_in* sin;
  int error;

  /*
   * Check if the interface is already up.
   */
  error = ifioctl(so, SIOCGIFFLAGS, (caddr_t)ireq, procp);
  if (error)
    return 0;

  if ((ireq->ifr_flags & IFF_UP) == 0)
    return 0;

  sin = (struct sockaddr_in *)&ireq->ifr_addr;
  bzero ((caddr_t)sin, sizeof (struct sockaddr_in));
  sin->sin_len = sizeof (struct sockaddr_in);
  sin->sin_family = AF_INET;
  error = ifioctl (so, SIOCGIFADDR, (caddr_t)ireq, procp);
  if (error)
    return 0;

  if (sin->sin_addr.s_addr != 0)
    return 1;

  return 0;
}

/*
 *  DCHP Client Routine
 *    - The first DHCP offer is always accepted
 *    - No DHCP DECLINE message is sent if ARPing fails
 *
 *    return value:
 *    0: ok
 *    < 0: failed to startup or configure interface
 */
static int
dhcp_init (int update_files)
{
  struct dhcp_packet   call;
  struct dhcp_packet   reply;
  static unsigned long xid = ~0xFF;
  struct ifreq         ireq;
  struct ifnet         *ifp;
  struct socket        *so;
  int                  error;
  struct sockaddr_in   myaddr;
  struct ifaddr        *ifa;
  struct sockaddr_dl   *sdl = NULL;
  struct proc          *procp = NULL;

  clean_dns_entries();
  
  /*
   * If we are to update the files create the root
   * file structure.
   */
  if (update_files)
    if (rtems_create_root_fs () < 0) {
      printf("Error creating the root filesystem.\nFile not created.\n");
      update_files = 0;
    }

  /*
   * Find a network interface.
   */
  for (ifp = ifnet; ifp != 0; ifp = ifp->if_next)
    if ((ifp->if_flags & (IFF_LOOPBACK | IFF_POINTOPOINT)) == 0)
      break;
  if (ifp == NULL){
    printf ("dhcpc_init: no suitable interface\n");
    return -1;
  }

  memset (&ireq, 0, sizeof (ireq));
  sprintf (ireq.ifr_name, "%s%d", ifp->if_name, ifp->if_unit);

  if ((error = socreate (AF_INET, &so, SOCK_DGRAM, 0, procp)) != 0) {
    printf ("dhcpc_init: socreate, error: %s\n", strerror(error));
    return -1;
  }

  if (!dhcp_interface_has_ip (&ireq, so, procp))
    bootpc_fakeup_interface (&ireq, so, procp);

  /*
   * Get HW address
   */
  for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
    if (ifa->ifa_addr->sa_family == AF_LINK &&
        (sdl = ((struct sockaddr_dl *) ifa->ifa_addr)) &&
        sdl->sdl_type == IFT_ETHER)
      break;

  if (!sdl){
    printf ("dhcpc_init: Unable to find HW address\n");
    soclose (so);
    return -1;
  }
  if (sdl->sdl_alen != EALEN) {
    printf ("dhcpc_init: HW address len is %d, expected value is %d\n",
           sdl->sdl_alen, EALEN);
    soclose (so);
    return -1;
  }

  /*
   * Build the DHCP Discover
   */
  dhcp_discover_req (&call, sdl, &xid);

  /*
   * Send the Discover.
   */
  error = bootpc_call ((struct bootp_packet *)&call, (struct bootp_packet *)&reply, procp);
  if (error) {
    printf ("BOOTP call failed -- %s\n", strerror(error));
    soclose (so);
    return -1;
  }

  /*
   * Check for DHCP OFFER
   */
  if (memcmp (&reply.vend[0], dhcp_magic_cookie, sizeof (dhcp_magic_cookie)) != 0) {
    printf ("DHCP server did not send Magic Cookie.\n");
    soclose (so);
    return -1;
  }

  process_options (&reply.vend[4], sizeof (reply.vend) - 4);

  if (dhcp_message_type != DHCP_OFFER) {
    printf ("DHCP server did not send a DHCP Offer.\n");
    soclose (so);
    return -1;
  }

  /*
   * Send a DHCP REQUEST
   */
  dhcp_request_req (&call, &reply, sdl, true);

  error = bootpc_call ((struct bootp_packet *)&call, (struct bootp_packet *)&reply, procp);
  if (error) {
    printf ("BOOTP call failed -- %s\n", strerror(error));
    soclose (so);
    return -1;
  }

  /*
   * Check for DHCP ACK/NACK
   */
  if (memcmp (&reply.vend[0], dhcp_magic_cookie, sizeof (dhcp_magic_cookie)) != 0) {
    printf ("DHCP server did not send Magic Cookie.\n");
    soclose (so);
    return -1;
  }

  process_options (&reply.vend[4], sizeof (reply.vend) - 4);

  if (dhcp_message_type != DHCP_ACK) {
    printf ("DHCP server did not accept the DHCP request\n");
    soclose (so);
    return -1;
  }

  /*
   * Initialize network address structures
   */
  memset (&myaddr, 0, sizeof (myaddr));
  memset (&dhcp_netmask, 0, sizeof (dhcp_netmask));
  memset (&dhcp_gw, 0, sizeof (dhcp_gw));
  myaddr.sin_len = sizeof (myaddr);
  myaddr.sin_family = AF_INET;
  dhcp_netmask.sin_len = sizeof (dhcp_netmask);
  dhcp_netmask.sin_family = AF_INET;
  dhcp_gw.sin_len = sizeof (dhcp_gw);
  dhcp_gw.sin_family = AF_INET;

  /*
   * Set our address
   */
  myaddr.sin_addr = reply.yiaddr;

  /*
   * Process BOOTP/DHCP options
   */
  if (memcmp (&reply.vend[0], dhcp_magic_cookie, sizeof (dhcp_magic_cookie)) == 0)
    process_options (&reply.vend[4], sizeof (reply.vend) - 4);

  if (dhcp_option_overload & 1)
    process_options ((unsigned char*) reply.file, sizeof reply.file);
  else
    if (reply.file[0])
      rtems_bsdnet_bootp_boot_file_name = strdup (reply.file);

  if (dhcp_option_overload & 2)
    process_options ((unsigned char*) reply.sname, sizeof reply.sname);
  else
    if (reply.sname[0])
      rtems_bsdnet_bootp_server_name = strdup (reply.sname);

  /*
   * Use defaults if values were not supplied by BOOTP/DHCP options
   */
  if (!dhcp_gotnetmask)
  {
    if (IN_CLASSA (ntohl (myaddr.sin_addr.s_addr)))
      dhcp_netmask.sin_addr.s_addr = htonl (IN_CLASSA_NET);
    else if (IN_CLASSB (ntohl (myaddr.sin_addr.s_addr)))
      dhcp_netmask.sin_addr.s_addr = htonl (IN_CLASSB_NET);
    else
      dhcp_netmask.sin_addr.s_addr = htonl (IN_CLASSC_NET);
  }

  if (!dhcp_gotserver)
    rtems_bsdnet_bootp_server_address = reply.siaddr;

  if (!dhcp_gotgw)
    dhcp_gw.sin_addr = reply.giaddr;

  if (!dhcp_gotlogserver)
    rtems_bsdnet_log_host_address = rtems_bsdnet_bootp_server_address;

  printsetup (ifp->if_name, myaddr.sin_addr, dhcp_netmask.sin_addr,
              rtems_bsdnet_bootp_server_address, dhcp_gw.sin_addr);

  /*
   * Update the files if we are asked too.
   */
  if (update_files) {
    char *dn = rtems_bsdnet_domain_name;
    char *hn = dhcp_hostname;
    if (!dn)
      dn = "mydomain";
    if (!hn)
    {
      hn = "me";
      sethostname (hn, strlen (hn));
    }
    rtems_rootfs_append_host_rec(myaddr.sin_addr.s_addr, hn, dn);

    /*
     * Should the given domainname be used here ?
     */
    if (dhcp_gotserver) {
      if (rtems_bsdnet_bootp_server_name)
        hn = rtems_bsdnet_bootp_server_name;
      else
        hn = "bootps";
      rtems_rootfs_append_host_rec(rtems_bsdnet_bootp_server_address.s_addr,
                                   hn, dn);
    }

    if (dhcp_gotlogserver) {
      rtems_rootfs_append_host_rec(rtems_bsdnet_log_host_address.s_addr,
                                   "logs", dn);
    }

    /*
     * Setup the DNS configuration file /etc/resolv.conf.
     */
    if (rtems_bsdnet_nameserver_count) {
      int        i;
      char       buf[64];
      const char *bufl[1];

      bufl[0] = buf;

#define MKFILE_MODE (S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP | S_IROTH)

      if (rtems_bsdnet_domain_name &&
          (strlen(rtems_bsdnet_domain_name) < (sizeof(buf) - 1))) {
        strcpy(buf, "search ");
        strcat(buf, rtems_bsdnet_domain_name);
        strcat(buf, "\n");
        rtems_rootfs_file_append ("/etc/resolv.conf", MKFILE_MODE, 1, bufl);
      }

      for (i = 0; i < rtems_bsdnet_nameserver_count; i++) {
        strcpy(buf, "nameserver ");
        strcat(buf, inet_ntoa(rtems_bsdnet_ntpserver[i]));
        strcat(buf, "\n");
        if (rtems_rootfs_file_append ("/etc/resolv.conf", MKFILE_MODE, 1, bufl))
          break;
      }
    }
  }

  /*
   * Configure the interface with the new settings
   */
  error = bootpc_adjust_interface (&ireq, so,
                                   &myaddr, &dhcp_netmask, &dhcp_gw, procp);

  /*
   * Start the DHCP task if the lease isn't infinite.
   */
  if (dhcp_lease_time != 0xffffffff)
    dhcp_start_task (sdl, &reply, 150);

  soclose (so);

  return 0;
}

/*
 *
 *  RTEMS Entry point to DHCP client
 *
 */
void rtems_bsdnet_do_dhcp (void)
{
  bool update = true;
  rtems_bsdnet_semaphore_obtain ();
  while( dhcp_init (update) < 0 ) {
    update = false;
    rtems_bsdnet_semaphore_release();
    rtems_task_wake_after(TOD_MILLISECONDS_TO_TICKS(1000));
    rtems_bsdnet_semaphore_obtain ();
  }
  rtems_bsdnet_semaphore_release ();
}

int rtems_bsdnet_do_dhcp_timeout( void )
{
  int return_value;

  rtems_bsdnet_semaphore_obtain ();
  return_value = dhcp_init (false);
  rtems_bsdnet_semaphore_release ();

  return return_value;
}

void rtems_bsdnet_dhcp_down (void)
{
   if(dhcp_task_id != 0) {
     rtems_event_send (dhcp_task_id, RTEMS_EVENT_0);
   }
}

void
rtems_bsdnet_do_dhcp_refresh_only (unsigned long xid,
                                   unsigned long lease_time,
                                   unsigned long elapsed_time,
                                   unsigned long ip_address,
                                   unsigned long srv_address,
                                   const char*   hostname)
{
  struct dhcp_packet reply;
  struct ifnet       *ifp = NULL;
  struct ifaddr      *ifa = NULL;
  struct sockaddr_dl *sdl = NULL;
  struct sockaddr_in *sin = NULL;
  int                match = 0;
  struct ifnet       *mtif = NULL;

  /*
   * If an infinite lease has been granted, no task is needed.
   */
  if (lease_time == 0xffffffff)
    return;

  /*
   * Find a network interface.
   */
  for (ifp = ifnet; (ifp != NULL) && !match; ifp = ifp->if_next)
    if ((ifp->if_flags & (IFF_LOOPBACK | IFF_POINTOPOINT)) == 0)
      for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
          sin = (struct sockaddr_in *) ifa->ifa_addr;
          if (sin->sin_addr.s_addr == htonl (ip_address))
          {
            mtif = ifp;
            match = 1;
            break;
          }
        }

  if (!match) {
    printf ("dhcpc: no matching interface\n");
    return;
  }

  for (ifa = mtif->if_addrlist; ifa != NULL; ifa = ifa->ifa_next)
    if (ifa->ifa_addr->sa_family == AF_LINK &&
        (sdl = ((struct sockaddr_dl *) ifa->ifa_addr)) &&
        sdl->sdl_type == IFT_ETHER)
      break;

  if (!match) {
    printf ("dhcpc: no matching interface address\n");
    return;
  }

  /*
   * Set up given values in a simulated DHCP reply.
   */
  memset (&reply, 0x00, sizeof (reply));
  reply.xid = htonl (xid);
  reply.yiaddr.s_addr = htonl (ip_address);
  reply.siaddr.s_addr = htonl (srv_address);
  if (reply.siaddr.s_addr != rtems_bsdnet_bootp_server_address.s_addr)
  {
    memcpy (&rtems_bsdnet_bootp_server_address, &reply.siaddr,
            sizeof (reply.siaddr));
  }

  dhcp_lease_time = lease_time;
  dhcp_elapsed_time = elapsed_time;

  if (hostname)
  {
    sethostname ((char *) hostname, strlen (hostname));
    dhcp_hostname = bootp_strdup_realloc (dhcp_hostname, hostname);
  }

  dhcp_start_task (sdl, &reply, 150);
}
