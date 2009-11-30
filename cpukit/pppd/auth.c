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

#define RCSID	"$Id$"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
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

#ifdef USE_PAM
#include <security/pam_appl.h>
#endif

#ifdef HAS_SHADOW
#include <shadow.h>
#ifndef PW_PPP
#define PW_PPP PW_LOGIN
#endif
#endif

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

static const char rcsid[] = RCSID;

/* The name by which the peer authenticated itself to us. */
char peer_authname[MAXNAMELEN];

/* Records which authentication operations haven't completed yet. */
static int auth_pending[NUM_PPP];

/* List of addresses which the peer may use. */
static struct permitted_ip *addresses[NUM_PPP];

/* Wordlist giving addresses which the peer may use
   without authenticating itself. */
static struct wordlist *noauth_addrs;

/* Extra options to apply, from the secrets file entry for the peer. */
static struct wordlist *extra_options;

/* Number of network protocols which we have opened. */
static int num_np_open;

/* Number of network protocols which have come up. */
static int num_np_up;

/* Set if we got the contents of passwd[] from the pap-secrets file. */
static int passwd_from_file;

/* Set if we require authentication only because we have a default route. */
static bool default_auth;

/* Hook for a link status */
void (*auth_linkup_hook)(void) = NULL;
void (*auth_linkdown_hook)(void) = NULL;

/* Hook to enable a plugin to control the idle time limit */
int (*idle_time_hook)(struct ppp_idle *) = NULL;

/* Hook for a plugin to say whether we can possibly authenticate any peer */
int (*pap_check_hook)(void) = NULL;

/* Hook for a plugin to check the PAP user and password */
int (*pap_auth_hook)(char *user, char *passwd/*, char **msgp,
			  struct wordlist **paddrs,
			  struct wordlist **popts*/) = NULL;

/* Hook for a plugin to know about the PAP user logout */
void (*pap_logout_hook)(void) = NULL;

/* Hook for a plugin to get the PAP password for authenticating us */
int (*pap_passwd_hook)(char *user, char *passwd) = NULL;

/*
 * This is used to ensure that we don't start an auth-up/down
 * script while one is already running.
 */
enum script_state {
    s_down,
    s_up
};

static enum script_state auth_state = s_down;
static enum script_state auth_script_state = s_down;

/*
 * Option variables.
 */
bool uselogin = 0;		/* Use /etc/passwd for checking PAP */
bool cryptpap = 0;		/* Passwords in pap-secrets are encrypted */
bool refuse_pap = 0;		/* Don't wanna auth. ourselves with PAP */
bool refuse_chap = 0;		/* Don't wanna auth. ourselves with CHAP */
bool usehostname = 0;		/* Use hostname for our_name */
bool auth_required = 0;		/* Always require authentication from peer */
bool allow_any_ip = 0;		/* Allow peer to use any IP address */
bool explicit_remote = 0;	/* User specified explicit remote name */
char remote_name[MAXNAMELEN];	/* Peer's name for authentication */

/* Bits in auth_pending[] */
#define PAP_WITHPEER	1
#define PAP_PEER	2
#define CHAP_WITHPEER	4
#define CHAP_PEER	8

extern char *crypt(const char *, const char *);

/* Prototypes for procedures local to this file. */

static void network_phase(int);
static void check_idle(void *);
static void connect_time_expired(void *);
static int  null_login(int);
static int  get_pap_passwd(char *);
static int  have_pap_secret(int *);
static int  have_chap_secret(char *, char *, int, int *);
#if 0
static int  ip_addr_check(uint32_t, struct permitted_ip *);
#endif
static void free_wordlist(struct wordlist *);
static void auth_script(enum script_state s);
static void set_allowed_addrs(int, struct wordlist *, struct wordlist *);


/*
 * Authentication-related options.
 */
option_t auth_options[] = {
    { "require-pap", o_bool, &lcp_wantoptions[0].neg_upap,
      "Require PAP authentication from peer", 1, &auth_required, 0, 0 },
    { "+pap", o_bool, &lcp_wantoptions[0].neg_upap,
      "Require PAP authentication from peer", 1, &auth_required, 0, 0 },
    { "refuse-pap", o_bool, &refuse_pap,
      "Don't agree to auth to peer with PAP", 1, NULL, 0, 0 },
    { "-pap", o_bool, &refuse_pap,
      "Don't allow PAP authentication with peer", 1, NULL, 0, 0 },
    { "require-chap", o_bool, &lcp_wantoptions[0].neg_chap,
      "Require CHAP authentication from peer", 1, &auth_required, 0, 0 },
    { "+chap", o_bool, &lcp_wantoptions[0].neg_chap,
      "Require CHAP authentication from peer", 1, &auth_required, 0, 0 },
    { "refuse-chap", o_bool, &refuse_chap,
      "Don't agree to auth to peer with CHAP", 1, NULL, 0, 0 },
    { "-chap", o_bool, &refuse_chap,
      "Don't allow CHAP authentication with peer", 1, NULL, 0, 0 },
    { "name", o_string, our_name,
      "Set local name for authentication",
      OPT_PRIV|OPT_STATIC, NULL, MAXNAMELEN, 0 },
    { "user", o_string, user,
      "Set name for auth with peer", OPT_STATIC, NULL, MAXNAMELEN, 0 },
    { "usehostname", o_bool, &usehostname,
      "Must use hostname for authentication", 1, NULL, 0, 0 },
    { "remotename", o_string, remote_name,
      "Set remote name for authentication", OPT_STATIC,
      &explicit_remote, MAXNAMELEN, 0 },
    { "auth", o_bool, &auth_required,
      "Require authentication from peer", 1, NULL, 0, 0 },
    { "noauth", o_bool, &auth_required,
      "Don't require peer to authenticate", OPT_PRIV, &allow_any_ip, 0, 0 },
    {  "login", o_bool, &uselogin,
      "Use system password database for PAP", 1, NULL, 0, 0 },
    { "papcrypt", o_bool, &cryptpap,
      "PAP passwords are encrypted", 1, NULL, 0, 0 },
/* Removed for RTEMS PORT
    { "+ua", o_special, setupapfile,
      "Get PAP user and password from file" },
*/
    { "password", o_string, passwd,
      "Password for authenticating us to the peer", OPT_STATIC,
      NULL, MAXSECRETLEN, 0 },
/* Removed for RTEMS_PORT
    { "privgroup", o_special, privgroup,
      "Allow group members to use privileged options", OPT_PRIV },
    { "allow-ip", o_special, set_noauth_addr,
      "Set IP address(es) which can be used without authentication",
      OPT_PRIV },
*/
    { NULL, 0, NULL, NULL, 0,  NULL, 0, 0 }
};

/*
 * An Open on LCP has requested a change from Dead to Establish phase.
 * Do what's necessary to bring the physical layer up.
 */
void
link_required(
    int unit )
{
}

/*
 * LCP has terminated the link; go to the Dead phase and take the
 * physical layer down.
 */
void
link_terminated(
    int unit)
{
    if (pppd_phase == PHASE_DEAD)
	return;
    if (pap_logout_hook) {
	pap_logout_hook();
    }
    new_phase(PHASE_DEAD);
    notice("Connection terminated.");
}

/*
 * LCP has gone down; it will either die or try to re-establish.
 */
void
link_down(
    int unit)
{
    int i;
    struct protent *protp;

    auth_state = s_down;
    if (auth_script_state == s_up) {
	update_link_stats(unit);
	auth_script(s_down);
    }
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
    if (pppd_phase != PHASE_DEAD)
	new_phase(PHASE_TERMINATE);
}

/*
 * The link is established.
 * Proceed to the Dead, Authenticate or Network phase as appropriate.
 */
void
link_established(
    int unit )
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
	 * if we have some address(es) it can use without auth, fine,
	 * otherwise treat it as though it authenticated with PAP using
	 * a username * of "" and a password of "".  If that's not OK,
	 * boot it out.
	 */
	if (noauth_addrs != NULL) {
	    set_allowed_addrs(unit, noauth_addrs, NULL);
	} else if (!wo->neg_upap || !null_login(unit)) {
	    warn("peer refused to authenticate: terminating link");
	    lcp_close(unit, "peer refused to authenticate");
	    pppd_status = EXIT_PEER_AUTH_FAILED;
	    return;
	}
    }

    new_phase(PHASE_AUTHENTICATE);
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
	    if (!get_pap_passwd(passwd))
		error("No secret found for PAP login");
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
network_phase(
    int unit )
{
#ifdef CBCP_SUPPORT
    lcp_options *go = &lcp_gotoptions[unit];
#endif

    /* always run the auth-up script */
    auth_state = s_up;
    if (auth_script_state == s_down) {
        auth_script(s_up);
    }

#ifdef CBCP_SUPPORT
    /*
     * If we negotiated callback, do it now.
     */
    if (go->neg_cbcp) {
	new_phase(PHASE_CALLBACK);
	(*cbcp_protent.open)(unit);
	return;
    }
#endif

    /*
     * Process extra options from the secrets file
     */
    if (extra_options) {
	options_from_list(extra_options, 1);
	free_wordlist(extra_options);
	extra_options = 0;
    }
    start_networks();
}

void
start_networks(void)
{
    int i;
    struct protent *protp;

    new_phase(PHASE_NETWORK);
    for (i = 0; (protp = protocols[i]) != NULL; ++i)
        if (protp->protocol < 0xC000 && protp->enabled_flag
	    && protp->open != NULL) {
	    (*protp->open)(0);
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
auth_peer_fail(
    int unit,
    int protocol)
{
    /*
     * Authentication failure: take the link down
     */
    lcp_close(unit, "Authentication failed");
    pppd_status = EXIT_PEER_AUTH_FAILED;
}

/*
 * The peer has been successfully authenticated using `protocol'.
 */
void
auth_peer_success(
    int unit,
    int protocol,
    char *name,
    int namelen)
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
	warn("auth_peer_success: unknown protocol %x", protocol);
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
auth_withpeer_fail(
    int unit,
    int protocol )
{
    if (passwd_from_file)
	BZERO(passwd, MAXSECRETLEN);
    /*
     * We've failed to authenticate ourselves to our peer.
     * Some servers keep sending CHAP challenges, but there
     * is no point in persisting without any way to get updated
     * authentication secrets.
     */
    lcp_close(unit, "Failed to authenticate ourselves to peer");
    pppd_status = EXIT_AUTH_TOPEER_FAILED;
}

/*
 * We have successfully authenticated ourselves with the peer using `protocol'.
 */
void
auth_withpeer_success(
    int unit,
    int protocol )
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
	warn("auth_withpeer_success: unknown protocol %x", protocol);
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
np_up(
    int unit,
    int proto )
{
    int tlim;

    if (num_np_up == 0) {
	/*
	 * At this point we consider that the link has come up successfully.
	 */
	pppd_status = EXIT_OK;
	unsuccess = 0;
	new_phase(PHASE_RUNNING);

	if (idle_time_hook != 0)
	    tlim = (*idle_time_hook)(NULL);
	else
	    tlim = idle_time_limit;
	if (tlim > 0)
	    TIMEOUT(check_idle, NULL, tlim);

	/*
	 * Set a timeout to close the connection once the maximum
	 * connect time has expired.
	 */
	if (maxconnect > 0)
	    TIMEOUT(connect_time_expired, 0, maxconnect);
    }
    ++num_np_up;
}

/*
 * np_down - a network protocol has gone down.
 */
void
np_down(
    int unit,
    int proto)
{
    if (--num_np_up == 0) {
	UNTIMEOUT(check_idle, NULL);
	new_phase(PHASE_NETWORK);
    }
}

/*
 * np_finished - a network protocol has finished using the link.
 */
void
np_finished(
    int unit,
    int proto )
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
check_idle(
    void *arg )
{
    struct ppp_idle idle;
    time_t itime;
    int tlim;

    if (!get_idle_time(0, &idle))
	return;
    if (idle_time_hook != 0) {
	tlim = idle_time_hook(&idle);
    } else {
	itime = MIN(idle.xmit_idle, idle.recv_idle);
	tlim = idle_time_limit - itime;
    }
    if (tlim <= 0) {
	/* link is idle: shut it down. */
	notice("Terminating connection due to lack of activity.");
	lcp_close(0, "Link inactive");
	need_holdoff = 0;
	pppd_status = EXIT_IDLE_TIMEOUT;
    } else {
	TIMEOUT(check_idle, NULL, tlim);
    }
}

/*
 * connect_time_expired - log a message and close the connection.
 */
static void
connect_time_expired(
    void *arg)
{
    info("Connect time expired");
    lcp_close(0, "Connect time expired");	/* Close connection */
    pppd_status = EXIT_CONNECT_TIME;
}

/*
 * auth_check_options - called to check authentication options.
 */
int
auth_check_options(void)
{
    lcp_options *wo = &lcp_wantoptions[0];
    int status      = 1;
    int can_auth;
    int lacks_ip;

    /* Default our_name to hostname, and user to our_name */
    if (our_name[0] == 0 || usehostname)
	strlcpy(our_name, hostname, sizeof(our_name));
    if (user[0] == 0)
	strlcpy(user, our_name, sizeof(user));

    /*
     * If we have a default route, require the peer to authenticate
     * unless the noauth option was given or the real user is root.
     */
    if (!auth_required && !allow_any_ip && have_route_to(0) && !privileged) {
        printf("auth_check_options: turning on\n");
	auth_required = 1;
	default_auth = 1;
    }

    /* If authentication is required, ask peer for CHAP or PAP. */
    if (auth_required) {
	if (!wo->neg_chap && !wo->neg_upap) {
	    wo->neg_chap = 1;
	    wo->neg_upap = 1;
	}
    } else {
	wo->neg_chap = 0;
	wo->neg_upap = 0;
    }

    /*
     * Check whether we have appropriate secrets to use
     * to authenticate the peer.
     */
    lacks_ip = 0;
    can_auth = wo->neg_upap && (uselogin || have_pap_secret(&lacks_ip));
    if (!can_auth && wo->neg_chap) {
	can_auth = have_chap_secret((explicit_remote? remote_name: NULL),
				    our_name, 1, &lacks_ip);
    }

    if (auth_required && !can_auth && noauth_addrs == NULL) {
	if (default_auth) {
	    option_error(
"By default the remote system is required to authenticate itself");
	    option_error(
"(because this system has a default route to the internet)");
	} else if (explicit_remote)
	    option_error(
"The remote system (%s) is required to authenticate itself",
			 remote_name);
	else
	    option_error(
"The remote system is required to authenticate itself");
	option_error(
"but I couldn't find any suitable secret (password) for it to use to do so.");
	if (lacks_ip)
	    option_error(
"(None of the available passwords would let it use an IP address.)");

        status = 0;
    }
    return ( status );
}

/*
 * auth_reset - called when LCP is starting negotiations to recheck
 * authentication options, i.e. whether we have appropriate secrets
 * to use for authenticating ourselves and/or the peer.
 */
void
auth_reset(
    int unit)
{
    lcp_options *go = &lcp_gotoptions[unit];
    lcp_options *ao = &lcp_allowoptions[0];

    ao->neg_upap = !refuse_pap && (passwd[0] != 0 || get_pap_passwd(NULL));
    ao->neg_chap = !refuse_chap
	&& (passwd[0] != 0
	    || have_chap_secret(user, (explicit_remote? remote_name: NULL),
				0, NULL));

    if (go->neg_upap && !uselogin && !have_pap_secret(NULL))
	go->neg_upap = 0;
    if (go->neg_chap) {
	if (!have_chap_secret((explicit_remote? remote_name: NULL),
			      our_name, 1, NULL))
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
check_passwd(
    int unit,
    char *auser,
    int userlen,
    char *apasswd,
    int passwdlen,
    char **msg)
{
    char passwd[64], user[64];

    if (pap_auth_hook)
    {
	slprintf(passwd, sizeof(passwd), "%.*v", passwdlen, apasswd);
	slprintf(user, sizeof(user), "%.*v", userlen, auser);

	return (*pap_auth_hook)(user, passwd/*, NULL, NULL, NULL*/) ?
	    UPAP_AUTHACK : UPAP_AUTHNAK;
    }

    return UPAP_AUTHACK;

#if 0
    int    ret = (int)UPAP_AUTHNAK;

    if (( userlen == 0 ) && ( passwdlen == 0 )) {
      ret = (int)UPAP_AUTHACK;
    }
    printf("check_passwd: %d\n", ret);

    return ret;
#endif
}

/*
 * null_login - Check if a username of "" and a password of "" are
 * acceptable, and iff so, set the list of acceptable IP addresses
 * and return 1.
 */
static int
null_login(
    int unit)
{
    return 0;
}


/*
 * get_pap_passwd - get a password for authenticating ourselves with
 * our peer using PAP.  Returns 1 on success, 0 if no suitable password
 * could be found.
 * Assumes passwd points to MAXSECRETLEN bytes of space (if non-null).
 */
static int
get_pap_passwd(
    char *passwd)
{
    int ret = (int)0;

    /*
     * Check whether a plugin wants to supply this.
     */
    if (pap_passwd_hook) {
	ret = (*pap_passwd_hook)(user, passwd);
    }

    return ( ret );
}


/*
 * have_pap_secret - check whether we have a PAP file with any
 * secrets that we could possibly use for authenticating the peer.
 */
static int
have_pap_secret(
    int *lacks_ipp)
{
    return 1;

#if 0
    int ret = (int)0;

    /* let the plugin decide, if there is one */
    printf("have_pap_secret:\n");
    if (pap_check_hook) {
	ret = (*pap_check_hook)();
    }

    return ( ret );
#endif
}


/*
 * have_chap_secret - check whether we have a CHAP file with a
 * secret that we could possibly use for authenticating `client'
 * on `server'.  Either can be the null string, meaning we don't
 * know the identity yet.
 */
static int
have_chap_secret(
    char *client,
    char *server,
    int need_ip,
    int *lacks_ipp)
{
    return 0;
}


/*
 * get_secret - open the CHAP secret file and return the secret
 * for authenticating the given client on the given server.
 * (We could be either client or server).
 */
int
get_secret(
    int unit,
    char *client,
    char *server,
    unsigned char *secret,
    int *secret_len,
    int am_server)
{
    int len;
    char secbuf[MAXWORDLEN];

    if (!am_server && passwd[0] != 0) {
	strlcpy(secbuf, passwd, sizeof(secbuf));
    } else {
        return 0;
    }

    len = strlen(secbuf);
    if (len > MAXSECRETLEN) {
	error("Secret for %s on %s is too long", client, server);
	len = MAXSECRETLEN;
    }
    BCOPY(secbuf, secret, len);
    BZERO(secbuf, sizeof(secbuf));
    *secret_len = len;

    return 1;
}

/*
 * set_allowed_addrs() - set the list of allowed addresses.
 * Also looks for `--' indicating options to apply for this peer
 * and leaves the following words in extra_options.
 */
static void
set_allowed_addrs(
    int unit,
    struct wordlist *addrs,
    struct wordlist *opts)
{
    int n;
    struct wordlist *ap, **pap;
    struct permitted_ip *ip;
    char *ptr_word, *ptr_mask;
    struct hostent *hp;
    struct netent *np;
    uint32_t a, mask, ah, offset;
    struct ipcp_options *wo = &ipcp_wantoptions[unit];
    uint32_t suggested_ip = 0;

    if (addresses[unit] != NULL)
	free(addresses[unit]);
    addresses[unit] = NULL;
    if (extra_options != NULL)
	free_wordlist(extra_options);
    extra_options = opts;

    /*
     * Count the number of IP addresses given.
     */
    for (n = 0, pap = &addrs; (ap = *pap) != NULL; pap = &ap->next)
	++n;
    if (n == 0)
	return;
    ip = (struct permitted_ip *) malloc((n + 1) * sizeof(struct permitted_ip));
    if (ip == 0)
	return;

    n = 0;
    for (ap = addrs; ap != NULL; ap = ap->next) {
	/* "-" means no addresses authorized, "*" means any address allowed */
	ptr_word = ap->word;
	if (strcmp(ptr_word, "-") == 0)
	    break;
	if (strcmp(ptr_word, "*") == 0) {
	    ip[n].permit = 1;
	    ip[n].base = ip[n].mask = 0;
	    ++n;
	    break;
	}

	ip[n].permit = 1;
	if (*ptr_word == '!') {
	    ip[n].permit = 0;
	    ++ptr_word;
	}

	mask = ~ (uint32_t) 0;
	offset = 0;
	ptr_mask = strchr (ptr_word, '/');
	if (ptr_mask != NULL) {
	    int bit_count;
	    char *endp;

	    bit_count = (int) strtol (ptr_mask+1, &endp, 10);
	    if (bit_count <= 0 || bit_count > 32) {
		warn("invalid address length %v in auth. address list",
		     ptr_mask+1);
		continue;
	    }
	    bit_count = 32 - bit_count;	/* # bits in host part */
	    if (*endp == '+') {
		offset = pppifunit + 1;
		++endp;
	    }
	    if (*endp != 0) {
		warn("invalid address length syntax: %v", ptr_mask+1);
		continue;
	    }
	    *ptr_mask = '\0';
	    mask <<= bit_count;
	}

	hp = gethostbyname(ptr_word);
	if (hp != NULL && hp->h_addrtype == AF_INET) {
	    a = *(uint32_t *)hp->h_addr;
	} else {
	    np = getnetbyname (ptr_word);
	    if (np != NULL && np->n_addrtype == AF_INET) {
		a = htonl (np->n_net);
		if (ptr_mask == NULL) {
		    /* calculate appropriate mask for net */
		    ah = ntohl(a);
		    if (IN_CLASSA(ah))
			mask = IN_CLASSA_NET;
		    else if (IN_CLASSB(ah))
			mask = IN_CLASSB_NET;
		    else if (IN_CLASSC(ah))
			mask = IN_CLASSC_NET;
		}
	    } else {
		a = inet_addr (ptr_word);
	    }
	}

	if (ptr_mask != NULL)
	    *ptr_mask = '/';

	if (a == (uint32_t)-1L) {
	    warn("unknown host %s in auth. address list", ap->word);
	    continue;
	}
	if (offset != 0) {
	    if (offset >= ~mask) {
		warn("interface unit %d too large for subnet %v",
		     pppifunit, ptr_word);
		continue;
	    }
	    a = htonl((ntohl(a) & mask) + offset);
	    mask = ~(uint32_t)0;
	}
	ip[n].mask = htonl(mask);
	ip[n].base = a & ip[n].mask;
	++n;
	if (~mask == 0 && suggested_ip == 0)
	    suggested_ip = a;
    }

    ip[n].permit = 0;		/* make the last entry forbid all addresses */
    ip[n].base = 0;		/* to terminate the list */
    ip[n].mask = 0;

    addresses[unit] = ip;

    /*
     * If the address given for the peer isn't authorized, or if
     * the user hasn't given one, AND there is an authorized address
     * which is a single host, then use that if we find one.
     */
    if (suggested_ip != 0
	&& (wo->hisaddr == 0 || !auth_ip_addr(unit, wo->hisaddr)))
	wo->hisaddr = suggested_ip;
}

/*
 * auth_ip_addr - check whether the peer is authorized to use
 * a given IP address.  Returns 1 if authorized, 0 otherwise.
 */
int
auth_ip_addr(
    int unit,
    uint32_t addr)
{
#if 0
    int ok;
#endif

    /* don't allow loopback or multicast address */
    if (bad_ip_adrs(addr))
	return 0;
	
    return 1;

#if 0
    if (addresses[unit] != NULL) {
	ok = ip_addr_check(addr, addresses[unit]);
	if (ok >= 0)
	    return ok;
    }
    if (auth_required)
	return 0;		/* no addresses authorized */
    return allow_any_ip || !have_route_to(addr);
#endif
}

#if 0
static int
ip_addr_check(
    uint32_t addr,
    struct permitted_ip *addrs)
{
    for (; ; ++addrs)
	if ((addr & addrs->mask) == addrs->base)
	    return addrs->permit;
}
#endif

/*
 * bad_ip_adrs - return 1 if the IP address is one we don't want
 * to use, such as an address in the loopback net or a multicast address.
 * addr is in network byte order.
 */
int
bad_ip_adrs(
    uint32_t addr)
{
    addr = ntohl(addr);
    return (addr >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET
	|| IN_MULTICAST(addr) || IN_BADCLASS(addr);
}

/*
 * free_wordlist - release memory allocated for a wordlist.
 */
static void
free_wordlist(
    struct wordlist *wp)
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
auth_script(
    enum script_state s)
{
    switch (s) {
    case s_up:
	auth_script_state = s_up;
	if ( auth_linkup_hook ) {
	  (*auth_linkup_hook)();
        }
        break;
    case s_down:
	auth_script_state = s_down;
	if ( auth_linkdown_hook ) {
	  (*auth_linkdown_hook)();
        }
	break;
    }
}
