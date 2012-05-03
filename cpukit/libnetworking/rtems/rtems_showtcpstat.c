#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_var.h>

/*
 * Display TCP statistics
 * Don't lock the rest of the network tasks out while printing.
 * It's no big deal if the values change while being printed.
 */
static void
showtcpstat (const char *name, unsigned long n)
{
	if (n)
		printf ("%35s%12lu\n", name, n);
}

void
rtems_bsdnet_show_tcp_stats (void)
{
	printf ("************ TCP Statistics ************\n");
	showtcpstat ("connections initiated", tcpstat.tcps_connattempt);
	showtcpstat ("connections accepted", tcpstat.tcps_accepts);
	showtcpstat ("connections established", tcpstat.tcps_connects);
	showtcpstat ("connections dropped", tcpstat.tcps_drops);
	showtcpstat ("embryonic connections dropped", tcpstat.tcps_conndrops);
	showtcpstat ("conn. closed (includes drops)", tcpstat.tcps_closed);
	showtcpstat ("segs where we tried to get rtt", tcpstat.tcps_segstimed);
	showtcpstat ("times we succeeded", tcpstat.tcps_rttupdated);
	showtcpstat ("delayed acks sent", tcpstat.tcps_delack);
	showtcpstat ("conn. dropped in rxmt timeout", tcpstat.tcps_timeoutdrop);
	showtcpstat ("retransmit timeouts", tcpstat.tcps_rexmttimeo);
	showtcpstat ("persist timeouts", tcpstat.tcps_persisttimeo);
	showtcpstat ("keepalive timeouts", tcpstat.tcps_keeptimeo);
	showtcpstat ("keepalive probes sent", tcpstat.tcps_keepprobe);
	showtcpstat ("connections dropped in keepalive", tcpstat.tcps_keepdrops);

	showtcpstat ("total packets sent", tcpstat.tcps_sndtotal);
	showtcpstat ("data packets sent", tcpstat.tcps_sndpack);
	showtcpstat ("data bytes sent", tcpstat.tcps_sndbyte);
	showtcpstat ("data packets retransmitted", tcpstat.tcps_sndrexmitpack);
	showtcpstat ("data bytes retransmitted", tcpstat.tcps_sndrexmitbyte);
	showtcpstat ("ack-only packets sent", tcpstat.tcps_sndacks);
	showtcpstat ("window probes sent", tcpstat.tcps_sndprobe);
	showtcpstat ("packets sent with URG only", tcpstat.tcps_sndurg);
	showtcpstat ("window update-only packets sent", tcpstat.tcps_sndwinup);
	showtcpstat ("control (SYN|FIN|RST) packets sent", tcpstat.tcps_sndctrl);

	showtcpstat ("total packets received", tcpstat.tcps_rcvtotal);
	showtcpstat ("packets received in sequence", tcpstat.tcps_rcvpack);
	showtcpstat ("bytes received in sequence", tcpstat.tcps_rcvbyte);
	showtcpstat ("packets received with ccksum errs", tcpstat.tcps_rcvbadsum);
	showtcpstat ("packets received with bad offset", tcpstat.tcps_rcvbadoff);
	showtcpstat ("packets received too short", tcpstat.tcps_rcvshort);
	showtcpstat ("duplicate-only packets received", tcpstat.tcps_rcvduppack);
	showtcpstat ("duplicate-only bytes received", tcpstat.tcps_rcvdupbyte);
	showtcpstat ("packets with some duplicate data", tcpstat.tcps_rcvpartduppack);
	showtcpstat ("dup. bytes in part-dup. packets", tcpstat.tcps_rcvpartdupbyte);
	showtcpstat ("out-of-order packets received", tcpstat.tcps_rcvoopack);
	showtcpstat ("out-of-order bytes received", tcpstat.tcps_rcvoobyte);
	showtcpstat ("packets with data after window", tcpstat.tcps_rcvpackafterwin);
	showtcpstat ("bytes rcvd after window", tcpstat.tcps_rcvbyteafterwin);
	showtcpstat ("packets rcvd after \"close\"", tcpstat.tcps_rcvafterclose);
	showtcpstat ("rcvd window probe packets", tcpstat.tcps_rcvwinprobe);
	showtcpstat ("rcvd duplicate acks", tcpstat.tcps_rcvdupack);
	showtcpstat ("rcvd acks for unsent data", tcpstat.tcps_rcvacktoomuch);
	showtcpstat ("rcvd ack packets", tcpstat.tcps_rcvackpack);
	showtcpstat ("bytes acked by rcvd acks", tcpstat.tcps_rcvackbyte);
	showtcpstat ("rcvd window update packets", tcpstat.tcps_rcvwinupd);
	showtcpstat ("segments dropped due to PAWS", tcpstat.tcps_pawsdrop);
	showtcpstat ("times hdr predict ok for acks", tcpstat.tcps_predack);
	showtcpstat ("times hdr predict ok for data pkts", tcpstat.tcps_preddat);
	showtcpstat ("pcb cache misses", tcpstat.tcps_pcbcachemiss);
	showtcpstat ("times cached RTT in route updated", tcpstat.tcps_cachedrtt);
	showtcpstat ("times cached rttvar updated", tcpstat.tcps_cachedrttvar);
	showtcpstat ("times cached ssthresh updated", tcpstat.tcps_cachedssthresh);
	showtcpstat ("times RTT initialized from route", tcpstat.tcps_usedrtt);
	showtcpstat ("times RTTVAR initialized from rt", tcpstat.tcps_usedrttvar);
	showtcpstat ("times ssthresh initialized from rt", tcpstat.tcps_usedssthresh);
	showtcpstat ("timeout in persist state", tcpstat.tcps_persistdrop);
	showtcpstat ("bogus SYN, e.g. premature ACK", tcpstat.tcps_badsyn);
	showtcpstat ("resends due to MTU discovery", tcpstat.tcps_mturesent);
	showtcpstat ("listen queue overflows", tcpstat.tcps_listendrop);
	printf ("\n");
}
