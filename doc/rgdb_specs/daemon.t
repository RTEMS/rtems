@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter RTEMS Debugger Server Daemon

We will describe in this section how this debugger server will be
implemented on RTEMS environment. Our initial target is based on Intel Pentium
and we will use an Ethernet link to communicate between the host and the target.

The RTEMS remote debugger will be composed by several tasks and exception
handlers :

@itemize @bullet
@item an initialization task which opens the sockets and runs the SUN RPC
server. This task will also connect the interrupt handlers and launch the communication
task
@item a communication task which receives the SUN RPC commands, executes
them and sends the result to the GDB client,
@item A debuggee event management task which waits for events. We need a different
task than the command management task in order to be able to still accept commands
while no event has yet occurred for the debuggee. An example could be a continue
command from GDB and then hitting to DEL key to see what is currently going
on on the target side because an expected breakpoint is not caught...
@item a debug exception handler which manages the hardware breakpoint and
single step exceptions (INT 1 on Intel x86),
@item a breakpoint exception handler which manages the software breakpoints
exceptions (INT 3 on Intel x86),
@item a default exception handler used to catch every possible errors make on the
target system,
@end itemize

@c XXX figure reference
@c XXX references to other sections
Figure @b{remote debugger tasks and handlers} represents these
different tasks and handlers. The synchronization between the different task
and exception handlers will be described below in the section
@b{Synchronization Among Tasks and Exception Handlers}.
Some open issues we have faced for a prototype implementation are described
in the section @b{Open Issues}. The temporary workaround we chose are described
in chapter @b{Workarounds for Open Issues in Prototype}.


@section The INITIALIZATION task

This is the task that must be executed at the boot phase of RTEMS.
It initializes the debug context. It must :

@itemize @bullet
@item open the UDP sockets,
@item run the SUN RPC server main loop,
@item create the COMMAND MANAGEMENT task,
@item connect the DEBUG EXCEPTION handler,
@item connect the SOFTWARE BREAKPOINT handler,
@item delete itself.
@end itemize
If an error occurs at any step of the execution, the connections established
before the error will be closed, before the initialization task deletes itself.


@section The COMMAND_MNGT task

This task is in charge of receiving the SUN RPC messages and executing
the associated commands. This task must have an important priority because it
must be executed each time a command message comes from the debugger. It must
be executed even if one or both exception handlers are executed. But the COMMAND
MANAGEMENT task must not block the TCP/IP module without which no message can
be received.

When not executing a command, this task is waiting for a SUN RPC message
on the primary port. This idle state blocks the task, so the other active tasks
can run. Once a message comes from Ethernet via the primary port, the COMMAND
MANAGEMENT task wakes up and receives the message which is a request from GDB.
This request is sent to the SUN RPC server code which extracts the command and
its arguments, executes it and, if needed, sends a result to GDB. After having
performed these actions, the task sleeps, waiting for another message. 

A particular case is the reception of the ATTACH command : in this
case the COMMAND_MNGT task creates the EVENT_MNGT task described below before
going to wait on UDP socket again.


@section The EVENT_MNGT task

This task is in charge of managing events happening on the debuggee such as
breakpoint, exceptions. This task does a basic simple loop waiting for event
on a synchronization variable. It is waken up by exception handlers code. It
then signals GDB that an event occurred and then go sleeping again as further
requests will be processed by the COMMAND_MNGT task.


@section The DEBUG EXCEPTION handler

This handler is connected to the DEBUG exception (INT 1 on Intel ix86).
This exception is entered when :

@itemize @bullet
@item executing a single-step instruction,
@item hardware breakpoint condition is true,
@end itemize
These events will be treated by the debugger because they are the
primary event used when debugging a software for instruction stepping. In both
cases, the DEBUG EXCEPTION handler code is executed. Please note that the execution
context of the exception handler is the supervisor stack of the task that generated
the exception. This implies:

@itemize @bullet
@item We may sleep in this context,
@item We have as many possible execution context for the DEBUG EXCEPTION handler as
we need to,
@item When we enter the high level exception handler code, a normalized exception
context has been pushed on the system stack and a pointer to this context is
available as the first argument (cf cpukit/score/cpu/i386/cpu.c for more
details),
@end itemize
First the exception handler wakeup the EVENT_MNGT task. Then it will
cause the faulting thread to sleep on a synchronization object. As soon as GDB
receives the event notifying that the debuggee status has changed, it will start
sending requests to get the debuggee status (registers set, faulty task id,
...). These requests are handled by the COMMAND MANAGEMENT task. When this task
receive a PTRACE_CONT command it will resume the execution of the task that
caused the exception by doing a V on the synchronization object. 


@section The BREAKPOINT EXCEPTION handler

This handler is connected to the BREAKPOINT exception (INT3 on Intel
Ix86). Each time the debugger wants to place a software breakpoint in the debuggee,
a debuggee opcode is temporarily replaced by an instruction causing BREAKPOINT
exception (the ``INT 3'' instruction on Intel ix86). When ``INT 3'' is executed,
the BREAKPOINT handler is executed. Otherwise, the exception processing is the
same than the one described in previous section.


@section Synchronization Among Tasks and Exception Handlers

The previous chapters have presented a simplified and static view of the various
tasks and exceptions handlers. This chapter is more focussed on synchronization
requirements about the various pieces of code executed when RGDBSD is operating.


@subsection Implicit Synchronization Using Task Priorities

This chapter is relevant on Uniprocessor System (UP) only. However, it will
also list the requirements for explicit synchronization on Multi-processor Systems
(MP). Below are the task priorities sorted by high priority. They are not supposed
to be equal :

@enumerate
@item Network Input Task. This is the highest priority task. This can be regarded
as a software interrupt task for FreeBSD code,
@item RGDBSD command task. As this task waits on UDP sockets, it shall not prevent
the previous task from running. As the main debug entry point, it should preempt
any other task in the system,
@item RGDBSD event task. This task should preempt any task but the two mentionned
before to signal a debug event to GDB. The command task shall be able to preempt
this task for emergency command such as DEL, or REBOOT,
@item Applications tasks (task we are able to debug),
@end enumerate

Using theses priorities eliminates the need for adding more synchronization
objects in the next section. My belief is that symmetric MP support will require
more important change in the RTEMS than RGDBSD itself like multiple scheduler
queues, task to processor binding for non symmetric IO, use a different implementation
for @emph{task_disable_preemption}, ...


@subsection Explicit Synchronization

This chapter will describe the synchronization variables that need to be implemented
in order to sequence debug events in a way that is compatible with what GDB
code expects. The root of the problem is that GDB code mainly expects that once
a debug event has occurred on the debuggee, the entire debuggee is frozen and
no other event will occur before the CONTINUE command is issued. This behavior
is hard to achieve in our case as once we hit a breakpoint, only the task that
hits the breakpoint will be asleep on a synchronization object. Other tasks
may hit other breakpoints while we are waiting commands from GDB generating
potential unexpected events. There is a solutions if RGDBSD itself use RTEMS
threads to fix this problem by creating a task that loops forever at a priority
superior to any debugged task but below RGDBSD task priorities. Unfortunately
this will not work for the case we use the nano-kernel implementation and we
think it is better to study synchronization problems now. We also expects that
multi-thread debug support hardening in GDB will remove some event serializations
requirements. Here is the list of synchronization variables we plan to use and
their usage. They are all regular semaphores. They are not binary semaphores
because the task that does V is not the task that has done the P.

@itemize @bullet
@item @emph{WakeUpEventTask} : used by exception handler code to wake up the EVENT_MNGT
task by doing a V operation on this object. When target code is running normally
the EVENT_MNGT task sleeps due to a P operation on this semaphore,
@item @emph{SerializeDebugEvent} : used to serialize events in a way compatible to
what GDB expects. Before doing a V operation on @emph{WakeUpEventTask}, the
exception handler does a P on this semaphore to be sure processing of another
exception is not in progress. Upon reception of a CONTINUE command, the COMMAND_MNGT
task will issue a V operation so that the exception code can wake up EVENT_MNGT
task using the mechanism described above,
@item @emph{RestartFromException} : (in fact one semaphore per task) used by exception
handling code to put a faulty task to sleep once it has generated an exception
by doing a P operation on this semaphore. In the case the exception was generated
due to a breakpoint, GDB command will modify back the BREAKPOINT opcode to the
original value before doing the CONTINUE command. This command will perform
a V on this semaphore. In the case it is a real non restartable exception (faulty
memory reference via invalid pointer for example), GDB will not allow to restart
the program avoiding any loop. So not special analysis of cause of exception
is foreseen as far as RGDBSD code is concerned,
@end itemize

@section Open Issues

Here are some problems we have faced while implementing our prototype :

@table @b
@item [Protected ReadMem/WriteMem (I1)]:
A GDB user can request to see the content
of a corrupted pointer. The request PEEK_DATA will be performed by the COMMAND_MNGT
task. It shall not enter the default exception handler set by RGDBSD or it will
cause a dead lock in the RGDBSD code. Replacing the default exception vector
before calling @b{readMem/writeMem} can be temporarily sufficient but :

@itemize @bullet
@item It will never work on MP system as it will rely on task priorities to insure
that other task will not cause exceptions while we have removed the default
exception handler,

@item This feature should not be usable in RGDBSD only but also by an embedded debugger
that may run without any task. It is also unavoidable in case of protected memory
and in this case no priority mechanism can be used,

@item In the case of using RGDBSD code on a dedicated nano kernel, this code will
be called from interrupt level and we need a way to be sure we can debug other
interrupts that may also cause exceptions,
@end itemize

@item [ATTACH Command Implementation (I2)]:
After the @emph{target rtems symbolic_ip_target_name}
command, the normal operation is to issue an @emph{attach lid} command where
@emph{lid} represents a valid execution context. For Unix this is a process
id, for other multi-tasking system this is the id of a thread. After the attach
command, GDB expects to be waken up in the same manner as it is for normal events.
Once waken up it expects to have a complete register context available and also
that the target task is in a stopped state and that it can restart it using
the regular CONTINUE command. In RTEMS there is a way to get force a thread
to become inactive via @emph{rtems_task_suspend} but no way to get the full
registers set for the thread. A partial context can be retrieved from the task
@emph{Registers} data structure. On the other hand, relying on @emph{rtems_task_suspend}
will be a problem for the nano-kernel implementation.

@item [Stopping Target System (I3)]:
Allthough it might not be obvious, most of the
actions made by a GDB user assume the target is not running. If you modify a
variable via the @emph{set variable = value} command you expect that the value
is the one you have put when restarting. If a still running task modifies the
same value in the mean time, this may be false. On the other hand, stopping
all the tasks on the target system impose to have a very deep knowledge of the
system. Using an interrupt driven RGDBSD, may facilitate the implementation
on the nano-kernel. 

@item [Getting Tasks Contexts (I4)]:
As previously mentionned there is no way to get
tasks execution contexts via the RTEMS API. This is needed when debugging for
example via this classical sequence :

@enumerate

@item @emph{(gdb) target rtems symbolic_ip_target_name}

@item @emph{(gdb) info threads <=} get a thread list on screen

@item @emph{(gdb)} @emph{attach thread_id} <= thread_id is one of the thread in
the list

@item @emph{(gdb) b a_function_of_interest }

@item @emph{(gdb) continue}

@item @emph{(gdb)} @emph{backtrace} <= print the call stack on the screen once we
have hit the breakpoint

@item @emph{(gdb) thread target another_thread_li <=} change implicit current thread
value for gdb commands

@item @emph{(gdb)} @emph{backtrace <=} should print the backtrace for the chosen thread
@end enumerate
In our execution model, we have a valid context only for the threads that hits
the breakpoint as it has been pushed by the exception handler code. The other
thread is still running and during the various RPC requesting memory access,
it even changes as the COMMAND_MNGT thread is going to sleep. So the backtrace
command will fail. We must find a way to make this work as it is very usefull
when debugging multi-threaded programs,


@item [Backtrace Stop convention (I5)]:
The backtrace command on RTEMS task does not
gracefully terminate as GDB does not find some backtrace termination condition
it expects.
@end table

@section Workarounds for Open Issues in Prototype

@table @b

@item [(I1)]:
Not implemented.We would rather like to work on the formalization of
per thread flags and global flags that are much more general than any kludge
we could implement,

@item [(I2)]:
We have tried two solutions in our prototype. The first one was to use
the @emph{idle} thread context contained in the @emph{Registers} task control
block field. The drawback of this solution was that we had to implement specific
code for the continue operation immediately following the attach command. We
then decided to create a dedicated task that will only exist during the attach
phase. This task will call the ``ENTER_RGDB'' exception. This call will execute
the Exception Handler that saves a valid context and that notifies a change
to GDB. After the first CONTINUE command from GDB, this task will continue its
execution and delete itself,

@item [(I3)]:
As explained above in the synchronization chapter, we choose to serialize
events in a way that makes GDB think the system is frozen,

@item [(I4)]:
As a temporary fix, we have called @emph{rtems_task_suspend} and used
the context switch contex for tasks that are unknown to RGDBSD,

@item [(I5)]:
Not Implemented yet. If I remember correctly, setting the frame pointer
to 0 at task initialization for CISC processor solves this problem (ebp = 0x0
on Intel or a6 = 0x0 on 680x0). This should be done in rtems_task_create function
in the path to really starts the task for the first time. The processor/system
specific stop condition can be found as macros in the GDB source tree.
@end table

@section Output of a Debug Session with the Prototype

This is a sample session with the remote debugging prototype.  Note that
some lines have been broken so they would print properly when printed.

@example
GNU gdb 4.17
Copyright 1998 Free Software Foundation, Inc.
GDB is free software, covered by the GNU General Public License,
and you are welcome to change it and/or distribute copies of it
under certain conditions.  Type "show copying" to see the conditions.
There is absolutely no warranty for GDB.
Type "show warranty" for details.
This GDB was configured as --host=i686-pc-linux-gnu --target=i386-rtems.
Attaching remote machine across net...
Connected to net-test. 
Now the "run" command will start a remote process.
Setting up the environment for debugging gdb. 
(gdb) attach 1 
Attaching program: /build-rtems/pc386/tests/debug.exe pid 1 
0x230715 in enterRdbg () 
(gdb) info threads 
There are 8 threads: 
Id.       Name   Detached   Suspended
134283273 Rini   No         No <= current target thread 
0x230715 in enterRdbg () 
134283272 Evnt   No         No 
_Thread_Dispatch () at /rtems/cpukit/score/src/thread.c:315 
134283271 SPE2   No         No 
_Thread_Dispatch () at /rtems/cpukit/score/src/thread.c:315 
134283270 SPE1   No         No 
_Thread_Handler  () at /rtems/cpukit/score/src/thread.c:1107 
134283269 RDBG   No         No 
0x230715 in enterRdbg () 
134283268 SCrx   No         No 
_Thread_Dispatch () at /rtems/cpukit/score/src/thread.c:315 
134283267 SCtx   No         No 
_Thread_Dispatch () at /rtems/cpukit/score/src/thread.c:315 
134283266 ntwk   No         No 
_Thread_Dispatch () at /rtems/cpukit/score/src/thread.c:315 
(gdb) b init.c:89 
Breakpoint 1 at 0x200180: file \
    /rtems/c/src/tests/samples/debug/init.c, line 89.
(gdb) c 
Continuing. 
Thread 134283273 (Rini) has been deleted. 
[Switching to Rtems thread 134283271 (Not suspended) \
    ( <= current target thread )]
Breakpoint 1, example2 (argument=4) at \
    /rtems/c/src/tests/samples/debug/init.c:89 
89          tuto += tuti; 
(gdb) s 
90          if (print_enable2) 
(gdb) c 
Continuing.
Breakpoint 1, example2 (argument=4) at \
    /rtems/c/src/tests/samples/debug/init.c:89 
89          tuto += tuti; 
(gdb) b init.c:66 
Breakpoint 2 at 0x200128: file \
    /rtems/c/src/tests/samples/debug/init.c, line 66.
(gdb) c
Continuing. 
Switching to Rtems thread 134283270 (Not suspended) \
    ( <= current target thread )]
Breakpoint 2, example1 (argument=4) at \
    /rtems/c/src/tests/samples/debug/init.c:66 
66          toto += titi; 
(gdb) c 
Continuing. 
[Switching to Rtems thread 134283271 (Not suspended) \
    ( <= current target thread )]
Breakpoint 1, example2 (argument=4) at \
    /rtems/c/src/tests/samples/debug/init.c:89 
89          tuto += tuti; 
(gdb) bt 
#0  example2 (argument=4) 
    at /rtems/c/src/tests/samples/debug/init.c:89 
#1  0xf0009bd0 in ?? () 
(gdb) thread target 134283270
thread 134283270 [SPE1], _Thread_Dispatch () at \
    /rtems/cpukit/score/src/thread.c:315 
315         executing = _Thread_Executing; 
(gdb) c 
Continuing.
Breakpoint 2, example1 (argument=4) at \
    /rtems/c/src/tests/samples/debug/init.c:66 
66          toto += titi; 
(gdb) detach 
Detaching program: /build-rtems/pc386/tests/debug.exe pid 1 
Warning: the next command will be done localy! \
    If you want to restart another remote 
program, reuse the target command 
(gdb) 
@end example


