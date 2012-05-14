#if HAVE_CONFIG_H
#include "config.h"
#endif

#define RTEMS_FAST_MUTEX

#ifdef RTEMS_FAST_MUTEX
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#endif

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/domain.h>
#include <sys/mbuf.h>
#include <sys/socketvar.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/callout.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <vm/vm.h>
#include <arpa/inet.h>

#include <net/netisr.h>
#include <net/route.h>

#include "loop.h"

/*
 * Sysctl init all.
 */
void sysctl_register_all(void *arg);

/*
 * Memory allocation
 */
static uint32_t nmbuf       = (64L * 1024L) / MSIZE;
       uint32_t nmbclusters = (128L * 1024L) / MCLBYTES;

/*
 * Network task synchronization
 */
static rtems_id networkSemaphore;
#ifdef RTEMS_FAST_MUTEX
Semaphore_Control   *the_networkSemaphore;
#endif
static rtems_id networkDaemonTid;
static uint32_t   networkDaemonPriority;
static void networkDaemon (void *task_argument);

/*
 * Network timing
 */
int			rtems_bsdnet_ticks_per_second;
int			rtems_bsdnet_microseconds_per_tick;

/*
 * Callout processing
 */
static rtems_interval	ticksWhenCalloutsLastChecked;
struct callout *callfree = NULL;
struct callout calltodo;

/*
 * FreeBSD variables
 */
int nfs_diskless_valid;

/*
 * BOOTP values
 */
struct in_addr rtems_bsdnet_log_host_address = {0};
struct in_addr rtems_bsdnet_bootp_server_address = {0};
char *rtems_bsdnet_bootp_boot_file_name = 0;
char *rtems_bsdnet_bootp_server_name = 0;
char *rtems_bsdnet_domain_name = 0;
char *rtems_bsdnet_bootp_cmdline = 0;
static struct in_addr _rtems_bsdnet_nameserver[sizeof rtems_bsdnet_config.name_server /
			sizeof rtems_bsdnet_config.name_server[0]];
struct in_addr *rtems_bsdnet_nameserver = _rtems_bsdnet_nameserver;
int rtems_bsdnet_nameserver_count = 0;
static struct in_addr _rtems_bsdnet_ntpserver[sizeof rtems_bsdnet_config.ntp_server /
			sizeof rtems_bsdnet_config.ntp_server[0]];
struct in_addr *rtems_bsdnet_ntpserver = _rtems_bsdnet_ntpserver;
int rtems_bsdnet_ntpserver_count = 0;
int32_t rtems_bsdnet_timeoffset = 0;

static const struct sockaddr_in address_template = {
	sizeof(address_template),
	AF_INET,
	0,
	{ INADDR_ANY },
	{ 0, 0, 0, 0, 0, 0, 0, 0 }
};

static void
rtems_bsdnet_initialize_sockaddr_in(struct sockaddr_in *addr)
{
	memcpy(addr, &address_template, sizeof(*addr));
}

/*
 * Perform FreeBSD memory allocation.
 * FIXME: This should be modified to keep memory allocation statistics.
 */
#undef malloc
#undef free
extern void *malloc (size_t);
extern void free (void *);
void *
rtems_bsdnet_malloc (size_t size, int type, int flags)
{
	void *p;
	int try = 0;

	for (;;) {
		p = malloc (size);
		if (p || (flags & M_NOWAIT))
			return p;
		rtems_bsdnet_semaphore_release ();
		if (++try >= 30) {
			rtems_bsdnet_malloc_starvation();
			try = 0;
		}
        rtems_task_wake_after (rtems_bsdnet_ticks_per_second);
		rtems_bsdnet_semaphore_obtain ();
	}
}

/*
 * Free FreeBSD memory
 * FIXME: This should be modified to keep memory allocation statistics.
 */
void
rtems_bsdnet_free (void *addr, int type)
{
	free (addr);
}

/*
 * Externs for BSD data we have to access during initialization
 */
extern struct domain routedomain;
extern struct domain inetdomain;

/*
 * Do the initializations required by the BSD code
 */
static int
bsd_init (void)
{
	int i;
	char *p;

	/*
	 * Set up mbuf cluster data strutures
	 */
	p = rtems_bsdnet_malloc_mbuf ((nmbclusters*MCLBYTES)+MCLBYTES-1, MBUF_MALLOC_NMBCLUSTERS);
	if (p == NULL) {
		printf ("Can't get network cluster memory.\n");
		return -1;
	}
	p = (char *)(((intptr_t)p + (MCLBYTES-1)) & ~(MCLBYTES-1));
	mbutl = (struct mbuf *)p;
	for (i = 0; i < nmbclusters; i++) {
		((union mcluster *)p)->mcl_next = mclfree;
		mclfree = (union mcluster *)p;
		p += MCLBYTES;
		mbstat.m_clfree++;
	}
	mbstat.m_clusters = nmbclusters;
	mclrefcnt = rtems_bsdnet_malloc_mbuf (nmbclusters, MBUF_MALLOC_MCLREFCNT);
	if (mclrefcnt == NULL) {
		printf ("Can't get mbuf cluster reference counts memory.\n");
		return -1;
	}
	memset (mclrefcnt, '\0', nmbclusters);

	/*
	 * Set up mbuf data structures
	 */

	p = rtems_bsdnet_malloc_mbuf(nmbuf * MSIZE + MSIZE - 1,MBUF_MALLOC_MBUF);
	p = (char *)(((uintptr_t)p + MSIZE - 1) & ~(MSIZE - 1));
	if (p == NULL) {
		printf ("Can't get network memory.\n");
		return -1;
	}
	for (i = 0; i < nmbuf; i++) {
		((struct mbuf *)p)->m_next = mmbfree;
		mmbfree = (struct mbuf *)p;
		p += MSIZE;
	}
	mbstat.m_mbufs = nmbuf;
	mbstat.m_mtypes[MT_FREE] = nmbuf;

	/*
	 * Set up domains
	 */
	{

	routedomain.dom_next = domains;
	domains = &routedomain;
	inetdomain.dom_next = domains;
	domains = &inetdomain;
	domaininit (NULL);
	}

  /*
   * Setup the sysctl, normally done by a SYSINIT call.
   */
  sysctl_register_all(0);

	/*
	 * Set up interfaces
	 */
	ifinit (NULL);
	return 0;
}

/*
 * RTEMS Specific Helper Routines
 */
extern void rtems_set_udp_buffer_sizes( u_long, u_long );
extern void rtems_set_tcp_buffer_sizes( u_long, u_long );
extern void rtems_set_sb_efficiency( u_long );

/*
 * Initialize and start network operations
 */
static int
rtems_bsdnet_initialize (void)
{
	rtems_status_code sc;

	/*
	 * Set the priority of all network tasks
	 */
	if (rtems_bsdnet_config.network_task_priority == 0)
		networkDaemonPriority = 100;
	else
		networkDaemonPriority = rtems_bsdnet_config.network_task_priority;

	/*
	 * Set the memory allocation limits
	 */
	if (rtems_bsdnet_config.mbuf_bytecount)
		nmbuf = rtems_bsdnet_config.mbuf_bytecount / MSIZE;
	if (rtems_bsdnet_config.mbuf_cluster_bytecount)
		nmbclusters = rtems_bsdnet_config.mbuf_cluster_bytecount / MCLBYTES;

        rtems_set_udp_buffer_sizes(
          rtems_bsdnet_config.udp_tx_buf_size,
          rtems_bsdnet_config.udp_rx_buf_size
        );

        rtems_set_tcp_buffer_sizes(
          rtems_bsdnet_config.tcp_tx_buf_size,
          rtems_bsdnet_config.tcp_rx_buf_size
        );

        rtems_set_sb_efficiency( rtems_bsdnet_config.sb_efficiency );

	/*
	 * Create the task-synchronization semaphore
	 */
	sc = rtems_semaphore_create (rtems_build_name('B', 'S', 'D', 'n'),
					0,
					RTEMS_PRIORITY |
						RTEMS_BINARY_SEMAPHORE |
						RTEMS_INHERIT_PRIORITY |
						RTEMS_NO_PRIORITY_CEILING |
						RTEMS_LOCAL,
					0,
					&networkSemaphore);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't create network seamphore: `%s'\n", rtems_status_text (sc));
		return -1;
	}
#ifdef RTEMS_FAST_MUTEX
	{
	Objects_Locations location;
	the_networkSemaphore = _Semaphore_Get( networkSemaphore, &location );
	_Thread_Enable_dispatch();
	}
#endif

	/*
	 * Compute clock tick conversion factors
	 */
	rtems_bsdnet_ticks_per_second = rtems_clock_get_ticks_per_second();
	if (rtems_bsdnet_ticks_per_second <= 0)
		rtems_bsdnet_ticks_per_second = 1;
	rtems_bsdnet_microseconds_per_tick =
		1000000 / rtems_bsdnet_ticks_per_second;

	/*
	 * Ensure that `seconds' is greater than 0
	 */
    while (rtems_bsdnet_seconds_since_boot() == 0)
        rtems_task_wake_after(1);

	/*
	 * Set up BSD-style sockets
	 */
	if (bsd_init () < 0)
		return -1;

	/*
	 * Start network daemon
	 */
	networkDaemonTid = rtems_bsdnet_newproc ("ntwk", 4096, networkDaemon, NULL);

	/*
	 * Let other network tasks begin
	 */
	rtems_bsdnet_semaphore_release ();

	rtems_bsdnet_initialize_loop();

	return 0;
}

/*
 * Obtain network mutex
 */
void
rtems_bsdnet_semaphore_obtain (void)
{
#ifdef RTEMS_FAST_MUTEX
	ISR_Level level;
	_ISR_Disable (level);
	_CORE_mutex_Seize (
		&the_networkSemaphore->Core_control.mutex,
		networkSemaphore,
		1,		/* wait */
		0,		/* forever */
		level
		);
	if (_Thread_Executing->Wait.return_code)
		rtems_panic ("rtems-net: can't obtain network sema: %d\n",
                 _Thread_Executing->Wait.return_code);
#else
	rtems_status_code sc;

	sc = rtems_semaphore_obtain (networkSemaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("rtems-net: can't obtain network semaphore: `%s'\n",
                 rtems_status_text (sc));
#endif
}

/*
 * Release network mutex
 */
void
rtems_bsdnet_semaphore_release (void)
{
#ifdef RTEMS_FAST_MUTEX
	int i;

	_Thread_Disable_dispatch();
	i = _CORE_mutex_Surrender (
		&the_networkSemaphore->Core_control.mutex,
		networkSemaphore,
		NULL
		);
	_Thread_Enable_dispatch();
	if (i)
		rtems_panic ("rtems-net: can't release network sema: %i\n");
#else
	rtems_status_code sc;

	sc = rtems_semaphore_release (networkSemaphore);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("rtems-net: can't release network semaphore: `%s'\n",
                 rtems_status_text (sc));
#endif
}

/*
 * Wait for something to happen to a socket buffer
 */
int
sbwait(struct sockbuf *sb)
{
	rtems_event_set events;
	rtems_id tid;
	rtems_status_code sc;

	/*
	 * Soak up any pending events.
	 * The sleep/wakeup synchronization in the FreeBSD
	 * kernel has no memory.
	 */
	rtems_event_receive (SBWAIT_EVENT, RTEMS_EVENT_ANY | RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT, &events);

	/*
	 * Set this task as the target of the wakeup operation.
	 */
	rtems_task_ident (RTEMS_SELF, 0, &tid);
	sb->sb_sel.si_pid = tid;

	/*
	 * Show that socket is waiting
	 */
	sb->sb_flags |= SB_WAIT;

	/*
	 * Release the network semaphore.
	 */
	rtems_bsdnet_semaphore_release ();

	/*
	 * Wait for the wakeup event.
	 */
	sc = rtems_event_receive (SBWAIT_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, sb->sb_timeo, &events);

	/*
	 * Reobtain the network semaphore.
	 */
	rtems_bsdnet_semaphore_obtain ();

	/*
	 * Return the status of the wait.
	 */
	switch (sc) {
	case RTEMS_SUCCESSFUL:	return 0;
	case RTEMS_TIMEOUT:	return EWOULDBLOCK;
	default:		return ENXIO;
	}
}


/*
 * Wake up the task waiting on a socket buffer.
 */
void
sowakeup(
	struct socket *so,
	struct sockbuf *sb)
{
	if (sb->sb_flags & SB_WAIT) {
		sb->sb_flags &= ~SB_WAIT;
		rtems_event_send (sb->sb_sel.si_pid, SBWAIT_EVENT);
	}
	if (sb->sb_wakeup) {
		(*sb->sb_wakeup) (so, sb->sb_wakeuparg);
	}
}

/*
 * For now, a socket can be used by only one task at a time.
 */
int
sb_lock(struct sockbuf *sb)
{
	rtems_panic ("Socket buffer is already in use.");
	return 0;
}
void
wakeup (void *p)
{
	rtems_panic ("Wakeup called");
}

/*
 * Wait for a connection/disconnection event.
 */
int
soconnsleep (struct socket *so)
{
	rtems_event_set events;
	rtems_id tid;
	rtems_status_code sc;

	/*
	 * Soak up any pending events.
	 * The sleep/wakeup synchronization in the FreeBSD
	 * kernel has no memory.
	 */
	rtems_event_receive (SOSLEEP_EVENT, RTEMS_EVENT_ANY | RTEMS_NO_WAIT, RTEMS_NO_TIMEOUT, &events);

	/*
	 * Set this task as the target of the wakeup operation.
	 */
	if (so->so_pgid)
		rtems_panic ("Another task is already sleeping on that socket");
	rtems_task_ident (RTEMS_SELF, 0, &tid);
	so->so_pgid = tid;

	/*
	 * Wait for the wakeup event.
	 */
	sc = rtems_bsdnet_event_receive (SOSLEEP_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, so->so_rcv.sb_timeo, &events);

	/*
	 * Relinquish ownership of the socket.
	 */
	so->so_pgid = 0;

	switch (sc) {
	case RTEMS_SUCCESSFUL:	return 0;
	case RTEMS_TIMEOUT:	return EWOULDBLOCK;
	default:		return ENXIO;
	}
}

/*
 * Wake up a task waiting for a connection/disconnection to complete.
 */
void
soconnwakeup (struct socket *so)
{
	if (so->so_pgid)
		rtems_event_send (so->so_pgid, SOSLEEP_EVENT);
}

/*
 * Send an event to the network daemon.
 * This corresponds to sending a software interrupt in the BSD kernel.
 */
void
rtems_bsdnet_schednetisr (int n)
{
	rtems_event_send (networkDaemonTid, 1 << n);
}

/*
 * The network daemon
 * This provides a context to run BSD software interrupts
 */
static void
networkDaemon (void *task_argument)
{
	rtems_status_code sc;
	rtems_event_set events;
	rtems_interval now;
	int ticksPassed;
	uint32_t   timeout;
	struct callout *c;

	for (;;) {
		c = calltodo.c_next;
		if (c)
			timeout = c->c_time;
		else
			timeout = RTEMS_NO_TIMEOUT;

		sc = rtems_bsdnet_event_receive (NETISR_EVENTS,
						RTEMS_EVENT_ANY | RTEMS_WAIT,
						timeout,
						&events);
		if ( sc == RTEMS_SUCCESSFUL ) {
			if (events & NETISR_IP_EVENT)
				ipintr ();
			if (events & NETISR_ARP_EVENT)
				arpintr ();
		}

  		now = rtems_clock_get_ticks_since_boot();
		ticksPassed = now - ticksWhenCalloutsLastChecked;
		if (ticksPassed != 0) {
			ticksWhenCalloutsLastChecked = now;

			c = calltodo.c_next;
			if (c) {
				c->c_time -= ticksPassed;
				while ((c = calltodo.c_next) != NULL && c->c_time <= 0) {
					void *arg;
					void (*func) (void *);

					func = c->c_func;
					arg = c->c_arg;
					calltodo.c_next = c->c_next;
					c->c_next = callfree;
					callfree = c;
					(*func)(arg);
				}
			}
		}
	}
}

/*
 * Structure passed to task-start stub
 */
struct newtask {
	void (*entry)(void *);
	void *arg;
};

/*
 * Task-start stub
 */
static void
taskEntry (rtems_task_argument arg)
{
	struct newtask t;

	/*
	 * Pick up task information and free
	 * the memory allocated to pass the
	 * information to this task.
	 */
	t = *(struct newtask *)arg;
	free ((struct newtask *)arg);

	/*
	 * Enter the competition for the network semaphore
	 */
	rtems_bsdnet_semaphore_obtain ();

	/*
	 * Enter the task
	 */
	(*t.entry)(t.arg);
	rtems_panic ("Network task returned!\n");
}

/*
 * Start a network task
 */
rtems_id
rtems_bsdnet_newproc (char *name, int stacksize, void(*entry)(void *), void *arg)
{
	struct newtask *t;
	char nm[4];
	rtems_id tid;
	rtems_status_code sc;

	strncpy (nm, name, 4);
	sc = rtems_task_create (rtems_build_name(nm[0], nm[1], nm[2], nm[3]),
		networkDaemonPriority,
		stacksize,
		RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
		RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
		&tid);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't create network daemon `%s': `%s'\n", name, rtems_status_text (sc));

	/*
	 * Set up task arguments
	 */
	t = malloc (sizeof *t);
	t->entry = entry;
	t->arg = arg;

	/*
	 * Start the task
	 */
	sc = rtems_task_start (tid, taskEntry, (rtems_task_argument)t);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't start network daemon `%s': `%s'\n", name, rtems_status_text (sc));

	/*
	 * Let our caller know the i.d. of the new task
	 */
	return tid;
}

rtems_status_code rtems_bsdnet_event_receive (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out)
{
	rtems_status_code sc;

	rtems_bsdnet_semaphore_release ();
	sc = rtems_event_receive (event_in, option_set, ticks, event_out);
	rtems_bsdnet_semaphore_obtain ();
	return sc;
}

/*
 * Return time since startup
 */
void
microtime (struct timeval *t)
{
	rtems_interval now;

	now = rtems_clock_get_ticks_since_boot();
	t->tv_sec = now / rtems_bsdnet_ticks_per_second;
	t->tv_usec = (now % rtems_bsdnet_ticks_per_second) * rtems_bsdnet_microseconds_per_tick;
}

unsigned long
rtems_bsdnet_seconds_since_boot (void)
{
	rtems_interval now;

	now = rtems_clock_get_ticks_since_boot();
	return now / rtems_bsdnet_ticks_per_second;
}

/*
 * Fake random number generator
 */
unsigned long
rtems_bsdnet_random (void)
{
	rtems_interval now;

	now = rtems_clock_get_ticks_since_boot();
	return (now * 99991);
}

/*
 * Callout list processing
 */
void
rtems_bsdnet_timeout(void (*ftn)(void *), void *arg, int ticks)
{
	register struct callout *new, *p, *t;

	if (ticks <= 0)
		ticks = 1;

	/* Fill in the next free callout structure. */
	if (callfree == NULL) {
		callfree = malloc (sizeof *callfree);
		if (callfree == NULL)
			rtems_panic ("No memory for timeout table entry");
		callfree->c_next = NULL;
	}

	new = callfree;
	callfree = new->c_next;
	new->c_arg = arg;
	new->c_func = ftn;

	/*
	 * The time for each event is stored as a difference from the time
	 * of the previous event on the queue.  Walk the queue, correcting
	 * the ticks argument for queue entries passed.  Correct the ticks
	 * value for the queue entry immediately after the insertion point
	 * as well.  Watch out for negative c_time values; these represent
	 * overdue events.
	 */
	for (p = &calltodo;
	    (t = p->c_next) != NULL && ticks > t->c_time; p = t)
		if (t->c_time > 0)
			ticks -= t->c_time;
	new->c_time = ticks;
	if (t != NULL)
		t->c_time -= ticks;

	/* Insert the new entry into the queue. */
	p->c_next = new;
	new->c_next = t;
}

/*
 * Ticks till specified time
 * XXX: This version worries only about seconds, but that's good
 * enough for the way the network code uses this routine.
 */
int
hzto(struct timeval *tv)
{
	long diff = tv->tv_sec - rtems_bsdnet_seconds_since_boot();

	if (diff <= 0)
		return 1;
	return diff * rtems_bsdnet_ticks_per_second;
}

/*
 * Kernel debugging
 */
int rtems_bsdnet_log_priority;
void
rtems_bsdnet_log (int priority, const char *fmt, ...)
{
	va_list args;

	if (priority & rtems_bsdnet_log_priority) {
		va_start (args, fmt);
		vprintf (fmt, args);
		va_end (args);
	}
}

/*
 * IP header checksum routine for processors which don't have an inline version
 */
u_int
in_cksum_hdr (const void *ip)
{
	uint32_t   sum;
	const uint16_t   *sp;
	int i;

	sum = 0;
	sp = (uint16_t   *)ip;
	for (i = 0 ; i < 10 ; i++)
		sum += *sp++;
	while (sum > 0xFFFF)
		sum = (sum & 0xffff) + (sum >> 16);
	return ~sum & 0xFFFF;
}

/*
 * Manipulate routing tables
 */
int rtems_bsdnet_rtrequest (
    int req,
    struct sockaddr *dst,
    struct sockaddr *gateway,
    struct sockaddr *netmask,
    int flags,
    struct rtentry **net_nrt)
{
	int error;

	rtems_bsdnet_semaphore_obtain ();
	error = rtrequest (req, dst, gateway, netmask, flags, net_nrt);
	rtems_bsdnet_semaphore_release ();
	if (error) {
		errno = error;
		return -1;
	}
	return 0;
}

static bool
rtems_bsdnet_setup_interface(
	const char *name,
	const char *ip_address,
	const char *ip_netmask
)
{
	struct sockaddr_in address;
	struct sockaddr_in netmask;
	short flags;

	/*
	 * Bring interface up
	 */
	flags = IFF_UP;
	if (rtems_bsdnet_ifconfig (name, SIOCSIFFLAGS, &flags) < 0) {
		printf ("Can't bring %s up: %s\n", name, strerror (errno));
		return false;
	}

	/*
	 * Set interface netmask
	 */
	rtems_bsdnet_initialize_sockaddr_in(&netmask);
	netmask.sin_addr.s_addr = inet_addr (ip_netmask);
	if (rtems_bsdnet_ifconfig (name, SIOCSIFNETMASK, &netmask) < 0) {
		printf ("Can't set %s netmask: %s\n", name, strerror (errno));
		return false;
	}

	/*
	 * Set interface address
	 */
	rtems_bsdnet_initialize_sockaddr_in(&address);
	address.sin_addr.s_addr = inet_addr (ip_address);
	if (rtems_bsdnet_ifconfig (name, SIOCSIFADDR, &address) < 0) {
		printf ("Can't set %s address: %s\n", name, strerror (errno));
		return false;
	}

	/*
	 * Set interface broadcast address if the interface has the
	 * broadcast flag set.
	 */
	if (rtems_bsdnet_ifconfig (name, SIOCGIFFLAGS, &flags) < 0) {
		printf ("Can't read %s flags: %s\n", name, strerror (errno));
		return false;
	}

	if (flags & IFF_BROADCAST) {
		struct sockaddr_in broadcast;

		rtems_bsdnet_initialize_sockaddr_in(&broadcast);
		broadcast.sin_addr.s_addr =
				address.sin_addr.s_addr | ~netmask.sin_addr.s_addr;
		if (rtems_bsdnet_ifconfig (name, SIOCSIFBRDADDR, &broadcast) < 0) {
			struct in_addr	in_addr;
			char			buf[20];
			in_addr.s_addr = broadcast.sin_addr.s_addr;
			if (!inet_ntop(AF_INET, &in_addr, buf, sizeof(buf)))
					strcpy(buf,"?.?.?.?");
			printf ("Can't set %s broadcast address %s: %s\n",
				name, buf, strerror (errno));
		}
	}

	return true;
}

static int
rtems_bsdnet_setup (void)
{
	struct rtems_bsdnet_ifconfig *ifp;
	int i;
	bool any_if_configured = false;

	/*
	 * Set local parameters
	 */
	if (rtems_bsdnet_config.hostname)
		sethostname (rtems_bsdnet_config.hostname,
					strlen (rtems_bsdnet_config.hostname));
	if (rtems_bsdnet_config.domainname)
		rtems_bsdnet_domain_name =
					strdup (rtems_bsdnet_config.domainname);
	if (rtems_bsdnet_config.log_host)
		rtems_bsdnet_log_host_address.s_addr =
				inet_addr (rtems_bsdnet_config.log_host);
	for (i = 0 ; i < sizeof rtems_bsdnet_config.name_server /
			sizeof rtems_bsdnet_config.name_server[0] ; i++) {
		if (!rtems_bsdnet_config.name_server[i])
			break;
		rtems_bsdnet_nameserver[rtems_bsdnet_nameserver_count++].s_addr
			= inet_addr (rtems_bsdnet_config.name_server[i]);
	}
	for (i = 0 ; i < sizeof rtems_bsdnet_config.ntp_server /
			sizeof rtems_bsdnet_config.ntp_server[0] ; i++) {
		if (!rtems_bsdnet_config.ntp_server[i])
			break;
		rtems_bsdnet_ntpserver[rtems_bsdnet_ntpserver_count++].s_addr
			= inet_addr (rtems_bsdnet_config.ntp_server[i]);
	}

	/*
	 * Configure interfaces
	 */
	any_if_configured |= rtems_bsdnet_setup_interface(
		"lo0",
		"127.0.0.1",
		"255.0.0.0"
	);
	for (ifp = rtems_bsdnet_config.ifconfig ; ifp ; ifp = ifp->next) {
		if (ifp->ip_address == NULL)
			continue;

		any_if_configured |= rtems_bsdnet_setup_interface(
			ifp->name,
			ifp->ip_address,
			ifp->ip_netmask
		);
	}

	/*
	 * Set default route
	 */
	if (rtems_bsdnet_config.gateway && any_if_configured) {
		struct sockaddr_in address;
		struct sockaddr_in netmask;
		struct sockaddr_in gateway;

		rtems_bsdnet_initialize_sockaddr_in(&address);
		rtems_bsdnet_initialize_sockaddr_in(&netmask);
		rtems_bsdnet_initialize_sockaddr_in(&gateway);

		gateway.sin_addr.s_addr = inet_addr (rtems_bsdnet_config.gateway);

		if (rtems_bsdnet_rtrequest (
				RTM_ADD,
				(struct sockaddr *)&address,
				(struct sockaddr *)&gateway,
				(struct sockaddr *)&netmask,
				(RTF_UP | RTF_GATEWAY | RTF_STATIC), NULL) < 0) {
			printf ("Can't set default route: %s\n", strerror (errno));
			return -1;
		}
	}
	return 0;
}

/*
 * Initialize the network
 */
int
rtems_bsdnet_initialize_network(void)
{
	struct rtems_bsdnet_ifconfig *ifp;

	/*
	 * Start network tasks.
	 * Initialize BSD network data structures.
	 */
	if (rtems_bsdnet_initialize () < 0)
		return -1;

	/*
	 * Attach interfaces
	 */
	for (ifp = rtems_bsdnet_config.ifconfig ; ifp ; ifp = ifp->next) {
		rtems_bsdnet_attach (ifp);
	}

	/*
	 * Bring up the network
	 */
	if (rtems_bsdnet_setup () < 0)
		return -1;
	if (rtems_bsdnet_config.bootp)
		(*rtems_bsdnet_config.bootp)();
	return 0;
}

/*
 * Attach a network interface.
 */
void rtems_bsdnet_attach(struct rtems_bsdnet_ifconfig *ifp)
{
	if (ifp) {
		rtems_bsdnet_semaphore_obtain ();
		(ifp->attach)(ifp, 1);
		rtems_bsdnet_semaphore_release ();
	}
}

/*
 * Detach a network interface.
 */
void rtems_bsdnet_detach (struct rtems_bsdnet_ifconfig *ifp)
{
	if (ifp) {
		rtems_bsdnet_semaphore_obtain ();
		(ifp->attach)(ifp, 0);
		rtems_bsdnet_semaphore_release ();
	}
}

/*
 * Interface Configuration.
 */
int rtems_bsdnet_ifconfig(const char *ifname, uint32_t cmd, void *param)
{
	int s, r = 0;
	struct ifreq ifreq;

	/*
	 * Configure interfaces
	 */
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		return -1;

	strncpy (ifreq.ifr_name, ifname, IFNAMSIZ);

	rtems_bsdnet_semaphore_obtain ();

	switch (cmd) {
		case SIOCSIFADDR:
		case SIOCSIFNETMASK:
			memcpy (&ifreq.ifr_addr, param, sizeof (struct sockaddr));
			r = ioctl (s, cmd, &ifreq);
			break;

		case OSIOCGIFADDR:
		case SIOCGIFADDR:
		case OSIOCGIFNETMASK:
		case SIOCGIFNETMASK:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			memcpy (param, &ifreq.ifr_addr, sizeof (struct sockaddr));
			break;

		case SIOCGIFFLAGS:
		case SIOCSIFFLAGS:
			if ((r = ioctl (s, SIOCGIFFLAGS, &ifreq)) < 0)
				break;
			if (cmd == SIOCGIFFLAGS) {
				*((short*) param) = ifreq.ifr_flags;
				break;
			}
			ifreq.ifr_flags |= *((short*) param);
			if ( (*((short*) param) & IFF_UP ) == 0 ) {
			    /* set the interface down */
			    ifreq.ifr_flags &= ~(IFF_UP);
			}
			r = ioctl (s, SIOCSIFFLAGS, &ifreq);
			break;

		case SIOCSIFDSTADDR:
			memcpy (&ifreq.ifr_dstaddr, param, sizeof (struct sockaddr));
			r = ioctl (s, cmd, &ifreq);
			break;

		case OSIOCGIFDSTADDR:
		case SIOCGIFDSTADDR:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			memcpy (param, &ifreq.ifr_dstaddr, sizeof (struct sockaddr));
			break;

		case SIOCSIFBRDADDR:
			memcpy (&ifreq.ifr_broadaddr, param, sizeof (struct sockaddr));
			r = ioctl (s, cmd, &ifreq);
			break;

		case OSIOCGIFBRDADDR:
		case SIOCGIFBRDADDR:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			memcpy (param, &ifreq.ifr_broadaddr, sizeof (struct sockaddr));
			break;

		case SIOCSIFMETRIC:
			ifreq.ifr_metric = *((int*) param);
			r = ioctl (s, cmd, &ifreq);
			break;

		case SIOCGIFMETRIC:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			*((int*) param) = ifreq.ifr_metric;
			break;

		case SIOCSIFMTU:
			ifreq.ifr_mtu = *((int*) param);
			r = ioctl (s, cmd, &ifreq);
			break;

		case SIOCGIFMTU:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			*((int*) param) = ifreq.ifr_mtu;
			break;

		case SIOCSIFPHYS:
			ifreq.ifr_phys = *((int*) param);
			r = ioctl (s, cmd, &ifreq);
			break;

		case SIOCGIFPHYS:
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			*((int*) param) = ifreq.ifr_phys;
			break;

		case SIOCSIFMEDIA:
			ifreq.ifr_media = *((int*) param);
			r = ioctl (s, cmd, &ifreq);
			break;

		case SIOCGIFMEDIA:
			/* 'param' passes the phy index they want to
			 * look at...
			 */
			ifreq.ifr_media = *((int*) param);
			if ((r = ioctl (s, cmd, &ifreq)) < 0)
				break;
			*((int*) param) = ifreq.ifr_media;
			break;

		case SIOCAIFADDR:
		case SIOCDIFADDR:
			r = ioctl(s, cmd, (struct ifreq *) param);
			break;

		default:
			errno = EOPNOTSUPP;
			r = -1;
			break;
	}

	rtems_bsdnet_semaphore_release ();

	close (s);
	return r;
}

/**
 * @brief Splits a network interface name with interface configuration @a
 * config into the unit name and number parts.
 *
 * Memory for the unit name will be allocated from the heap and copied to @a
 * namep.  If @a namep is NULL nothing will be allocated and copied.
 *
 * Returns the unit number or -1 on error.
 */
int
rtems_bsdnet_parse_driver_name (const struct rtems_bsdnet_ifconfig *config, char **namep)
{
	const char *cp = config->name;
	char c;
	int unitNumber = 0;

	if (cp == NULL) {
		printf ("No network driver name.\n");
		return -1;
	}
	while ((c = *cp++) != '\0') {
		if ((c >= '0') && (c <= '9')) {
			int len = cp - config->name;
			if ((len < 2) || (len > 50))
				break;
			for (;;) {
				unitNumber = (unitNumber * 10) + (c - '0');
				c = *cp++;
				if (c == '\0') {
					if (namep != NULL) {
						char *unitName = malloc (len);
						if (unitName == NULL) {
							printf ("No memory.\n");
							return -1;
						}
						strncpy (unitName, config->name, len - 1);
						unitName[len-1] = '\0';
						*namep = unitName;
					}
					return unitNumber;
				}
				if ((c < '0') || (c > '9'))
					break;
			}
			break;
		}
	}
	printf ("Bad network driver name `%s'.\n", config->name);
	return -1;
}

/*
 * Handle requests for more network memory
 * XXX: Another possibility would be to use a semaphore here with
 *      a release in the mbuf free macro.  I have chosen this `polling'
 *      approach because:
 *      1) It is simpler.
 *      2) It adds no complexity to the free macro.
 *      3) Running out of mbufs should be a rare
 *         condition -- predeployment testing of
 *         an application should indicate the
 *         required mbuf pool size.
 * XXX: Should there be a panic if a task is stuck in the loop for
 *      more than a minute or so?
 */
int
m_mballoc(int nmb, int nowait)
{
	if (nowait)
		return 0;
	m_reclaim ();
	if (mmbfree == NULL) {
		int try = 0;
		int print_limit = 30 * rtems_bsdnet_ticks_per_second;

		mbstat.m_wait++;
		for (;;) {
			rtems_bsdnet_semaphore_release ();
			rtems_task_wake_after (1);
			rtems_bsdnet_semaphore_obtain ();
			if (mmbfree)
				break;
			if (++try >= print_limit) {
				printf ("Still waiting for mbuf.\n");
				try = 0;
			}
		}
	}
	else {
		mbstat.m_drops++;
	}
	return 1;
}

int
m_clalloc(int ncl, int nowait)
{
	if (nowait)
		return 0;
	m_reclaim ();
	if (mclfree == NULL) {
		int try = 0;
		int print_limit = 30 * rtems_bsdnet_ticks_per_second;

		mbstat.m_wait++;
		for (;;) {
			rtems_bsdnet_semaphore_release ();
			rtems_task_wake_after (1);
			rtems_bsdnet_semaphore_obtain ();
			if (mclfree)
				break;
			if (++try >= print_limit) {
				printf ("Still waiting for mbuf cluster.\n");
				try = 0;
			}
		}
	}
	else {
		mbstat.m_drops++;
	}
	return 1;
}
