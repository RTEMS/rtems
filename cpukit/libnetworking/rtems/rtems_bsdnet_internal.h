/*
 * Declarations to fit FreeBSD to RTEMS.
 *
 *******************************************************************
 *                            WARNING                              *
 * This file should *never* be included by any application program *
 *******************************************************************
 *
 *  $Id$
 */

#ifndef _RTEMS_BSDNET_INTERNAL_H_
#define _RTEMS_BSDNET_INTERNAL_H_

#include <rtems.h>

typedef	unsigned int		vm_offset_t;
typedef	long long		vm_ooffset_t;
typedef	unsigned int		vm_pindex_t;
typedef	unsigned int		vm_size_t;

#define _BSD_OFF_T_	rtems_signed32
#define _BSD_PID_T_	rtems_id
#define _BSD_VA_LIST_	char *

/* make sure we get the network versions of these */
#include <machine/types.h>
#include <machine/param.h>

#include <sys/time.h>
/*
struct  itimerval {
  struct  timeval it_interval;
  struct  timeval it_value;
};
*/
struct mdproc {
	int	md_flags;
	int	*md_regs;
};

#define USHRT_MAX	65535

/*
 * Other RTEMS/BSD glue
 */
struct socket;
extern int soconnsleep (struct socket *so);
extern void soconnwakeup (struct socket *so);
#define splnet()	0
#define splimp()	0
#define splx(_s)	do { (_s) = 0; } while(0)

#define ovbcopy(f,t,n) bcopy(f,t,n)
#define copyout(f,t,n) (memcpy(t,f,n),0)
#define copyin(f,t,n) (memcpy(t,f,n),0)

#define random()	rtems_bsdnet_random()
#define panic	rtems_panic
#define suser(a,b)	0

void	microtime (struct timeval *tv);
#define hz rtems_bsdnet_ticks_per_second
#define tick rtems_bsdnet_microseconds_per_tick

#define log	rtems_bsdnet_log

/*
 * Since we can't have two sys/types.h files, we'll hack around
 * and copy the contents of the BSD sys/types.h to here....
 */
#include <sys/cdefs.h>

typedef	u_int64_t	u_quad_t;	/* quads */
typedef	int64_t		quad_t;
typedef	quad_t *	qaddr_t;

typedef void __sighandler_t __P((int));
typedef	__sighandler_t	*sig_t;	/* type of pointer to a signal function */
#define NSIG    32
struct	sigaltstack {
	char	*ss_sp;			/* signal stack base */
	int	ss_size;		/* signal stack length */
	int	ss_flags;		/* SS_DISABLE and/or SS_ONSTACK */
};

#ifdef KERNEL
typedef	int		boolean_t;
typedef	struct vm_page	*vm_page_t;
#endif

#ifndef _POSIX_SOURCE
/*
 * minor() gives a cookie instead of an index since we don't want to
 * change the meanings of bits 0-15 or waste time and space shifting
 * bits 16-31 for devices that don't use them.
 */
#define	major(x)	((int)(((u_int)(x) >> 8)&0xff))	/* major number */
#define	minor(x)	((int)((x)&0xffff00ff))		/* minor number */
#define	makedev(x,y)	((dev_t)(((x) << 8) | (y)))	/* create dev_t */
#endif

#include <machine/endian.h>

typedef quad_t          rlim_t;         /* resource limit */
typedef	u_int32_t	fixpt_t;	/* fixed point number */

/*
 * Forward structure declarations for function prototypes.  We include the
 * common structures that cross subsystem boundaries here; others are mostly
 * used in the same place that the structure is defined.
 */
struct	proc;
struct	pgrp;
struct	ucred;
struct	rusage;
struct	file;
struct	buf;
struct	tty;
struct	uio;
struct	rtems_bsdnet_ifconfig;

/*
 * Redo kernel memory allocation
 */
#define malloc rtems_bsdnet_malloc
#define free rtems_bsdnet_free

#define	M_NOWAIT	0x0001
void *rtems_bsdnet_malloc (unsigned long size, int type, int flags);
void rtems_bsdnet_free (void *addr, int type);

void rtems_bsdnet_semaphore_obtain (void);
void rtems_bsdnet_semaphore_release (void);
void rtems_bsdnet_schednetisr (int n);
int rtems_bsdnet_parse_driver_name (const struct rtems_bsdnet_ifconfig *config, char **namep);

unsigned long rtems_bsdnet_seconds_since_boot (void);
unsigned long rtems_bsdnet_random (void);

rtems_id rtems_bsdnet_newproc (
  char  *name,
  int   stacksize,
  void  (*entry)(void *),
  void  *arg
);

rtems_status_code rtems_bsdnet_event_receive (
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/*
 * Network configuration
 */
extern int rtems_bsdnet_ticks_per_second;
extern int rtems_bsdnet_microseconds_per_tick;
extern struct in_addr rtems_bsdnet_log_host_address;
extern char *rtems_bsdnet_domain_name;
extern struct in_addr rtems_bsdnet_nameserver[];
extern int rtems_bsdnet_nameserver_count;

/*
 * Internal IOCTL command
 */
#define SIO_RTEMS_SHOW_STATS	_IO('i', 250)

/*
 * Some extra prototypes
 */
int sethostname (char *name, size_t namelen);
void domaininit (void *);
void ifinit (void *);
void ipintr (void);
void arpintr (void);
void bootpc_init(int );
int socket (int, int, int);
int ioctl (int, int, ...);

/*
 * Events used by networking routines.
 * Everything will break if the application
 * tries to use these events or if the `sleep'
 * events are equal to any of the NETISR * events.
 */
#define SBWAIT_EVENT   RTEMS_EVENT_24
#define SOSLEEP_EVENT  RTEMS_EVENT_25
#define NETISR_IP_EVENT                (1 << NETISR_IP)
#define NETISR_ARP_EVENT       (1 << NETISR_ARP)
#define NETISR_EVENTS  (NETISR_IP_EVENT|NETISR_ARP_EVENT)
#if (SBWAIT_EVENT & SOSLEEP_EVENT & NETISR_EVENTS)
# error "Network event conflict"
#endif

#endif /* _RTEMS_BSDNET_INTERNAL_H_ */
