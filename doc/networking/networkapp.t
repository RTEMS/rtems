@c
@c  Written by Eric Norum
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Using Networking in an RTEMS Application

@section Makefile changes
@subsection Including the required managers
The FreeBSD networking code requires several RTEMS managers
in the application:

@example
MANAGERS = io event semaphore
@end example

@subsection Increasing the size of the heap
The networking tasks allocate a lot of memory.  For most applications
the heap should be at least 256 kbytes.
The amount of memory set aside for the heap can be adjusted by setting
the @code{CFLAGS_LD} definition as shown below:

@example
CFLAGS_LD += -Wl,--defsym -Wl,HeapSize=0x80000
@end example

This sets aside 512 kbytes of memory for the heap.

@section System Configuration

The networking tasks allocate some RTEMS objects.  These
must be accounted for in the application configuration table.  The following
lists the requirements.

@table @b
@item TASKS
One network task plus a receive and transmit task for each device.

@item SEMAPHORES
One network semaphore plus one syslog mutex semaphore if the application uses
openlog/syslog.

@item EVENTS
The network stack uses @code{RTEMS_EVENT_24} and @code{RTEMS_EVENT_25}.
This has no effect on the application configuration, but
application tasks which call the network functions should not
use these events for other purposes.

@end table

@section Initialization
@subsection Additional include files
The source file which declares the network configuration
structures and calls the network initialization function must include

@example
#include <rtems/rtems_bsdnet.h>
@end example

@subsection Network Configuration
The network configuration is specified by declaring
and initializing the @code{rtems_bsdnet_config}
structure.

@example
@group
struct rtems_bsdnet_config @{
  /*
   * This entry points to the head of the ifconfig chain.
   */
  struct rtems_bsdnet_ifconfig *ifconfig;

  /*
   * This entry should be rtems_bsdnet_do_bootp if BOOTP
   * is being used to configure the network, and NULL
   * if BOOTP is not being used.
   */
  void                    (*bootp)(void);

  /*
   * The remaining items can be initialized to 0, in
   * which case the default value will be used.
   */
  rtems_task_priority  network_task_priority;  /* 100        */
  unsigned long        mbuf_bytecount;         /* 64 kbytes  */
  unsigned long        mbuf_cluster_bytecount; /* 128 kbytes */
  char                *hostname;               /* BOOTP      */
  char                *domainname;             /* BOOTP      */
  char                *gateway;                /* BOOTP      */
  char                *log_host;               /* BOOTP      */
  char                *name_server[3];         /* BOOTP      */
  char                *ntp_server[3];          /* BOOTP      */
  unsigned long        sb_efficiency;          /* 2          */
  /* UDP TX: 9216 bytes */
  unsigned long        udp_tx_buf_size;
  /* UDP RX: 40 * (1024 + sizeof(struct sockaddr_in)) */
  unsigned long        udp_rx_buf_size;
  /* TCP TX: 16 * 1024 bytes */
  unsigned long        tcp_tx_buf_size;
  /* TCP TX: 16 * 1024 bytes */
  unsigned long        tcp_rx_buf_size;
@};
@end group
@end example

The structure entries are described in the following table.
If your application uses BOOTP/DHCP to obtain network configuration
information and if you are happy with the default values described
below, you need to provide only the first two entries in this structure.

@table @code

@item struct rtems_bsdnet_ifconfig *ifconfig
A pointer to the first configuration structure of the first network
device.  This structure is described in the following section.
You must provide a value for this entry since there is no default value for it.

@item void (*bootp)(void)
This entry should be set to @code{rtems_bsdnet_do_bootp} if your
application by default uses the BOOTP/DHCP client protocol to obtain
network configuration information.  It should be set to @code{NULL} if
your application does not use BOOTP/DHCP.

You can also use @code{rtems_bsdnet_do_bootp_rootfs} to have a set of
standard files created with the information return by the BOOTP/DHCP
protocol. The IP address is added to @file{/etc/hosts} with the host
name and domain returned. If no host name or domain is returned
@code{me.mydomain} is used. The BOOTP/DHCP server's address is also
added to @file{/etc/hosts}. The domain name server listed in the
BOOTP/DHCP information are added to @file{/etc/resolv.conf}. A
@code{search} record is also added if a domain is returned. The files
are created if they do not exist.

The default @code{rtems_bsdnet_do_bootp} and
@code{rtems_bsdnet_do_bootp_rootfs} handlers will loop for-ever
waiting for a BOOTP/DHCP server to respond. If an error is detected
such as not valid interface or valid hardware address the target will
reboot allowing any hardware reset to correct itself.

You can provide your own custom handler which allows you to perform
an initialization that meets your specific system requirements. For
example you could try BOOTP/DHCP then enter a configuration tool if no
server is found allowing the user to switch to a static configuration.

@item int network_task_priority 
The priority at which the network task and network device
receive and transmit tasks will run.
If a value of 0 is specified the tasks will run at priority 100.

@item unsigned long mbuf_bytecount
The number of bytes to allocate from the heap for use as mbufs.  
If a value of 0 is specified, 64 kbytes will be allocated.

@item unsigned long mbuf_cluster_bytecount
The number of bytes to allocate from the heap for use as mbuf clusters.  
If a value of 0 is specified, 128 kbytes will be allocated.

@item char *hostname
The host name of the system.
If this, or any of the following, entries are @code{NULL} the value
may be obtained from a BOOTP/DHCP server.

@item char *domainname
The name of the Internet domain to which the system belongs.

@item char *gateway
The Internet host number of the network gateway machine,
specified in 'dotted decimal' (@code{129.128.4.1}) form.

@item char *log_host
The Internet host number of the machine to which @code{syslog} messages
will be sent.

@item char *name_server[3]
The Internet host numbers of up to three machines to be used as
Internet Domain Name Servers.

@item char *ntp_server[3]
The Internet host numbers of up to three machines to be used as
Network Time Protocol (NTP) Servers.

@item unsigned long sb_efficiency
This is the first of five configuration parameters related to
the amount of memory each socket may consume for buffers.  The
TCP/IP stack reserves buffers (e.g. mbufs) for each open socket.  The
TCP/IP stack has different limits for the transmit and receive
buffers associated with each TCP and UDP socket.  By tuning these
parameters, the application developer can make trade-offs between
memory consumption and performance.  The default parameters favor
performance over memory consumption.  See
@uref{http://www.rtems.org/ml/rtems-users/2004/february/msg00200.html}
for more details but note that after the RTEMS 4.8 release series,
the sb_efficiency default was changed from @code{8} to @code{2}.

The user should also be aware of the @code{SO_SNDBUF} and @code{SO_RCVBUF}
IO control operations.  These can be used to specify the 
send and receive buffer sizes for a specific socket.  There
is no standard IO control to change the @code{sb_efficiency} factor.

The @code{sb_efficiency} parameter is a buffering factor used
in the implementation of the TCP/IP stack.  The default is @code{2}
which indicates double buffering.  When allocating memory for each
socket, this number is multiplied by the buffer sizes for that socket.

@item unsigned long udp_tx_buf_size
This configuration parameter specifies the maximum amount of 
buffer memory which may be used for UDP sockets to transmit
with.  The default size is 9216 bytes which corresponds to
the maximum datagram size.

@item unsigned long udp_rx_buf_size
This configuration parameter specifies the maximum amount of 
buffer memory which may be used for UDP sockets to receive
into.  The default size is the following length in bytes:

@example 
40 * (1024 + sizeof(struct sockaddr_in)
@end example

@item unsigned long tcp_tx_buf_size
This configuration parameter specifies the maximum amount of 
buffer memory which may be used for TCP sockets to transmit
with.  The default size is sixteen kilobytes.

@item unsigned long tcp_rx_buf_size
This configuration parameter specifies the maximum amount of 
buffer memory which may be used for TCP sockets to receive
into.  The default size is sixteen kilobytes.

@end table

In addition, the following fields in the @code{rtems_bsdnet_ifconfig}
are of interest.

@table @b

@item int port
The I/O port number (ex: 0x240) on which the external Ethernet
can be accessed.

@item int irno
The interrupt number of the external Ethernet controller.

@item int bpar
The address of the shared memory on the external Ethernet controller.


@end table

@subsection Network device configuration
Network devices are specified and configured by declaring and initializing a
@code{struct rtems_bsdnet_ifconfig} structure for each network device.

The structure entries are described in the following table.  An application
which uses a single network interface, gets network configuration information
from a BOOTP/DHCP server, and uses the default values for all driver
parameters needs to initialize only the first two entries in the
structure.

@table @code
@item char *name
The full name of the network device.  This name consists of the
driver name and the unit number (e.g. @code{"scc1"}).
The @code{bsp.h} include file usually defines RTEMS_BSP_NETWORK_DRIVER_NAME as
the name of the primary (or only) network driver.

@item int (*attach)(struct rtems_bsdnet_ifconfig *conf)
The address of the driver @code{attach} function.   The network 
initialization function calls this function to configure the driver and
attach it to the network stack.
The @code{bsp.h} include file usually defines RTEMS_BSP_NETWORK_DRIVER_ATTACH as
the name of the  attach function of the primary (or only) network driver.

@item struct rtems_bsdnet_ifconfig *next
A pointer to the network device configuration structure for the next network 
interface, or @code{NULL} if this is the configuration structure of the
last network interface.

@item char *ip_address
The Internet address of the device,
specified in `dotted decimal' (@code{129.128.4.2}) form, or @code{NULL}
if the device configuration information is being obtained from a
BOOTP/DHCP server.

@item char *ip_netmask
The Internet inetwork mask of the device,
specified in `dotted decimal' (@code{255.255.255.0}) form, or @code{NULL}
if the device configuration information is being obtained from a
BOOTP/DHCP server.


@item void *hardware_address
The hardware address of the device, or @code{NULL} if the driver is
to obtain the hardware address in some other way (usually  by reading
it from the device or from the bootstrap ROM).

@item int ignore_broadcast
Zero if the device is to accept broadcast packets, non-zero if the device
is to ignore broadcast packets.

@item int mtu
The maximum transmission unit of the device, or zero if the driver
is to choose a default value (typically 1500 for Ethernet devices).

@item int rbuf_count
The number of receive buffers to use, or zero if the driver is to
choose a default value

@item int xbuf_count
The number of transmit buffers to use, or zero if the driver is to
choose a default value
Keep in mind that some network devices may use 4 or more
transmit descriptors for a single transmit buffer.

@end table

A complete network configuration specification can be as simple as the one
shown in the following example.
This configuration uses a single network interface, gets
network configuration information
from a BOOTP/DHCP server, and uses the default values for all driver
parameters.

@example
static struct rtems_bsdnet_ifconfig netdriver_config = @{
  RTEMS_BSP_NETWORK_DRIVER_NAME,
  RTEMS_BSP_NETWORK_DRIVER_ATTACH
@};
struct rtems_bsdnet_config rtems_bsdnet_config = @{
  &netdriver_config,
  rtems_bsdnet_do_bootp,
@};
@end example


@subsection Network initialization
The networking tasks must be started before any network I/O operations
can be performed. This is done by calling: 


@example
rtems_bsdnet_initialize_network ();
@end example

This function is declared in @code{rtems/rtems_bsdnet.h}.
t returns 0 on success and -1 on failure with an error code
in @code{errno}.  It is not possible to undo the effects of
a partial initialization, though, so the function can be
called only once irregardless of the return code.  Consequently,
if the condition for the failure can be corrected, the
system must be reset to permit another network initialization
attempt.



@section Application Programming Interface

The RTEMS network package provides almost a complete set of BSD network
services.  The network functions work like their BSD counterparts
with the following exceptions:

@itemize @bullet
@item A given socket can be read or written by only one task at a time.

@item The @code{select} function only works for file descriptors associated
with sockets.

@item You must call @code{openlog} before calling any of the @code{syslog} functions.

@item @b{Some of the network functions are not thread-safe.}
For example the following functions return a pointer to a static
buffer which remains valid only until the next call:

@table @code
@item gethostbyaddr
@item gethostbyname
@item inet_ntoa
(@code{inet_ntop} is thread-safe, though).
@end table

@item The RTEMS network package gathers statistics.

@item Addition of a mechanism to "tap onto" an interface
and monitor every packet received and transmitted.

@item Addition of @code{SO_SNDWAKEUP} and @code{SO_RCVWAKEUP} socket options.

@end itemize

Some of the new features are discussed in more detail in the following
sections.

@subsection Network Statistics

There are a number of functions to print statistics gathered by
the network stack.
These function are declared in @code{rtems/rtems_bsdnet.h}.

@table @code
@item rtems_bsdnet_show_if_stats
Display statistics gathered by network interfaces.

@item rtems_bsdnet_show_ip_stats
Display IP packet statistics.

@item rtems_bsdnet_show_icmp_stats
Display ICMP packet statistics.

@item rtems_bsdnet_show_tcp_stats
Display TCP packet statistics.

@item rtems_bsdnet_show_udp_stats
Display UDP packet statistics.

@item rtems_bsdnet_show_mbuf_stats
Display mbuf statistics.

@item rtems_bsdnet_show_inet_routes
Display the routing table.

@end table

@subsection Tapping Into an Interface

RTEMS add two new ioctls to the BSD networking code:
SIOCSIFTAP and SIOCGIFTAP.  These may be used to set and get a
@i{tap function}.  The tap function will be called for every
Ethernet packet received by the interface.

These are called like other interface ioctls, such as SIOCSIFADDR.
When setting the tap function with SIOCSIFTAP, set the ifr_tap field
of the ifreq struct to the tap function.  When retrieving the tap
function with SIOCGIFTAP, the current tap function will be returned in
the ifr_tap field.  To stop tapping packets, call SIOCSIFTAP with a
ifr_tap field of 0.

The tap function is called like this:

@example
int tap (struct ifnet *, struct ether_header *, struct mbuf *)
@end example

The tap function should return 1 if the packet was fully handled, in
which case the caller will simply discard the mbuf.  The tap function
should return 0 if the packet should be passed up to the higher
networking layers.

The tap function is called with the network semaphore locked.  It must
not make any calls on the application levels of the networking level
itself.  It is safe to call other non-networking RTEMS functions.

@subsection Socket Options

RTEMS adds two new @code{SOL_SOCKET} level options for @code{setsockopt} and
@code{getsockopt}: @code{SO_SNDWAKEUP} and @code{SO_RCVWAKEUP}.  For both, the
option value should point to a sockwakeup structure.  The sockwakeup
structure has the following fields:

@example
@group
  void    (*sw_pfn) (struct socket *, caddr_t);
  caddr_t sw_arg;
@end group
@end example

These options are used to set a callback function to be called when, for
example, there is
data available from the socket (@code{SO_RCVWAKEUP}) and when there is space
available to accept data written to the socket (@code{SO_SNDWAKEUP}).

If @code{setsockopt} is called with the @code{SO_RCVWAKEUP} option, and the
@code{sw_pfn} field is not zero, then when there is data
available to be read from
the socket, the function pointed to by the @code{sw_pfn} field will be
called.  A pointer to the socket structure will be passed as the first
argument to the function.  The @code{sw_arg} field set by the
@code{SO_RCVWAKEUP} call will be passed as the second argument to the function.

If @code{setsockopt} is called with the @code{SO_SNDWAKEUP}
function, and the @code{sw_pfn} field is not zero, then when
there is space available to accept data written to the socket,
the function pointed to by the @code{sw_pfn} field
will be called.  The arguments passed to the function will be as with
@code{SO_SNDWAKEUP}.

When the function is called, the network semaphore will be locked and 
the callback function runs in the context of the networking task.
The function must be careful not to call any networking functions.  It
is OK to call an RTEMS function; for example, it is OK to send an
RTEMS event.

The purpose of these callback functions is to permit a more efficient
alternative to the select call when dealing with a large number of
sockets.

The callbacks are called by the same criteria that the select
function uses for indicating "ready" sockets. In Stevens @cite{Unix
Network Programming} on page 153-154 in the section "Under what Conditions
Is a Descriptor Ready?" you will find the definitive list of conditions
for readable and writable that also determine when the functions are
called.

When the number of received bytes equals or exceeds the socket receive
buffer "low water mark" (default 1 byte) you get a readable callback. If
there are 100 bytes in the receive buffer and you only read 1, you will
not immediately get another callback. However, you will get another
callback after you read the remaining 99 bytes and at least 1 more byte
arrives. Using a non-blocking socket you should probably read until it
produces error  EWOULDBLOCK and then allow the readable callback to tell
you when more data has arrived.  (Condition 1.a.)

For sending, when the socket is connected and the free space becomes at
or above the "low water mark" for the send buffer (default 4096 bytes)
you will receive a writable callback. You don't get continuous callbacks
if you don't write anything. Using a non-blocking write socket, you can
then call write until it returns a value less than the amount of data
requested to be sent or it produces error EWOULDBLOCK (indicating buffer
full and no longer writable). When this happens you can
try the write again, but it is often better to go do other things and
let the writable callback tell you when space is available to send
again. You only get a writable callback when the free space transitions
to above the "low water mark" and not every time you
write to a non-full send buffer. (Condition 2.a.) 

The remaining conditions enumerated by Stevens handle the fact that
sockets become readable and/or writable when connects, disconnects and
errors occur, not just when data is received or sent. For example, when
a server "listening" socket becomes readable it indicates that a client
has connected and accept can be called without blocking, not that
network data was received (Condition 1.c).

@subsection Adding an IP Alias

The following code snippet adds an IP alias:

@example
void addAlias(const char *pName, const char *pAddr, const char *pMask)
@{
  struct ifaliasreq      aliasreq;
  struct sockaddr_in    *in;

  /* initialize alias request */
  memset(&aliasreq, 0, sizeof(aliasreq));
  sprintf(aliasreq.ifra_name, pName);

  /* initialize alias address */
  in = (struct sockaddr_in *)&aliasreq.ifra_addr;
  in->sin_family = AF_INET;
  in->sin_len    = sizeof(aliasreq.ifra_addr);
  in->sin_addr.s_addr = inet_addr(pAddr);

  /* initialize alias mask */
  in = (struct sockaddr_in *)&aliasreq.ifra_mask;
  in->sin_family = AF_INET;
  in->sin_len    = sizeof(aliasreq.ifra_mask);
  in->sin_addr.s_addr = inet_addr(pMask);

  /* call to setup the alias */
  rtems_bsdnet_ifconfig(pName, SIOCAIFADDR, &aliasreq);
@}
@end example

Thanks to @uref{mailto:mikes@@poliac.com,Mike Seirs} for this example
code.

@subsection Adding a Default Route

The function provided in this section is functionally equivalent to
the command @code{route add default gw yyy.yyy.yyy.yyy}:

@example
void mon_ifconfig(int argc, char *argv[],  unsigned32 command_arg,
                  bool verbose)
@{
    struct sockaddr_in  ipaddr;
    struct sockaddr_in  dstaddr;
    struct sockaddr_in  netmask;
    struct sockaddr_in  broadcast;
    char               *iface;
    int                 f_ip        = 0;
    int                 f_ptp       = 0;
    int                 f_netmask   = 0;
    int                 f_up        = 0;
    int                 f_down      = 0;
    int                 f_bcast     = 0;
    int                 cur_idx;
    int                 rc;
    int                 flags;

    bzero((void*) &ipaddr, sizeof(ipaddr));
    bzero((void*) &dstaddr, sizeof(dstaddr));
    bzero((void*) &netmask, sizeof(netmask));
    bzero((void*) &broadcast, sizeof(broadcast));
    
    ipaddr.sin_len = sizeof(ipaddr);
    ipaddr.sin_family = AF_INET;
    
    dstaddr.sin_len = sizeof(dstaddr);
    dstaddr.sin_family = AF_INET;
    
    netmask.sin_len = sizeof(netmask);
    netmask.sin_family = AF_INET;
    
    broadcast.sin_len = sizeof(broadcast);
    broadcast.sin_family = AF_INET;
    
    cur_idx = 0;
    if (argc <= 1) @{
        /* display all interfaces */
        iface = NULL;
        cur_idx += 1;
    @} else @{
        iface = argv[1];
        if (isdigit(*argv[2])) @{
            if (inet_pton(AF_INET, argv[2], &ipaddr.sin_addr) < 0) @{
                printf("bad ip address: %s\n", argv[2]);
                return;
            @}
            f_ip = 1;
            cur_idx += 3;
        @} else @{
            cur_idx += 2;
        @}
    @}
    
    if ((f_down !=0) && (f_ip != 0)) @{
        f_up = 1;
    @}
    
    while(argc > cur_idx) @{
        if (strcmp(argv[cur_idx], "up") == 0) @{
            f_up = 1;
            if (f_down != 0) @{
                printf("Can't make interface up and down\n");
            @}
        @} else if(strcmp(argv[cur_idx], "down") == 0) @{
            f_down = 1;
            if (f_up != 0) @{
                printf("Can't make interface up and down\n");
            @}
        @} else if(strcmp(argv[cur_idx], "netmask") == 0) @{
            if ((cur_idx + 1) >= argc) @{
                printf("No netmask address\n");
                return;
            @} 
            if (inet_pton(AF_INET, argv[cur_idx+1], &netmask.sin_addr) < 0) @{
                printf("bad netmask: %s\n", argv[cur_idx]);
                return;
            @}
            f_netmask = 1;
            cur_idx += 1;
        @} else if(strcmp(argv[cur_idx], "broadcast") == 0) @{
            if ((cur_idx + 1) >= argc) @{
                printf("No broadcast address\n");
                return;
            @} 
            if (inet_pton(AF_INET, argv[cur_idx+1], &broadcast.sin_addr) < 0) @{
                printf("bad broadcast: %s\n", argv[cur_idx]);
                return;
            @}
            f_bcast = 1;
            cur_idx += 1;
        @} else if(strcmp(argv[cur_idx], "pointopoint") == 0) @{
            if ((cur_idx + 1) >= argc) @{
                printf("No pointopoint address\n");
                return;
            @} 
            if (inet_pton(AF_INET, argv[cur_idx+1], &dstaddr.sin_addr) < 0) @{
                printf("bad pointopoint: %s\n", argv[cur_idx]);
                return;
            @}
            
            f_ptp = 1;
            cur_idx += 1;
        @} else @{
            printf("Bad parameter: %s\n", argv[cur_idx]);
            return;
        @}
        
        cur_idx += 1;
    @}
    
    printf("ifconfig ");
    if (iface != NULL) @{
        printf("%s ", iface);
        if (f_ip != 0) @{
            char str[256];
            inet_ntop(AF_INET, &ipaddr.sin_addr, str, 256);
            printf("%s ", str);
        @}
        
        if (f_netmask != 0) @{
            char str[256];
            inet_ntop(AF_INET, &netmask.sin_addr, str, 256);
            printf("netmask %s ", str);
        @}
        
        if (f_bcast != 0) @{
            char str[256];
            inet_ntop(AF_INET, &broadcast.sin_addr, str, 256);
            printf("broadcast %s ", str);
        @}
        
        if (f_ptp != 0) @{
            char str[256];
            inet_ntop(AF_INET, &dstaddr.sin_addr, str, 256);
            printf("pointopoint %s ", str);
        @}
        
        if (f_up != 0) @{
            printf("up\n");
        @} else if (f_down != 0) @{
            printf("down\n");
        @} else @{
            printf("\n");
        @}
    @}
    
    if ((iface == NULL) || ((f_ip == 0) && (f_down == 0) && (f_up == 0))) @{
        rtems_bsdnet_show_if_stats();
        return;
    @}
    
    flags = 0;
    if (f_netmask) @{
        rc = rtems_bsdnet_ifconfig(iface, SIOCSIFNETMASK, &netmask);
        if (rc < 0) @{
            printf("Could not set netmask: %s\n", strerror(errno));
            return;
        @}
    @}
    
    if (f_bcast) @{
        rc = rtems_bsdnet_ifconfig(iface, SIOCSIFBRDADDR, &broadcast);
        if (rc < 0) @{
            printf("Could not set broadcast: %s\n", strerror(errno));
            return;
        @}
    @}
    
    if (f_ptp) @{
        rc = rtems_bsdnet_ifconfig(iface, SIOCSIFDSTADDR, &dstaddr);
        if (rc < 0) @{
            printf("Could not set destination address: %s\n", strerror(errno));
            return;
        @}
        flags |= IFF_POINTOPOINT;
    @}
    
    /* This must come _after_ setting the netmask, broadcast addresses */    
    if (f_ip) @{
        rc = rtems_bsdnet_ifconfig(iface, SIOCSIFADDR, &ipaddr);
        if (rc < 0) @{
            printf("Could not set IP address: %s\n", strerror(errno));
            return;
        @}
    @}
    
    if (f_up != 0) @{
        flags |= IFF_UP;
    @}
    
    if (f_down != 0) @{
        printf("Warning: taking interfaces down is not supported\n");
    @}
    
    rc = rtems_bsdnet_ifconfig(iface, SIOCSIFFLAGS, &flags);
    if (rc < 0) @{
        printf("Could not set interface flags: %s\n", strerror(errno));
        return;
    @}
@}



void mon_route(int argc, char *argv[],  unsigned32 command_arg,
               bool verbose)
@{
    int                cmd;
    struct sockaddr_in dst;
    struct sockaddr_in gw;
    struct sockaddr_in netmask;
    int                f_host;
    int                f_gw       = 0;
    int                cur_idx;
    int                flags;
    int                rc;
    
    memset(&dst, 0, sizeof(dst));
    memset(&gw, 0, sizeof(gw));
    memset(&netmask, 0, sizeof(netmask));
    
    dst.sin_len = sizeof(dst);
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = inet_addr("0.0.0.0");  
    
    gw.sin_len = sizeof(gw);
    gw.sin_family = AF_INET;
    gw.sin_addr.s_addr = inet_addr("0.0.0.0"); 
    
    netmask.sin_len = sizeof(netmask);
    netmask.sin_family = AF_INET;
    netmask.sin_addr.s_addr = inet_addr("255.255.255.0"); 
    
    if (argc < 2) @{
        rtems_bsdnet_show_inet_routes();
        return;
    @} 
    
    if (strcmp(argv[1], "add") == 0) @{
        cmd = RTM_ADD;
    @} else if (strcmp(argv[1], "del") == 0) @{
        cmd = RTM_DELETE;
    @} else @{
        printf("invalid command: %s\n", argv[1]);
        printf("\tit should be 'add' or 'del'\n");
        return;
    @}
    
    if (argc < 3) @{
        printf("not enough arguments\n");
        return;
    @}
    
    if (strcmp(argv[2], "-host") == 0) @{
        f_host = 1;
    @} else if (strcmp(argv[2], "-net") == 0) @{
        f_host = 0;
    @} else @{
        printf("Invalid type: %s\n", argv[1]);
        printf("\tit should be '-host' or '-net'\n");
        return;
    @}
    
    if (argc < 4) @{
        printf("not enough arguments\n");
        return;
    @}
    
    inet_pton(AF_INET, argv[3], &dst.sin_addr);
    
    cur_idx = 4;
    while(cur_idx < argc) @{
        if (strcmp(argv[cur_idx], "gw") == 0) @{
            if ((cur_idx +1) >= argc) @{
                printf("no gateway address\n");
                return;
            @}
            f_gw = 1;
            inet_pton(AF_INET, argv[cur_idx + 1], &gw.sin_addr);
            cur_idx += 1;
        @} else if(strcmp(argv[cur_idx], "netmask") == 0) @{
            if ((cur_idx +1) >= argc) @{
                printf("no netmask address\n");
                return;
            @}
            f_gw = 1;
            inet_pton(AF_INET, argv[cur_idx + 1], &netmask.sin_addr);
            cur_idx += 1;
        @} else @{
            printf("Unknown argument\n");
            return;
        @}
        cur_idx += 1;
    @}
    
    flags = RTF_STATIC;
    if (f_gw != 0) @{
        flags |= RTF_GATEWAY;
    @}
    if (f_host != 0) @{
        flags |= RTF_HOST;
    @}

    rc = rtems_bsdnet_rtrequest(cmd, &dst, &gw, &netmask, flags, NULL);
    if (rc < 0) @{
        printf("Error adding route\n");
    @}
@}
@end example

Thanks to @uref{mailto:jtm@@smoothmsmoothie.com,Jay Monkman} for this example
code.

@subsection Time Synchronization Using NTP

@example
int rtems_bsdnet_synchronize_ntp (int interval, rtems_task_priority priority);
@end example

If the interval argument is 0 the routine synchronizes the RTEMS time-of-day
clock with the first NTP server in the rtems_bsdnet_ntpserve array and
returns.  The priority argument is ignored.

If the interval argument is greater than 0, the routine also starts an
RTEMS task at the specified priority and polls the NTP server every 
`interval' seconds.  NOTE: This mode of operation has not yet been
implemented.

On successful synchronization of the RTEMS time-of-day clock the routine
returns 0.  If an error occurs a message is printed and the routine returns -1
with an error code in errno.
There is no timeout -- if there is no response from an NTP server the
routine will wait forever.




