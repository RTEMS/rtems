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
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>

/*
 * Display ICMP statistics
 * Don't lock the rest of the network tasks out while printing.
 * It's no big deal if the values change while being printed.
 */
static void
showicmpstat (const char *name, unsigned long n)
{
	if (n)
		printf ("%35s%12lu\n", name, n);
}

/*
 * External data we peek at during statistics reporting
 */
extern unsigned int icmplenPanicAvoided;

void
rtems_bsdnet_show_icmp_stats (void)
{
	int i;
	char cbuf[20];

	printf ("************ ICMP Statistics ************\n");
	showicmpstat ("Calls to icmp_error()", icmpstat.icps_error);
	showicmpstat ("Errors not sent -- old was icmp", icmpstat.icps_oldicmp);
	for (i = 0 ; i <= ICMP_MAXTYPE ; i++) {
		if (icmpstat.icps_outhist[i]) {
			sprintf (cbuf, "Type %d sent", i);
			showicmpstat (cbuf, icmpstat.icps_outhist[i]);
		}
	}
	showicmpstat ("icmp_code out of range", icmpstat.icps_badcode);
	showicmpstat ("packet < ICMP_MINLEN", icmpstat.icps_tooshort);
	showicmpstat ("bad checksum", icmpstat.icps_checksum);
	showicmpstat ("calculated bound mismatch", icmpstat.icps_badlen);
	showicmpstat ("number of responses", icmpstat.icps_reflect);
        showicmpstat ("all echo requests dropped", icmpstat.icps_allecho);
	showicmpstat ("b/mcast echo requests dropped", icmpstat.icps_bmcastecho);
	showicmpstat ("b/mcast tstamp requests dropped", icmpstat.icps_bmcasttstamp);
	for (i = 0 ; i <= ICMP_MAXTYPE ; i++) {
		if (icmpstat.icps_inhist[i]) {
			sprintf (cbuf, "Type %d received", i);
			showicmpstat (cbuf, icmpstat.icps_inhist[i]);
		}
	}
	showicmpstat ("ICMP panic avoided", icmplenPanicAvoided);
	printf ("\n");
}
