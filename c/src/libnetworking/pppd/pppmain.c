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

#ifndef lint
/* static char rcsid[] = "$Id$"; */
#endif

#include <stdio.h>
#include <ctype.h>
/* #include <stdlib.h> */
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <netdb.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>

/* #include <stbconfig.h> */
#include "pppd.h"
#include "magic.h"
#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#include "upap.h"
#include "chap.h"
#include "ccp.h"
#include "pathnames.h"
#include "patchlevel.h"

#ifdef CBCP_SUPPORT
#include "cbcp.h"
#endif

#if defined(SUNOS4)
extern char *strerror();
#endif

#ifdef IPX_CHANGE
#include "ipxcp.h"
#endif							/* IPX_CHANGE */
#ifdef AT_CHANGE
#include "atcp.h"
#endif

void SetStatusInfo(int state, char * text, int res);

/* prototypes for routines in this file */
int connect_script(int fd);

/* interface vars */
char ifname[32];				/* Interface name */
int interfunit;					/* Interface unit number */

char *progname;					/* Name of this program */
char hostname[MAXNAMELEN] = "dcna";	/* Our hostname */
static char default_devnam[MAXPATHLEN];		/* name of default device */
/*static pid_t pid;	*/			/* Our pid */
static uid_t uid;				/* Our real user-id */
static int conn_running;		/* we have a [dis]connector running */

int ttyfd = -1;					/* Serial port file descriptor */
mode_t tty_mode = -1;			/* Original access permissions to tty */
int baud_rate;					/* Actual bits/second for serial device */
int hungup;						/* terminal has been hung up */
int privileged;					/* we're running as real uid root */
int need_holdoff;				/* need holdoff period before restarting */
int detached;					/* have detached from terminal */

int phase;						/* where the link is at */
int kill_link;
int open_ccp_flag;

char **script_env;				/* Env. variable values for scripts */
int s_env_nalloc;				/* # words avail at script_env */

u_char outpacket_buf[PPP_MRU + PPP_HDRLEN];		/* buffer for outgoing packet */
u_char inpacket_buf[PPP_MRU + PPP_HDRLEN];	/* buffer for incoming packet */



char *no_ppp_msg = "lack of PPP\n";

/* Prototypes for procedures local to this file. */
static void cleanup(void);
static void close_tty __P((void));
static void get_input __P((void));
static void calltimeout __P((void));
static struct timeval *timeleft __P((struct timeval *));
static void holdoff_end __P((void *));
static void reap_kids __P((void));
/* XXX currently unused */
#if 0
static int device_script __P((char *[], int, int));
static void pr_log __P((void *, char *,...));
#endif

extern char *ttyname __P((int));
extern char *getlogin __P((void));
int main __P((int, char *[]));





/*
 * PPP Data Link Layer "protocol" table.
 * One entry per supported protocol.
 * The last entry must be NULL.
 */
struct protent *protocols[] =
{
	&lcp_protent,
	&pap_protent,
	&chap_protent,
#ifdef CBCP_SUPPORT
	&cbcp_protent,
#endif
	&ipcp_protent,
	&ccp_protent,
#ifdef IPX_CHANGE
	&ipxcp_protent,
#endif
#ifdef AT_CHANGE
	&atcp_protent,
#endif
	NULL
};

extern int connect_stb();


extern int disconnect_stb();


extern struct in_addr rtems_bsdnet_nameserver[];
extern int rtems_bsdnet_nameserver_count;
extern int __res_init(void);

int pppdmain(argc, argv)
int argc;
char *argv[];
{
	int i;
#if 0
	struct timeval timo;
#endif
	struct protent *protp;
	struct stat statbuf;
#if 0
    	char t[100];
#endif


	phase = PHASE_INITIALIZE;

	strcpy(default_devnam, "/dev/modem");
	strcpy(devnam, "/dev/modem");

	script_env = NULL;

/*    if (gethostname(hostname, MAXNAMELEN) < 0 ) {
   die(1);
   }
 */
	hostname[MAXNAMELEN - 1] = 0;

	uid = 0;
	privileged = uid == 0;

	/*
	 * Initialize to the standard option set, then parse, in order,
	 * the system options file, the user's options file,
	 * the tty's options file, and the command line arguments.
	 */
	for (i = 0; (protp = protocols[i]) != NULL; ++i)
		(*protp->init) (0);



#if 1
	if (!ppp_available()) {
		exit(1);
	}
#endif
	/*
	 * Check that the options given are valid and consistent.
	 */
	sys_check_options();
	auth_check_options();
	for (i = 0; (protp = protocols[i]) != NULL; ++i)
		if (protp->check_options != NULL)
			(*protp->check_options) ();

	/*
	 * If the user has specified the default device name explicitly,
	 * pretend they hadn't.
	 */
	if (!default_device && strcmp(devnam, default_devnam) == 0)
		default_device = 1;
	if (default_device)
		nodetach = 1;

	/*
	 * Initialize system-dependent stuff and magic number package.
	 */
	sys_init();
	magic_init();

	/*
	 * Detach ourselves from the terminal, if required,
	 * and identify who is running us.
	 */

/*
	syslog(LOG_NOTICE, "pppd %s.%d%s started by %s, uid %d",
	     VERSION, PATCHLEVEL, IMPLEMENTATION, p, uid);
*/


	for (;;) {
#if 0 /* FIXME: how do we synchonize? */

/* !!! here should be done some kind of synchronization - I did it following way ... */
/* GlobalSystemStatus is set of different things shared between different tasks ... */
#if 0
/* XXX PPPConfiguration */
	GlobalSystemStatus * volatile stat;
	stat=LockSTBSystemParam();
	stat->ConnectionStatus = NotConnected;
	UnlockSTBSystemParam();
#endif


		while (1) {
			rtems_event_set events;
			int status;

#if 0
/* XXX PPPConfiguration */
			stat=LockSTBSystemParam();
			status=stat->WantConnection;
			UnlockSTBSystemParam();
			
			if (status == Connect) break;
#endif
			rtems_event_receive(RTEMS_EVENT_1, RTEMS_WAIT | RTEMS_EVENT_ANY, RTEMS_NO_TIMEOUT, &events);
		}
#if 0
/* XXX PPPConfiguration */
	stat=LockSTBSystemParam();
/* 	stat->WantConnection=DontConnect; */
	stat->ConnectionStatus = Connecting;

/* Here you can change default nameserver ... */
	rtems_bsdnet_nameserver[0].s_addr=inet_addr(stat->DNS);
	rtems_bsdnet_nameserver_count=1;
	UnlockSTBSystemParam();
#endif
/* initialize DNS services here */
	 SetStatusInfo(0, "Connecting...",0);

	__res_init();
#endif /* FIXME */
		/*
		 * Open the serial device and set it up to be the ppp interface.
		 * First we open it in non-blocking mode so we can set the
		 * various termios flags appropriately.  If we aren't dialling
		 * out and we want to use the modem lines, we reopen it later
		 * in order to wait for the carrier detect signal from the modem.
		 */
		while ((ttyfd = open(devnam, O_NONBLOCK | O_RDWR, 0)) < 0) {
			if (errno != EINTR)
				syslog(LOG_ERR, "Failed to open %s: %m", devnam);

			if (!persist || errno != EINTR)
				goto fail;
		}

		hungup = 0;
		kill_link = 0;

		/*
		 * Do the equivalent of `mesg n' to stop broadcast messages.
		 */
		tty_mode = statbuf.st_mode;

		/* run connection script */
		/*
		 * Set line speed, flow control, etc.
		 * On most systems we set CLOCAL for now so that we can talk
		 * to the modem before carrier comes up.  But this has the
		 * side effect that we might miss it if CD drops before we
		 * get to clear CLOCAL below.  On systems where we can talk
		 * successfully to the modem with CLOCAL clear and CD down,
		 * we can clear CLOCAL at this point.
		 */
		set_up_tty(ttyfd, 1);

		/* drop dtr to hang up in case modem is off hook */
		setdtr(ttyfd, FALSE);
		sleep(1);
		setdtr(ttyfd, TRUE);
	    {
/* Make a call ... */
#if 0
/* XXX PPPConfiguration */
	    	char t[100];
			stat=LockSTBSystemParam();
			sprintf(t,"Dzwoniê pod numer %s ...",stat->Phone_Number);
			UnlockSTBSystemParam();	    	
	    	SetStatusInfo(0, t,0);
#endif
	    }
	    

		if ((i=connect_script(ttyfd)) >0) {
#if 0
/* here go error messages ... */
		static char *error_msgs[]={ "Bad script", "IO Error"
		 	"Timeout", "Busy", "No dialtone", "No carrier",
			"No answer", "No answer from server" };
			setdtr(ttyfd, FALSE);
	    	sprintf(t,"Communication error: %s",error_msgs[i-1]);
		syslog(LOG_ERR, "Connect script failed");
	    	SetStatusInfo(0, t,1);
#endif
			goto fail;
		}
	
		else
		if (i<0)
		{
#if 0
	    	char t[100];
			sprintf(t,"£¹czenie z Internetem z prêdkosci¹ %d baud...",-i);
		    SetStatusInfo(0, t,0);
#endif
		}
		else
		{
#if 0
			    SetStatusInfo(0, "£¹czenie z Internetem...",0);
#endif
		}
		syslog(LOG_INFO, "Serial connection established.");

		sleep(1);				/* give it time to set up its terminal */

		/* set line speed, flow control, etc.; clear CLOCAL if modem option */
		set_up_tty(ttyfd, 0);

		/* reopen tty if necessary to wait for carrier */

		/* run welcome script, if any */
/*  if (welcomer && welcomer[0]) {
   if (device_script(welcomer, 21, ttyfd) < 0)
   syslog(LOG_WARNING, "Welcome script failed");
   }
 */
		/* set up the serial device as a ppp interface */
		establish_ppp(ttyfd);

/*  syslog(LOG_INFO, "Using interface ppp%d", interfunit);  */
		(void) sprintf(ifname, "ppp%d", interfunit);

		/*
		 * Start opening the connection and wait for
		 * incoming events (reply, timeout, etc.).
		 */
/*   syslog(LOG_NOTICE, "Connect: %s <--> %s", ifname, devnam); */

		rtems_bsdnet_semaphore_obtain();

		lcp_lowerup(0);
		lcp_open(0);			/* Start protocol */

		rtems_bsdnet_semaphore_release();
#if 0
        SetStatusInfo(0, "Po³¹czenie z Internetem nawi¹zane.",1);
#endif
		for (phase = PHASE_ESTABLISH; phase != PHASE_DEAD;) {
#if 0
			wait_input(timeleft(&timo));
#endif
			calltimeout();
			get_input();
#if 0
/* XXX PPPConfiguration */
			stat=LockSTBSystemParam();
			if (stat->WantConnection==DontConnect) {
				stat->ConnectionStatus = NotConnected;
			    SetStatusInfo(0, "Roz³¹czam siê ...",0);
				lcp_close(0, "");
				kill_link = 0;
			}
			UnlockSTBSystemParam();
#endif
			if (open_ccp_flag) {
				if (phase == PHASE_NETWORK) {
					ccp_fsm[0].flags = OPT_RESTART;		/* clears OPT_SILENT */
					(*ccp_protent.open) (0);
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
		disestablish_ppp(ttyfd);
#if 0
	    SetStatusInfo(0, "Po³¹czenie z Internetem zerwane.",1);
#endif
		/*
		 * Run disconnector script, if requested.
		 * XXX we may not be able to do this if the line has hung up!
		 */
/*  if (disconnector && !hungup) {
   set_up_tty(ttyfd, 1);
   if (device_script(disconnector, ttyfd, ttyfd) < 0) {
   syslog(LOG_WARNING, "disconnect script failed");
   } else {
   syslog(LOG_INFO, "Serial link disconnected.");
   }
   }
 */
	  fail:
#if 0
/* XXX PPPConfiguration */
		stat=LockSTBSystemParam();
		stat->ConnectionStatus = NotConnected;
		stat->WantConnection=DontConnect;
		UnlockSTBSystemParam();
#endif
		if (ttyfd >= 0)
			close_tty();



	}

	return 0;
}

/*
 * detach - detach us from the controlling terminal.
 */
void detach()
{
}

/*
 * holdoff_end - called via a timeout when the holdoff period ends.
 */

static void holdoff_end(arg)
void *arg;
{
	phase = PHASE_DORMANT;
}

/*
 * get_input - called when incoming data is available.
 */
static void get_input()
{
	int len, i;
	u_char *p;
	u_short protocol;
	struct protent *protp;

	p = inpacket_buf;			/* point to beginning of packet buffer */

	len = read_packet(inpacket_buf);
	if (len < 0)
		return;

	if (len == 0) {
#if 0
/* XXX PPPConfiguration */
		GlobalSystemStatus * volatile stat;
#endif
/*   syslog(LOG_NOTICE, "Modem hangup"); */
		hungup = 1;
#if 0
/* XXX PPPConfiguration */
		stat=LockSTBSystemParam();
		stat->ConnectionStatus = NotConnected;
		UnlockSTBSystemParam();
#endif

		lcp_lowerdown(0);		/* serial link is no longer available */
		link_terminated(0);
		return;
	}
/*	if ((debug))
		log_packet(p, len, "rcvd ", LOG_DEBUG);
*/

	if (len < PPP_HDRLEN) {
/*
		if (debug)
			MAINDEBUG((LOG_INFO, "io(): Received short packet."));
*/
		return;
	}
/* We need to modify internal network structures here */
	rtems_bsdnet_semaphore_obtain();

	p += 2;						/* Skip address and control */
	GETSHORT(protocol, p);
	len -= PPP_HDRLEN;

	/*
	 * Toss all non-LCP packets unless LCP is OPEN.
	 */
	if (protocol != PPP_LCP && lcp_fsm[0].state != OPENED) {
		MAINDEBUG((LOG_INFO,
			   "get_input: Received non-LCP packet when LCP not open."));
		rtems_bsdnet_semaphore_release();
		return;
	}
	/*
	 * Until we get past the authentication phase, toss all packets
	 * except LCP, LQR and authentication packets.
	 */
	if (phase <= PHASE_AUTHENTICATE
		&& !(protocol == PPP_LCP || protocol == PPP_LQR
			 || protocol == PPP_PAP || protocol == PPP_CHAP)) {

		rtems_bsdnet_semaphore_release();
		return;
	}
	/*
	 * Upcall the proper protocol input routine.
	 */
	for (i = 0; (protp = protocols[i]) != NULL; ++i) {
		if (protp->protocol == protocol && protp->enabled_flag) {
			(*protp->input) (0, p, len);
			rtems_bsdnet_semaphore_release();
			return;
		}
		if (protocol == (protp->protocol & ~0x8000) && protp->enabled_flag
			&& protp->datainput != NULL) {
			(*protp->datainput) (0, p, len);
			rtems_bsdnet_semaphore_release();
			return;
		}
	}

	lcp_sprotrej(0, p - PPP_HDRLEN, len + PPP_HDRLEN);
	rtems_bsdnet_semaphore_release();
}


/*
 * quit - Clean up state and exit (with an error indication).
 */
void quit()
{
	die(1);
}

/*
 * die - like quit, except we can specify an exit status.
 */
void die(status)
int status;
{
	cleanup();
}

/*
 * cleanup - restore anything which needs to be restored before we exit
 */
/* ARGSUSED */
static void cleanup()
{
	sys_cleanup();

	if (ttyfd >= 0)
		close_tty();

}

/*
 * close_tty - restore the terminal device and close it.
 */
static void close_tty()
{
	disestablish_ppp(ttyfd);

	/* drop dtr to hang up */
	setdtr(ttyfd, FALSE);
	/*
	 * This sleep is in case the serial port has CLOCAL set by default,
	 * and consequently will reassert DTR when we close the device.
	 */
	sleep(1);

	restore_tty(ttyfd);
	close(ttyfd);
	ttyfd = -1;
}


struct callout {
	struct timeval c_time;
	void *c_arg;
	void (*c_func) __P((void *));
	struct callout *c_next;
};

static struct callout *callout = NULL;
static struct timeval timenow;

/*
 * timeout - Schedule a timeout.
 *
 * Note that this timeout takes the number of seconds, NOT hz (as in
 * the kernel).
 */
void my_timeout(func, arg, time)
void (*func) __P((void *));
void *arg;
int time;
{
	struct callout *newp, *p, **pp;

	MAINDEBUG((LOG_DEBUG, "Timeout %lx:%lx in %d seconds.",
			   (long) func, (long) arg, time));
	/*
	 * Allocate timeout.
	 */
	if ((newp = (struct callout *) malloc(sizeof(struct callout))) == NULL) {
/*  syslog(LOG_ERR, "Out of memory in timeout()!"); */
		die(1);
	}
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
				&& newp->c_time.tv_usec < p->c_time.tv_sec))
			break;
	newp->c_next = p;
	*pp = newp;
}

/*
 * untimeout - Unschedule a timeout.
 */

void untimeout(func, arg)
void (*func) __P((void *));
void *arg;
{
	struct callout **copp, *freep;

	MAINDEBUG((LOG_DEBUG, "Untimeout %lx:%lx.", (long) func, (long) arg));

	for (copp = &callout; (freep = *copp); copp = &freep->c_next)
		if (freep->c_func == func && freep->c_arg == arg) {
			*copp = freep->c_next;
			(void) free((char *) freep);
			break;
		}
}


/*
 * calltimeout - Call any timeout routines which are now due.
 */
static void calltimeout()
{
	struct callout *p;

	while (callout != NULL) {
		p = callout;

		if (gettimeofday(&timenow, NULL) < 0) {
			die(1);
		}
		if (!(p->c_time.tv_sec < timenow.tv_sec
			  || (p->c_time.tv_sec == timenow.tv_sec
				  && p->c_time.tv_usec <= timenow.tv_usec)))
			break;				/* no, it's not time yet */

		callout = p->c_next;
		(*p->c_func) (p->c_arg);

		free((char *) p);
	}
}


/*
 * timeleft - return the length of time until the next timeout is due.
 */
static struct timeval *
 timeleft(tvp)
struct timeval *tvp;
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
 * term - Catch SIGTERM signal and SIGINT signal (^C/del).
 *
 * Indicates that we should initiate a graceful disconnect and exit.
 */
static void term(sig)
int sig;
{
	/*    persist = 0; *//* don't try to restart */
	kill_link = 1;
}

int modem_fd; /* FIXME: should not be global... */
int connect_script(int fd)
{
#if 0 /* FIXME: This is WinNT special */
	char program[256] = "TIMEOUT@10@@CLIENT@CLIENTSERVER";
#else
	char program[256] = { 0 };
#endif
#if 0
/* XXX PPPConfiguration */
	GlobalSystemStatus * volatile stat;
#endif
#if 0
/* Connect scripts are almost the same as in Linux Chat ... */
	static char *scripts[] =
	{
		"TIMEOUT@5@@\rAT@OK-+++\\c-OK@ATH0@TIMEOUT@90@OK@ATDT%s@CONNECT@",
		"TIMEOUT@5@@\rAT@OK-+++\\c-OK@ATH0@TIMEOUT@90@OK@ATDT%s@CONNECT@@ppp@@Username:@%s@Password:@%s@"
	};
#endif
	modem_fd = fd;
#if 0
/* XXX PPPConfiguration */
	stat=LockSTBSystemParam();	
	if (strcmp("",stat->PPP_User))
	{
		stat->provider=Poland_TPSA;
	}
	else
		stat->provider=DumbLogin;
	switch (stat->provider) {
	case Poland_TPSA:
		sprintf(program, scripts[1], stat->Phone_Number, stat->PPP_User, stat->PPP_Password);
		break;
	default:
		sprintf(program, scripts[0], stat->Phone_Number);

	}
	UnlockSTBSystemParam();
#endif
	conn_running = 0;

	return chatmain(program);
}


/*
 * run-program - execute a program with given arguments,
 * but don't wait for it.
 * If the program can't be executed, logs an error unless
 * must_exist is 0 and the program file doesn't exist.
 */
int run_program(prog, args, must_exist)
char *prog;
char **args;
int must_exist;
{

	return 0;
}


/*
 * reap_kids - get status from any dead child processes,
 * and log a message for abnormal terminations.
 */
static void reap_kids()
{
}


/*
 * log_packet - format a packet and log it.
 */

char line[256];
char *linep;

/*#define log_packet(p, len, prefix, level) */
void log_packet(p, len, prefix, level)
u_char *p;
int len;
char *prefix;
int level;
{
	strcpy(line, prefix);
	linep = line + strlen(line);
/*	format_packet(p, len, pr_log, NULL); */
/*    if (linep != line)
   syslog(level, "%s", line);
 */
}

/*
 * format_packet - make a readable representation of a packet,
 * calling `printer(arg, format, ...)' to output it.
 */
/*#define format_packet(p, len, printer, arg) */

void format_packet(p, len, printer, arg)
u_char *p;
int len;
void (*printer) __P((void *, char *,...));
void *arg;
{
/*
	int i, n;
	u_short proto;
	struct protent *protp;

	if (len >= PPP_HDRLEN && p[0] == PPP_ALLSTATIONS && p[1] == PPP_UI) {
		p += 2;
		GETSHORT(proto, p);
		len -= PPP_HDRLEN;
		for (i = 0; (protp = protocols[i]) != NULL; ++i)
			if (proto == protp->protocol)
				break;
		if (protp != NULL) {
			printer(arg, "[%s", protp->name);
			n = (*protp->printpkt) (p, len, printer, arg);
			printer(arg, "]");
			p += n;
			len -= n;
		} else {
			printer(arg, "[proto=0x%x]", proto);
		}
	}
*/
/*    for (; len > 0; --len) {
   GETCHAR(x, p);
   printer(arg, " %.2x", x);
   }
 */
}



/* XXX currently unused */
#if 0
static void
pr_log __V((void *arg, char *fmt,...))
{
	int n;
	va_list pvar;
	char buf[256];

#if __STDC__
	va_start(pvar, fmt);
#else
	void *arg;
	char *fmt;
	va_start(pvar);
	arg = va_arg(pvar, void *);
	fmt = va_arg(pvar, char *);
#endif

	n = vfmtmsg(buf, sizeof(buf), fmt, pvar);
	va_end(pvar);

	if (linep + n + 1 > line + sizeof(line)) {
/*  syslog(LOG_DEBUG, "%s", line); */
		linep = line;
	}
	strcpy(linep, buf);
	linep += n;
}
#endif

/*
 * print_string - print a readable representation of a string using
 * printer.
 */
/*#define print_string(p, len, printer, arg) */

void print_string(p, len, printer, arg)
char *p;
int len;
void (*printer) __P((void *, char *,...));
void *arg;
{
	int c;

	printer(arg, "\"");
	for (; len > 0; --len) {
		c = *p++;
		if (' ' <= c && c <= '~') {
			if (c == '\\' || c == '"')
				printer(arg, "\\");
			printer(arg, "%c", c);
		} else {
			switch (c) {
			case '\n':
				printer(arg, "\\n");
				break;
			case '\r':
				printer(arg, "\\r");
				break;
			case '\t':
				printer(arg, "\\t");
				break;
			default:
				printer(arg, "\\%.3o", c);
			}
		}
	}
	printer(arg, "\"");
}


/*
 * novm - log an error message saying we ran out of memory, and die.
 */
void novm(msg)
char *msg;
{
/*    syslog(LOG_ERR, "Virtual memory exhausted allocating %s\n", msg);
 */ die(1);
}

/*
 * fmtmsg - format a message into a buffer.  Like sprintf except we
 * also specify the length of the output buffer, and we handle
 * %r (recursive format), %m (error message) and %I (IP address) formats.
 * Doesn't do floating-point formats.
 * Returns the number of chars put into buf.
 */
int
fmtmsg __V((char *buf, int buflen, char *fmt,...))
{
	va_list args;
	int n;

#if __STDC__
	va_start(args, fmt);
#else
	char *buf;
	int buflen;
	char *fmt;
	va_start(args);
	buf = va_arg(args, char *);
	buflen = va_arg(args, int);
	fmt = va_arg(args, char *);
#endif
	n = vfmtmsg(buf, buflen, fmt, args);
	va_end(args);
	return n;
}

/*
 * vfmtmsg - like fmtmsg, takes a va_list instead of a list of args.
 */
#define OUTCHAR(c)	(buflen > 0? (--buflen, *buf++ = (c)): 0)
/*#define vfmtmsg(buf, buflen, fmt, args) */

int vfmtmsg(buf, buflen, fmt, args)
char *buf;
int buflen;
char *fmt;
va_list args;
{
/*    int c, i, n;
   int width, prec, fillch;
   int base, len, neg, quoted;
   unsigned long val = 0;
   char *str, *f, *buf0;
   unsigned char *p;
   char num[32];
   time_t t;
   static char hexchars[] = "0123456789abcdef";

   buf0 = buf;
   --buflen;
   while (buflen > 0) {
   for (f = fmt; *f != '%' && *f != 0; ++f)
   ;
   if (f > fmt) {
   len = f - fmt;
   if (len > buflen)
   len = buflen;
   memcpy(buf, fmt, len);
   buf += len;
   buflen -= len;
   fmt = f;
   }
   if (*fmt == 0)
   break;
   c = *++fmt;
   width = prec = 0;
   fillch = ' ';
   if (c == '0') {
   fillch = '0';
   c = *++fmt;
   }
   if (c == '*') {
   width = va_arg(args, int);
   c = *++fmt;
   } else {
   while (isdigit(c)) {
   width = width * 10 + c - '0';
   c = *++fmt;
   }
   }
   if (c == '.') {
   c = *++fmt;
   if (c == '*') {
   prec = va_arg(args, int);
   c = *++fmt;
   } else {
   while (isdigit(c)) {
   prec = prec * 10 + c - '0';
   c = *++fmt;
   }
   }
   }
   str = 0;
   base = 0;
   neg = 0;
   ++fmt;
   switch (c) {
   case 'd':
   i = va_arg(args, int);
   if (i < 0) {
   neg = 1;
   val = -i;
   } else
   val = i;
   base = 10;
   break;
   case 'o':
   val = va_arg(args, unsigned int);
   base = 8;
   break;
   case 'x':
   val = va_arg(args, unsigned int);
   base = 16;
   break;
   case 'p':
   val = (unsigned long) va_arg(args, void *);
   base = 16;
   neg = 2;
   break;
   case 's':
   str = va_arg(args, char *);
   break;
   case 'c':
   num[0] = va_arg(args, int);
   num[1] = 0;
   str = num;
   break;
   case 'm':
   str = strerror(errno);
   break;
   case 'I':
   str = ip_ntoa(va_arg(args, u_int32_t));
   break;
   case 'r':
   f = va_arg(args, char *);
   #ifndef __powerpc__
   n = vfmtmsg(buf, buflen + 1, f, va_arg(args, va_list));
   #else

   n = vfmtmsg(buf, buflen + 1, f, va_arg(args, void *));
   #endif
   buf += n;
   buflen -= n;
   continue;
   case 't':
   break;
   case 'v':         
   case 'q':     
   quoted = c == 'q';
   p = va_arg(args, unsigned char *);
   if (fillch == '0' && prec > 0) {
   n = prec;
   } else {
   n = strlen((char *)p);
   if (prec > 0 && prec < n)
   n = prec;
   }
   while (n > 0 && buflen > 0) {
   c = *p++;
   --n;
   if (!quoted && c >= 0x80) {
   OUTCHAR('M');
   OUTCHAR('-');
   c -= 0x80;
   }
   if (quoted && (c == '"' || c == '\\'))
   OUTCHAR('\\');
   if (c < 0x20 || (0x7f <= c && c < 0xa0)) {
   if (quoted) {
   OUTCHAR('\\');
   switch (c) {
   case '\t':   OUTCHAR('t');   break;
   case '\n':   OUTCHAR('n');   break;
   case '\b':   OUTCHAR('b');   break;
   case '\f':   OUTCHAR('f');   break;
   default:
   OUTCHAR('x');
   OUTCHAR(hexchars[c >> 4]);
   OUTCHAR(hexchars[c & 0xf]);
   }
   } else {
   if (c == '\t')
   OUTCHAR(c);
   else {
   OUTCHAR('^');
   OUTCHAR(c ^ 0x40);
   }
   }
   } else
   OUTCHAR(c);
   }
   continue;
   default:
   *buf++ = '%';
   if (c != '%')
   --fmt;       
   --buflen;
   continue;
   }
   if (base != 0) {
   str = num + sizeof(num);
   *--str = 0;
   while (str > num + neg) {
   *--str = hexchars[val % base];
   val = val / base;
   if (--prec <= 0 && val == 0)
   break;
   }
   switch (neg) {
   case 1:
   *--str = '-';
   break;
   case 2:
   *--str = 'x';
   *--str = '0';
   break;
   }
   len = num + sizeof(num) - 1 - str;
   } else {
   len = strlen(str);
   if (prec > 0 && len > prec)
   len = prec;
   }
   if (width > 0) {
   if (width > buflen)
   width = buflen;
   if ((n = width - len) > 0) {
   buflen -= n;
   for (; n > 0; --n)
   *buf++ = fillch;
   }
   }
   if (len > buflen)
   len = buflen;
   memcpy(buf, str, len);
   buf += len;
   buflen -= len;
   }
   *buf = 0;
   return buf - buf0;
 */
	return 0;
}


/*
 * script_setenv - set an environment variable value to be used
 * for scripts that we run (e.g. ip-up, auth-up, etc.)
 */
#define script_setenv(var, value)
/*
   void
   script_setenv(var, value)
   char *var, *value;
   {
   int vl = strlen(var);
   int i;
   char *p, *newstring;

   newstring = (char *) malloc(vl + strlen(value) + 2);
   if (newstring == 0)
   return;
   strcpy(newstring, var);
   newstring[vl] = '=';
   strcpy(newstring+vl+1, value);

   if (script_env != 0) {
   for (i = 0; (p = script_env[i]) != 0; ++i) {
   if (strncmp(p, var, vl) == 0 && p[vl] == '=') {
   free(p);
   script_env[i] = newstring;
   return;
   }
   }
   } else {
   i = 0;
   script_env = (char **) malloc(16 * sizeof(char *));
   if (script_env == 0)
   return;
   s_env_nalloc = 16;
   }

   if (i + 1 >= s_env_nalloc) {
   int new_n = i + 17;
   char **newenv = (char **) realloc((void *)script_env,
   new_n * sizeof(char *));
   if (newenv == 0)
   return;
   script_env = newenv;
   s_env_nalloc = new_n;
   }

   script_env[i] = newstring;
   script_env[i+1] = 0;
   }

		    *//*
		      * script_unsetenv - remove a variable from the environment
		      * for scripts.
		    */
#define script_unsetenv(var)
/*
   void
   script_unsetenv(var)
   char *var;
   {
   int vl = strlen(var);
   int i;
   char *p;

   if (script_env == 0)
   return;
   for (i = 0; (p = script_env[i]) != 0; ++i) {
   if (strncmp(p, var, vl) == 0 && p[vl] == '=') {
   free(p);
   while ((script_env[i] = script_env[i+1]) != 0)
   ++i;
   break;
   }
   }
   }
 */
