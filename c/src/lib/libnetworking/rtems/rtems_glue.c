/*
 *  $Id$
 */

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

/*
 * Events used by networking routines.
 * Everything will break if the application
 * tries to use these events or if the `sleep'
 * events are equal to any of the NETISR * events.
 */
#define SBWAIT_EVENT	RTEMS_EVENT_24
#define SOSLEEP_EVENT	RTEMS_EVENT_25
#define NETISR_IP_EVENT		(1 << NETISR_IP)
#define NETISR_ARP_EVENT	(1 << NETISR_ARP)
#define NETISR_EVENTS	(NETISR_IP_EVENT|NETISR_ARP_EVENT)


/*
 * Memory allocation
 */
static int nmbuf	= (64 * 1024) / MSIZE;
       int nmbclusters	= (128 * 1024) / MCLBYTES;

/*
 * Socket buffering parameters
 */
unsigned long sb_efficiency = 8;

/*
 * Network task synchronization
 */
static rtems_id networkSemaphore;
static rtems_id networkDaemonTid;
static rtems_unsigned32 networkDaemonPriority;
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
static struct callout *callfree, calltodo;

/*
 * FreeBSD variables
 */
int nfs_diskless_valid;

/*
 * BOOTP values
 */
struct in_addr rtems_bsdnet_log_host_address;
struct in_addr rtems_bsdnet_bootp_server_address;
char *rtems_bsdnet_bootp_boot_file_name;
char *rtems_bsdnet_domain_name;
struct in_addr rtems_bsdnet_nameserver[sizeof rtems_bsdnet_config.name_server /
			sizeof rtems_bsdnet_config.name_server[0]];
int rtems_bsdnet_nameserver_count;

extern rtems_libio_handler_t rtems_bsdnet_io_handler;

/*
 * Perform FreeBSD memory allocation.
 * FIXME: This should be modified to keep memory allocation statistics.
 */
#undef malloc
#undef free
extern void *malloc (size_t);
extern void free (void *);
void *
rtems_bsdnet_malloc (unsigned long size, int type, int flags)
{
	void *p;

	for (;;) {
		p = malloc (size);
		if (p)
			return p;
		if (flags & M_NOWAIT)
			return p;
		/*
		 * FIXME: This should be redone as:
		 * static volatile int rtems_bsdnet_need_memory;
		 *     
		 *	rtems_bsdnet_need_memory = 1;
		 *	message_queue_receive
		 *
		 * Then in rtems_bsdnet_freee:
		 *	free (....);
		 *	if (rtems_bsdnet_need_memory)
		 *		rtems_bsdnet_need_memory = 0;
		 *		message_queue_broadcast
		 */
		rtems_task_wake_after (rtems_bsdnet_ticks_per_second);
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
 * Do the initializations required by the BSD code
 * FIXME: Maybe we should use a different memory allocation scheme that
 * would let us share space between mbufs and mbuf clusters.
 * For now, we'll just take the easy way out!
 */
static void
bsd_init ()
{
	/*
	 * Set up mbuf data strutures
	 * Cluster allocation *must* come first -- see comment on kmem_malloc().
	 */
	m_clalloc (nmbclusters, M_DONTWAIT);
	mclrefcnt = malloc (nmbclusters);
	if (mclrefcnt == NULL)
		rtems_panic ("No memory for mbuf cluster reference counts.");
	memset (mclrefcnt, '\0', nmbclusters);
	m_mballoc (nmbuf, M_DONTWAIT);
	mbstat.m_mtypes[MT_FREE] = nmbuf;


	/*
	 * Set up domains
	 */
	{
	extern struct domain routedomain;
	extern struct domain inetdomain;

	routedomain.dom_next = domains;
	domains = &routedomain;
	inetdomain.dom_next = domains;
	domains = &inetdomain;
	domaininit (NULL);
	}

	/*
	 * Set up interfaces
	 */
	ifinit (NULL);
}

/*
 * Initialize and start network operations
 */
static void
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

	/*
	 * Create the task-synchronization semaphore
	 */
	sc = rtems_semaphore_create (rtems_build_name('B', 'S', 'D', 'n'),
					0,
					RTEMS_FIFO |
						RTEMS_BINARY_SEMAPHORE |
						RTEMS_NO_INHERIT_PRIORITY |
						RTEMS_NO_PRIORITY_CEILING |
						RTEMS_LOCAL,
					0,
					&networkSemaphore);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't create network seamphore: `%s'\n", rtems_status_text (sc));

	/*
	 * Compute clock tick conversion factors
	 */
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &rtems_bsdnet_ticks_per_second);
	if (rtems_bsdnet_ticks_per_second <= 0)
		rtems_bsdnet_ticks_per_second = 1;
	rtems_bsdnet_microseconds_per_tick = 1000000 / rtems_bsdnet_ticks_per_second;

	/*
	 * Ensure that `seconds' is greater than 0
	 */
	rtems_task_wake_after (rtems_bsdnet_ticks_per_second);

	/*
	 * Set up BSD-style sockets
	 */
	bsd_init ();

	/*
	 * Start network daemon
	 */
	networkDaemonTid = rtems_bsdnet_newproc ("ntwk", 4096, networkDaemon, NULL);

	/*
	 * Register as an external I/O handler
	 */
	rtems_register_libio_handler (RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET,
						&rtems_bsdnet_io_handler);

	/*
	 * Let other network tasks begin
	 */
	rtems_bsdnet_semaphore_release ();
}

rtems_id TaskWithSemaphore;
/*
 * Obtain network mutex
 */
void
rtems_bsdnet_semaphore_obtain (void)
{
	rtems_status_code sc;

	sc = rtems_semaphore_obtain (networkSemaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
rtems_task_ident (RTEMS_SELF, 0, &TaskWithSemaphore);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't obtain network semaphore: `%s'\n", rtems_status_text (sc));
}

/*
 * Release network mutex
 */
void
rtems_bsdnet_semaphore_release (void)
{
	rtems_status_code sc;

TaskWithSemaphore = 0;
	sc = rtems_semaphore_release (networkSemaphore);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't release network semaphore: `%s'\n", rtems_status_text (sc));
										}

/*
 * Wait for something to happen to a socket buffer
 */
int
sbwait(sb)
	struct sockbuf *sb;
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
	if (sb->sb_sel.si_pid)
		rtems_panic ("Another task is already sleeping on that socket buffer");
	rtems_task_ident (RTEMS_SELF, 0, &tid);
	sb->sb_sel.si_pid = tid;

	/*
	 * Mark the socket buffer as waiting.
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
	 * Relinquish ownership of the socket buffer
	 */
	sb->sb_flags &= ~SB_WAIT;
	sb->sb_sel.si_pid = 0;

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
sowakeup(so, sb)
	register struct socket *so;
	register struct sockbuf *sb;
{
	if (sb->sb_flags & SB_WAIT) {
		sb->sb_flags &= ~SB_WAIT;
		rtems_event_send (sb->sb_sel.si_pid, SBWAIT_EVENT);
	}
}

/*
 * For now, a socket can be used by only one task at a time.
 */
int
sb_lock(sb)
	register struct sockbuf *sb;
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
void
soconnsleep (struct socket *so)
{
	rtems_event_set events;
	rtems_id tid;

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
	rtems_bsdnet_event_receive (SOSLEEP_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);

	/*
	 * Relinquish ownership of the socket.
	 */
	so->so_pgid = 0;
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
	rtems_event_set events;
	rtems_interval now;
	int ticksPassed;
	unsigned32 timeout;
	struct callout *c;

	for (;;) {
		c = calltodo.c_next;
		if (c)
			timeout = c->c_time;
		else
			timeout = RTEMS_NO_TIMEOUT;
		rtems_bsdnet_event_receive (NETISR_EVENTS,
						RTEMS_EVENT_ANY | RTEMS_WAIT,
						timeout,
						&events);
		if (events & NETISR_IP_EVENT)
			ipintr ();
		if (events & NETISR_ARP_EVENT)
			arpintr ();
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
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

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	t->tv_sec = now / rtems_bsdnet_ticks_per_second;
	t->tv_usec = (now % rtems_bsdnet_ticks_per_second) * rtems_bsdnet_microseconds_per_tick;
}

unsigned long
rtems_bsdnet_seconds_since_boot (void)
{
	rtems_interval now;

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	return now / rtems_bsdnet_ticks_per_second;
}

/*
 * Fake random number generator
 */
unsigned long
rtems_bsdnet_random (void)
{
	rtems_interval now;

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	return (now * 99991);
}

/*
 * Callout list processing
 */
void
timeout(void (*ftn)(void *), void *arg, int ticks)
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
 * FIXME: This version worries only about seconds, but that's good
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
 * Hack alert: kmem_malloc `knows' that its
 * first invocation is to get mbuf clusters!
 */
int mb_map_full;
vm_map_t mb_map;
vm_offset_t
kmem_malloc (vm_map_t *map, vm_size_t size, boolean_t waitflag)
{
	void *p;
	
	/*
	 * Can't get memory if we're already running.
	 */
	if (networkDaemonTid) {
		if (waitflag == M_WAITOK)
			rtems_panic ("Someone is asking for more memory");
		return 0;
	}

#define ROUNDSIZE 2048
	p = malloc (size+ROUNDSIZE);
	p = (void *)((unsigned long)p & ~(ROUNDSIZE-1));
	if ((p == NULL) && (waitflag == M_WAITOK))
		rtems_panic ("Can't get initial network memory!");
	if (mbutl == NULL)
		mbutl = p;
	return (vm_offset_t)p;
}

/*
 * IP header checksum routine for processors which don't have an inline version
 */
u_int
in_cksum_hdr (const void *ip)
{
	rtems_unsigned32 sum;
	const rtems_unsigned16 *sp;
	int i;

	sum = 0;
	sp = (rtems_unsigned16 *)ip;
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

static void
rtems_bsdnet_setup (void)
{
	struct rtems_bsdnet_ifconfig *ifp;
	int s;
	struct ifreq ifreq;
	struct sockaddr_in address;
	struct sockaddr_in netmask;
	struct sockaddr_in broadcast;
	struct sockaddr_in gateway;
	int i;

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

	/*
	 * Configure interfaces
	 */
	s = socket (AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		rtems_panic ("Can't create initial socket: %s", strerror (errno));
	for (ifp = rtems_bsdnet_config.ifconfig ; ifp ; ifp = ifp->next) {
		if (ifp->ip_address == NULL)
			continue;

		/*
		 * Get the interface flags
		 */
		strcpy (ifreq.ifr_name, ifp->name);
		if (ioctl (s, SIOCGIFFLAGS, &ifreq) < 0)
			rtems_panic ("Can't get %s flags: %s", ifp->name, strerror (errno));

		/*
		 * Bring interface up
		 */
		ifreq.ifr_flags |= IFF_UP;
		if (ioctl (s, SIOCSIFFLAGS, &ifreq) < 0)
			rtems_panic ("Can't bring %s up: %s", ifp->name, strerror (errno));

		/*
		 * Set interface netmask
		 */
		memset (&netmask, '\0', sizeof netmask);
		netmask.sin_len = sizeof netmask;
		netmask.sin_family = AF_INET;
		netmask.sin_addr.s_addr = inet_addr (ifp->ip_netmask);
		memcpy (&ifreq.ifr_addr, &netmask, sizeof netmask);
		if (ioctl (s, SIOCSIFNETMASK, &ifreq) < 0)
			rtems_panic ("Can't set %s netmask: %s", ifp->name, strerror (errno));

		/*
		 * Set interface address
		 */
		memset (&address, '\0', sizeof address);
		address.sin_len = sizeof address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr (ifp->ip_address);
		memcpy (&ifreq.ifr_addr, &address, sizeof address);
		if (ioctl (s, SIOCSIFADDR, &ifreq) < 0)
			rtems_panic ("Can't set %s address: %s", ifp->name, strerror (errno));

		/*
		 * Set interface broadcast address
		 */
		memset (&broadcast, '\0', sizeof broadcast);
		broadcast.sin_len = sizeof broadcast;
		broadcast.sin_family = AF_INET;
		broadcast.sin_addr.s_addr = address.sin_addr.s_addr | ~netmask.sin_addr.s_addr;
		memcpy (&ifreq.ifr_broadaddr, &broadcast, sizeof broadcast);
		if (ioctl (s, SIOCSIFBRDADDR, &ifreq) < 0)
			rtems_panic ("Can't set %s broadcast address: %s", ifp->name, strerror (errno));
	}

	/*
	 * We're done with the dummy socket
	 */
	close (s);

	/*
	 * Set default route
	 */
	if (rtems_bsdnet_config.gateway) {
		address.sin_addr.s_addr = INADDR_ANY;
		netmask.sin_addr.s_addr = INADDR_ANY;
		memset (&gateway, '\0', sizeof gateway);
		gateway.sin_len = sizeof gateway;
		gateway.sin_family = AF_INET;
		gateway.sin_addr.s_addr = inet_addr (rtems_bsdnet_config.gateway);
		if (rtems_bsdnet_rtrequest (
				RTM_ADD, 
				(struct sockaddr *)&address,
				(struct sockaddr *)&gateway,
				(struct sockaddr *)&netmask,
				(RTF_UP | RTF_GATEWAY | RTF_STATIC), NULL) < 0)
			rtems_panic ("Can't set default route: %s", strerror (errno));
	}
}

/*
 * Initialize the network
 */
int
rtems_bsdnet_initialize_network (void)
{
	struct rtems_bsdnet_ifconfig *ifp;

	/*
	 * Start network tasks.
	 * Initialize BSD network data structures.
	 */
	rtems_bsdnet_initialize ();

	/*
	 * Attach interfaces
	 */
	for (ifp = rtems_bsdnet_config.ifconfig ; ifp ; ifp = ifp->next) {
		rtems_bsdnet_semaphore_obtain ();
		(ifp->attach)(ifp);
		rtems_bsdnet_semaphore_release ();
	}

	/*
	 * Bring up the network
	 */
	rtems_bsdnet_setup ();
	if (rtems_bsdnet_config.bootp)
		(*rtems_bsdnet_config.bootp)();
	return 0;
}
