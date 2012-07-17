/*
 * Declarations to fit FreeBSD to RTEMS.
 *
 *******************************************************************
 *                            WARNING                              *
 * This file should *never* be included by any application program *
 *******************************************************************
 */

#ifndef _RTEMS_RTEMS_BSDNET_INTERNAL_H
#define _RTEMS_RTEMS_BSDNET_INTERNAL_H

#include <rtems.h>
#include <rtems/fs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef	unsigned int		vm_offset_t;
typedef	long long		vm_ooffset_t;
typedef	unsigned int		vm_pindex_t;
typedef	unsigned int		vm_size_t;

#define _BSD_OFF_T_	int32_t
#define _BSD_PID_T_	rtems_id
#define _BSD_VA_LIST_	char *

/* make sure we get the network versions of these */
#include <machine/types.h>
#include <machine/param.h>
#include <sys/cdefs.h>

#include <sys/time.h>
#include <sys/ioctl.h>

struct mdproc {
	int	md_flags;
	int	*md_regs;
};

/*
 * Other RTEMS/BSD glue
 */
struct socket;
extern int soconnsleep (struct socket *so);
extern void soconnwakeup (struct socket *so);
#define splnet()	0
#define splimp()	0
#define splx(_s)	do { (_s) = 0; (void) (_s); } while(0)

/* to avoid warnings */
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

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

typedef	u_int64_t	u_quad_t;	/* quads */
typedef	int64_t		quad_t;
typedef	quad_t *	qaddr_t;

typedef void __sighandler_t(int);
typedef	__sighandler_t	*sig_t;	/* type of pointer to a signal function */
#define NSIG    32
struct	sigaltstack {
	char	*ss_sp;			/* signal stack base */
	int	ss_size;		/* signal stack length */
	int	ss_flags;		/* SS_DISABLE and/or SS_ONSTACK */
};

#ifdef _KERNEL
typedef	int		boolean_t;
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

#include <rtems/endian.h>

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
struct	buf;
struct	tty;
struct	uio;
struct	rtems_bsdnet_ifconfig;

/*
 * Redo kernel memory allocation
 */
#define malloc(size,type,flags) rtems_bsdnet_malloc(size,type,flags)
#define free(ptr,type) rtems_bsdnet_free(ptr,type)
#define timeout(ftn,arg,ticks) rtems_bsdnet_timeout(ftn,arg,ticks)

#define	M_NOWAIT	0x0001
void *rtems_bsdnet_malloc (size_t size, int type, int flags);
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
int socket (int, int, int);
int ioctl (int, ioctl_command_t, ...);

/*
 * Events used by networking routines.
 * Everything will break if the application
 * tries to use these events or if the `sleep'
 * events are equal to any of the NETISR * events.
 */
#define SBWAIT_EVENT   RTEMS_EVENT_24
#define SOSLEEP_EVENT  RTEMS_EVENT_25
#define NETISR_IP_EVENT        (1L << NETISR_IP)
#define NETISR_ARP_EVENT       (1L << NETISR_ARP)
#define NETISR_EVENTS  (NETISR_IP_EVENT|NETISR_ARP_EVENT)
#if (SBWAIT_EVENT & SOSLEEP_EVENT & NETISR_EVENTS)
# error "Network event conflict"
#endif

struct socket *rtems_bsdnet_fdToSocket(int fd);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_BSDNET_INTERNAL_H */
