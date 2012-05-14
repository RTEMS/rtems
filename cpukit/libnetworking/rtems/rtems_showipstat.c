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
#include <net/if_var.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>

/*
 * Display IP statistics
 * Don't lock the rest of the network tasks out while printing.
 * It's no big deal if the values change while being printed.
 */
static void
showipstat (const char *name, unsigned long n)
{
	if (n)
		printf ("%35s%12lu\n", name, n);
}

void
rtems_bsdnet_show_ip_stats (void)
{
	printf ("************ IP Statistics ************\n");
	showipstat ("total packets received", ipstat.ips_total);
	showipstat ("checksum bad", ipstat.ips_badsum);
	showipstat ("packet too short", ipstat.ips_tooshort);
	showipstat ("not enough data", ipstat.ips_toosmall);
	showipstat ("ip header length < data size", ipstat.ips_badhlen);
	showipstat ("ip length < ip header length", ipstat.ips_badlen);
	showipstat ("fragments received", ipstat.ips_fragments);
	showipstat ("frags dropped (dups, out of space)", ipstat.ips_fragdropped);
	showipstat ("fragments timed out", ipstat.ips_fragtimeout);
	showipstat ("packets forwarded", ipstat.ips_forward);
	showipstat ("packets rcvd for unreachable dest", ipstat.ips_cantforward);
	showipstat ("packets forwarded on same net", ipstat.ips_redirectsent);
	showipstat ("unknown or unsupported protocol", ipstat.ips_noproto);
	showipstat ("datagrams delivered to upper level", ipstat.ips_delivered);
	showipstat ("total ip packets generated here", ipstat.ips_localout);
	showipstat ("lost packets due to nobufs, etc.", ipstat.ips_odropped);
	showipstat ("total packets reassembled ok", ipstat.ips_reassembled);
	showipstat ("datagrams successfully fragmented", ipstat.ips_fragmented);
	showipstat ("output fragments created", ipstat.ips_ofragments);
	showipstat ("don't fragment flag was set, etc.", ipstat.ips_cantfrag);
	showipstat ("error in option processing", ipstat.ips_badoptions);
	showipstat ("packets discarded due to no route", ipstat.ips_noroute);
	showipstat ("ip version != 4", ipstat.ips_badvers);
	showipstat ("total raw ip packets generated", ipstat.ips_rawout);
	showipstat ("ip length > max ip packet size", ipstat.ips_toolong);
	showipstat ("ip input queue drops", ipintrq.ifq_drops);
	printf ("\n");
}
