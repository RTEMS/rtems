@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c


@chapter Troubleshooting

This section describes symptoms in some situations where the debugger does not
work correctly, and it gives guidelines for resolving the problems encountered. 


@section Target machine name unknown on the host

RTEMS/GDB fails to connect to target machine named my_target:

@example
(gdb) target rtems my_target 

Attaching remote machine across net... \
      Invalid hostname. Couldn't find remote host address.
@end example

@i{==> Ask your system administrator to add an entry with the
Internet number of machine my_target in the file /etc/hosts on your host machine. }


@section Debug server not present

The target rtems command fails:

@example
(gdb) target rtems my_target

Attaching remote machine across net... RPC timed out. Couldn't connect
to remote target
@end example


@i{==> Verify that the target system is properly configured,
and is running the debugging daemon and communication stack.}


