/*
 * Synchronize with an NTP server
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Canadian Light Source
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@cls.usask.ca
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h> /* close */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <rtems/bsdnet/servers.h>

/*
 * RTEMS base: 1988, January 1
 *  UNIX base: 1970, January 1
 *   NTP base: 1900, January 1
 */
#define UNIX_BASE_TO_NTP_BASE (uint32_t)(((70UL*365UL)+17UL) * (24UL*60UL*60UL))

struct ntpPacket {
	struct ntpPacketSmall	ntp;
	char			authenticator[96];
};

static int
processPacket (struct ntpPacketSmall *p, int state, void *unused)
{
	time_t tbuf;
	struct tm *lt;
	rtems_time_of_day rt;
	rtems_interval ticks_per_second;

	if ( state )
		return 0;

	ticks_per_second = rtems_clock_get_ticks_per_second();
	tbuf = ntohl (p->transmit_timestamp.integer) - UNIX_BASE_TO_NTP_BASE - rtems_bsdnet_timeoffset;
	lt = gmtime (&tbuf);
	rt.year = lt->tm_year + 1900;
	rt.month = lt->tm_mon + 1;
	rt.day = lt->tm_mday;
	rt.hour = lt->tm_hour;
	rt.minute = lt->tm_min;
	rt.second = lt->tm_sec;
	rt.ticks = ntohl (p->transmit_timestamp.fraction) / (ULONG_MAX / ticks_per_second);
	if (rt.ticks >= ticks_per_second)
		rt.ticks = ticks_per_second - 1;
	rtems_clock_set (&rt);
	return 0;
}

static int
getServerTimespec(struct ntpPacketSmall *p, int state, void *usr_data)
{
struct timespec *ts = usr_data;
unsigned long long tmp;

	if ( 0 == state ) {
		ts->tv_sec  = ntohl( p->transmit_timestamp.integer );
		ts->tv_sec -= rtems_bsdnet_timeoffset + UNIX_BASE_TO_NTP_BASE;

		tmp  = 1000000000 * (unsigned long long)ntohl(p->transmit_timestamp.fraction);

		ts->tv_nsec = (unsigned long) (tmp>>32);
	}
	return 0;
}

int rtems_bsdnet_ntp_retry_count  = 5;
int rtems_bsdnet_ntp_timeout_secs = 5;
int rtems_bsdnet_ntp_bcast_timeout_secs = 80;

static int
tryServer (int i, int s, rtems_bsdnet_ntp_callback_t callback, void *usr_data)
{
	int l = 0;
	struct timeval tv;
	socklen_t farlen;
	struct sockaddr_in farAddr;
	struct ntpPacketSmall packet;

	if (i < 0)
		tv.tv_sec = rtems_bsdnet_ntp_bcast_timeout_secs;
	else
		tv.tv_sec = rtems_bsdnet_ntp_timeout_secs;
	tv.tv_usec = 0;
	if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv) < 0) {
		fprintf (stderr, "rtems_bsdnet_get_ntp() Can't set socket receive timeout: %s\n", strerror (errno));
		close (s);
		return -1;
	}
	if (i >= 0) {
		memset (&farAddr, 0, sizeof farAddr);
		farAddr.sin_family = AF_INET;
		farAddr.sin_port = htons (123);
		farAddr.sin_addr = rtems_bsdnet_ntpserver[i];
		memset (&packet, 0, sizeof packet);
		packet.li_vn_mode = (3 << 3) | 3; /* NTP version 3, client */
		if ( callback( &packet, 1, usr_data ) )
			return -1;
		l = sendto (s, &packet, sizeof packet, 0, (struct sockaddr *)&farAddr, sizeof farAddr);
		if (l != sizeof packet) {
			fprintf (stderr, "rtems_bsdnet_get_ntp() Can't send: %s\n", strerror (errno));
			return -1;
		}
	} else {
		if ( callback( &packet, -1, usr_data ) )
			return -1;
	}
	farlen = sizeof farAddr;
	i = recvfrom (s, &packet, sizeof packet, 0, (struct sockaddr *)&farAddr, &farlen);
	if (i == 0)
		fprintf (stderr, "rtems_bsdnet_get_ntp() Unexpected EOF");
	if (i < 0) {
		if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
			return -1;
		fprintf (stderr, "rtems_bsdnet_get_ntp() Can't receive: %s\n", strerror (errno));
	}

	if ( i >= sizeof packet &&
		((packet.li_vn_mode & (0x7 << 3)) == (3 << 3)) &&
	    ((packet.transmit_timestamp.integer != 0) || (packet.transmit_timestamp.fraction != 0)) &&
		0 == callback( &packet, 0 , usr_data) )
		return 0;

	return -1;
}

int rtems_bsdnet_get_ntp(int sock, rtems_bsdnet_ntp_callback_t callback, void *usr_data)
{
int s = -1;
int i;
int retry;
struct sockaddr_in myAddr;
int reuseFlag;
int ret;

	if ( !callback )
		callback = getServerTimespec;

	if ( sock < 0 ) {
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		fprintf (stderr, "rtems_bsdnet_get_ntp() Can't create socket: %s\n", strerror (errno));
		return -1;
	}
	reuseFlag = 1;
	if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseFlag, sizeof reuseFlag) < 0) {
		fprintf (stderr, "rtems_bsdnet_get_ntp() Can't set socket reuse: %s\n", strerror (errno));
		close (s);
		return -1;
	}
	memset (&myAddr, 0, sizeof myAddr);
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons (0);
	myAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0) {
		fprintf (stderr, "rtems_bsdnet_get_ntp() Can't bind socket: %s\n", strerror (errno));
		close (s);
		return -1;
	}
	sock = s;
	}
	ret = -1;
	for (retry = 0 ; (ret == -1) && (retry < rtems_bsdnet_ntp_retry_count) ; retry++) {
		/*
		 * If there's no server we just have to wait
		 * and hope that there's an NTP broadcast
		 * server out there somewhere.
		 */
		if (rtems_bsdnet_ntpserver_count < 0) {
			ret = tryServer (-1, sock, callback, usr_data);
		}
		else {
			for (i = 0 ; (ret == -1) && (i < rtems_bsdnet_ntpserver_count) ; i++) {
				ret = tryServer (i, sock, callback, usr_data);
			}
		}
	}
	if ( s >= 0 )
		close (s);
	return ret;
}

int
rtems_bsdnet_synchronize_ntp (int interval, rtems_task_priority priority)
{
	if (interval != 0) {
		fprintf (stderr, "Daemon-mode note yet supported.\n");
		errno = EINVAL;
		return -1;
	}
	return rtems_bsdnet_get_ntp( -1, processPacket, 0);
}
