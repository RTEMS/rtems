@c
@c  RTEMS Remote Debugger Server Specifications
@c
@c  Written by: Eric Valette <valette@crf.canon.fr>
@c              Emmanuel Raguet <raguet@crf.canon.fr>
@c
@c
@c  $Id$
@c

@chapter A Rapid Tour of GDB Internals

To help the reader to understand what needs to be implemented, we
will present briefly how GDB works regardless if the target is local or remote.
A debugger is a tool which enables control of the execution of software on a
target system. In most of cases, the debugger connects to a target system, attaches
a process, inserts breakpoints and resumes execution. Then the normal execution
is completely events driven (process execution stopped due to a breakpoint,
process fault, single-step,...) coming from the debuggee. It can also directly
access some parts of the target processor context (registers, data memory, code
memory,...) and change their content. Native GDB debugger can just be seen as
special cases where the host and the target are on the same machine and GDB
can directly access the target system debug API.


In our case, the host and the target are not on the same machine and
an Ethernet link is used to communicate between the different machines. Because
GDB needs to be able to support various targets (including Unix core file, ...),
each action that needs to be performed on the debuggee is materialized by a
field of the following @emph{targets_op}s structure : 

@example
struct target_ops
@{
  char         *to_shortname;   /* Name this target type */
  char         *to_longname;    /* Name for printing */
  char         *to_doc;    /* Documentation.  Does not include trailing
                              newline, and starts with a one-line
                              description (probably similar to
                              to_longname). */
  void        (*to_open) PARAMS ((char *, int));
  void        (*to_close) PARAMS ((int));
  void        (*to_attach) PARAMS ((char *, int));
  void        (*to_detach) PARAMS ((char *, int));
  void        (*to_resume) PARAMS ((int, int, enum target_signal));
  int         (*to_wait) PARAMS ((int, struct target_waitstatus *));
  void        (*to_fetch_registers) PARAMS ((int));
  void        (*to_store_registers) PARAMS ((int));
  void        (*to_prepare_to_store) PARAMS ((void));

  /* Transfer LEN bytes of memory between GDB address MYADDR and
     target address MEMADDR.  If WRITE, transfer them to the target,
     else transfer them from the target.  TARGET is the target from
     which we get this function.

     Return value, N, is one of the following:

     0 means that we can't handle this.  If errno has been set,
     it is the error which prevented us from doing it (FIXME:
     What about bfd_error?).

     positive (call it N) means that we have transferred N bytes
     starting at MEMADDR.  We might be able to handle more bytes
     beyond this length, but no promises.
 
     negative (call its absolute value N) means that we cannot
     transfer right at MEMADDR, but we could transfer at least
     something at MEMADDR + N.  */

  int         (*to_xfer_memory)
                 PARAMS ((CORE_ADDR memaddr, char *myaddr,
                          int len, int write,
                          struct target_ops * target));

  void        (*to_files_info) PARAMS ((struct target_ops *));
  int         (*to_insert_breakpoint) PARAMS ((CORE_ADDR, char *));
  int         (*to_remove_breakpoint) PARAMS ((CORE_ADDR, char *));
  void        (*to_terminal_init) PARAMS ((void));
  void        (*to_terminal_inferior) PARAMS ((void));
  void        (*to_terminal_ours_for_output) PARAMS ((void));
  void        (*to_terminal_ours) PARAMS ((void));
  void        (*to_terminal_info) PARAMS ((char *, int));
  void        (*to_kill) PARAMS ((void));
  void        (*to_load) PARAMS ((char *, int));
  int         (*to_lookup_symbol) PARAMS ((char *, CORE_ADDR *));
  void        (*to_create_inferior) PARAMS ((char *, char *, char **));
  void        (*to_mourn_inferior) PARAMS ((void));
  int         (*to_can_run) PARAMS ((void));
  void        (*to_notice_signals) PARAMS ((int pid));
  int         (*to_thread_alive) PARAMS ((int pid));
  void        (*to_stop) PARAMS ((void));
  enum strata   to_stratum;
  struct target_ops
                *DONT_USE;      /* formerly to_next */
  int           to_has_all_memory;
  int           to_has_memory;
  int           to_has_stack;
  int           to_has_registers;
  int           to_has_execution;
  struct section_table
               *to_sections;
  struct section_table
               *to_sections_end;
  int           to_magic;
  /* Need sub-structure for target machine related rather than comm related? */
@};
@end example

This structure contains pointers to functions (in C++, this would
be called a virtual class). Each different target supported by GDB has its own
structure with the relevant implementation of the functions (some functions
may be not implemented). When a user connects GDB to a target via the ``target''
command, GDB points to the structure corresponding to this target. Then the
user can attache GDB to a specific task via the ``attach'' command. We have
therefore identified two steps to begin a remote debug session :

@enumerate
@item the choice of the target type (in our case RTEMS),
@item the choice of what to debug (entire system, specific task,...),
@end enumerate
Note that in the case of natives debugger, the choice of the target is implicitly
performed by commands like @b{run}, @b{attach}, @b{detach}. Several
figures will now be described showing the main steps of a debug session. 

@c XXX figure reference
Figure @b{Debug session initialization} explains how the debugger connects to the target
:

@enumerate
@item  The debugger opens a connection to the target. The word ``connection''
doesn't only mean Ethernet or serial link connection but all the ways by which
a process can communicate with another one (direct function call, messages mailbox,
...),
@item  The targets checks if it can accept or reject this connection,
@item  If the connection is accepted, the host ``attaches'' the process,
@item  the target stops the process, notifies a child's stop to the host
and waits for command,
@item  the host can ask information about the debugged process (name, registers,...)
or perform some action like setting breakpoints, ...
@end enumerate

@c XXX figure reference
Figure @b{Breakpoint and process execution} explains how the debugger manages the
breakpoints and controls the execution of a process :

@enumerate
@item  The host asks the debuggee what is the opcode at the concerned address
in order for GDB to memorize this instruction,
@item  the host sends a CONTINUE command : it asks the target to write the
``DEBUG'' opcode (for example, the INTEL ``DEBUG'' opcode is INT3 which
generate a breakpoint trap) instead of the debugged opcode.
@item  then the host waits for events,
@item  after the change of instruction, the target resumes the execution
of the debuggee,
@item  when the ``DEBUG'' opcode is executed, the breakpoint exception
handler is executed and it notifies the host that the process is stopped. Then
it waits for commands (if no command is sent after a certain amount of time,
the connection will be closed by the target).
@item  the host asks the target to re-write the right opcode instead of the
''DEBUG'' opcode and then can ask information
@end enumerate

@c XXX figure reference
Figure @b{Breakpoint and process execution} also shows the case of other ``CONTINUE''
commands (remember that the ``DEBUG'' opcode has been replaced by the right
instruction): 

@enumerate
@item  Host sends first a ``single step'' command to execute the debugged
instruction,
@item  It then waits for ``single step`` exception event,
@item  the target, once the single step executed, calls the debug exception
handler. It notifies the host that execution is suspended and wait for commands.
@item  the host asks the target to re-write the ``DEBUG'' opcode (breakpoint
trap) instead of the debugged one.
@item  then the host sends a ``CONTINUE'' command in order the target to
resume the process execution to the next breakpoint.
@end enumerate

@c XXX figure reference
Figure @b{Detach a process and close a connection} explains how the debugger disconnects from
a target :

@enumerate
@item  the host sends a detach command to the target.
@item  the target detaches the concerned process, notifies the detachment
and resumes the process execution.
@item  once notified, the host sends a close connection command.
@item  the target closes the connection.
@end enumerate
These 3 examples show that the mains actions that are performed by
the host debugger on the target are only simple actions which look like :

@itemize @bullet
@item read/write code,
@item read/write data,
@item read/write registers,
@item manage exceptions,
@item send/receive messages to/from the host. 
@end itemize


@c
@c Debug session initialization Figure
@c

@ifset use-ascii
@example
@group
XXXXX reference it in the previous paragraph
XXXXX insert seqinit.eps
XXXXX Caption Debug session initialization
@end group
@end example
@end ifset

@ifset use-tex
@example
@group
@c XXXXX reference it in the previous paragraph
@c XXXXX insert seqinit.eps
@c XXXXX Caption Debug session initialization
@end group
@end example
@page
@image{seqinit}
@end ifset


@ifset use-html
@c <IMG SRC="seqinit.jpg" WIDTH=500 HEIGHT=600 ALT="Debug session initialization">
@html
<IMG SRC="seqinit.jpg" ALT="Debug session initialization">
@end html
@end ifset


@c
@c Breakpoint and process execution Figure
@c

@ifset use-ascii
@example
@group
XXXXX reference it in the previous paragraph
XXXXX insert seqbreak.eps
XXXXX Caption Breakpoint and process execution
@end group
@end example
@end ifset

@ifset use-tex
@example
@group
@c XXXXX reference it in the previous paragraph
@c XXXXX insert seqbreak.eps
@c XXXXX Caption Breakpoint and process execution
@end group
@end example
@page
@sp 5
@image{seqbreak,,9in}
@end ifset

@ifset use-html
@c <IMG SRC="seqbreak.jpg" WIDTH=500 HEIGHT=600 ALT="Breakpoint and process execution">
@html
<IMG SRC="seqbreak.jpg" ALT="Breakpoint and process execution">
@end html
@end ifset



@c
@c Detach a process and close a connection Figure
@c

@ifset use-ascii
@example
@group
XXXXX reference it in the previous paragraph
XXXXX insert seqdetach.eps
XXXXX Caption Detach a process and close a connection
@end group
@end example
@end ifset

@ifset use-tex
@example
@group
@c XXXXX reference it in the previous paragraph
@c XXXXX insert seqdetach.eps
@c XXXXX Caption Detach a process and close a connection
@end group
@end example
@sp 10
@image{seqdetach}
@end ifset

@ifset use-html
@c <IMG SRC="seqdetach.jpg" WIDTH=500 HEIGHT=600 ALT="Detach a process and close a connection">
@html
<IMG SRC="seqdetach.jpg" ALT="Detach a process and close a connection">
@end html
@end ifset




