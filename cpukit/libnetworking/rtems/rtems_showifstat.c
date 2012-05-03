#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * Display an address
 */
static int
showaddress (char *name, struct sockaddr *a)
{
	struct sockaddr_in *sa;
	char	buf[17];

	if (!a)
		return 0;
	printf ("%s:", name);
	sa = (struct sockaddr_in *)a;
	printf ("%-16s", inet_ntop (AF_INET, &sa->sin_addr, buf, sizeof(buf)));
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
	unsigned short bit, flags;

	printf ("************ INTERFACE STATISTICS ************\n");
	for (ifp = ifnet; ifp; ifp = ifp->if_next) {
		printf ("***** %s%d *****\n", ifp->if_name, ifp->if_unit);
		for (ifa = ifp->if_addrlist ; ifa ; ifa = ifa->ifa_next) {

			if ( !ifa->ifa_addr )
				continue;

			switch ( ifa->ifa_addr->sa_family ) {
				case AF_LINK:
					{
					struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifa->ifa_addr;
					unsigned char *cp = (unsigned char *)LLADDR(sdl);
					int		       i;

					switch ( sdl->sdl_type ) {
						case IFT_ETHER:
							if ( (i=sdl->sdl_alen) > 0 ) {
								printf("Ethernet Address: ");
								do {
									i--;
									printf("%02X%c", *cp++, i ? ':' : '\n');
								} while ( i>0 );
							}
						break;

						default:
						break;
					}
					}
				break;

				case AF_INET:
					{
					int printed;
					printed = showaddress ("Address", ifa->ifa_addr);
					if (ifp->if_flags & IFF_BROADCAST)
						printed |= showaddress ("Broadcast Address", ifa->ifa_broadaddr);
					if (ifp->if_flags & IFF_POINTOPOINT)
						printed |= showaddress ("Destination Address", ifa->ifa_dstaddr);
					printed |= showaddress ("Net mask", ifa->ifa_netmask);
					if (printed)
						printf ("\n");
					}
				break;

				default:
				break;
			}
		}

		printf ("Flags:");
		for (bit = 1, flags = ifp->if_flags ; flags ; bit <<= 1) {
			char *cp;
			char xbuf[20];
			switch (flags & bit) {
			case 0:			cp = NULL;		break;
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
			default: sprintf (xbuf, "%#x", bit); cp = xbuf;	break;
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
