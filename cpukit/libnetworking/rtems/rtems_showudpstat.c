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
#include <netinet/udp.h>
#include <netinet/udp_var.h>

/*
 * Display UDP statistics
 * Don't lock the rest of the network tasks out while printing.
 * It's no big deal if the values change while being printed.
 */
static void
showudpstat (const char *name, unsigned long n)
{
	if (n)
		printf ("%35s%12lu\n", name, n);
}

void
rtems_bsdnet_show_udp_stats (void)
{
	printf ("************ UDP Statistics ************\n");
	showudpstat ("total input packets", udpstat.udps_ipackets);
	showudpstat ("packet shorter than header", udpstat.udps_hdrops);
	showudpstat ("checksum error", udpstat.udps_badsum);
	showudpstat ("data length larger than packet", udpstat.udps_badlen);
	showudpstat ("no socket on port", udpstat.udps_noport);
	showudpstat ("of above, arrived as broadcast", udpstat.udps_noportbcast);
	showudpstat ("not delivered, input socket full", udpstat.udps_fullsock);
	showudpstat ("input packets missing pcb cache", udpstat.udpps_pcbcachemiss);
	showudpstat ("input packets not for hashed pcb", udpstat.udpps_pcbhashmiss);
	showudpstat ("total output packets", udpstat.udps_opackets);
	printf ("\n");
}
