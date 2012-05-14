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

/*
 * Display MBUF statistics
 * Don't lock the rest of the network tasks out while printing.
 * It's no big deal if the values change while being printed.
 */
void
rtems_bsdnet_show_mbuf_stats (void)
{
	int i;
	int printed = 0;
	char *cp;

	printf ("************ MBUF STATISTICS ************\n");
	printf ("mbufs:%4lu    clusters:%4lu    free:%4lu\n",
			mbstat.m_mbufs, mbstat.m_clusters, mbstat.m_clfree);
	printf ("drops:%4lu       waits:%4lu  drains:%4lu\n",
			mbstat.m_drops, mbstat.m_wait, mbstat.m_drain);
	for (i = 0 ; i < 20 ; i++) {
		switch (i) {
		case MT_FREE:		cp = "free";		break;
		case MT_DATA:		cp = "data";		break;
		case MT_HEADER:		cp = "header";		break;
		case MT_SOCKET:		cp = "socket";		break;
		case MT_PCB:		cp = "pcb";		break;
		case MT_RTABLE:		cp = "rtable";		break;
		case MT_HTABLE:		cp = "htable";		break;
		case MT_ATABLE:		cp = "atable";		break;
		case MT_SONAME:		cp = "soname";		break;
		case MT_SOOPTS:		cp = "soopts";		break;
		case MT_FTABLE:		cp = "ftable";		break;
		case MT_RIGHTS:		cp = "rights";		break;
		case MT_IFADDR:		cp = "ifaddr";		break;
		case MT_CONTROL:	cp = "control";		break;
		case MT_OOBDATA:	cp = "oobdata";		break;
		default:		cp = NULL;		break;
		}
		if ((cp != NULL) || (mbstat.m_mtypes[i] != 0)) {
			char cbuf[16];
			if (cp == NULL) {
				sprintf (cbuf, "Type %d", i);
				cp = cbuf;
			}
			printf ("%10s:%-8u", cp, mbstat.m_mtypes[i]);
			if (++printed == 4) {
				printf ("\n");
				printed = 0;
			}
		}
	}
	if (printed)
		printf ("\n");
	printf ("\n");
}
