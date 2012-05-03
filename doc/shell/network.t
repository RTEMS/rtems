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

