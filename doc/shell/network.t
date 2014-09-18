@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Network Commands

@section Introduction

The RTEMS shell has the following network commands:

@itemize @bullet

@item @code{netstats} - obtain network statistics
@item @code{ifconfig} - configure a network interface
@item @code{route} - show or manipulate the IP routing table
@item @code{ping} - ping a host or IP address

@end itemize

@section Commands

This section details the Network Commands available.  A
subsection is dedicated to each of the commands and
describes the behavior and configuration of that
command as well as providing an example usage.

@c
@c
@c
@page
@subsection netstats - obtain network statistics

@pgindex netstats

@subheading SYNOPSYS:

@example
netstats [-Aimfpcut]
@end example

@subheading DESCRIPTION:

This command is used to display various types of network statistics.  The
information displayed can be specified using command line arguments in
various combinations.  The arguments are interpreted as follows:

@table @b
@item -A
print All statistics

@item -i
print Inet Routes

@item -m
print MBUF Statistics

@item -f
print IF Statistics

@item -p
print IP Statistics

@item -c
print ICMP Statistics

@item -u
print UDP Statistics

@item -t
print TCP Statistics

@end table

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{netstats}:

The following is an example of using the @code{netstats}
command to print the IP routing table:

@smallexample
[/] $ netstats -i
Destination     Gateway/Mask/Hw    Flags     Refs     Use Expire Interface
default         192.168.1.14       UGS         0        0      0 eth1
192.168.1.0     255.255.255.0      U           0        0      1 eth1
192.168.1.14    00:A0:C8:1C:EE:28  UHL         1        0   1219 eth1
192.168.1.51    00:1D:7E:0C:D0:7C  UHL         0      840   1202 eth1
192.168.1.151   00:1C:23:B2:0F:BB  UHL         1       23   1219 eth1
@end smallexample

The following is an example of using the @code{netstats}
command to print the MBUF statistics:

@smallexample
[/] $ netstats -m
************ MBUF STATISTICS ************
mbufs:2048    clusters: 128    free:  63
drops:   0       waits:   0  drains:   0
      free:1967          data:79          header:2           socket:0
       pcb:0           rtable:0           htable:0           atable:0
    soname:0           soopts:0           ftable:0           rights:0
    ifaddr:0          control:0          oobdata:0
@end smallexample

The following is an example of using the @code{netstats}
command to print the print the interface statistics:

@smallexample
[/] $ netstats -f
************ INTERFACE STATISTICS ************
***** eth1 *****
Ethernet Address: 00:04:9F:00:5B:21
Address:192.168.1.244   Broadcast Address:192.168.1.255   Net mask:255.255.255.0
Flags: Up Broadcast Running Active Multicast
Send queue limit:50   length:1    Dropped:0
      Rx Interrupts:889            Not First:0               Not Last:0
              Giant:0              Non-octet:0
            Bad CRC:0                Overrun:0              Collision:0
      Tx Interrupts:867             Deferred:0         Late Collision:0
   Retransmit Limit:0               Underrun:0             Misaligned:0
@end smallexample

The following is an example of using the @code{netstats}
command to print the print IP statistics:

@smallexample
[/] $ netstats -p
************ IP Statistics ************
             total packets received         894
  packets rcvd for unreachable dest          13
 datagrams delivered to upper level         881
    total ip packets generated here         871

@end smallexample

The following is an example of using the @code{netstats}
command to print the ICMP statistics:

@smallexample
[/] $ netstats -c
************ ICMP Statistics ************
                        Type 0 sent         843
                number of responses         843
                    Type 8 received         843

@end smallexample

The following is an example of using the @code{netstats}
command to print the UDP statistics:

@smallexample
[/] $ netstats -u
************ UDP Statistics ************

@end smallexample

The following is an example of using the @code{netstats}
command to print the TCP statistics:

@smallexample
[/] $ netstats -t
************ TCP Statistics ************
               connections accepted           1
            connections established           1
     segs where we tried to get rtt          34
                 times we succeeded          35
                  delayed acks sent           2
                 total packets sent          37
                  data packets sent          35
                    data bytes sent        2618
              ack-only packets sent           2
             total packets received          47
       packets received in sequence          12
         bytes received in sequence         307
                   rcvd ack packets          35
           bytes acked by rcvd acks        2590
      times hdr predict ok for acks          27
 times hdr predict ok for data pkts          10
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_NETSTATS
@findex CONFIGURE_SHELL_COMMAND_NETSTATS

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_NETSTATS} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_NETSTATS} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_netstats

The @code{netstats} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_netstats(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{netstats} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_NETSTATS_Command;
@end example

@c
@c
@c
@page
@subsection ifconfig - configure a network interface

@pgindex ifconfig

@subheading SYNOPSYS:

@example
ifconfig
ifconfig interface
ifconfig interface [up|down]
ifconfig interface [netmask|pointtopoint|broadcast] IP

@end example

@subheading DESCRIPTION:

This command may be used to display information about the
network interfaces in the system or configure them.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

Just like its counterpart on GNU/Linux and BSD systems, this command
is complicated.  More example usages would be a welcome submission.

@subheading EXAMPLES:

The following is an example of how to use @code{ifconfig}:

@smallexample
 ************ INTERFACE STATISTICS ************
***** eth1 *****
Ethernet Address: 00:04:9F:00:5B:21
Address:192.168.1.244   Broadcast Address:192.168.1.255   Net mask:255.255.255.0
Flags: Up Broadcast Running Active Multicast
Send queue limit:50   length:1    Dropped:0
      Rx Interrupts:5391           Not First:0               Not Last:0
              Giant:0              Non-octet:0
            Bad CRC:0                Overrun:0              Collision:0
      Tx Interrupts:5256            Deferred:0         Late Collision:0
   Retransmit Limit:0               Underrun:0             Misaligned:0
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_IFCONFIG
@findex CONFIGURE_SHELL_COMMAND_IFCONFIG

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_IFCONFIG} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_IFCONFIG} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ifconfig

The @code{ifconfig} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ifconfig(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ifconfig} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_IFCONFIG_Command;
@end example

@c
@c
@c
@page
@subsection route - show or manipulate the ip routing table

@pgindex route

@subheading SYNOPSYS:

@example
route [subcommand] [args]
@end example

@subheading DESCRIPTION:

This command is used to display and manipulate the routing table.
When invoked with no arguments, the current routing information is
displayed.  When invoked with the subcommands @code{add} or @code{del},
then additional arguments must be provided to describe the route.

Command templates include the following:

@smallexample
route [add|del] -net IP_ADDRESS gw GATEWAY_ADDRESS [netmask MASK]
route [add|del] -host IP_ADDRESS gw GATEWAY_ADDRES [netmask MASK]
@end smallexample

When not provided the netmask defaults to @code{255.255.255.0}

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

Just like its counterpart on GNU/Linux and BSD systems, this command
is complicated.  More example usages would be a welcome submission.

@subheading EXAMPLES:

The following is an example of how to use @code{route} to display,
add, and delete a new route:

@smallexample
[/] $ route
Destination     Gateway/Mask/Hw    Flags     Refs     Use Expire Interface
default         192.168.1.14       UGS         0        0      0 eth1
192.168.1.0     255.255.255.0      U           0        0      1 eth1
192.168.1.14    00:A0:C8:1C:EE:28  UHL         1        0   1444 eth1
192.168.1.51    00:1D:7E:0C:D0:7C  UHL         0    10844   1202 eth1
192.168.1.151   00:1C:23:B2:0F:BB  UHL         2       37   1399 eth1
[/] $  route add -net 192.168.3.0 gw 192.168.1.14
[/] $ route
Destination     Gateway/Mask/Hw    Flags     Refs     Use Expire Interface
default         192.168.1.14       UGS         0        0      0 eth1
192.168.1.0     255.255.255.0      U           0        0      1 eth1
192.168.1.14    00:A0:C8:1C:EE:28  UHL         2        0   1498 eth1
192.168.1.51    00:1D:7E:0C:D0:7C  UHL         0    14937   1202 eth1
192.168.1.151   00:1C:23:B2:0F:BB  UHL         2       96   1399 eth1
192.168.3.0     192.168.1.14       UGS         0        0      0 eth1
[/] $ route del -net 192.168.3.0 gw 192.168.1.14
[/] $ route
Destination     Gateway/Mask/Hw    Flags     Refs     Use Expire Interface
default         192.168.1.14       UGS         0        0      0 eth1
192.168.1.0     255.255.255.0      U           0        0      1 eth1
192.168.1.14    00:A0:C8:1C:EE:28  UHL         1        0   1498 eth1
192.168.1.51    00:1D:7E:0C:D0:7C  UHL         0    15945   1202 eth1
192.168.1.151   00:1C:23:B2:0F:BB  UHL         2      117   1399 eth1
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ROUTE
@findex CONFIGURE_SHELL_COMMAND_ROUTE

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ROUTE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ROUTE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_route

The @code{route} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_route(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{route} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ROUTE_Command;
@end example

@c
@c
@c
@page
@subsection ping - ping a host or IP address

@pgindex ping

@subheading SYNOPSYS:

@example
ping [-AaDdfnoQqRrv] [-c count] [-G sweepmaxsize] [-g sweepminsize]
     [-h sweepincrsize] [-i wait] [-l preload] [-M mask | time] [-m ttl]
     [-p pattern] [-S src_addr] [-s packetsize] [-t timeout]
     [-W waittime] [-z tos] host
ping [-AaDdfLnoQqRrv] [-c count] [-I iface] [-i wait] [-l preload]
     [-M mask | time] [-m ttl] [-p pattern] [-S src_addr]
     [-s packetsize] [-T ttl] [-t timeout] [-W waittime]
     [-z tos] mcast-group
@end example

@subheading DESCRIPTION:

The ping utility uses the ICMP protocol's mandatory ECHO_REQUEST
datagram to elicit an ICMP ECHO_RESPONSE from a host or gateway.
ECHO_REQUEST datagrams (``pings'') have an IP and ICMP header,
followed by a ``struct timeval'' and then an arbitrary number of
``pad'' bytes used to fill out the packet.  The options are as
follows:

@table @b
@item -A
Audible.  Output a bell (ASCII 0x07) character when no packet is
received before the next packet is transmitted.  To cater for
round-trip times that are longer than the interval between
transmissions, further missing packets cause a bell only if the
maximum number of unreceived packets has increased.

@item -a
Audible.  Include a bell (ASCII 0x07) character in the output when any
packet is received.  This option is ignored if other format options
are present.

@item -c count
Stop after sending (and receiving) count ECHO_RESPONSE packets.  If
this option is not specified, ping will operate until interrupted.  If
this option is specified in conjunction with ping sweeps, each sweep
will consist of count packets.

@item -D
Set the Don't Fragment bit.

@item -d
Set the SO_DEBUG option on the socket being used.

@item -f
Flood ping.  Outputs packets as fast as they come back or one
hundred times per second, whichever is more.  For every ECHO_REQUEST
sent a period ``.'' is printed, while for every ECHO_REPLY received a
backspace is printed.  This provides a rapid display of how many
packets are being dropped.  Only the super-user may use this option.
This can be very hard on a network and should be used with caution.

@item -G sweepmaxsize
Specify the maximum size of ICMP payload when sending sweeping pings.
This option is required for ping sweeps.

@item -g sweepminsize
Specify the size of ICMP payload to start with when sending sweeping
pings.  The default value is 0.

@item -h sweepincrsize
Specify the number of bytes to increment the size of ICMP payload
after each sweep when sending sweeping pings.  The default value is 1.

@item -I iface
Source multicast packets with the given interface address.  This flag
only applies if the ping destination is a multicast address.

@item -i wait
Wait wait seconds between sending each packet.  The default is to wait
for one second between each packet.  The wait time may be fractional,
but only the super-user may specify values less than 1 second.  This
option is incompatible with the -f option.

@item -L
Suppress loopback of multicast packets.  This flag only applies if the
ping destination is a multicast address.

@item -l preload
If preload is specified, ping sends that many packets as fast as
possible before falling into its normal mode of behavior.  Only the
super-user may use this option.

@item -M mask | time
Use ICMP_MASKREQ or ICMP_TSTAMP instead of ICMP_ECHO.  For mask, print
the netmask of the remote machine.  Set the net.inet.icmp.maskrepl MIB
variable to enable ICMP_MASKREPLY.  For time, print the origination,
reception and transmission timestamps.

@item -m ttl
Set the IP Time To Live for outgoing packets.  If not specified, the
kernel uses the value of the net.inet.ip.ttl MIB variable.

@item -n
Numeric output only.  No attempt will be made to lookup symbolic names
for host addresses.

@item -o
Exit successfully after receiving one reply packet.

@item -p pattern
You may specify up to 16 ``pad'' bytes to fill out the packet you
send.  This is useful for diagnosing data-dependent problems in a
network.  For example, ``-p ff'' will cause the sent packet to be
filled with all ones.

@item -Q
Somewhat quiet output.  Don't display ICMP error messages that are in
response to our query messages.  Originally, the -v flag was required
to display such errors, but -v displays all ICMP error messages.  On a
busy machine, this output can be overbear- ing.  Without the -Q flag,
ping prints out any ICMP error mes- sages caused by its own
ECHO_REQUEST messages.

@item -q
Quiet output.  Nothing is displayed except the summary lines at
startup time and when finished.

@item -R
Record route.  Includes the RECORD_ROUTE option in the ECHO_REQUEST
packet and displays the route buffer on returned packets.  Note that
the IP header is only large enough for nine such routes; the
traceroute(8) command is usually better at determining the route
packets take to a particular destination.  If more routes come back
than should, such as due to an illegal spoofed packet, ping will print
the route list and then truncate it at the correct spot.  Many hosts
ignore or discard the RECORD_ROUTE option.

@item -r
Bypass the normal routing tables and send directly to a host on an
attached network.  If the host is not on a directly-attached network,
an error is returned.  This option can be used to ping a local host
through an interface that has no route through it (e.g., after the
interface was dropped).

@item -S src_addr
Use the following IP address as the source address in outgoing
packets.  On hosts with more than one IP address, this option can be
used to force the source address to be something other than the IP
address of the interface the probe packet is sent on.  If the IP
address is not one of this machine's interface addresses, an error is
returned and nothing is sent.

@item -s packetsize
Specify the number of data bytes to be sent.  The default is 56, which
translates into 64 ICMP data bytes when combined with the 8 bytes of
ICMP header data.  Only the super-user may specify val- ues more than
default.  This option cannot be used with ping sweeps.

@item -T ttl
Set the IP Time To Live for multicasted packets.  This flag only
applies if the ping destination is a multicast address.

@item -t timeout
Specify a timeout, in seconds, before ping exits regardless of how
many packets have been received.

@item -v
Verbose output.  ICMP packets other than ECHO_RESPONSE that are
received are listed.

@item -W waittime
Time in milliseconds to wait for a reply for each packet sent.  If a
reply arrives later, the packet is not printed as replied, but
considered as replied when calculating statistics.

@item -z tos
Use the specified type of service.

@end table

@subheading EXIT STATUS:
The ping utility exits with one of the following values:

0    At least one response was heard from the specified host.

2    The transmission was successful but no responses were
     received.

any other value an error occurred.  These values are defined in
<sysexits.h>.

@subheading NOTES:

When using ping for fault isolation, it should first be run on the
local host, to verify that the local network interface is up and
running.  Then, hosts and gateways further and further away should be
``pinged''.  Round-trip times and packet loss statistics are computed.
If duplicate packets are received, they are not included in the packet
loss calculation, although the round trip time of these packets is
used in calculating the round-trip time statistics.  When the
specified number of packets have been sent a brief summary is
displayed, showing the number of packets sent and received, and the
minimum, mean, maximum, and standard deviation of the round-trip
times.

This program is intended for use in network testing, measurement and
management.  Because of the load it can impose on the network, it is
unwise to use ping during normal operations or from automated scripts.

@subheading EXAMPLES:

The following is an example of how to use @code{oing} to ping:

@smallexample
[/] # ping 10.10.10.1
PING 10.10.10.1 (10.10.10.1): 56 data bytes
64 bytes from 10.10.10.1: icmp_seq=0 ttl=63 time=0.356 ms
64 bytes from 10.10.10.1: icmp_seq=1 ttl=63 time=0.229 ms
64 bytes from 10.10.10.1: icmp_seq=2 ttl=63 time=0.233 ms
64 bytes from 10.10.10.1: icmp_seq=3 ttl=63 time=0.235 ms
64 bytes from 10.10.10.1: icmp_seq=4 ttl=63 time=0.229 ms

--- 10.10.10.1 ping statistics ---
5 packets transmitted, 5 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 0.229/0.256/0.356/0.050 ms
[/] # ping -f -c 10000  10.10.10.1
PING 10.10.10.1 (10.10.10.1): 56 data bytes
.
--- 10.10.10.1 ping statistics ---
10000 packets transmitted, 10000 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 0.154/0.225/0.533/0.027 ms
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_PING
@findex CONFIGURE_SHELL_COMMAND_PING

This command is included in the default shell command set.
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_PING} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_PING} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_ping

The @code{ping} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_ping(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{ping} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_PING_Command;
@end example

