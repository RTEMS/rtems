/*
 * auth.c - PPP authentication and phase control.
 *
 * Copyright (c) 1993 The Australian National University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the Australian National University.  The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
#include <stddef.h>
#include <stdlib.h> 
#include <unistd.h>
#include <syslog.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <fcntl.h>
#if defined(_PATH_LASTLOG) && defined(_linux_)
#include <lastlog.h>
#endif

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* #include <stbconfig.h> */

#include "pppd.h"
#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#include "upap.h"
#include "chap.h"
#ifdef CBCP_SUPPORT
#include "cbcp.h"
#endif
#include "pathnames.h"

/* Used for storing a sequence of words.  Usually malloced. */
struct wordlist {
    struct wordlist	*next;
    char		word[1];
};

/* Bits in scan_authfile return value */
#define NONWILD_SERVER	1
#define NONWILD_CLIENT	2

#define ISWILD(word)	(word[0] == '*' && word[1] == 0)

#define FALSE	0
#define TRUE	1

/* The name by which the peer authenticated itself to us. */
char peer_authname[MAXNAMELEN];

/* Records which authentication operations haven't completed yet. */
static int auth_pending[NUM_PPP];

/* Set if we have successfully called plogin() */
static int logged_in;

/* Set if we have run the /etc/ppp/auth-up script. */
static int did_authup;

/* List of addresses which the peer may use. */
static struct wordlist *addresses[NUM_PPP];

/* Number of network protocols which we have opened. */
static int num_np_open;

/* Number of network protocols which have come up. */
static int num_np_up;

/* Set if we got the contents of passwd[] from the pap-secrets file. */
static int passwd_from_file;

/* Bits in auth_pending[] */
#define PAP_WITHPEER	1
#define PAP_PEER	2
#define CHAP_WITHPEER	4
#define CHAP_PEER	8

extern char *crypt __P((const char *, const char *));

/* Prototypes for procedures local to this file. */

static void network_phase __P((int));
static void check_idle __P((void *));
static void connect_time_expired __P((void *));
static int  plogin __P((char *, char *, char **, int *));
static void plogout __P((void));
static int  null_login __P((int));
static int  get_pap_passwd __P((char *));
static int  have_pap_secret __P((void));
static int  have_chap_secret __P((char *, char *, u_int32_t));
static int  ip_addr_check __P((u_int32_t, struct wordlist *));
static int  scan_authfile __P((FILE *, char *, char *, u_int32_t, char *,
			       struct wordlist **, char *));
static void free_wordlist __P((struct wordlist *));
static void auth_script __P((char *));
static void set_allowed_addrs __P((int, struct wordlist *));

/*
 * An Open on LCP has requested a change from Dead to Establish phase.
 * Do what's necessary to bring the physical layer up.
 */
void
link_required(unit)
    int unit;
{
}

/*
 * LCP has terminated the link; go to the Dead phase and take the
 * physical layer down.
 */
void
link_terminated(unit)
    int unit;
{
    if (phase == PHASE_DEAD)
	return;
    if (logged_in)
	plogout();
    phase = PHASE_DEAD;
    syslog(LOG_NOTICE, "Connection terminated.");
}

/*
 * LCP has gone down; it will either die or try to re-establish.
 */
void
link_down(unit)
    int unit;
{
    int i;
    struct protent *protp;

	did_authup = 0;
    for (i = 0; (protp = protocols[i]) != NULL; ++i) {
	if (!protp->enabled_flag)
	    continue;
        if (protp->protocol != PPP_LCP && protp->lowerdown != NULL)
	    (*protp->lowerdown)(unit);
        if (protp->protocol < 0xC000 && protp->close != NULL)
	    (*protp->close)(unit, "LCP down");
    }
    num_np_open = 0;
    num_np_up = 0;
    if (phase != PHASE_DEAD)
	phase = PHASE_TERMINATE;
}

/*
 * The link is established.
 * Proceed to the Dead, Authenticate or Network phase as appropriate.
 */
void
link_established(unit)
    int unit;
{
    int auth;
    lcp_options *wo = &lcp_wantoptions[unit];
    lcp_options *go = &lcp_gotoptions[unit];
    lcp_options *ho = &lcp_hisoptions[unit];
    int i;
    struct protent *protp;

    /*
     * Tell higher-level protocols that LCP is up.
     */
    for (i = 0; (protp = protocols[i]) != NULL; ++i)
        if (protp->protocol != PPP_LCP && protp->enabled_flag
	    && protp->lowerup != NULL)
	    (*protp->lowerup)(unit);

    if (auth_required && !(go->neg_chap || go->neg_upap)) {
	/*
	 * We wanted the peer to authenticate itself, and it refused:
	 * treat it as though it authenticated with PAP using a username
	 * of "" and a password of "".  If that's not OK, boot it out.
	 */
	if (!wo->neg_upap || !null_login(unit)) {
	    lcp_close(unit, "peer refused to authenticate");
	    return;
	}
    }

    phase = PHASE_AUTHENTICATE;
    auth = 0;
    if (go->neg_chap) {
	ChapAuthPeer(unit, our_name, go->chap_mdtype);
	auth |= CHAP_PEER;
    } else if (go->neg_upap) {
	upap_authpeer(unit);
	auth |= PAP_PEER;
    }
    if (ho->neg_chap) {
	ChapAuthWithPeer(unit, user, ho->chap_mdtype);
	auth |= CHAP_WITHPEER;
    } else if (ho->neg_upap) {
	if (passwd[0] == 0) {
	    passwd_from_file = 1;
	    get_pap_passwd(passwd);
	}
	upap_authwithpeer(unit, user, passwd);
	auth |= PAP_WITHPEER;
    }
    auth_pending[unit] = auth;

    if (!auth)
	network_phase(unit);
}

/*
 * Proceed to the network phase.
 */
static void
network_phase(unit)
    int unit;
{
    int i;
    struct protent *protp;
    lcp_options *go = &lcp_gotoptions[unit];

    /*
     * If the peer had to authenticate, run the auth-up script now.
     */
    if ((go->neg_chap || go->neg_upap) && !did_authup) {
	auth_script(_PATH_AUTHUP);
	did_authup = 1;
    }

#ifdef CBCP_SUPPORT
    /*
     * If we negotiated callback, do it now.
     */
    if (go->neg_cbcp) {
	phase = PHASE_CALLBACK;
	(*cbcp_protent.open)(unit);
	return;
    }
#endif

    phase = PHASE_NETWORK;

    for (i = 0; (protp = protocols[i]) != NULL; ++i)
        if (protp->protocol < 0xC000 && protp->enabled_flag
	    && protp->open != NULL) {
	    (*protp->open)(unit);
	    if (protp->protocol != PPP_CCP)
		++num_np_open;
	}

    if (num_np_open == 0)
	/* nothing to do */
	lcp_close(0, "No network protocols running");
}

/*
 * The peer has failed to authenticate himself using `protocol'.
 */
void
auth_peer_fail(unit, protocol)
    int unit, protocol;
{
    /*
     * Authentication failure: take the link down
     */
    lcp_close(unit, "Authentication failed");
}

/*
 * The peer has been successfully authenticated using `protocol'.
 */
void
auth_peer_success(unit, protocol, name, namelen)
    int unit, protocol;
    char *name;
    int namelen;
{
    int bit;

    switch (protocol) {
    case PPP_CHAP:
	bit = CHAP_PEER;
	break;
    case PPP_PAP:
	bit = PAP_PEER;
	break;
    default:
	syslog(LOG_WARNING, "auth_peer_success: unknown protocol %x",
	       protocol);
	return;
    }

    /*
     * Save the authenticated name of the peer for later.
     */
    if (namelen > sizeof(peer_authname) - 1)
	namelen = sizeof(peer_authname) - 1;
    BCOPY(name, peer_authname, namelen);
    peer_authname[namelen] = 0;
    /*
     * If there is no more authentication still to be done,
     * proceed to the network (or callback) phase.
     */
    if ((auth_pending[unit] &= ~bit) == 0)
        network_phase(unit);
}

/*
 * We have failed to authenticate ourselves to the peer using `protocol'.
 */
void
auth_withpeer_fail(unit, protocol)
    int unit, protocol;
{
    if (passwd_from_file)
	BZERO(passwd, MAXSECRETLEN);
    /*
     * We've failed to authenticate ourselves to our peer.
     * He'll probably take the link down, and there's not much
     * we can do except wait for that.
     */
}

/*
 * We have successfully authenticated ourselves with the peer using `protocol'.
 */
void
auth_withpeer_success(unit, protocol)
    int unit, protocol;
{
    int bit;

    switch (protocol) {
    case PPP_CHAP:
	bit = CHAP_WITHPEER;
	break;
    case PPP_PAP:
	if (passwd_from_file)
	    BZERO(passwd, MAXSECRETLEN);
	bit = PAP_WITHPEER;
	break;
    default:

	bit = 0;
    }

    /*
     * If there is no more authentication still being done,
     * proceed to the network (or callback) phase.
     */
    if ((auth_pending[unit] &= ~bit) == 0)
	network_phase(unit);
}


/*
 * np_up - a network protocol has come up.
 */
void
np_up(unit, proto)
    int unit, proto;
{
    if (num_np_up == 0) {
	/*
	 * At this point we consider that the link has come up successfully.
	 */
	need_holdoff = 0;

	if (idle_time_limit > 0)
	    TIMEOUT(check_idle, NULL, idle_time_limit);

	/*
	 * Set a timeout to close the connection once the maximum
	 * connect time has expired.
	 */
	if (maxconnect > 0)
	    TIMEOUT(connect_time_expired, 0, maxconnect);

	/*
	 * Detach now, if the updetach option was given.
	 */
	if (nodetach == -1)
	    detach();
    }
    ++num_np_up;
}

/*
 * np_down - a network protocol has gone down.
 */
void
np_down(unit, proto)
    int unit, proto;
{
    if (--num_np_up == 0 && idle_time_limit > 0) {
	UNTIMEOUT(check_idle, NULL);
    }
}

/*
 * np_finished - a network protocol has finished using the link.
 */
void
np_finished(unit, proto)
    int unit, proto;
{
    if (--num_np_open <= 0) {
	/* no further use for the link: shut up shop. */
	lcp_close(0, "No network protocols running");
    }
}

/*
 * check_idle - check whether the link has been idle for long
 * enough that we can shut it down.
 */
static void
check_idle(arg)
     void *arg;
{
    struct ppp_idle idle;
    time_t itime;

    if (!get_idle_time(0, &idle))
	return;
    itime = MIN(idle.xmit_idle, idle.recv_idle);
    if (itime >= idle_time_limit) {
	/* link is idle: shut it down. */
	syslog(LOG_INFO, "Terminating connection due to lack of activity.");
	lcp_close(0, "Link inactive");
    } else {
	TIMEOUT(check_idle, NULL, idle_time_limit - itime);
    }
}

/*
 * connect_time_expired - log a message and close the connection.
 */
static void
connect_time_expired(arg)
    void *arg;
{
    syslog(LOG_INFO, "Connect time expired");
    lcp_close(0, "Connect time expired");	/* Close connection */
}

/*
 * auth_check_options - called to check authentication options.
 */
void
auth_check_options()
{
    lcp_options *wo = &lcp_wantoptions[0];
    int can_auth;
    ipcp_options *ipwo = &ipcp_wantoptions[0];
    u_int32_t remote;

    /* Default our_name to hostname, and user to our_name */
    if (our_name[0] == 0 || usehostname)
	strcpy(our_name, hostname);
    if (user[0] == 0)
	strcpy(user, our_name);

    /* If authentication is required, ask peer for CHAP or PAP. */
    if (auth_required && !wo->neg_chap && !wo->neg_upap) {
	wo->neg_chap = 1;
	wo->neg_upap = 1;
    }

    /*
     * Check whether we have appropriate secrets to use
     * to authenticate the peer.
     */
    can_auth = wo->neg_upap && (uselogin || have_pap_secret());
    if (!can_auth && wo->neg_chap) {
	remote = ipwo->accept_remote? 0: ipwo->hisaddr;
	can_auth = have_chap_secret(remote_name, our_name, remote);
    }

    if (auth_required && !can_auth) {
	option_error("peer authentication required but no suitable secret(s) found\n");
	if (remote_name[0] == 0)
	    option_error("for authenticating any peer to us (%s)\n", our_name);
	else
	    option_error("for authenticating peer %s to us (%s)\n",
			 remote_name, our_name);
	exit(1);
    }

    /*
     * Check whether the user tried to override certain values
     * set by root.
     */
    if (!auth_required && auth_req_info.priv > 0) {
	if (!default_device && devnam_info.priv == 0) {
	    option_error("can't override device name when noauth option used");
	    exit(1);
	}
	if ((connector != NULL && connector_info.priv == 0)
	    || (disconnector != NULL && disconnector_info.priv == 0)
	    || (welcomer != NULL && welcomer_info.priv == 0)) {
	    option_error("can't override connect, disconnect or welcome");
	    option_error("option values when noauth option used");
	    exit(1);
	}
    }
}

/*
 * auth_reset - called when LCP is starting negotiations to recheck
 * authentication options, i.e. whether we have appropriate secrets
 * to use for authenticating ourselves and/or the peer.
 */
void
auth_reset(unit)
    int unit;
{
    lcp_options *go = &lcp_gotoptions[unit];
    lcp_options *ao = &lcp_allowoptions[0];
    ipcp_options *ipwo = &ipcp_wantoptions[0];
    u_int32_t remote;

    ao->neg_upap = !refuse_pap /*&& (passwd[0] != 0 || get_pap_passwd(NULL))*/;
    ao->neg_chap = !refuse_chap
	&& have_chap_secret(user, remote_name, (u_int32_t)0);

    if (go->neg_upap && !uselogin && !have_pap_secret())
	go->neg_upap = 0;
    if (go->neg_chap) {
	remote = ipwo->accept_remote? 0: ipwo->hisaddr;
	if (!have_chap_secret(remote_name, our_name, remote))
	    go->neg_chap = 0;
    }
}


/*
 * check_passwd - Check the user name and passwd against the PAP secrets
 * file.  If requested, also check against the system password database,
 * and login the user if OK.
 *
 * returns:
 *	UPAP_AUTHNAK: Authentication failed.
 *	UPAP_AUTHACK: Authentication succeeded.
 * In either case, msg points to an appropriate message.
 */
int
check_passwd(unit, auser, userlen, apasswd, passwdlen, msg, msglen)
    int unit;
    char *auser;
    int userlen;
    char *apasswd;
    int passwdlen;
    char **msg;
    int *msglen;
{


    return UPAP_AUTHNAK;
}


/*
 * plogin - Check the user name and password against the system
 * password database, and login the user if OK.
 *
 * returns:
 *	UPAP_AUTHNAK: Login failed.
 *	UPAP_AUTHACK: Login succeeded.
 * In either case, msg points to an appropriate message.
 */

static int
plogin(user, passwd, msg, msglen)
    char *user;
    char *passwd;
    char **msg;
    int *msglen;
{
    syslog(LOG_INFO, "user %s logged in", user);
    logged_in = TRUE;

    return (UPAP_AUTHACK);
}

/*
 * plogout - Logout the user.
 */
static void
plogout()
{

    logged_in = FALSE;
}


/*
 * null_login - Check if a username of "" and a password of "" are
 * acceptable, and iff so, set the list of acceptable IP addresses
 * and return 1.
 */
static int
null_login(unit)
    int unit;
{
    return 1;
}


/*
 * get_pap_passwd - get a password for authenticating ourselves with
 * our peer using PAP.  Returns 1 on success, 0 if no suitable password
 * could be found.
 */

static int
get_pap_passwd(passwd)
    char *passwd;
{
#if 0
/* XXX PPPConfiguration */
	GlobalSystemStatus *stat;
	stat=LockSTBSystemParam();
	strncpy(passwd,  stat->PPP_Password, MAXSECRETLEN);
	UnlockSTBSystemParam();
#endif
    return 1;
}


/*
 * have_pap_secret - check whether we have a PAP file with any
 * secrets that we could possibly use for authenticating the peer.
 */
static int
have_pap_secret()
{
    return 1;
}


/*
 * have_chap_secret - check whether we have a CHAP file with a
 * secret that we could possibly use for authenticating `client'
 * on `server'.  Either can be the null string, meaning we don't
 * know the identity yet.
 */
static int
have_chap_secret(client, server, remote)
    char *client;
    char *server;
    u_int32_t remote;
{

    return 1;
}


/*
 * get_secret - open the CHAP secret file and return the secret
 * for authenticating the given client on the given server.
 * (We could be either client or server).
 */
int
get_secret(unit, client, server, secret, secret_len, save_addrs)
    int unit;
    char *client;
    char *server;
    char *secret;
    int *secret_len;
    int save_addrs;
{
#if 0
/* XXX PPPConfiguration */
    int len;
	GlobalSystemStatus *stat;
	stat=LockSTBSystemParam();
    len=strlen(stat->PPP_Password);
    strcpy( secret,stat->PPP_Password);
	UnlockSTBSystemParam();

    *secret_len = len;
#endif
    return 1;
}

/*
 * set_allowed_addrs() - set the list of allowed addresses.
 */
static void
set_allowed_addrs(unit, addrs)
    int unit;
    struct wordlist *addrs;
{

}

/*
 * auth_ip_addr - check whether the peer is authorized to use
 * a given IP address.  Returns 1 if authorized, 0 otherwise.
 */
int
auth_ip_addr(unit, addr)
    int unit;
    u_int32_t addr;
{
    return ip_addr_check(addr, addresses[unit]);
}

static int
ip_addr_check(addr, addrs)
    u_int32_t addr;
    struct wordlist *addrs;
{
#if 0
    u_int32_t a, mask, ah;
    int accept;
    char *ptr_word, *ptr_mask;
    struct hostent *hp;
    struct netent *np;
#endif

    /* don't allow loopback or multicast address */
    if (bad_ip_adrs(addr))
	return 0;

    if (addrs == NULL)
	return !auth_required;		/* no addresses authorized */

	return 1;
}

/*
 * bad_ip_adrs - return 1 if the IP address is one we don't want
 * to use, such as an address in the loopback net or a multicast address.
 * addr is in network byte order.
 */
int
bad_ip_adrs(addr)
    u_int32_t addr;
{
    addr = ntohl(addr);
    return (addr >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET
	|| IN_MULTICAST(addr) || IN_BADCLASS(addr);
}

/*
 * check_access - complain if a secret file has too-liberal permissions.
 */
void
check_access(f, filename)
    FILE *f;
    char *filename;
{

}


/*
 * scan_authfile - Scan an authorization file for a secret suitable
 * for authenticating `client' on `server'.  The return value is -1
 * if no secret is found, otherwise >= 0.  The return value has
 * NONWILD_CLIENT set if the secret didn't have "*" for the client, and
 * NONWILD_SERVER set if the secret didn't have "*" for the server.
 * Any following words on the line (i.e. address authorization
 * info) are placed in a wordlist and returned in *addrs.  
 */
static int
scan_authfile(f, client, server, ipaddr, secret, addrs, filename)
    FILE *f;
    char *client;
    char *server;
    u_int32_t ipaddr;
    char *secret;
    struct wordlist **addrs;
    char *filename;
{

    return -1;
}

/*
 * free_wordlist - release memory allocated for a wordlist.
 */
static void
free_wordlist(wp)
    struct wordlist *wp;
{
    struct wordlist *next;

    while (wp != NULL) {
	next = wp->next;
	free(wp);
	wp = next;
    }
}

/*
 * auth_script - execute a script with arguments
 * interface-name peer-name real-user tty speed
 */
static void
auth_script(script)
    char *script;
{
}
