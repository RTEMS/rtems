/**
 * @file rtems/rtems_bsdnet.h
 */


#ifndef _RTEMS_BSDNET_H
#define _RTEMS_BSDNET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <sys/cpuset.h>

/*
 *  If this file is included from inside the Network Stack proper or
 *  a device driver, then __INSIDE_RTEMS_BSD_TCPIP_STACK__ should be
 *  defined.  This triggers a number of internally used definitions.
 */

#if defined(__INSIDE_RTEMS_BSD_TCPIP_STACK__)
#undef _KERNEL
#undef INET
#undef NFS
#undef DIAGNOSTIC
#undef BOOTP_COMPAT
#undef __BSD_VISIBLE

#define _KERNEL
#define INET
#define NFS
#define DIAGNOSTIC
#define BOOTP_COMPAT
#define __BSD_VISIBLE 1
#endif

/*
 * Values that may be obtained by BOOTP
 */
extern struct in_addr rtems_bsdnet_bootp_server_address;
extern char *rtems_bsdnet_bootp_server_name;
extern char *rtems_bsdnet_bootp_boot_file_name;
extern char *rtems_bsdnet_bootp_cmdline;
extern int32_t rtems_bsdnet_timeoffset;

/*
 * Manipulate routing tables
 */
struct sockaddr;
struct rtentry;
int rtems_bsdnet_rtrequest (
    int req,
    struct sockaddr *dst,
    struct sockaddr *gateway,
    struct sockaddr *netmask,
    int flags,
    struct rtentry **net_nrt);

/*
 * Diagnostics
 */
void rtems_bsdnet_show_inet_routes (void);
void rtems_bsdnet_show_mbuf_stats (void);
void rtems_bsdnet_show_if_stats (void);
void rtems_bsdnet_show_ip_stats (void);
void rtems_bsdnet_show_icmp_stats (void);
void rtems_bsdnet_show_udp_stats (void);
void rtems_bsdnet_show_tcp_stats (void);

/*
 * Network configuration
 */
struct rtems_bsdnet_ifconfig {
	/*
	 * These three entries must be supplied for each interface.
	 */
	char		*name;

	/*
	 * This function now handles attaching and detaching an interface.
	 * The parameter attaching indicates the operation being invoked.
	 * For older attach functions which do not have the extra parameter
	 * it will be ignored.
	 */
	int		(*attach)(struct rtems_bsdnet_ifconfig *conf, int attaching);

	/*
	 * Link to next interface
	 */
	struct rtems_bsdnet_ifconfig *next;

	/*
	 * The following entries may be obtained
	 * from BOOTP or explicitily supplied.
	 */
	char		*ip_address;
	char		*ip_netmask;
	void		*hardware_address;

	/*
	 * The driver assigns defaults values to the following
	 * entries if they are not explicitly supplied.
	 */
	int		ignore_broadcast;
	int		mtu;
	int		rbuf_count;
	int		xbuf_count;

	/*
	 * For external ethernet controller board the following
	 * parameters are needed
	 */
	unsigned int	port;   /* port of the board */
	unsigned int	irno;   /* irq of the board */
	unsigned int	bpar;   /* memory of the board */

  /*
   * Driver control block pointer. Typcially this points to the driver's
   * controlling structure. You set this when you have the structure allocated
   * externally to the driver.
   */
  void *drv_ctrl;

};

struct rtems_bsdnet_config {
	/*
	 * This entry points to the head of the ifconfig chain.
	 */
	struct rtems_bsdnet_ifconfig *ifconfig;

	/*
	 * This entry should be rtems_bsdnet_do_bootp if BOOTP
	 * is being used to configure the network, and NULL
	 * if BOOTP is not being used.
	 */
	void			(*bootp)(void);

	/*
	 * The remaining items can be initialized to 0, in
	 * which case the default value will be used.
	 */
	rtems_task_priority	network_task_priority;	/* 100		*/
	unsigned long		mbuf_bytecount;		/* 64 kbytes	*/
	unsigned long		mbuf_cluster_bytecount;	/* 128 kbytes	*/
	char			*hostname;		/* BOOTP	*/
	char			*domainname;		/* BOOTP	*/
	char			*gateway;		/* BOOTP	*/
	char			*log_host;		/* BOOTP	*/
	char			*name_server[3];	/* BOOTP	*/
	char			*ntp_server[3];		/* BOOTP	*/
        /*
	 *  Default "multiplier" on buffer size.  This is
	 *  claimed by the TCP/IP implementation to be for
	 *  efficiency but you will have to measure the
	 *  benefit for buffering beyond double buffering
	 *  in your own application.
	 *
	 *  The default value is 2.
	 *
	 *  See kern/uipc_socket2.c for details.
	 */
	unsigned long		sb_efficiency;
	/*
	 * Default UDP buffer sizes PER SOCKET!!
	 *
	 *   TX = 9216 -- max datagram size
	 *   RX = 40 * (1024 + sizeof(struct sockaddr_in))
	 *
	 * See netinet/udp_usrreq.c for details
	 */
	unsigned long		udp_tx_buf_size;
	unsigned long		udp_rx_buf_size;
	/*
	 * Default UDP buffer sizes PER SOCKET!!
	 *
	 *   TX = 16 * 1024
	 *   RX = 16 * 1024
	 *
	 * See netinet/tcp_usrreq.c for details
	 */
	unsigned long		tcp_tx_buf_size;
	unsigned long		tcp_rx_buf_size;

	/*
	 * Default Network Tasks CPU Affinity
	 */
#ifdef RTEMS_SMP
	const cpu_set_t		*network_task_cpuset;
	size_t			network_task_cpuset_size;
#endif
};

/*
 * Default global device configuration structure. This is scanned
 * by the initialize network function. Check the network demo's for
 * an example of the structure. Like the RTEMS configuration tables,
 * they are not part of RTEMS but part of your application or bsp
 * code.
 */
extern struct rtems_bsdnet_config rtems_bsdnet_config;

/*
 * Initialise the BSD stack, attach and `up' interfaces
 * in the `rtems_bsdnet_config'. RTEMS must already be initialised.
 */
int rtems_bsdnet_initialize_network (void);

/*
 * Dynamic interface control. Drivers must free any resources such as
 * memory, interrupts, io regions claimed during the `attach' and/or
 * `up' operations when asked to `detach'.
 * You must configure the interface after attaching it.
 */
void rtems_bsdnet_attach (struct rtems_bsdnet_ifconfig *ifconfig);
void rtems_bsdnet_detach (struct rtems_bsdnet_ifconfig *ifconfig);

/*
 * Interface configuration. The commands are listed in `sys/sockio.h'.
 */
int rtems_bsdnet_ifconfig (const char *ifname, uint32_t   cmd, void *param);

void rtems_bsdnet_do_bootp (void);
void rtems_bsdnet_do_bootp_and_rootfs (void);

/* NTP tuning parameters */
extern int rtems_bsdnet_ntp_retry_count;
extern int rtems_bsdnet_ntp_timeout_secs;
extern int rtems_bsdnet_ntp_bcast_timeout_secs;


struct timestamp {
	uint32_t	integer;
	uint32_t	fraction;
};

/* Data is passed in network byte order */
struct ntpPacketSmall {
	uint8_t		li_vn_mode;
	uint8_t		stratum;
	int8_t		poll_interval;
	int8_t		precision;
	int32_t		root_delay;
	int32_t		root_dispersion;
	char			reference_identifier[4];
	struct timestamp	reference_timestamp;
	struct timestamp	originate_timestamp;
	struct timestamp	receive_timestamp;
	struct timestamp	transmit_timestamp;
};

/* NOTE: packet data is *only* accessible from the callback
 *
 * 'callback' is invoked twice, once prior to sending a request
 * to the server and one more time after receiving a valid reply.
 * This allows for the user to measure round-trip times.
 *
 * Semantics of the 'state' parameter:
 *
 *    state ==  1:  1st call, just prior to sending request. The
 *                  packet has been set up already but may be
 *                  modified by the callback (e.g. to set the originate
 *                  timestamp).
 *    state == -1:  1st call - no request will be sent but we'll
 *                  wait for a reply from a broadcast server. The
 *                  packet has not been set up.
 *    state ==  0:  2nd call. The user is responsible for keeping track
 *                  of the 'state' during the first call in order to
 *                  know if it makes sense to calculate 'round-trip' times.
 *
 * RETURN VALUE: the callback should return 0 if processing the packet was
 *               successful and -1 on error in which case rtems_bsdnet_get_ntp()
 *				 may try another server.
 */
typedef int (*rtems_bsdnet_ntp_callback_t)(
	struct ntpPacketSmall  *packet,
	int                     state,
	void                   *usr_data);

/* Obtain time from a NTP server and call user callback to process data;
 * socket parameter may be -1 to request the routine to open and close its own socket.
 * Networking parameters as configured are used...
 *
 * It is legal to pass a NULL callback pointer. In this case, a default callback
 * is used which determines the current time by contacting an NTP server. The current
 * time is converted to a 'struct timespec' (seconds/nanoseconds) and passed into *usr_data.
 * The caller is responsible for providing a memory area >= sizeof(struct timespec).
 *
 * RETURNS: 0 on success, -1 on failure.
 */
int rtems_bsdnet_get_ntp(int socket, rtems_bsdnet_ntp_callback_t callback, void *usr_data);

int rtems_bsdnet_synchronize_ntp (int interval, rtems_task_priority priority);

/*
 * Callback to report BSD malloc starvation.
 * The default implementation just prints a message but an application
 * can provide its own version.
 */
void rtems_bsdnet_malloc_starvation(void);

/*
 * mbuf malloc interface to enable custom allocation of mbuf's
 *
 * May be declared in user code.  If not, then the default is to
 * malloc.
 */
void* rtems_bsdnet_malloc_mbuf(size_t size, int type);

/*
 * Possible values of the type parameter to rtems_bsdnet_malloc_mbuf to assist
 * in allocation of the structure.
 */
#define MBUF_MALLOC_NMBCLUSTERS (0)
#define MBUF_MALLOC_MCLREFCNT   (1)
#define MBUF_MALLOC_MBUF        (2)

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_BSDNET_H */
