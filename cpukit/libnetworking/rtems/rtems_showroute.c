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
#include <sys/socketvar.h>
#include <sys/domain.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>

#include <netinet/in.h>

#include <arpa/inet.h>

/*
 * We'll use the application versions of realloc and free.
 */
#undef free
#undef malloc
#include <stdlib.h>

/*
 * Information per route
 */
struct rinfo {
	struct sockaddr	dst;
	union {
		struct sockaddr		sa;
		struct sockaddr_in	sin;
		struct sockaddr_dl	sdl;
	} un;
	unsigned long	pksent;
	unsigned long	expire;
	int		flags;
	char		ifname[16];
	short		ifunit;
	short		refcnt;
};

/*
 * Information per display
 */
struct dinfo {
	int		capacity;
	int		count;
	struct rinfo	*routes;
};

/*
 * Copy address
 */
static void
copyAddress (void *to, void *from, int tolen)
{
	int ncopy;
	struct sockaddr dummy;

	if (from == NULL) {
		/*
		 * Create a fake address of unspecified type
		 */
		from = &dummy;
		dummy.sa_len = 4;
		dummy.sa_family = AF_UNSPEC;
	}
	ncopy = ((struct sockaddr *)from)->sa_len;
	if (ncopy > tolen)
		ncopy = tolen;
	memcpy (to, from, ncopy);
}

/*
 * Package everything up before printing it.
 * We don't want to block all network operations till
 * the printing completes!
 */
static int
show_inet_route (
	struct radix_node *rn,
	void *vw )
{
	struct rtentry *rt = (struct rtentry *)rn;
	struct ifnet *ifp;
	struct dinfo *dp = (struct dinfo *)vw;
	struct rinfo *r;

	/*
	 * Get a pointer to a new route info structure
	 */
	if (dp->count >= dp->capacity) {
		r = realloc (dp->routes, (sizeof *r) * (dp->capacity + 20));
		if (r == 0)
			return ENOMEM;
		dp->capacity += 20;
		dp->routes = r;
	}
	r = dp->routes + dp->count++;

	/*
	 * Fill in the route info structure
	 */
	copyAddress (&r->dst, rt_key(rt), sizeof r->dst);
	if (rt->rt_flags & (RTF_GATEWAY | RTF_HOST)) {
		copyAddress (&r->un, rt->rt_gateway, sizeof r->un);
	}
	else {
		/*
		 * Create a fake address to hold the mask
		 */
		struct sockaddr_in dummy;

		dummy.sin_family = AF_INET;
		dummy.sin_len = sizeof dummy;
		dummy.sin_addr = ((struct sockaddr_in *)rt_mask(rt))->sin_addr;
		copyAddress (&r->un, &dummy, sizeof r->un);
	}
	r->flags = rt->rt_flags;
	r->refcnt = rt->rt_refcnt;
	r->pksent = rt->rt_rmx.rmx_pksent;
	r->expire = rt->rt_rmx.rmx_expire;
	ifp = rt->rt_ifp;
	strncpy (r->ifname, (ifp->if_name ? ifp->if_name : ""), sizeof r->ifname);
	r->ifunit = ifp->if_unit;
	return 0;
}

/*
 * Convert link address to ASCII
 */
static char *
link_ascii (struct sockaddr_dl *sdl, char *buf, int bufsize)
{
	char *cp;
	int i;
	int first = 1;
	int nleft = sdl->sdl_alen;
	char *ap = LLADDR (sdl);
	static const char hextab[16] = "0123456789ABCDEF";

	cp = buf;
	while (nleft && (bufsize > 4)) {
		if (first) {
			first = 0;
		}
		else {
			*cp++ = ':';
			bufsize--;
		}
		i = *ap++;
		*cp++ = hextab[(i >> 4) & 0xf];
		*cp++ = hextab[i & 0xf];
		nleft--;
		bufsize -= 2;
	}
	*cp = '\0';
	return buf;
}

void
rtems_bsdnet_show_inet_routes (void)
{
	struct radix_node_head *rnh;
	struct dinfo d;
	struct rinfo *r;
	int i, error;

	/*
	 * For now we'll handle only AF_INET
	 */
	rnh = rt_tables[AF_INET];
	if (!rnh)
		return;
	d.count = d.capacity = 0;
	d.routes = NULL;
	rtems_bsdnet_semaphore_obtain ();
	error = rnh->rnh_walktree(rnh, show_inet_route, &d);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		printf ("Can't get route info: %s\n", strerror (error));
		return;
	}
	if (d.count == 0) {
		printf ("No routes!\n");
		return;
	}
	printf ("Destination     Gateway/Mask/Hw    Flags     Refs     Use Expire Interface\n");
	for (i = 0, r = d.routes ; i < d.count ; i++, r++) {
		char buf[30];
		char *cp, *fc, flagbuf[10];
		const char *addr;
		unsigned long flagbit;
		struct sockaddr_in *sin;

		sin = (struct sockaddr_in *)&r->dst;
		if (sin->sin_addr.s_addr == INADDR_ANY)
			addr = "default";
		else
			addr = inet_ntop (AF_INET, &sin->sin_addr, buf, sizeof buf);
		printf ("%-16s", addr);
		switch (r->un.sa.sa_family) {
		case AF_INET:
			addr = inet_ntop (AF_INET, &r->un.sin.sin_addr, buf, sizeof buf);
			break;

		case AF_LINK:
			addr = link_ascii (&r->un.sdl, buf, sizeof buf);
			break;

		default:
			addr = "";
			break;
		}
		printf ("%-19s", addr);
		fc = "UGHRDM   XLS";
		for (flagbit = 0x1, cp = flagbuf ; *fc ; flagbit <<= 1, fc++) {
			if ((r->flags & flagbit) && (*fc != ' '))
				*cp++ = *fc;
		}
		*cp = '\0';
		printf ("%-10s%3d%9ld%7ld %.*s%d\n", flagbuf,
					r->refcnt, r->pksent,
					r->expire,
					(int)sizeof r->ifname, r->ifname,
					r->ifunit);
	}
	free (d.routes);
}
