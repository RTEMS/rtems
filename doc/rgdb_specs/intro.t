@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter Introduction

The TOOLS project aims to provide a complete development environment
for RTEMS OS. This environment must be as close as possible to the Chorus one
(gnu compiler, gnu linker, gnu debugger, ...), because it is currently the OS
which is the most used at CRF and we want to simplify the migration path from
the ChorusOs environment to the RTEMS environment. One important item of this
development environment is the remote debugger which allows the developer to
debug his software on a target machine from a host machine via a communication
link (Ethernet, serial link, ...).


The choice of GDB as debugger has been made with because in CRF, every
developer, which uses the ChorusOs development environment, debugs his software
using the remote debugging functionality of GDB. Providing a remote GDB debug
server running on RTEMS, will enable the developers to use transparently the
same debugger for a different RTOS. Another reason for the choice of GDB is
its constant evolution, and that it can be interfaced with graphic user interfaces
like DDD providing a very powerfull debugging environment.


The subject of this document is to explain how GDB works and the way
to implement a daemon on RTEMS that will be used as a debugger server for a
GDB client. We will call this daemon Rtems GDB Debug Server Daemon (RGDBSD).
We aim to provide this debugger running at least on 2 host systems : Linux 2.x
and Solaris 2.5.1 as they are the two platforms used for developing Chorus applications
today.


