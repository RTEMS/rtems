/*
 * options.c - handles option processing for PPP.
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

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
/* #include <stdlib.h> */
#include <termios.h>
#include <syslog.h>
#include <string.h>
#include <netdb.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef PPP_FILTER
#include <pcap.h>
#include <pcap-int.h>	/* XXX: To get struct pcap */
#endif

#include "pppd.h"
#include "pathnames.h"
#include "patchlevel.h"
#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#include "upap.h"
#include "chap.h"
#include "ccp.h"
#ifdef CBCP_SUPPORT
#include "cbcp.h"
#endif

#ifdef IPX_CHANGE
#include "ipxcp.h"
#endif /* IPX_CHANGE */

#include <net/ppp-comp.h>

#define FALSE	0
#define TRUE	1


#ifndef GIDSET_TYPE
#define GIDSET_TYPE	gid_t
#endif
#if 0
static int privileged_option;	/* set iff the current option came from root */
static char *option_source;	/* string saying where the option came from */
#endif

/*
 * Option variables and default values.
 */
#ifdef PPP_FILTER
int	dflag = 0;		/* Tell libpcap we want debugging */
#endif
int	debug = 1;		/* Debug flag */
int	kdebugflag = 1;		/* Tell kernel to print debug messages */
int	default_device = 1;	/* Using /dev/tty or equivalent */
char	devnam[MAXPATHLEN] = "/dev/sccppp";	/* Device name */
int	crtscts = 0;		/* Use hardware flow control */
int	modem = 0;		/* Use modem control lines */
int	inspeed = B115200;		/* Input/Output speed requested */
u_int32_t netmask = 0;		/* IP netmask to set on interface */
int	lockflag = 0;		/* Create lock file to lock the serial dev */
int	nodetach = 0;		/* Don't detach from controlling tty */
char *connector[]={"TIMEOUT","3","ABORT","\nBUSY\r","ABORT","\nNO DIALTONE\r","ABORT","\nNO CARRIER\r","ABORT","\nNO ANSWER\r","ABORT","\nRINGING\r\n\r\nRINGING\r",
												"","\rAT","OK-+++\\c-OK","ATH0","TIMEOUT","30","OK","ATDT13","CONNECT",""};
/*char *connector[]={"TIMEOUT","3","ABORT","\nBUSY\r","ABORT","\nNO DIALTONE\r","ABORT","\nNO CARRIER\r","ABORT","\nNO ANSWER\r","ABORT","\nRINGING\r\n\r\nRINGING\r",
												"","\rAT","OK-+++\\c-OK","ATH0","TIMEOUT","30","OK","ATDT0202122","CONNECT","","ppp","","Username:","ppp","Password:","ppp"};
*/
char	*disconnector [];	/* Script to disestablish physical link */
char	*welcomer [];	/* Script to run after phys link estab. */
int	maxconnect = 0;		/* Maximum connect time */
char	user[MAXNAMELEN]="stb";	/* Username for PAP */
char	passwd[MAXSECRETLEN]="stb";	/* Password for PAP */
int	auth_required = 0;	/* Peer is required to authenticate */
int	defaultroute = 1;	/* assign default route through interface */
int	proxyarp = 0;		/* Set up proxy ARP entry for peer */
int	persist = 1;		/* Reopen link after it goes down */
int	uselogin = 0;		/* Use /etc/passwd for checking PAP */
int	lcp_echo_interval = 0; 	/* Interval between LCP echo-requests */
int	lcp_echo_fails = 0;	/* Tolerance to unanswered echo-requests */
char	our_name[MAXNAMELEN]="infotel";	/* Our name for authentication purposes */
char	remote_name[MAXNAMELEN]; /* Peer's name for authentication */
int	explicit_remote = 0;	/* User specified explicit remote name */
int	usehostname = 0;	/* Use hostname for our_name */
int	disable_defaultip = 1;	/* Don't use hostname for default IP adrs */
int	demand = 0;		/* do dial-on-demand */
char	*ipparam = NULL;	/* Extra parameter for ip up/down scripts */
int	cryptpap;		/* Passwords in pap-secrets are encrypted */
int	idle_time_limit = 0;	/* Disconnect if idle for this many seconds */
int	holdoff = 30;		/* # seconds to pause before reconnecting */
int	refuse_pap = 0;		/* Set to say we won't do PAP */
int	refuse_chap = 1;	/* Set to say we won't do CHAP */

#ifdef MSLANMAN
int	ms_lanman = 0;    	/* Nonzero if use LanMan password instead of NT */
			  	/* Has meaning only with MS-CHAP challenges */
#endif

struct option_info auth_req_info;
struct option_info connector_info;
struct option_info disconnector_info;
struct option_info welcomer_info;
struct option_info devnam_info;
#ifdef PPP_FILTER
struct	bpf_program pass_filter;/* Filter program for packets to pass */
struct	bpf_program active_filter; /* Filter program for link-active pkts */
pcap_t  pc;			/* Fake struct pcap so we can compile expr */
#endif

/*
 * Prototypes
 */
#if 0
static int setdevname __P((char *, int));
static int setipaddr __P((char *));
static int setspeed __P((char *));
static int setdebug __P((char **));
static int setkdebug __P((char **));
static int setpassive __P((char **));
static int setsilent __P((char **));
static int noopt __P((char **));
static int setnovj __P((char **));
static int setnovjccomp __P((char **));
static int setvjslots __P((char **));
static int reqpap __P((char **));
static int nopap __P((char **));
#ifdef OLD_OPTIONS
static int setupapfile __P((char **));
#endif
static int nochap __P((char **));
static int reqchap __P((char **));
static int noaccomp __P((char **));
static int noasyncmap __P((char **));
static int noip __P((char **));
static int nomagicnumber __P((char **));
static int setasyncmap __P((char **));
static int setescape __P((char **));
static int setmru __P((char **));
static int setmtu __P((char **));
#ifdef CBCP_SUPPORT
static int setcbcp __P((char **));
#endif
static int nomru __P((char **));
static int nopcomp __P((char **));
static int setconnector __P((char **));
static int setdisconnector __P((char **));
static int setwelcomer __P((char **));
static int setmaxconnect __P((char **));
static int setdomain __P((char **));
static int setnetmask __P((char **));
static int setcrtscts __P((char **));
static int setnocrtscts __P((char **));
static int setxonxoff __P((char **));
static int setnodetach __P((char **));
static int setupdetach __P((char **));
static int setmodem __P((char **));
static int setlocal __P((char **));
static int setlock __P((char **));
static int setname __P((char **));
static int setuser __P((char **));
static int setremote __P((char **));
static int setauth __P((char **));
static int setnoauth __P((char **));
static int readfile __P((char **));
static int callfile __P((char **));
static int setdefaultroute __P((char **));
static int setnodefaultroute __P((char **));
static int setproxyarp __P((char **));
static int setnoproxyarp __P((char **));
static int setpersist __P((char **));
static int setnopersist __P((char **));
static int setdologin __P((char **));
static int setusehostname __P((char **));
static int setnoipdflt __P((char **));
static int setlcptimeout __P((char **));
static int setlcpterm __P((char **));
static int setlcpconf __P((char **));
static int setlcpfails __P((char **));
static int setipcptimeout __P((char **));
static int setipcpterm __P((char **));
static int setipcpconf __P((char **));
static int setipcpfails __P((char **));
static int setpaptimeout __P((char **));
static int setpapreqs __P((char **));
static int setpapreqtime __P((char **));
static int setchaptimeout __P((char **));
static int setchapchal __P((char **));
static int setchapintv __P((char **));
static int setipcpaccl __P((char **));
static int setipcpaccr __P((char **));
static int setlcpechointv __P((char **));
static int setlcpechofails __P((char **));
static int noccp __P((char **));
static int setbsdcomp __P((char **));
static int setnobsdcomp __P((char **));
static int setdeflate __P((char **));
static int setnodeflate __P((char **));
static int setnodeflatedraft __P((char **));
static int setdemand __P((char **));
static int setpred1comp __P((char **));
static int setnopred1comp __P((char **));
static int setipparam __P((char **));
static int setpapcrypt __P((char **));
static int setidle __P((char **));
static int setholdoff __P((char **));
static int setdnsaddr __P((char **));
static int resetipxproto __P((char **));
static int setwinsaddr __P((char **));
static int showversion __P((char **));
static int showhelp __P((char **));

#ifdef PPP_FILTER
static int setpdebug __P((char **));
static int setpassfilter __P((char **));
static int setactivefilter __P((char **));
#endif

#ifdef IPX_CHANGE
static int setipxproto __P((char **));
static int setipxanet __P((char **));
static int setipxalcl __P((char **));
static int setipxarmt __P((char **));
static int setipxnetwork __P((char **));
static int setipxnode __P((char **));
static int setipxrouter __P((char **));
static int setipxname __P((char **));
static int setipxcptimeout __P((char **));
static int setipxcpterm __P((char **));
static int setipxcpconf __P((char **));
static int setipxcpfails __P((char **));
#endif /* IPX_CHANGE */

#ifdef MSLANMAN
static int setmslanman __P((char **));
#endif

static int number_option __P((char *, u_int32_t *, int));
static int int_option __P((char *, int *));
static int readable __P((int fd));
#endif

/*
 * Valid arguments.
 */
 
/*
 * parse_args - parse a string of arguments from the command line.
 */
int
parse_args(argc, argv)
    int argc;
    char **argv;
{


return 0;
}

/*
 * scan_args - scan the command line arguments to get the tty name,
 * if specified.
 */

/*
 * usage - print out a message telling how to use the program.
 */
void
usage()
{
 }

/*
 * showhelp - print out usage message and exit.
 */
static int
showhelp(argv)
    char **argv;
{
    return 0;
}

/*
 * showversion - print out the version number and exit.
 */
static int
showversion(argv)
    char **argv;
{
    return 0;
}

void
option_error __V((char *fmt, ...))
{
}
/*
 * readable - check if a file is readable by the real user.
 */
/*static int
readable(fd)
    int fd;
{
    uid_t uid;
    int ngroups, i;
    struct stat sbuf;
    GIDSET_TYPE groups[NGROUPS_MAX];

    uid = getuid();
    if (uid == 0)
	return 1;
    if (fstat(fd, &sbuf) != 0)
	return 0;
    if (sbuf.st_uid == uid)
	return sbuf.st_mode & S_IRUSR;
    if (sbuf.st_gid == getgid())
	return sbuf.st_mode & S_IRGRP;
    ngroups = getgroups(NGROUPS_MAX, groups);
    for (i = 0; i < ngroups; ++i)
	if (sbuf.st_gid == groups[i])
	    return sbuf.st_mode & S_IRGRP;
    return sbuf.st_mode & S_IROTH;
}
*/
/*
 * Read a word from a file.
 * Words are delimited by white-space or by quotes (" or ').
 * Quotes, white-space and \ may be escaped with \.
 * \<newline> is ignored.
 */

 /*
 * The following procedures parse options.
 */

/*
 * readfile - take commands from a file.
 */
 
/*
 * callfile - take commands from /etc/ppp/peers/<name>.
 * Name may not contain /../, start with / or ../, or end in /..
 */
 /*
 * setdebug - Set debug (command line argument).
 */
 

/*
 * noopt - Disable all options.
 */
static int
noopt(argv)
    char **argv;
{
    BZERO((char *) &lcp_wantoptions[0], sizeof (struct lcp_options));
    BZERO((char *) &lcp_allowoptions[0], sizeof (struct lcp_options));
    BZERO((char *) &ipcp_wantoptions[0], sizeof (struct ipcp_options));
    BZERO((char *) &ipcp_allowoptions[0], sizeof (struct ipcp_options));

#ifdef IPX_CHANGE
    BZERO((char *) &ipxcp_wantoptions[0], sizeof (struct ipxcp_options));
    BZERO((char *) &ipxcp_allowoptions[0], sizeof (struct ipxcp_options));
#endif /* IPX_CHANGE */

    return (1);
}
