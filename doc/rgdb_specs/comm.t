@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter Communication with GDB

The RTEMS remote debugger will be accessed by GDB on a host machine
through a communication link. We will use the TCP/IP stack included in RTEMS
: the FreeBSD stack. The communication link will be based based on the UDP protocol
and the BSD sockets which are parts of the FreeBSD stack. On top of these layers,
we will plug a module which allows a simple communication between different
machines (especially between different endianess machines) : the SUN Remote
Procedure Call (SUN RPC). This code is freely available on the net and comes
with a BSD like license. With this module, a process can invoke a procedure
on a remote system. The RTEMS remote debugger will be seen by GDB as a SUN RPC
server. Commands will be packed by the GDB SUN RPC client and sent to the server.
This server will unpack these commands, execute them and, if needed, return
results to the SUN RPC client.


Only a minimal subset of the SUN RPC library must be implemented.
For example, the portmapper related API which allows a dynamic allocation of
port numbers will not be implemented and some specific UDP port numbers will
be used to establish the communication between the host and the target. The
SUN RPC library implements the XDR module (eXternal Data Representation) which
is a standard way of encoding data in a portable fashion between different endian
systems. Below are figures describing the additional code and data size for
the minimal library implementation we currently have already implemented for
RTEMS :

@example
$ size -x librpc.a 
text  data bss dec hex filename 
0x40e 0x0 0x0 1038 40e rpc_callmsg.o (ex librpc.a) 
0x2f1 0x18 0x0 777 309 rpc_prot.o (ex librpc.a) 
0x458 0x0 0x0 1112 458 svc.o (ex librpc.a) 
0x4f 0x4 0x0 83 53 svc_auth.o (ex librpc.a) 
0x75c 0x18 0x0 1908 774 svc_udp.o (ex librpc.a) 
0x711 0x4 0x10 1829 725 xdr.o (ex librpc.a)
0x149 0x0 0x0 329 149 xdr_array.o (ex librpc.a) 
0x165 0x20 0x0 389 185 xdr_mem.o (ex librpc.a)
@end example

We have a constraint with the use of the UDP protocol. Because this
protocol is connectionless, it is impossible, especially for the target, to
detect if the connection is always active. On the other hand, using the TCP/IP
protocols seems to be heavy especially if we plan to implement a dedicated micro
stack for debug in the future. It can be a real problem to let the debugged
process stopped during a long time even if there is no more debugger connected
to the system. To avoid such a problem, the target must periodically test the
connection with the host on another way than the one used to receive the commands.
We must therefore open two communication ways so we need two fixed UDP port
numbers. 

@enumerate
@item One port will be used by the debugger to send its commands to the
debugged process and to receive the result of these commands. View from the
remote debugger, this port will be called primary port. For this one, we choose
arbitrarily the port number 2000. 
@item The other socket will be used as secondary port by the target to sometimes
test the connection between the host and the target. These tests will occur
in specific situations, when a process will be stopped on a breakpoint, single
step instruction or other means. This secondary port will also be used by the
target to signal any change in the behavior of a debugged process (stopped,
killed, waiting for,...). For the secondary port, we choose the port number
2010.
@end enumerate

These two port numbers are used by the remote debugger to open the
two communication sockets. GDB will use its own mean to choose its port numbers
(probably the Unix portmapper). The figure layer shows the different
layers we need to implement. 

@c
@c Communications Layers Figure
@c

@ifset use-ascii
@example
@group
XXXXX reference it in the previous paragraph
XXXXX insert layers.eps
XXXXX Caption Communications Layers
@end group
@end example
@end ifset

@ifset use-tex
@example
@group
XXXXX reference it in the previous paragraph
XXXXX insert layers.eps
XXXXX Caption Communications Layers
@end group
@end example
@end ifset

@c @image{layers}

@ifset use-html
@c <IMG SRC="layers.jpg" WIDTH=500 HEIGHT=600 ALT="Communications Layers">
@html
<IMG SRC="layers.jpg" ALT="Communications Layers">
@end html
@end ifset




