@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Debug Session Example

The following is a debug session.  Note that some lines have been broken
to aviod formatting problems in the printed document.

@example
GNU gdb 4.17 
Copyright 1998 Free Software Foundation, Inc. 
GDB is free software, covered by the GNU General Public License, and
you are welcome to change it and/or distribute copies of it under
certain conditions.  Type "show copying" to see the conditions.
There is absolutely no warranty for GDB.  
Type "show warranty" for details.
This GDB was configured as --host=i686-pc-linux-gnu --target=i386-rtems. 
(gdb) setrpcmode sun 
(gdb) setdaemontype rdbg 
(gdb) target rtems usnet-test 
Attaching remote machine across net... 
Connected to usnet-test. 
Now the "run" command will start a remote process. 
(gdb) file /buildr4/pc386/tests/debug.exe 
Reading symbols from /buildr4/pc386/tests/debug.exe...done. 
(gdb) attach 1 
Attaching program: /buildr4/pc386/tests/debug.exe pid 1 
0x22fe65 in enterRdbg () 
(gdb) info threads 
There are 8 threads: 
Id.               Name       Detached   Suspended
134283273         Rini       No         No <= current target thread 
0x22fe65 in enterRdbg () 
134283272         Evnt       No         No
_Thread_Dispatch () at /rtems4/c/src/exec/score/src/thread.c:315 
134283271         SPE2       No         No 
_Thread_Dispatch () at /rtems4/c/src/exec/score/src/thread.c:315 
134283270         SPE1       No         No 
_Thread_Handler () at /rtems4/c/src/exec/score/src/thread.c:1107 
134283269         RDBG       No         No 
0x22fe65 in enterRdbg () 
134283268         SCrx       No         No 
_Thread_Dispatch () at /rtems4/c/src/exec/score/src/thread.c:315 
134283267         SCtx       No         No 
_Thread_Dispatch () at /rtems4/c/src/exec/score/src/thread.c:315 
134283266         ntwk       No         No 
_Thread_Dispatch () at /rtems4/c/src/exec/score/src/thread.c:315 
(gdb) b init.c:92 
Breakpoint 1 at 0x200180: file \
       /rtems4/c/src/tests/samples/debug/init.c, line 92. 
(gdb) c 
Continuing. 
Thread 134283273 (Rini) has been deleted. 
[Switching to Rtems thread 134283271 (Not suspended) \
    ( <= current target thread )]
Breakpoint 1, example2 (argument=4) at \
    /rtems4/c/src/tests/samples/debug/init.c:92 
92 tuto += tuti; 
(gdb) b init.c:66 
Breakpoint 2 at 0x200128: file \
    /rtems4/c/src/tests/samples/debug/init.c, line 66. 
(gdb) c 
Continuing. 
[Switching to Rtems thread 134283270 (Not suspended) \
    ( <= current target thread )]
Breakpoint 2, example1 (argument=4) at \
    /rtems4/c/src/tests/samples/debug/init.c:66 
66 toto += titi; 
(gdb) c 
Continuing. 
[Switching to Rtems thread 134283271 (Not suspended) \
    ( <= current target thread )]
Breakpoint 1, example2 (argument=4) at \
    /rtems4/c/src/tests/samples/debug/init.c:92 
92 tuto += tuti; 
(gdb) s 
93 if (print_enable2) 
(gdb) detach 
Detaching program: /buildr4/pc386/tests/debug.exe pid 1 
Warning: the next command will be done localy! 
If you want to restart another remote program, reuse the target command 
(gdb) quit
@end example
