/*
 * main.c - Point-to-Point Protocol main module
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#define RCSID	"$Id$"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <setjmp.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>

#include "pppd.h"
#include "magic.h"
#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#ifdef INET6
#include "ipv6cp.h"
#endif
#include "upap.h"
#include "chap.h"
#include "ccp.h"
#include "pathnames.h"
#include "patchlevel.h"
#include "rtemsdialer.h"

#ifdef CBCP_SUPPORT
#include "cbcp.h"
#endif

#ifdef IPX_CHANGE
#include "ipxcp.h"
#endif /* IPX_CHANGE */
#ifdef AT_CHANGE
#include "atcp.h"
#endif

static const char rcsid[] = RCSID;

/* interface vars */
char ifname[32];		/* Interface name */
int pppifunit;			/* Interface unit number */

char hostname[MAXNAMELEN];	/* Our hostname */
static char ppp_devnam[MAXPATHLEN]; /* name of PPP tty (maybe ttypx) */

int pppd_ttyfd;			/* Serial port file descriptor */
int baud_rate;			/* Actual bits/second for serial device */
int hungup;			/* terminal has been hung up */
int privileged;			/* we're running as real uid root */
int need_holdoff;		/* need holdoff period before restarting */
int detached;			/* have detached from terminal */
struct stat devstat;		/* result of stat() on devnam */
int prepass = 0;		/* doing prepass to find device name */
int devnam_fixed;		/* set while in options.ttyxx file */
volatile int pppd_status;		/* exit status for pppd */
int unsuccess;			/* # unsuccessful connection attempts */
int do_callback;		/* != 0 if we should do callback next */
int doing_callback;		/* != 0 if we are doing callback */
char *callback_script;		/* script for doing callback */
dialerfp pppd_dialer;

int (*holdoff_hook)(void) = NULL;
int (*new_phase_hook)(int) = NULL;

static int fd_ppp = -1;		/* fd for talking PPP */
static int pty_master;		/* fd for master side of pty */
static int pty_slave;		/* fd for slave side of pty */
static int real_ttyfd;		/* fd for actual serial port (not pty) */

int pppd_phase;			/* where the link is at */
int pppd_kill_link;
int open_ccp_flag;

char **script_env;		/* Env. variable values for scripts */
int s_env_nalloc;		/* # words avail at script_env */

u_char outpacket_buf[PPP_MRU+PPP_HDRLEN]; /* buffer for outgoing packet */
u_char inpacket_buf[PPP_MRU+PPP_HDRLEN]; /* buffer for incoming packet */

char *no_ppp_msg = "Sorry - this system lacks PPP kernel support\n";

static struct timeval start_time;	/* Time when link was started. */

struct pppd_stats link_stats;
int link_connect_time;
int link_stats_valid;

/* Prototypes for procedures local to this file. */

static void cleanup(void);
static void close_tty(void);
static void get_input(void);
static void calltimeout(void);
static struct timeval *timeleft(struct timeval *);
static void holdoff_end(void *);
static int device_script(int, int, char *);

extern	char	*ttyname(int);
extern	char	*getlogin(void);
int pppdmain(int, char *[]);

/*
 * PPP Data Link Layer "protocol" table.
 * One entry per supported protocol.
 * The last entry must be NULL.
 */
struct protent *protocols[] = {
    &lcp_protent,
    &pap_protent,
    &chap_protent,
#ifdef CBCP_SUPPORT
    &cbcp_protent,
#endif
    &ipcp_protent,
#ifdef INET6
    &ipv6cp_protent,
#endif
    &ccp_protent,
#ifdef IPX_CHANGE
    &ipxcp_protent,
#endif
#ifdef AT_CHANGE
    &atcp_protent,
#endif
    NULL
};

int
pppdmain(
    int argc,
    char *argv[])
{
    int i, fdflags, t;
    char *connector;
    struct timeval timo;
    struct protent *protp;

    new_phase(PHASE_INITIALIZE);

    script_env = NULL;
    hostname[MAXNAMELEN-1] = 0;
    privileged = 1;
    privileged_option = 1;

    /*
     * Initialize magic number generator now so that protocols may
     * use magic numbers in initialization.
     */
    magic_init();

#ifdef XXX_XXX
    /* moved code the the rtems_pppd_reset_options function */

    /*
     * Initialize to the standard option set, then parse, in order,
     * the system options file, the user's options file,
     * the tty's options file, and the command line arguments.
     */
    for (i = 0; (protp = protocols[i]) != NULL; ++i)
        (*protp->init)(0);
#endif


    if (!ppp_available()) {
	option_error(no_ppp_msg);
	return(EXIT_NO_KERNEL_SUPPORT);
    }

    /*
     * Check that the options given are valid and consistent.
     */
    if (!sys_check_options()) {
	return(EXIT_OPTION_ERROR);
    }
    if (!auth_check_options()) {
	return(EXIT_OPTION_ERROR);
    }
    for (i = 0; (protp = protocols[i]) != NULL; ++i)
	if (protp->check_options != NULL)
	    (*protp->check_options)();

    /* default holdoff to 0 if no connect script has been given */
    if (connect_script == 0 && !holdoff_specified)
	holdoff = 0;

    if (default_device)
	nodetach = 1;

    /*
     * Initialize system-dependent stuff.
     */
    sys_init();
    /* if (debug)
	setlogmask(LOG_UPTO(LOG_DEBUG));
    */

    do_callback = 0;
    for (;;) {

	need_holdoff = 1;
	pppd_ttyfd = -1;
	real_ttyfd = -1;
	pppd_status = EXIT_OK;
	++unsuccess;
	doing_callback = do_callback;
	do_callback = 0;

	new_phase(PHASE_SERIALCONN);

	/*
	 * Get a pty master/slave pair if the pty, notty, or record
	 * options were specified.
	 */
	strlcpy(ppp_devnam, devnam, sizeof(ppp_devnam));
	pty_master = -1;
	pty_slave = -1;

	/*
	 * Open the serial device and set it up to be the ppp interface.
	 * First we open it in non-blocking mode so we can set the
	 * various termios flags appropriately.  If we aren't dialling
	 * out and we want to use the modem lines, we reopen it later
	 * in order to wait for the carrier detect signal from the modem.
	 */
	hungup = 0;
	pppd_kill_link = 0;
	connector = doing_callback? callback_script: connect_script;
	if (devnam[0] != 0) {
	    for (;;) {
		/* If the user specified the device name, become the
		   user before opening it. */
		int err;
		pppd_ttyfd = open(devnam, O_NONBLOCK | O_RDWR, 0);
		err = errno;
		if (pppd_ttyfd >= 0) {
		    break;
		}
		errno = err;
		if (err != EINTR) {
		    error("Failed to open %s: %m", devnam);
		    pppd_status = EXIT_OPEN_FAILED;
		}
		if (!persist || err != EINTR)
		    goto fail;
	    }
	    if ((fdflags = fcntl(pppd_ttyfd, F_GETFL)) == -1
		|| fcntl(pppd_ttyfd, F_SETFL, fdflags & ~O_NONBLOCK) < 0)
		warn("Couldn't reset non-blocking mode on device: %m");

	    /*
	     * Set line speed, flow control, etc.
	     * If we have a non-null connection or initializer script,
	     * on most systems we set CLOCAL for now so that we can talk
	     * to the modem before carrier comes up.  But this has the
	     * side effect that we might miss it if CD drops before we
	     * get to clear CLOCAL below.  On systems where we can talk
	     * successfully to the modem with CLOCAL clear and CD down,
	     * we could clear CLOCAL at this point.
	     */
	    set_up_tty(pppd_ttyfd, ((connector != NULL && connector[0] != 0)
			       || initializer != NULL));
	    real_ttyfd = pppd_ttyfd;
	}

	/* run connection script */
	if ((connector && connector[0]) || initializer) {
	    if (real_ttyfd != -1) {
		/* XXX do this if doing_callback == CALLBACK_DIALIN? */
		if (!default_device && modem) {
		    setdtr(real_ttyfd, 0);	/* in case modem is off hook */
		    sleep(1);
		    setdtr(real_ttyfd, 1);
		}
	    }

	    if (initializer && initializer[0]) {
		if (device_script(pppd_ttyfd, DIALER_INIT, initializer) < 0) {
		    error("Initializer script failed");
		    pppd_status = EXIT_INIT_FAILED;
		    goto fail;
		}
		if (pppd_kill_link)
		    goto disconnect;

		info("Serial port initialized.");
	    }

	    if (connector && connector[0]) {
		if (device_script(pppd_ttyfd, DIALER_CONNECT, connector) < 0) {
		    error("Connect script failed");
		    pppd_status = EXIT_CONNECT_FAILED;
		    goto fail;
		}
		if (pppd_kill_link)
		    goto disconnect;

		info("Serial connection established.");
	    }

	    /* set line speed, flow control, etc.;
	       clear CLOCAL if modem option */
	    if (real_ttyfd != -1)
		set_up_tty(real_ttyfd, 0);

	    if (doing_callback == CALLBACK_DIALIN)
		connector = NULL;
	}

	/* reopen tty if necessary to wait for carrier */
	if (connector == NULL && modem && devnam[0] != 0) {
	    for (;;) {
		if ((i = open(devnam, O_RDWR)) >= 0)
		    break;
		if (errno != EINTR) {
		    error("Failed to reopen %s: %m", devnam);
		    pppd_status = EXIT_OPEN_FAILED;
		}
		if (!persist || errno != EINTR || hungup || pppd_kill_link)
		    goto fail;
	    }
	    close(i);
	}

        info("Serial connection established.");
        sleep(1);

	/* run welcome script, if any */
	if (welcomer && welcomer[0]) {
	    if (device_script(pppd_ttyfd, DIALER_WELCOME, welcomer) < 0)
		warn("Welcome script failed");
	}

	/* set up the serial device as a ppp interface */
	fd_ppp = establish_ppp(pppd_ttyfd);
	if (fd_ppp < 0) {
	    pppd_status = EXIT_FATAL_ERROR;
	    goto disconnect;
	}

	if (!demand) {
	    info("Using interface ppp%d", pppifunit);
	    slprintf(ifname, sizeof(ifname), "ppp%d", pppifunit);
	}

	/*
	 * Start opening the connection and wait for
	 * incoming events (reply, timeout, etc.).
	 */
	notice("Connect: %s <--> %s", ifname, ppp_devnam);
	gettimeofday(&start_time, NULL);

	lcp_lowerup(0);
	lcp_open(0);		/* Start protocol */

	open_ccp_flag = 0;
	pppd_status = EXIT_NEGOTIATION_FAILED;
	new_phase(PHASE_ESTABLISH);
	while (pppd_phase != PHASE_DEAD) {
   	    wait_input(timeleft(&timo));
	    calltimeout();
            get_input();

	    if (pppd_kill_link) {
		lcp_close(0, "User request");
		pppd_kill_link = 0;
	    }
	    if (open_ccp_flag) {
		if (pppd_phase == PHASE_NETWORK || pppd_phase == PHASE_RUNNING) {
		    ccp_fsm[0].flags = OPT_RESTART; /* clears OPT_SILENT */
		    (*ccp_protent.open)(0);
		}
		open_ccp_flag = 0;
	    }
	}

	/*
	 * If we may want to bring the link up again, transfer
	 * the ppp unit back to the loopback.  Set the
	 * real serial device back to its normal mode of operation.
	 */
	clean_check();
	if (demand)
	    restore_loop();
	disestablish_ppp(pppd_ttyfd);
	fd_ppp = -1;
	if (!hungup)
	    lcp_lowerdown(0);

	/*
	 * Run disconnector script, if requested.
	 * XXX we may not be able to do this if the line has hung up!
	 */
    disconnect:
	if (disconnect_script && !hungup) {
	    new_phase(PHASE_DISCONNECT);
	    if (real_ttyfd >= 0)
		set_up_tty(real_ttyfd, 1);
	    if (device_script(pppd_ttyfd, DIALER_DISCONNECT, disconnect_script) < 0) {
		warn("disconnect script failed");
	    } else {
		info("Serial link disconnected.");
	    }
	}

    fail:
	if (pty_master >= 0)
	    close(pty_master);
	if (pty_slave >= 0)
	    close(pty_slave);
	if (real_ttyfd >= 0)
	    close_tty();

	if (!persist || (maxfail > 0 && unsuccess >= maxfail))
	    break;

	pppd_kill_link = 0;
	if (demand)
	    demand_discard();
	t = need_holdoff? holdoff: 0;
	if (holdoff_hook)
	    t = (*holdoff_hook)();
	if (t > 0) {
	    new_phase(PHASE_HOLDOFF);
	    TIMEOUT(holdoff_end, NULL, t);
	    do {
   	        wait_input(timeleft(&timo));

		calltimeout();
		if (pppd_kill_link) {
		    pppd_kill_link = 0;
		    new_phase(PHASE_DORMANT); /* allow signal to end holdoff */
		}
	    } while (pppd_phase == PHASE_HOLDOFF);
	    if (!persist)
		break;
	}
    }

    die(pppd_status);
    return pppd_status;
}

/*
 * holdoff_end - called via a timeout when the holdoff period ends.
 */
static void
holdoff_end(
    void *arg)
{
    new_phase(PHASE_DORMANT);
}

/* List of protocol names, to make our messages a little more informative. */
struct protocol_list {
    u_short	proto;
    const char	*name;
} protocol_list[] = {
    { 0x21,	"IP" },
    { 0x23,	"OSI Network Layer" },
    { 0x25,	"Xerox NS IDP" },
    { 0x27,	"DECnet Phase IV" },
    { 0x29,	"Appletalk" },
    { 0x2b,	"Novell IPX" },
    { 0x2d,	"VJ compressed TCP/IP" },
    { 0x2f,	"VJ uncompressed TCP/IP" },
    { 0x31,	"Bridging PDU" },
    { 0x33,	"Stream Protocol ST-II" },
    { 0x35,	"Banyan Vines" },
    { 0x39,	"AppleTalk EDDP" },
    { 0x3b,	"AppleTalk SmartBuffered" },
    { 0x3d,	"Multi-Link" },
    { 0x3f,	"NETBIOS Framing" },
    { 0x41,	"Cisco Systems" },
    { 0x43,	"Ascom Timeplex" },
    { 0x45,	"Fujitsu Link Backup and Load Balancing (LBLB)" },
    { 0x47,	"DCA Remote Lan" },
    { 0x49,	"Serial Data Transport Protocol (PPP-SDTP)" },
    { 0x4b,	"SNA over 802.2" },
    { 0x4d,	"SNA" },
    { 0x4f,	"IP6 Header Compression" },
    { 0x6f,	"Stampede Bridging" },
    { 0xfb,	"single-link compression" },
    { 0xfd,	"1st choice compression" },
    { 0x0201,	"802.1d Hello Packets" },
    { 0x0203,	"IBM Source Routing BPDU" },
    { 0x0205,	"DEC LANBridge100 Spanning Tree" },
    { 0x0231,	"Luxcom" },
    { 0x0233,	"Sigma Network Systems" },
    { 0x8021,	"Internet Protocol Control Protocol" },
    { 0x8023,	"OSI Network Layer Control Protocol" },
    { 0x8025,	"Xerox NS IDP Control Protocol" },
    { 0x8027,	"DECnet Phase IV Control Protocol" },
    { 0x8029,	"Appletalk Control Protocol" },
    { 0x802b,	"Novell IPX Control Protocol" },
    { 0x8031,	"Bridging NCP" },
    { 0x8033,	"Stream Protocol Control Protocol" },
    { 0x8035,	"Banyan Vines Control Protocol" },
    { 0x803d,	"Multi-Link Control Protocol" },
    { 0x803f,	"NETBIOS Framing Control Protocol" },
    { 0x8041,	"Cisco Systems Control Protocol" },
    { 0x8043,	"Ascom Timeplex" },
    { 0x8045,	"Fujitsu LBLB Control Protocol" },
    { 0x8047,	"DCA Remote Lan Network Control Protocol (RLNCP)" },
    { 0x8049,	"Serial Data Control Protocol (PPP-SDCP)" },
    { 0x804b,	"SNA over 802.2 Control Protocol" },
    { 0x804d,	"SNA Control Protocol" },
    { 0x804f,	"IP6 Header Compression Control Protocol" },
    { 0x006f,	"Stampede Bridging Control Protocol" },
    { 0x80fb,	"Single Link Compression Control Protocol" },
    { 0x80fd,	"Compression Control Protocol" },
    { 0xc021,	"Link Control Protocol" },
    { 0xc023,	"Password Authentication Protocol" },
    { 0xc025,	"Link Quality Report" },
    { 0xc027,	"Shiva Password Authentication Protocol" },
    { 0xc029,	"CallBack Control Protocol (CBCP)" },
    { 0xc081,	"Container Control Protocol" },
    { 0xc223,	"Challenge Handshake Authentication Protocol" },
    { 0xc281,	"Proprietary Authentication Protocol" },
    { 0,	NULL },
};

/*
 * protocol_name - find a name for a PPP protocol.
 */
const char *
protocol_name(
    int proto)
{
    struct protocol_list *lp;

    for (lp = protocol_list; lp->proto != 0; ++lp)
	if (proto == lp->proto)
	    return lp->name;
    return NULL;
}

/*
 * get_input - called when incoming data is available.
 */
static void
get_input(void)
{
    int len, i;
    u_char *p;
    u_short protocol;
    struct protent *protp;

    p = inpacket_buf;	/* point to beginning of packet buffer */

    len = read_packet(inpacket_buf);
    if (len < 0)
	return;

    if (len == 0) {
	notice("Modem hangup");
	hungup = 1;
	pppd_status = EXIT_HANGUP;
	lcp_lowerdown(0);	/* serial link is no longer available */
	link_terminated(0);
	return;
    }

    if (debug /*&& (debugflags & DBG_INPACKET)*/)
	dbglog("rcvd %P", p, len);

    if (len < PPP_HDRLEN) {
	MAINDEBUG(("io(): Received short packet."));
	return;
    }

    p += 2;				/* Skip address and control */
    GETSHORT(protocol, p);
    len -= PPP_HDRLEN;

    /*
     * Toss all non-LCP packets unless LCP is OPEN.
     */
    if (protocol != PPP_LCP && lcp_fsm[0].state != OPENED) {
	MAINDEBUG(("get_input: Received non-LCP packet when LCP not open."));
	return;
    }

    /*
     * Until we get past the authentication phase, toss all packets
     * except LCP, LQR and authentication packets.
     */
    if (pppd_phase <= PHASE_AUTHENTICATE
	&& !(protocol == PPP_LCP || protocol == PPP_LQR
	     || protocol == PPP_PAP || protocol == PPP_CHAP)) {
	MAINDEBUG(("get_input: discarding proto 0x%x in phase %d",
		   protocol, pppd_phase));
	return;
    }

    /*
     * Upcall the proper protocol input routine.
     */
    for (i = 0; (protp = protocols[i]) != NULL; ++i) {
	if (protp->protocol == protocol && protp->enabled_flag) {
	    (*protp->input)(0, p, len);
	    return;
	}
        if (protocol == (protp->protocol & ~0x8000) && protp->enabled_flag
	    && protp->datainput != NULL) {
	    (*protp->datainput)(0, p, len);
	    return;
	}
    }

    if (debug) {
	const char *pname = protocol_name(protocol);
	if (pname != NULL)
	    warn("Unsupported protocol '%s' (0x%x) received", pname, protocol);
	else
	    warn("Unsupported protocol 0x%x received", protocol);
    }
    lcp_sprotrej(0, p - PPP_HDRLEN, len + PPP_HDRLEN);

    return;
}

/*
 * new_phase - signal the start of a new phase of pppd's operation.
 */
void
new_phase(
    int p)
{
    pppd_phase = p;
    if (new_phase_hook)
	(*new_phase_hook)(p);
}

/*
 * die - clean up state and exit with the specified status.
 */
void
die(
    int status)
{
    cleanup();
}

/*
 * cleanup - restore anything which needs to be restored before we exit
 */
/* ARGSUSED */
static void
cleanup(void)
{
    sys_cleanup();

    if (fd_ppp >= 0)
	disestablish_ppp(pppd_ttyfd);
    if (real_ttyfd >= 0)
	close_tty();

    sys_close();
}

/*
 * close_tty - restore the terminal device and close it.
 */
static void
close_tty(void)
{
    /* drop dtr to hang up */
    if (!default_device && modem) {
	setdtr(real_ttyfd, 0);
	/*
	 * This sleep is in case the serial port has CLOCAL set by default,
	 * and consequently will reassert DTR when we close the device.
	 */
	sleep(1);
    }

    restore_tty(real_ttyfd);

    close(real_ttyfd);
    real_ttyfd = -1;
}

/*
 * update_link_stats - get stats at link termination.
 */
void
update_link_stats(
    int u)
{
    struct timeval now;
    char numbuf[32];

    if (!get_ppp_stats(u, &link_stats)
	|| gettimeofday(&now, NULL) < 0)
	return;
    link_connect_time = now.tv_sec - start_time.tv_sec;
    link_stats_valid = 1;

    slprintf(numbuf, sizeof(numbuf), "%d", link_connect_time);
    slprintf(numbuf, sizeof(numbuf), "%d", link_stats.bytes_out);
    slprintf(numbuf, sizeof(numbuf), "%d", link_stats.bytes_in);
}

struct	callout {
    struct timeval	c_time;		/* time at which to call routine */
    void		*c_arg;		/* argument to routine */
    void		(*c_func)(void *); /* routine */
    struct		callout *c_next;
};

static struct callout *callout = NULL;	/* Callout list */
static struct timeval timenow;		/* Current time */

/*
 * timeout - Schedule a timeout.
 *
 * Note that this timeout takes the number of seconds, NOT hz (as in
 * the kernel).
 */
void
ppptimeout(
    void (*func)(void *),
    void *arg,
    int time)
{
    struct callout *newp, *p, **pp;

    MAINDEBUG(("Timeout %p:%p in %d seconds.", func, arg, time));

    /*
     * Allocate timeout.
     */
    if ((newp = (struct callout *) malloc(sizeof(struct callout))) == NULL)
	fatal("Out of memory in timeout()!");
    newp->c_arg = arg;
    newp->c_func = func;
    gettimeofday(&timenow, NULL);
    newp->c_time.tv_sec = timenow.tv_sec + time;
    newp->c_time.tv_usec = timenow.tv_usec;

    /*
     * Find correct place and link it in.
     */
    for (pp = &callout; (p = *pp); pp = &p->c_next)
	if (newp->c_time.tv_sec < p->c_time.tv_sec
	    || (newp->c_time.tv_sec == p->c_time.tv_sec
		&& newp->c_time.tv_usec < p->c_time.tv_usec))
	    break;
    newp->c_next = p;
    *pp = newp;
}


/*
 * untimeout - Unschedule a timeout.
 */
void
pppuntimeout(
    void (*func)(void *),
    void *arg)
{
    struct callout **copp, *freep;

    MAINDEBUG(("Untimeout %p:%p.", func, arg));

    /*
     * Find first matching timeout and remove it from the list.
     */
    for (copp = &callout; (freep = *copp); copp = &freep->c_next)
	if (freep->c_func == func && freep->c_arg == arg) {
	    *copp = freep->c_next;
	    free((char *) freep);
	    break;
	}
}


/*
 * calltimeout - Call any timeout routines which are now due.
 */
static void
calltimeout(void)
{
    struct callout *p;

    while (callout != NULL) {
	p = callout;

	if (gettimeofday(&timenow, NULL) < 0)
	    fatal("Failed to get time of day: %m");
	if (!(p->c_time.tv_sec < timenow.tv_sec
	      || (p->c_time.tv_sec == timenow.tv_sec
		  && p->c_time.tv_usec <= timenow.tv_usec)))
	    break;		/* no, it's not time yet */

	callout = p->c_next;
	(*p->c_func)(p->c_arg);

	free((char *) p);
    }
}


/*
 * timeleft - return the length of time until the next timeout is due.
 */
static struct timeval *
timeleft(
    struct timeval *tvp)
{
    if (callout == NULL)
	return NULL;

    gettimeofday(&timenow, NULL);
    tvp->tv_sec = callout->c_time.tv_sec - timenow.tv_sec;
    tvp->tv_usec = callout->c_time.tv_usec - timenow.tv_usec;
    if (tvp->tv_usec < 0) {
	tvp->tv_usec += 1000000;
	tvp->tv_sec -= 1;
    }
    if (tvp->tv_sec < 0)
	tvp->tv_sec = tvp->tv_usec = 0;

    return tvp;
}

/*
 * device_script - run a program to talk to the serial device
 * (e.g. to run the connector or disconnector script).
 */
static int device_script(int fd, int mode, char *program)
{
    int    iReturn = -1;
    char   pScript[128];

    /* copyt script into temporary location */
    strcpy(pScript, program);

    /* check to see if dialer was initialized */
    if ( !pppd_dialer ) {
      /* set default dialer to chatmain */
      pppd_dialer = chatmain;
    }

    /* check to see if dialer is set */
    if ( pppd_dialer ) {
      /* call the dialer */
      iReturn = (*pppd_dialer)(fd, mode, program);
    }

    return ( -iReturn );
}

/*
 * novm - log an error message saying we ran out of memory, and die.
 */
void
novm(
    char *msg)
{
    fatal("Virtual memory exhausted allocating %s\n", msg);
}
