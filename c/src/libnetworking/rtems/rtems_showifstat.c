#include <sys/param.h>
#include <sys/queue.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>

/*
 * Display an address
 */
static int
showaddress (char *name, struct sockaddr *a)
{
	struct sockaddr_in *sa;

	if (!a || (a->sa_family != AF_INET))
		return 0;
	printf ("%s:", name);
	sa = (struct sockaddr_in *)a;
	printf ("%-16s", inet_ntoa (sa->sin_addr));
	return 1;
}

/*
 * Display interface statistics
 */
void
rtems_bsdnet_show_if_stats (void)
{
	struct ifnet *ifp;
	struct ifaddr *ifa;
	unsigned int bit, flags;
	int printed;

	printf ("************ INTERFACE STATISTICS ************\n");
	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		printf ("***** %s%d *****\n", ifp->if_name, ifp->if_unit);
		for (ifa = ifp->if_addrlist ; ifa ; ifa = ifa->ifa_next) {
			printed = showaddress ("Address", ifa->ifa_addr);
			if (ifp->if_flags & IFF_BROADCAST)
				printed |= showaddress ("Broadcast Address", ifa->ifa_broadaddr);
			if (ifp->if_flags & IFF_POINTOPOINT)
				printed |= showaddress ("Destination Address", ifa->ifa_dstaddr);
			printed |= showaddress ("Net mask", ifa->ifa_netmask);
			if (printed)
				printf ("\n");
		}

		printf ("Flags:");
		for (bit = 1, flags = ifp->if_flags ; flags ; bit <<= 1) {
			char *cp;
			switch (flags & bit) {
			default:		cp = NULL;		break;
			case IFF_UP:		cp = "Up";		break;
			case IFF_BROADCAST:	cp = "Broadcast";	break;
			case IFF_DEBUG:		cp = "Debug";		break;
			case IFF_LOOPBACK:	cp = "Loopback";	break;
			case IFF_POINTOPOINT:	cp = "Point-to-point";	break;
			case IFF_RUNNING:	cp = "Running";		break;
			case IFF_NOARP:		cp = "No-ARP";		break;
			case IFF_PROMISC:	cp = "Promiscuous";	break;
			case IFF_ALLMULTI:	cp = "All-multicast";	break;
			case IFF_OACTIVE:	cp = "Active";		break;
			case IFF_SIMPLEX:	cp = "Simplex";		break;
			case IFF_LINK0:		cp = "Link0";		break;
			case IFF_LINK1:		cp = "Link1";		break;
			case IFF_LINK2:		cp = "Link2";		break;
			case IFF_MULTICAST:	cp = "Multicast";	break;
			}
			if (cp) {
				flags &= ~bit;
				printf (" %s", cp);
			}
		}
		printf ("\n");

		printf ("Send queue limit:%-4d length:%-4d Dropped:%-8d\n",
							ifp->if_snd.ifq_maxlen,
							ifp->if_snd.ifq_len,
							ifp->if_snd.ifq_drops);

		/*
		 * FIXME: Could print if_data statistics here,
		 *        but right now the drivers maintain their
		 *        own statistics.
		 */

		/*
		 * Grab the network semaphore.
		 * In most cases this is not necessary, but it's
		 * easier to always call the driver ioctl function
		 * while holding the semaphore than to try
		 * and explain why some ioctl commands are invoked
		 * while holding the semaphore and others are
		 * invoked while not holding the semaphore.
		 */
		rtems_bsdnet_semaphore_obtain ();
		(*ifp->if_ioctl)(ifp, SIO_RTEMS_SHOW_STATS, NULL);
		rtems_bsdnet_semaphore_release ();
	}
	printf ("\n");
}
