/*
 *  $Id$
 */

#include <stdlib.h>

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
#include <net/route.h>

#include <netinet/in.h>

/*
 * We'll use the application versions of malloc and free.
 */
#undef malloc
#undef free
/*
 * Information per route
 */
struct rinfo {
	struct in_addr	dst;
	struct in_addr	gw_mask;
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
 * Package everything up before printing it.
 * We don't want to block all network operations till
 * the printing completes!
 */
static int
show_inet_route (rn, vw)
	struct radix_node *rn;
	void *vw;
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
	r->dst = ((struct sockaddr_in *)rt_key(rt))->sin_addr;
	if (rt->rt_flags & RTF_GATEWAY)
		r->gw_mask = ((struct sockaddr_in *)rt->rt_gateway)->sin_addr;
	else if (!(rt->rt_flags & RTF_HOST))
		r->gw_mask = ((struct sockaddr_in *)rt_mask(rt))->sin_addr;
	r->flags = rt->rt_flags;
	r->refcnt = rt->rt_refcnt;
	r->pksent = rt->rt_rmx.rmx_pksent;
	r->expire = rt->rt_rmx.rmx_expire;
	ifp = rt->rt_ifp;
	strncpy (r->ifname, ifp->if_name, sizeof r->ifname);
	r->ifunit = ifp->if_unit;
	return 0;
}

/*
 * Reentrant version of inet_ntoa
 */
char *
inet_ntoa_r (struct in_addr ina, char *buf)
{
	unsigned char *ucp = (unsigned char *)&ina;

	sprintf (buf, "%d.%d.%d.%d",
				ucp[0] & 0xff,
				ucp[1] & 0xff,
				ucp[2] & 0xff,
				ucp[3] & 0xff);
											return buf;
}

void
rtems_bsdnet_show_inet_routes (void)
{
	struct radix_node_head *rnh;
	struct dinfo d;
	struct rinfo *r;
	int i, error;

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
	printf ("Destination     Gateway/Mask    Flags     Refs     Use Expire Interface\n");
	for (i = 0, r = d.routes ; i < d.count ; i++, r++) {
		char buf[30];
		char *cp, *fc, flagbuf[10];
		unsigned long flagbit;

		if (r->dst.s_addr == INADDR_ANY)
			cp = "default";
		else
			cp = inet_ntoa_r (r->dst, buf);
		printf ("%-16s", cp);
		if (!(r->flags & RTF_HOST))
			cp = inet_ntoa_r (r->gw_mask, buf);
		else
			cp = "";
		printf ("%-16s", cp);
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
