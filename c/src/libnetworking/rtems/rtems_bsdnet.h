/*
 *  $Id$
 */

#ifndef _RTEMS_BSDNET_
#define _RTEMS_BSDNET_

#include <rtems.h>

/*
 * Values that may be obtained by BOOTP
 */
extern struct in_addr rtems_bsdnet_bootp_server_address;
extern char *rtems_bsdnet_bootp_boot_file_name;

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
	 * These two entries must be supplied for each interface.
	 */
	char		*name;
	int		(*attach)(struct rtems_bsdnet_ifconfig *conf);

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
  unsigned int 			port;   /* port of the board */
  unsigned int			irno;   /* irq of the board */
  unsigned int			bpar;   /* memory of the board */

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
};
extern struct rtems_bsdnet_config rtems_bsdnet_config;
int rtems_bsdnet_initialize_network (void);
void rtems_bsdnet_do_bootp (void);

#endif /* _RTEMS_BSDNET_ */
