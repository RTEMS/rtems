@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter Interfacing GDB with RTEMS as a Target

@C XXX figure reference
So, basically, porting GDB to RTEMS environment requires implementing
the functions contained in the target_ops structure. The native debugger implementation
(where the host machine is also the target one) uses direct function calls.
For our needs (remote debugging), these functions must be implemented to support
the encapsulation in UDP/IP layers and communications between different types
of host machines : the best solution is use the SUN Remote Procedure Calls API
(SUN RPC). This SUN RPC module will be explained below (see paragraph @b{Communication with GDB}).

We can note that the functions described in the target_ops structure
are high-level functions. The main reason is that GDB was designed in order
to be able to use monitor firmware as a debug server. In the case of a Unix
OS target, these high-level functions are implemented themselves using a lower
level POSIX API. Because we want to simplify the code running on the target
and decrease its size of this code, we propose to use the POSIX layer API used
for the debug like @b{waitpid}, @b{ptrace}, ... Due to the GDB working
mode and due to our requirements, we can establish here a non-exhaustive list
of some commands required to implement the previously described functions :

@itemize @b
@item set up a connection with a target,
@item close a connection,
@item send a signal to the specified process,
@item get a list of process/thread/connection running,
@item control process under debug,
@item ...
@end itemize
Control means that, with this function, we can read, write the memory
of the debuggee, insert breakpoint to stop the process and resume the process
execution. This command can be implemented by emulating in the RTEMS environment
a Unix function called ``ptrace''. This function allows the control of a child
process. The ``ptrace'' function has some sub-functions which are described
below (some of these actions and standardized, the others are added due to our
needs) :

@itemize @b
@item PTRACE_PEEKTEXT, PTRACE_PEEKDATA : read word at address,
@item PTRACE_POKETEXT, PTRACE_POKEDATA :write word at address,
@item PTRACE_CONT : restart after signal,
@item PTRACE_KILL : send the child a SIGKILL to make it exit,
@item PTRACE_SINGLESTEP : set the trap flag for single stepping,
@item PTRACE_ATTACH : attach to the process specified,
@item PTRACE_DETACH : detach a process that was previously attached.
@end itemize
This list only contains the command that are described in the ptrace
Unix manpage. For some specific needs (debug of one task among several ones,
register read/write,...), it is possible to create some special ptrace commands
as described after :

@itemize @b
@item get current task registers,
@item set current task registers,
@item list of the threads,
@item identifier of the target thread,
@item restart to address,
@item set breakpoint at address,
@item clear breakpoint,
@item get breakpoints,
@item load dynamically a task,
@item ...
@end itemize
This list is not exhaustive and can be increased due to the needs.
All the described functions will not be implemented in a first version, only
the strictly needed. If some commands are added, the modifications must be implemented
both in RTEMS and in GDB.


