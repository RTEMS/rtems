@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter Objectives

@C XXX reference
This section is intended to clearly define the current objectives of our work.
First, we will try here to list some ambitious requirements for the debugger
in section @b{List of Requirements}. These requirements will deliberately be much more
ambitious than what we will provide directly ourselves in the hope that the
Internet development model will enable others to implement some features we
rejected for man-power reasons in the first step. We are committed to do the
core work and redistribute it but would appreciate any comment and enhancement.
Then, in section @b{Requirements Analysis} we will analyze each requirement to see
what technical problem must be solved if we want to fullfill it. After this
analysis, we will determine in section @b{Requirements Selection} the requirements we
chose to implement and the ones we will not. We will then clearly identify the
limits of our solution in section @b{Implied Restrictions}.


@subsection List of Requirements

We will identify here possible requirements for the type of debug that may be
provided :

@table @b

@item [(R1)]:
We want to use GDB as the front-end debugger,

@item [(R2)]:
We want to support at least Intel and PowerPC as target processor architecture,

@item [(R3)]:
We want to use the GDB thread debugging interface,

@item [(R4)]:
We want to be able to debug a remote target over a serial line,

@item [(R5)]:
We want to be able to debug a remote target over Ethernet,

@item [(R6)]:
The set of target code path we will be able to debug using RGDBSD must
be clearly identified. It will be called Debug Path Set (@emph{DPS}) in the
remaining of this document,

@item [(R7)]:
@emph{DPS} must include the RTEMS core executive itself,

@item [(R8)]:
@emph{DPS} must include the FreeBSD stack, 

@item [(R9)]:
@emph{DPS} must include anything but the FreeBSD stack and the RTEMS
core executive,

@item [(R10)]:
We want to enable several persons to debug different parts of the code
running on the target,

@item [(R11)]:
As much as possible the system must be frozen during a debug session
so that debugging a particular portion of code does not prevent another part
from functioning,
@end table

@subsection Requirements Analysis

@table @b

@item [(R1)]:
Worth recalling it. It mainly imposes few restrictions on the binary
files type, target processor type as :

@itemize @b

@item the binary format must be understood by GDB (to find debugging information).
Elf, Coff and A.out are the main formats currently supported. Elf/Dwarf 2.0
binary support will be our main target as they are the preferred format for
Intel and PowerPC processors. No change in GDB will be required for other binaries
except may be a new configuration file changing the binary/debug file format,

@item the processor must be supported for disassemble/step instruction command,

@item the target system must be supported. As far as I know RTEMS is not currently
@emph{officially} supported anyway,
@end itemize

@item [(R2)]:
Our primary targets are Intel and PowerPC. We however do not think implementing
RGDBSD for other processors will be a heavy task. It will mainly require :

@enumerate
@item Implementing exception handling for the target processor, 

@item Interfacing the generic part of RGDBSD with the low level exception handling
and make RGDBSD aware of exception used for debugging (usually illegal instruction
or dedicated trap, single step),

@item Making GDB aware of the frame layout pushed on exceptions,

@item Implement the code for data transfer for the exception frame,

@item Implement code to copy data cache back to main memory and invalidate instruction
cache. This is needed in order to be sure opcode modification used to set breakpoint
that use the data cache will be proagated to the instruction cache,
@end enumerate

As soon as we will have completed the first core work a document describing
how to port it to a new processor should be written. So far we will organize
the source tree with processor dependent directories so that port will be as
easy as possible. May be a bare processor support should be created,


@item [(R3)]:
GDB already has an interface for manipulating multi-threaded programs.
This interface is rather weak at the moment but it will certainly be improved
in the future with the generalization of POSIX thread API on Linux and other
operating systems. This implies that either GDB or RGDBSD is able to obtain
the list of threads currently executing. The choice of implementing this in
GDB or RGDBSD is a tradeof between target code size and simplicity,

@item [(R4)]:
Regular GDB code contains clients code for debugging over a serial line.
However only few functions are implemented. We would like to provide a better
support and to uniformize serial line debugging with debugging over Ethernet
via the use of SLIP,

@item [(R5)]:
Regular GDB code contains client code for debugging over Ethernet for
VxWorks via the SUN RPC library. So there will be at least one starting point
to implement remote debugging over Ethernet via SUN RPC. The Chorus remote debugging
code has been disclosed under GPL and also contains code for debugging suing
SUN RPC,

@item [(R6)]:
Due to a classical chicken and egg problems, the remote debugging daemon
cannot be used to debug code it uses to function. Thus depending on the API
used by RGDBSD, some parts of the target system will not be debuggable via GDB.
The most important point is documentation because my feeling is that implementing
RGDBSD on a totally different @emph{dedicated} nano kernel should be possible,

@item [(R7)]:
RTEMS core executive is a real-time OS which implements priority level
scheduling, synchronization objects, and interrupt handling. As mentioned in
previous item, we may not debug theses features if RGDBSD uses them. This requirement
is thus very strong because it impose that :

@enumerate

@item RGDBSD is totally interrupt driven (no thread API available),

@item But it does not use RTEMS interrupt management,

@item Nor does not use RTEMS exception management,

@item RGDBSD must provide its own UDP/IP stack as the current FreeBSD code rely on
tasks switching and RTEMS provided synchronization object for input path handling,
@end enumerate

So our feeling is that the @b{(R7)} more or less requires to write a @emph{dedicated}
nano kernel with a very small dedicated UDP/IP stack.


@item [(R8)]:
GDB remote debugging over Ethernet code communicates with the remote
target via the SUN RPC protocol. This requires a UDP/IP protocol and a minimal
socket like interface. In RTEMS environment, this feature is currently provided
by the FreeBSD stack. Again, if we use the FreeBSD stack itself for remote communication,
it will be impossible to debug this stack as a breakpoint in the stack code
will stop its execution and there would be no more way to communicate with the
target. A solution consists in implementing a minimal, dedicated UDP/IP stack
(with at least IP and UDP protocols, a minimal BSD sockets) and a simple SUN
RPC library, which will be both dedicated to the debug. We can use RTEMS API
to implement it if @b{(R7)} is not required. As the two stack will need
to share the same chip, a kind of shared filter must be implemented at the bottom
of the two stacks so that Ethernet frames can be dynamically directed either
to the dedicated UDP/IP debug stack or to the regular FreeBSD stack. The fact
that in the current design, the low level ethernet input routine mainly signal
a thread should facilitate the design of this filter. The output path is less
complicated as it is run by a task and thus can sleep on a synchronization object,

@item [(R9)]:
This requirement represents what we find reasonable as a first target.
However, we can still present to the final user this kind of debugging via different
model. RTEMS can be represented as a single threaded system or, because RTEMS
is a multitasking system, as an ensemble of separate tasks. In the first representation,
the debugger sees only 1 ``task'' without distinguishing the core executive
part from the applicative part. This is the simplest way to implement the debugger
but also implies that there is no way to protect the core executive. Some of
these tasks are system tasks (tasks form the core executive and from the FreeBSD
stack), the other ones are tasks implemented by the developer. The developer
wants to debug his tasks, and sometimes only one of his tasks. We can provide
a way to debug not the entire system but only the concerned task by testing
if the current running task is a debugged task (test on the task identifier).
GDB offers an API to ``detach'' thread so that if a detached thread hits a
breakpoint it is automatically restarted without user intervention,

@item [(R10)]:
Several developers can work on a large project, each on a specific
module. Sometimes only one target is available for everyone. This requirements
is not really meaningfull until RTEMS supports dynamic code loading,

@item [(R11)]:
This requirement heavily depends on the @b{(R7)} and @b{(R8)}
requirements.
@end table

@subsection Requirements Selection


@subsubsection Requirement We Will Take Into Account For the First Implementation

@table @b

@item [(R1)]:
Obviously.

@item [(R2)]:
As these are our targets. Of course other will be free to contribute.
We will however document the works that needs to be done in order to port the
debug code to other processors,

@item [(R3)]:
We think it is feasible with only few RTEMS modifications,

@item [(R5)]:
We think serial line debugging is nowadays too restrictive as most equipment
are now connected via Ethernet,

@item [(R6)]:
This is a documentation problem and should be fairly easy to describe
once we have the RGDBSD code,

@item [(R9)]:
We will try to provide the multi-thread target system presentation,
@end table

@subsubsection Requirements We Will Not Implement

@table @b

@item [(R4)]:
it will not be implemented for the moment. It is just a matter on implementing
SLIP in the FreeBSD stack and alternative solutions already exist in the meantime,

@item [(R7)]:
To simplify the first developments, we don't plan to implement a @emph{dedicated}
nano-kernel to allow the RTEMS kernel to be debugged. It means that, if any
breakpoint is set in the kernel, unpredictable behaviors can occur. So, developers
must keep in mind to avoid stopping the kernel. They must also keep in mind,
in order to not stop the kernel, that the user's tasks must have a lower priority
than the tasks used for debug. The solution is to use a specific very-high priority
level for the system tasks used directly or indirectly by RGDBSD. The SYSTEM_TASK
attribute that already exists should be fine.

@item [(R8)]:
To avoid increasing the code size and the used memory and because the
FreeBSD stack doesn't need to be debug any more, we choose not to implement
a minimal TCP/IP stack but rather to use the FreeBSD one as communication protocol,

@item [(R10)]:
We will see later when a file system will be available and we can implement
@b{exec} system call,

@item [(R11)]:
Without a separate TCP/IP stack it will be hard to freeze the system
as some interrupts must occur in order to enable the FreeBSD stack to function,
@end table

@subsection Implied Restrictions

High priority level must be used for these features :

@itemize @b

@item FreeBSD interrupt handling thread,

@item Debugger threads.
@end itemize

This will allows these tasks not to be stopped when a process is stopped
in debug mode 

If we don't want to use a ``specific'' priority level, we must affect
priority to each of these tasks as follow :

@itemize @b

@item FreeBSD stack (high priority)

@item Debugger (less high priority)
@end itemize

The user must remember the higher priority level he can use for his
software in order not to block one of the previous threads and to not put breakpoints
in part of the code executed by RGDBSD.


