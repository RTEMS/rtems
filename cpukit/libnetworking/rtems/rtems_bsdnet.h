/*
 *  $Id$
 */

#ifndef _RTEMS_BSDNET_
#define _RTEMS_BSDNET_

#include <rtems.h>

/*
 *  If this file is included from inside the Network Stack proper or
 *  a device driver, then __INSIDE_RTEMS_BSD_TCPIP_STACK__ should be
 *  defined.  This triggers a number of internally used definitions.
 */

#if defined(__INSIDE_RTEMS_BSD_TCPIP_STACK__)
#undef _COMPILING_BSD_KERNEL_
#undef KERNEL
#undef INET
#undef NFS
#undef DIAGNOSTIC
#undef BOOTP_COMPAT

#define _COMPILING_BSD_KERNEL_
#define KERNEL
#define INET
#define NFS
#define DIAGNOSTIC
#define BOOTP_COMPAT
#endif

/*
 * Values that may be obtained by BOOTP
 */
extern struct in_addr rtems_bsdnet_bootp_server_address;
extern char *rtems_bsdnet_bootp_server_name;
extern char *rtems_bsdnet_bootp_boot_file_name;
extern char *rtems_bsdnet_bootp_cmdline;
extern struct in_addr rtems_bsdnet_ntpserver[];
extern int rtems_bsdnet_ntpserver_count;
extern long rtems_bsdnet_timeoffset;

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
int rtems_bsdnet_ifconfig (const char *ifname, unsigned32 cmd, void *param);

void rtems_bsdnet_do_bootp (void);
void rtems_bsdnet_do_bootp_and_rootfs (void);

int rtems_bsdnet_synchronize_ntp (int interval, rtems_task_priority priority);

#endif /* _RTEMS_BSDNET_ */
