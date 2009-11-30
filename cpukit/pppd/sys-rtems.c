/*
 * sys-bsd.c - System-dependent procedures for setting up
 * PPP interfaces on bsd-4.4-ish systems (including 386BSD, NetBSD, etc.)
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * Copyright (c) 1995 The Australian National University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University and The Australian National University.
 * The names of the Universities may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#define RCSID	"$Id$"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/param.h>
#ifdef PPP_FILTER
#include <net/bpf.h>
#endif

#include <net/if.h>
#include <net/ppp_defs.h>
#include <net/if_ppp.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/in.h>

#if RTM_VERSION >= 3
#include <sys/param.h>
#if defined(NetBSD) && (NetBSD >= 199703)
#include <netinet/if_inarp.h>
#else	/* NetBSD 1.2D or later */
#include <netinet/if_ether.h>
#endif
#endif

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/termiostypes.h>
extern int      rtems_bsdnet_microseconds_per_tick;
extern rtems_id rtems_pppd_taskid;

#include "pppd.h"
#include "fsm.h"
#include "ipcp.h"

static const char rcsid[] = RCSID;


static int initdisc = -1;	/* Initial TTY discipline for ppp_fd */
static int initfdflags = -1;	/* Initial file descriptor flags for ppp_fd */
static int ppp_fd = -1;		/* fd which is set to PPP discipline */
static int rtm_seq;

static int restore_term;	/* 1 => we've munged the terminal */
static struct termios inittermios; /* Initial TTY termios */
static struct winsize wsinfo;	/* Initial window size info */

static int loop_slave = -1;
static int loop_master;

static unsigned char inbuf[512]; /* buffer for chars read from loopback */

static int sockfd;		/* socket for doing interface ioctls */

static int if_is_up;		/* the interface is currently up */
static uint32_t ifaddrs[2];	/* local and remote addresses we set */
static uint32_t default_route_gateway;	/* gateway addr for default route */
static uint32_t proxy_arp_addr;	/* remote addr for proxy arp */

/* Prototypes for procedures local to this file. */
static int dodefaultroute(uint32_t, int);
static int get_ether_addr(uint32_t, struct sockaddr_dl *);


/*
 * sys_init - System-dependent initialization.
 */
void
sys_init(void)
{
    /* Get an internet socket for doing socket ioctl's on. */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	fatal("Couldn't create IP socket: %m");
}

/*
 * sys_cleanup - restore any system state we modified before exiting:
 * mark the interface down, delete default route and/or proxy arp entry.
 * This should call die() because it's called from die().
 */
void
sys_cleanup(void)
{
    struct ifreq ifr;

    if (if_is_up) {
	strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) >= 0
	    && ((ifr.ifr_flags & IFF_UP) != 0)) {
	    ifr.ifr_flags &= ~IFF_UP;
	    ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	}
    }
    if (ifaddrs[0] != 0)
	cifaddr(0, ifaddrs[0], ifaddrs[1]);
    if (default_route_gateway)
	cifdefaultroute(0, 0, default_route_gateway);
    if (proxy_arp_addr)
	cifproxyarp(0, proxy_arp_addr);
}

/*
 * sys_close - Clean up in a child process before execing.
 */
void
sys_close(void)
{
    close(sockfd);
    if (loop_slave >= 0) {
	close(loop_slave);
	close(loop_master);
    }
}

/*
 * sys_check_options - check the options that the user specified
 */
int
sys_check_options(void)
{
    return 1;
}

/*
 * ppp_available - check whether the system has any ppp interfaces
 * (in fact we check whether we can do an ioctl on ppp0).
 */
int
ppp_available(void)
{
    int s, ok;
    struct ifreq ifr;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	return 1;		/* can't tell */

    strlcpy(ifr.ifr_name, "ppp0", sizeof (ifr.ifr_name));
    ok = ioctl(s, SIOCGIFFLAGS, (caddr_t) &ifr) >= 0;
    close(s);

    return ok;
}

/*
 * establish_ppp - Turn the serial port into a ppp interface.
 */
int
establish_ppp(
    int fd)
{
    int taskid  = (int)rtems_pppd_taskid;
    int pppdisc = PPPDISC;
    int x;

    if (demand) {
	/*
	 * Demand mode - prime the old ppp device to relinquish the unit.
	 */
	if (ioctl(ppp_fd, PPPIOCXFERUNIT, 0) < 0)
	    fatal("ioctl(transfer ppp unit): %m");
    }

    /*
     * Save the old line discipline of fd, and set it to PPP.
     */
    if (ioctl(fd, TIOCGETD, &initdisc) < 0)
	fatal("ioctl(TIOCGETD): %m");
    if (ioctl(fd, TIOCSETD, &pppdisc) < 0)
	fatal("ioctl(TIOCSETD): %m");

    /* set pppd taskid into the driver */
    ioctl(fd, PPPIOCSTASK, &taskid);

    if (!demand) {
	/*
	 * Find out which interface we were given.
	 */
	if (ioctl(fd, PPPIOCGUNIT, &pppifunit) < 0)
	    fatal("ioctl(PPPIOCGUNIT): %m");
    } else {
	/*
	 * Check that we got the same unit again.
	 */
	if (ioctl(fd, PPPIOCGUNIT, &x) < 0)
	    fatal("ioctl(PPPIOCGUNIT): %m");
	if (x != pppifunit)
	    fatal("transfer_ppp failed: wanted unit %d, got %d", pppifunit, x);
	x = TTYDISC;
	ioctl(loop_slave, TIOCSETD, &x);
    }

    ppp_fd = fd;

    /*
     * Enable debug in the driver if requested.
     */
    if (kdebugflag) {
	if (ioctl(fd, PPPIOCGFLAGS, (caddr_t) &x) < 0) {
	    warn("ioctl (PPPIOCGFLAGS): %m");
	} else {
	    x |= (kdebugflag & 0xFF) * SC_DEBUG;
	    if (ioctl(fd, PPPIOCSFLAGS, (caddr_t) &x) < 0)
		warn("ioctl(PPPIOCSFLAGS): %m");
	}
    }

    /*
     * Set device for non-blocking reads.
     */
    if ((initfdflags = fcntl(fd, F_GETFL)) == -1
	|| fcntl(fd, F_SETFL, initfdflags | O_NONBLOCK) == -1) {
	warn("Couldn't set device to non-blocking mode: %m");
    }

    return fd;
}

/*
 * restore_loop - reattach the ppp unit to the loopback.
 */
void
restore_loop(void)
{
    int x;

    /*
     * Transfer the ppp interface back to the loopback.
     */
    if (ioctl(ppp_fd, PPPIOCXFERUNIT, 0) < 0)
	fatal("ioctl(transfer ppp unit): %m");
    x = PPPDISC;
    if (ioctl(loop_slave, TIOCSETD, &x) < 0)
	fatal("ioctl(TIOCSETD): %m");

    /*
     * Check that we got the same unit again.
     */
    if (ioctl(loop_slave, PPPIOCGUNIT, &x) < 0)
	fatal("ioctl(PPPIOCGUNIT): %m");
    if (x != pppifunit)
	fatal("transfer_ppp failed: wanted unit %d, got %d", pppifunit, x);
    ppp_fd = loop_slave;
}


/*
 * disestablish_ppp - Restore the serial port to normal operation.
 * This shouldn't call die() because it's called from die().
 */
void
disestablish_ppp(
    int fd)
{
    int taskid = (int)0;

    /* clear pppd taskid from the driver */
    ioctl(fd, PPPIOCSTASK, &taskid);

    /* Reset non-blocking mode on fd. */
    if (initfdflags != -1 && fcntl(fd, F_SETFL, initfdflags) < 0)
	warn("Couldn't restore device fd flags: %m");
    initfdflags = -1;

    /* Restore old line discipline. */
    if (initdisc >= 0 && ioctl(fd, TIOCSETD, &initdisc) < 0)
	error("ioctl(TIOCSETD): %m");
    initdisc = -1;

    if (fd == ppp_fd)
	ppp_fd = -1;
}

/*
 * Check whether the link seems not to be 8-bit clean.
 */
void
clean_check(void)
{
    int x;
    char *s;

    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) == 0) {
	s = NULL;
	switch (~x & (SC_RCV_B7_0|SC_RCV_B7_1|SC_RCV_EVNP|SC_RCV_ODDP)) {
	case SC_RCV_B7_0:
	    s = "bit 7 set to 1";
	    break;
	case SC_RCV_B7_1:
	    s = "bit 7 set to 0";
	    break;
	case SC_RCV_EVNP:
	    s = "odd parity";
	    break;
	case SC_RCV_ODDP:
	    s = "even parity";
	    break;
	}
	if (s != NULL) {
	    warn("Serial link is not 8-bit clean:");
	    warn("All received characters had %s", s);
	}
    }
}

/*
 * set_up_tty: Set up the serial port on `fd' for 8 bits, no parity,
 * at the requested speed, etc.  If `local' is true, set CLOCAL
 * regardless of whether the modem option was specified.
 *
 * For *BSD, we assume that speed_t values numerically equal bits/second.
 */
void
set_up_tty(
    int fd, int local)
{
    struct termios     tios;

    if (tcgetattr(fd, &tios) < 0)
	fatal("tcgetattr: %m");

    if (!restore_term) {
	inittermios = tios;
	ioctl(fd, TIOCGWINSZ, &wsinfo);
    }

    tios.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CLOCAL);
    if (crtscts > 0 && !local) {
        if (crtscts == 2) {
#ifdef CDTRCTS
            tios.c_cflag |= CDTRCTS;
#endif
	} else
	    tios.c_cflag |= CRTSCTS;
    } else if (crtscts < 0) {
	tios.c_cflag &= ~CRTSCTS;
#ifdef CDTRCTS
	tios.c_cflag &= ~CDTRCTS;
#endif
    }

    tios.c_cflag |= CS8 | CREAD | HUPCL;
    if (local || !modem)
	tios.c_cflag |= CLOCAL;
    tios.c_iflag = IGNBRK | IGNPAR;
    tios.c_oflag = 0;
    tios.c_lflag = 0;
    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;

    if (crtscts == -2) {
	tios.c_iflag |= IXON | IXOFF;
	tios.c_cc[VSTOP] = 0x13;	/* DC3 = XOFF = ^S */
	tios.c_cc[VSTART] = 0x11;	/* DC1 = XON  = ^Q */
    }

    if (inspeed) {
	cfsetospeed(&tios, inspeed);
	cfsetispeed(&tios, inspeed);
    } else {
	inspeed = cfgetospeed(&tios);
	/*
	 * We can't proceed if the serial port speed is 0,
	 * since that implies that the serial port is disabled.
	 */
	if (inspeed == 0)
	    fatal("Baud rate for %s is 0; need explicit baud rate", devnam);
    }
    baud_rate = inspeed;

/*    if (tcsetattr(fd, TCSAFLUSH, &tios) < 0) {  */
    if (tcsetattr(fd, TCSADRAIN, &tios) < 0) {
	fatal("tcsetattr: %m");
    }

    restore_term = 1;
}

/*
 * restore_tty - restore the terminal to the saved settings.
 */
void
restore_tty(
    int fd)
{
    if (restore_term) {
	if (!default_device) {
	    /*
	     * Turn off echoing, because otherwise we can get into
	     * a loop with the tty and the modem echoing to each other.
	     * We presume we are the sole user of this tty device, so
	     * when we close it, it will revert to its defaults anyway.
	     */
	    inittermios.c_lflag &= ~(ECHO | ECHONL);
	}
/*	if (tcsetattr(fd, TCSAFLUSH, &inittermios) < 0) { */
	if (tcsetattr(fd, TCSADRAIN, &inittermios) < 0) {
	    if (errno != ENXIO)
		warn("tcsetattr: %m");
        }
	ioctl(fd, TIOCSWINSZ, &wsinfo);
	restore_term = 0;
    }
}

/*
 * setdtr - control the DTR line on the serial port.
 * This is called from die(), so it shouldn't call die().
 */
void
setdtr(
  int fd, int on )
{
    int modembits = TIOCM_DTR;

    ioctl(fd, (on? TIOCMBIS: TIOCMBIC), &modembits);
}

/*
 * get_pty - get a pty master/slave pair and chown the slave side
 * to the uid given.  Assumes slave_name points to >= 12 bytes of space.
 */
int
get_pty(
    int *master_fdp,
    int *slave_fdp,
    char *slave_name,
    int uid)
{
    return 1;
}


/*
 * open_ppp_loopback - open the device we use for getting
 * packets in demand mode, and connect it to a ppp interface.
 * Here we use a pty.
 */
int
open_ppp_loopback(void)
{
    return loop_master;
}


/*
 * output - Output PPP packet.
 */
void
output(
    int unit,
    u_char *p,
    int len)
{
    if (debug)
	dbglog("sent %P", p, len);
/*    printf("sent packet [%d]\n", len); */

    if (write(pppd_ttyfd, p, len) < 0) {
	if (errno != EIO)
	    error("write: %m");
    }
}

void
ppp_delay(void)
{
  rtems_interval     ticks;

  /* recommended delay to help negotiation */
  ticks = 300000/rtems_bsdnet_microseconds_per_tick;
  rtems_task_wake_after(ticks);
}

/*
 * wait_input - wait until there is data available,
 * for the length of time specified by *timo (indefinite
 * if timo is NULL).
 */
void
wait_input(
    struct timeval *timo)
{
  rtems_event_set    events;
  rtems_interval     ticks = 0;
  rtems_option       wait = RTEMS_WAIT;

  if(timo) {
    if(timo->tv_sec == 0 && timo->tv_usec == 0)
      wait = RTEMS_NO_WAIT;
    else {
      ticks = (timo->tv_sec * 1000000 + timo->tv_usec) /
        rtems_bsdnet_microseconds_per_tick;
      if(ticks <= 0)
        ticks = 1;
    }
  }
  rtems_event_receive(RTEMS_EVENT_31, RTEMS_EVENT_ANY | wait, ticks, &events);
}

/*
 * read_packet - get a PPP packet from the serial device.
 */
int
read_packet(
    u_char *buf)
{
    int len;

    if ((len = read(pppd_ttyfd, buf, PPP_MTU + PPP_HDRLEN)) < 0) {
	if (errno == EWOULDBLOCK || errno == EINTR) len = -1;
	/*fatal("read: %m"); */
    }

/*    printf("read packet [%d]\n", len); */
    return len;
}


/*
 * get_loop_output - read characters from the loopback, form them
 * into frames, and detect when we want to bring the real link up.
 * Return value is 1 if we need to bring up the link, 0 otherwise.
 */
int
get_loop_output(void)
{
    int rv = 0;
    int n;

    while ((n = read(loop_master, inbuf, sizeof(inbuf))) >= 0) {
	if (loop_chars(inbuf, n))
	    rv = 1;
    }

    if (n == 0)
	fatal("eof on loopback");
    if (errno != EWOULDBLOCK)
	fatal("read from loopback: %m");

    return rv;
}


/*
 * ppp_send_config - configure the transmit characteristics of
 * the ppp interface.
 */
void
ppp_send_config(
    int unit,
    int mtu,
    uint32_t asyncmap,
    int pcomp,
    int accomp)
{
    u_int x;
    struct ifreq ifr;

    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    ifr.ifr_mtu = mtu;
    if (ioctl(sockfd, SIOCSIFMTU, (caddr_t) &ifr) < 0)
	fatal("ioctl(SIOCSIFMTU): %m");

    if (ioctl(ppp_fd, PPPIOCSASYNCMAP, (caddr_t) &asyncmap) < 0)
	fatal("ioctl(PPPIOCSASYNCMAP): %m");

    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) < 0)
	fatal("ioctl (PPPIOCGFLAGS): %m");
    x = pcomp? x | SC_COMP_PROT: x &~ SC_COMP_PROT;
    x = accomp? x | SC_COMP_AC: x &~ SC_COMP_AC;
/*    x = sync_serial ? x | SC_SYNC : x & ~SC_SYNC; */
    if (ioctl(ppp_fd, PPPIOCSFLAGS, (caddr_t) &x) < 0)
	fatal("ioctl(PPPIOCSFLAGS): %m");
}


/*
 * ppp_set_xaccm - set the extended transmit ACCM for the interface.
 */
void
ppp_set_xaccm(
    int unit,
    ext_accm accm)
{
    if (ioctl(ppp_fd, PPPIOCSXASYNCMAP, accm) < 0 && errno != ENOTTY)
	warn("ioctl(set extended ACCM): %m");
}


/*
 * ppp_recv_config - configure the receive-side characteristics of
 * the ppp interface.
 */
void
ppp_recv_config(
    int unit,
    int mru,
    uint32_t asyncmap,
    int pcomp, int accomp)
{
    int x;

    if (ioctl(ppp_fd, PPPIOCSMRU, (caddr_t) &mru) < 0)
	fatal("ioctl(PPPIOCSMRU): %m");
    if (ioctl(ppp_fd, PPPIOCSRASYNCMAP, (caddr_t) &asyncmap) < 0)
	fatal("ioctl(PPPIOCSRASYNCMAP): %m");
    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) < 0)
	fatal("ioctl (PPPIOCGFLAGS): %m");
    x = !accomp? x | SC_REJ_COMP_AC: x &~ SC_REJ_COMP_AC;
    if (ioctl(ppp_fd, PPPIOCSFLAGS, (caddr_t) &x) < 0)
	fatal("ioctl(PPPIOCSFLAGS): %m");
}

/*
 * ccp_test - ask kernel whether a given compression method
 * is acceptable for use.  Returns 1 if the method and parameters
 * are OK, 0 if the method is known but the parameters are not OK
 * (e.g. code size should be reduced), or -1 if the method is unknown.
 */
int
ccp_test(
    int unit, u_char *opt_ptr, int opt_len, int for_transmit)
{
    struct ppp_option_data data;

    data.ptr = opt_ptr;
    data.length = opt_len;
    data.transmit = for_transmit;
    if (ioctl(pppd_ttyfd, PPPIOCSCOMPRESS, (caddr_t) &data) >= 0)
	return 1;
    return (errno == ENOBUFS)? 0: -1;
}

/*
 * ccp_flags_set - inform kernel about the current state of CCP.
 */
void
ccp_flags_set(
    int unit, int isopen, int isup)
{
    int x;

    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) < 0) {
	error("ioctl (PPPIOCGFLAGS): %m");
	return;
    }
    x = isopen? x | SC_CCP_OPEN: x &~ SC_CCP_OPEN;
    x = isup? x | SC_CCP_UP: x &~ SC_CCP_UP;
    if (ioctl(ppp_fd, PPPIOCSFLAGS, (caddr_t) &x) < 0)
	error("ioctl(PPPIOCSFLAGS): %m");
}

/*
 * ccp_fatal_error - returns 1 if decompression was disabled as a
 * result of an error detected after decompression of a packet,
 * 0 otherwise.  This is necessary because of patent nonsense.
 */
int
ccp_fatal_error(
    int unit)
{
    int x;

    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) < 0) {
	error("ioctl(PPPIOCGFLAGS): %m");
	return 0;
    }
    return x & SC_DC_FERROR;
}

/*
 * get_idle_time - return how long the link has been idle.
 */
int
get_idle_time(
    int u,
    struct ppp_idle *ip)
{
    return ioctl(ppp_fd, PPPIOCGIDLE, ip) >= 0;
}

/*
 * get_ppp_stats - return statistics for the link.
 */
int
get_ppp_stats(
    int u,
    struct pppd_stats *stats)
{
    struct ifpppstatsreq req;

    memset (&req, 0, sizeof (req));
    strlcpy(req.ifr_name, ifname, sizeof(req.ifr_name));
    if (ioctl(sockfd, SIOCGPPPSTATS, &req) < 0) {
	error("Couldn't get PPP statistics: %m");
	return 0;
    }
    stats->bytes_in = req.stats.p.ppp_ibytes;
    stats->bytes_out = req.stats.p.ppp_obytes;
    return 1;
}


#ifdef PPP_FILTER
/*
 * set_filters - transfer the pass and active filters to the kernel.
 */
int
set_filters(
    struct bpf_program *pass, struct bpf_program *active)
{
    int ret = 1;

    if (pass->bf_len > 0) {
	if (ioctl(ppp_fd, PPPIOCSPASS, pass) < 0) {
	    error("Couldn't set pass-filter in kernel: %m");
	    ret = 0;
	}
    }
    if (active->bf_len > 0) {
	if (ioctl(ppp_fd, PPPIOCSACTIVE, active) < 0) {
	    error("Couldn't set active-filter in kernel: %m");
	    ret = 0;
	}
    }
    return ret;
}
#endif

/*
 * sifvjcomp - config tcp header compression
 */
int
sifvjcomp(
    int u, int vjcomp, int cidcomp, int maxcid)
{
    u_int x;

    if (ioctl(ppp_fd, PPPIOCGFLAGS, (caddr_t) &x) < 0) {
	error("ioctl (PPPIOCGFLAGS): %m");
	return 0;
    }
    x = vjcomp ? x | SC_COMP_TCP: x &~ SC_COMP_TCP;
    x = cidcomp? x & ~SC_NO_TCP_CCID: x | SC_NO_TCP_CCID;
    if (ioctl(ppp_fd, PPPIOCSFLAGS, (caddr_t) &x) < 0) {
	error("ioctl(PPPIOCSFLAGS): %m");
	return 0;
    }
    if (vjcomp && ioctl(ppp_fd, PPPIOCSMAXCID, (caddr_t) &maxcid) < 0) {
	error("ioctl(PPPIOCSMAXCID): %m");
	return 0;
    }
    return 1;
}

/*
 * sifup - Config the interface up and enable IP packets to pass.
 */
int
sifup(
    int u)
{
    struct ifreq ifr;

    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    if (ioctl(sockfd, SIOCGIFFLAGS, (caddr_t) &ifr) < 0) {
	error("ioctl (SIOCGIFFLAGS): %m");
	return 0;
    }
    ifr.ifr_flags |= IFF_UP;
    if (ioctl(sockfd, SIOCSIFFLAGS, (caddr_t) &ifr) < 0) {
	error("ioctl(SIOCSIFFLAGS): %m");
	return 0;
    }
    if_is_up = 1;
    return 1;
}

/*
 * sifnpmode - Set the mode for handling packets for a given NP.
 */
int
sifnpmode(
    int u,
    int proto,
    enum NPmode mode)
{
    struct npioctl npi;

    npi.protocol = proto;
    npi.mode = mode;
    if (ioctl(ppp_fd, PPPIOCSNPMODE, &npi) < 0) {
	error("ioctl(set NP %d mode to %d): %m", proto, mode);
	return 0;
    }
    return 1;
}

/*
 * sifdown - Config the interface down and disable IP.
 */
int
sifdown(
    int u)
{
    struct ifreq ifr;
    int rv;
    struct npioctl npi;

    rv = 1;
    npi.protocol = PPP_IP;
    npi.mode = NPMODE_ERROR;
    ioctl(ppp_fd, PPPIOCSNPMODE, (caddr_t) &npi);
    /* ignore errors, because ppp_fd might have been closed by now. */

    strlcpy(ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
    if (ioctl(sockfd, SIOCGIFFLAGS, (caddr_t) &ifr) < 0) {
	error("ioctl (SIOCGIFFLAGS): %m");
	rv = 0;
    } else {
	ifr.ifr_flags &= ~IFF_UP;
	if (ioctl(sockfd, SIOCSIFFLAGS, (caddr_t) &ifr) < 0) {
	    error("ioctl(SIOCSIFFLAGS): %m");
	    rv = 0;
	} else
	    if_is_up = 0;
    }
    return rv;
}

/*
 * SET_SA_FAMILY - set the sa_family field of a struct sockaddr,
 * if it exists.
 */
#define SET_SA_FAMILY(addr, family)		\
    BZERO((char *) &(addr), sizeof(addr));	\
    addr.sa_family = (family); 			\
    addr.sa_len = sizeof(addr);

/*
 * sifaddr - Config the interface IP addresses and netmask.
 */
int
sifaddr(
    int u,
    uint32_t o, uint32_t h, uint32_t m )
{
    struct ifaliasreq ifra;
    struct ifreq ifr;

    strlcpy(ifra.ifra_name, ifname, sizeof(ifra.ifra_name));
    SET_SA_FAMILY(ifra.ifra_addr, AF_INET);
    ((struct sockaddr_in *) &ifra.ifra_addr)->sin_addr.s_addr = o;
    SET_SA_FAMILY(ifra.ifra_broadaddr, AF_INET);
    ((struct sockaddr_in *) &ifra.ifra_broadaddr)->sin_addr.s_addr = h;
    if (m != 0) {
	SET_SA_FAMILY(ifra.ifra_mask, AF_INET);
	((struct sockaddr_in *) &ifra.ifra_mask)->sin_addr.s_addr = m;
    } else
	BZERO(&ifra.ifra_mask, sizeof(ifra.ifra_mask));
    BZERO(&ifr, sizeof(ifr));
    strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    if (ioctl(sockfd, SIOCDIFADDR, (caddr_t) &ifr) < 0) {
	if (errno != EADDRNOTAVAIL)
	    warn("Couldn't remove interface address: %m");
    }
    if (ioctl(sockfd, SIOCAIFADDR, (caddr_t) &ifra) < 0) {
	if (errno != EEXIST) {
	    error("Couldn't set interface address: %m");
	    return 0;
	}
	warn("Couldn't set interface address: Address %I already exists", o);
    }
    ifaddrs[0] = o;
    ifaddrs[1] = h;
    return 1;
}

/*
 * cifaddr - Clear the interface IP addresses, and delete routes
 * through the interface if possible.
 */
int
cifaddr(
    int u,
    uint32_t o, uint32_t h )
{
    struct ifaliasreq ifra;

    ifaddrs[0] = 0;
    strlcpy(ifra.ifra_name, ifname, sizeof(ifra.ifra_name));
    SET_SA_FAMILY(ifra.ifra_addr, AF_INET);
    ((struct sockaddr_in *) &ifra.ifra_addr)->sin_addr.s_addr = o;
    SET_SA_FAMILY(ifra.ifra_broadaddr, AF_INET);
    ((struct sockaddr_in *) &ifra.ifra_broadaddr)->sin_addr.s_addr = h;
    BZERO(&ifra.ifra_mask, sizeof(ifra.ifra_mask));
    if (ioctl(sockfd, SIOCDIFADDR, (caddr_t) &ifra) < 0) {
	if (errno != EADDRNOTAVAIL)
	    warn("Couldn't delete interface address: %m");
	return 0;
    }
    return 1;
}

/*
 * sifdefaultroute - assign a default route through the address given.
 */
int
sifdefaultroute(
    int u,
    uint32_t l, uint32_t g)
{
    return dodefaultroute(g, 's');
}

/*
 * cifdefaultroute - delete a default route through the address given.
 */
int
cifdefaultroute(
    int u,
    uint32_t l, uint32_t g)
{
    return dodefaultroute(g, 'c');
}

/*
 * dodefaultroute - talk to a routing socket to add/delete a default route.
 */
static int
dodefaultroute(
    uint32_t g,
    int cmd)
{
/*    int    status;  */
    struct sockaddr_in address;
    struct sockaddr_in netmask;
    struct sockaddr_in gateway;

    memset((void *) &address, 0, sizeof(address));
    address.sin_len = sizeof address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    memset((void *) &netmask, 0, sizeof(netmask));
    netmask.sin_len = sizeof netmask;
    netmask.sin_addr.s_addr = INADDR_ANY;
    netmask.sin_family = AF_INET;

    if (cmd=='s') {	
      memset((void *) &gateway, 0, sizeof(gateway));
      gateway.sin_len = sizeof gateway;
      gateway.sin_family = AF_INET;
      gateway.sin_addr.s_addr = g;

      rtems_bsdnet_rtrequest(RTM_ADD,
                             (struct sockaddr *)&address,
                             (struct sockaddr *)&gateway,
                             (struct sockaddr *)&netmask,
                             (RTF_UP|RTF_GATEWAY|RTF_STATIC), NULL);
    }
    else {
      memset((void *) &gateway, 0, sizeof(gateway));
      gateway.sin_len = sizeof gateway;
      gateway.sin_family = AF_INET;
      gateway.sin_addr.s_addr =  INADDR_ANY;

      rtems_bsdnet_rtrequest(RTM_DELETE,
                             (struct sockaddr *)&address,
                             (struct sockaddr *)&gateway,
                             (struct sockaddr *)&netmask,
                             (RTF_UP|RTF_STATIC), NULL);
    }

    default_route_gateway = (cmd == 's')? g: 0;

    return 1;
}

#if RTM_VERSION >= 3

/*
 * sifproxyarp - Make a proxy ARP entry for the peer.
 */
static struct {
    struct rt_msghdr		hdr;
    struct sockaddr_inarp	dst;
    struct sockaddr_dl		hwa;
    char			extra[128];
} arpmsg;

static int arpmsg_valid;

int
sifproxyarp(
    int unit,
    uint32_t hisaddr)
{
    int routes;

    /*
     * Get the hardware address of an interface on the same subnet
     * as our local address.
     */
    memset(&arpmsg, 0, sizeof(arpmsg));
    if (!get_ether_addr(hisaddr, &arpmsg.hwa)) {
	error("Cannot determine ethernet address for proxy ARP");
	return 0;
    }

    if ((routes = socket(PF_ROUTE, SOCK_RAW, AF_INET)) < 0) {
	error("Couldn't add proxy arp entry: socket: %m");
	return 0;
    }

    arpmsg.hdr.rtm_type = RTM_ADD;
    arpmsg.hdr.rtm_flags = RTF_ANNOUNCE | RTF_HOST | RTF_STATIC;
    arpmsg.hdr.rtm_version = RTM_VERSION;
    arpmsg.hdr.rtm_seq = ++rtm_seq;
    arpmsg.hdr.rtm_addrs = RTA_DST | RTA_GATEWAY;
    arpmsg.hdr.rtm_inits = RTV_EXPIRE;
    arpmsg.dst.sin_len = sizeof(struct sockaddr_inarp);
    arpmsg.dst.sin_family = AF_INET;
    arpmsg.dst.sin_addr.s_addr = hisaddr;
    arpmsg.dst.sin_other = SIN_PROXY;

    arpmsg.hdr.rtm_msglen = (char *) &arpmsg.hwa - (char *) &arpmsg
	+ arpmsg.hwa.sdl_len;
    if (write(routes, &arpmsg, arpmsg.hdr.rtm_msglen) < 0) {
	error("Couldn't add proxy arp entry: %m");
	close(routes);
	return 0;
    }

    close(routes);
    arpmsg_valid = 1;
    proxy_arp_addr = hisaddr;
    return 1;
}

/*
 * cifproxyarp - Delete the proxy ARP entry for the peer.
 */
int
cifproxyarp(
    int unit,
    uint32_t hisaddr)
{
    int routes;

    if (!arpmsg_valid)
	return 0;
    arpmsg_valid = 0;

    arpmsg.hdr.rtm_type = RTM_DELETE;
    arpmsg.hdr.rtm_seq = ++rtm_seq;

    if ((routes = socket(PF_ROUTE, SOCK_RAW, AF_INET)) < 0) {
	error("Couldn't delete proxy arp entry: socket: %m");
	return 0;
    }

    if (write(routes, &arpmsg, arpmsg.hdr.rtm_msglen) < 0) {
	error("Couldn't delete proxy arp entry: %m");
	close(routes);
	return 0;
    }

    close(routes);
    proxy_arp_addr = 0;
    return 1;
}

#else	/* RTM_VERSION */

/*
 * sifproxyarp - Make a proxy ARP entry for the peer.
 */
int
sifproxyarp(
    int unit,
    uint32_t hisaddr)
{
    struct arpreq arpreq;
    struct {
	struct sockaddr_dl	sdl;
	char			space[128];
    } dls;

    BZERO(&arpreq, sizeof(arpreq));

    /*
     * Get the hardware address of an interface on the same subnet
     * as our local address.
     */
    if (!get_ether_addr(hisaddr, &dls.sdl)) {
	error("Cannot determine ethernet address for proxy ARP");
	return 0;
    }

    arpreq.arp_ha.sa_len = sizeof(struct sockaddr);
    arpreq.arp_ha.sa_family = AF_UNSPEC;
    BCOPY(LLADDR(&dls.sdl), arpreq.arp_ha.sa_data, dls.sdl.sdl_alen);
    SET_SA_FAMILY(arpreq.arp_pa, AF_INET);
    ((struct sockaddr_in *) &arpreq.arp_pa)->sin_addr.s_addr = hisaddr;
    arpreq.arp_flags = ATF_PERM | ATF_PUBL;
    if (ioctl(sockfd, SIOCSARP, (caddr_t)&arpreq) < 0) {
	error("Couldn't add proxy arp entry: %m");
	return 0;
    }

    proxy_arp_addr = hisaddr;
    return 1;
}

/*
 * cifproxyarp - Delete the proxy ARP entry for the peer.
 */
int
cifproxyarp(
    int unit,
    uint32_t hisaddr)
{
    struct arpreq arpreq;

    BZERO(&arpreq, sizeof(arpreq));
    SET_SA_FAMILY(arpreq.arp_pa, AF_INET);
    ((struct sockaddr_in *) &arpreq.arp_pa)->sin_addr.s_addr = hisaddr;
    if (ioctl(sockfd, SIOCDARP, (caddr_t)&arpreq) < 0) {
	warn("Couldn't delete proxy arp entry: %m");
	return 0;
    }
    proxy_arp_addr = 0;
    return 1;
}
#endif	/* RTM_VERSION */


/*
 * get_ether_addr - get the hardware address of an interface on the
 * the same subnet as ipaddr.
 */
#define MAX_IFS		32

static int
get_ether_addr(
    uint32_t ipaddr,
    struct sockaddr_dl *hwaddr)
{
    struct ifreq *ifr, *ifend, *ifp;
    uint32_t ina, mask;
    struct sockaddr_dl *dla;
    struct ifreq ifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];

    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
	error("ioctl(SIOCGIFCONF): %m");
	return 0;
    }

    /*
     * Scan through looking for an interface with an Internet
     * address on the same subnet as `ipaddr'.
     */
    ifend = (struct ifreq *) (ifc.ifc_buf + ifc.ifc_len);
    for (ifr = ifc.ifc_req; ifr < ifend; ifr = (struct ifreq *)
	 	((char *)&ifr->ifr_addr + ifr->ifr_addr.sa_len)) {
	if (ifr->ifr_addr.sa_family == AF_INET) {
	    ina = ((struct sockaddr_in *) &ifr->ifr_addr)->sin_addr.s_addr;
	    strlcpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
	    /*
	     * Check that the interface is up, and not point-to-point
	     * or loopback.
	     */
	    if (ioctl(sockfd, SIOCGIFFLAGS, &ifreq) < 0)
		continue;
	    if ((ifreq.ifr_flags &
		 (IFF_UP|IFF_BROADCAST|IFF_POINTOPOINT|IFF_LOOPBACK|IFF_NOARP))
		 != (IFF_UP|IFF_BROADCAST))
		continue;
	    /*
	     * Get its netmask and check that it's on the right subnet.
	     */
	    if (ioctl(sockfd, SIOCGIFNETMASK, &ifreq) < 0)
		continue;
	    mask = ((struct sockaddr_in *) &ifreq.ifr_addr)->sin_addr.s_addr;
	    if ((ipaddr & mask) != (ina & mask))
		continue;

	    break;
	}
    }

    if (ifr >= ifend)
	return 0;
    info("found interface %s for proxy arp", ifr->ifr_name);

    /*
     * Now scan through again looking for a link-level address
     * for this interface.
     */
    ifp = ifr;
    for (ifr = ifc.ifc_req; ifr < ifend; ) {
	if (strcmp(ifp->ifr_name, ifr->ifr_name) == 0
	    && ifr->ifr_addr.sa_family == AF_LINK) {
	    /*
	     * Found the link-level address - copy it out
	     */
	    dla = (struct sockaddr_dl *) &ifr->ifr_addr;
	    BCOPY(dla, hwaddr, dla->sdl_len);
	    return 1;
	}
	ifr = (struct ifreq *) ((char *)&ifr->ifr_addr + ifr->ifr_addr.sa_len);
    }

    return 0;
}

/*
 * Return user specified netmask, modified by any mask we might determine
 * for address `addr' (in network byte order).
 * Here we scan through the system's list of interfaces, looking for
 * any non-point-to-point interfaces which might appear to be on the same
 * network as `addr'.  If we find any, we OR in their netmask to the
 * user-specified netmask.
 */
uint32_t
GetMask(
    uint32_t addr)
{
    uint32_t mask, nmask, ina;
    struct ifreq *ifr, *ifend, ifreq;
    struct ifconf ifc;
    struct ifreq ifs[MAX_IFS];

    addr = ntohl(addr);
    if (IN_CLASSA(addr))	/* determine network mask for address class */
	nmask = IN_CLASSA_NET;
    else if (IN_CLASSB(addr))
	nmask = IN_CLASSB_NET;
    else
	nmask = IN_CLASSC_NET;
    /* class D nets are disallowed by bad_ip_adrs */
    mask = netmask | htonl(nmask);

    /*
     * Scan through the system's network interfaces.
     */
    ifc.ifc_len = sizeof(ifs);
    ifc.ifc_req = ifs;
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
	warn("ioctl(SIOCGIFCONF): %m");
	return mask;
    }
    ifend = (struct ifreq *) (ifc.ifc_buf + ifc.ifc_len);
    for (ifr = ifc.ifc_req; ifr < ifend; ifr = (struct ifreq *)
	 	((char *)&ifr->ifr_addr + ifr->ifr_addr.sa_len)) {
	/*
	 * Check the interface's internet address.
	 */
	if (ifr->ifr_addr.sa_family != AF_INET)
	    continue;
	ina = ((struct sockaddr_in *) &ifr->ifr_addr)->sin_addr.s_addr;
	if ((ntohl(ina) & nmask) != (addr & nmask))
	    continue;
	/*
	 * Check that the interface is up, and not point-to-point or loopback.
	 */
	strlcpy(ifreq.ifr_name, ifr->ifr_name, sizeof(ifreq.ifr_name));
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifreq) < 0)
	    continue;
	if ((ifreq.ifr_flags & (IFF_UP|IFF_POINTOPOINT|IFF_LOOPBACK))
	    != IFF_UP)
	    continue;
	/*
	 * Get its netmask and OR it into our mask.
	 */
	if (ioctl(sockfd, SIOCGIFNETMASK, &ifreq) < 0)
	    continue;
	mask |= ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
    }

    return mask;
}

/*
 * have_route_to - determine if the system has any route to
 * a given IP address.
 * For demand mode to work properly, we have to ignore routes
 * through our own interface.
 */
int have_route_to(uint32_t addr)
{
    return -1;
}

/*
 * Use the hostid as part of the random number seed.
 */
int
get_host_seed(void)
{
    return 17;
}
