@c
@c  COPYRIGHT (c) 1988-2008.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter RTEMS Specific Commands

@section Introduction

The RTEMS shell has the following rtems commands:

@itemize @bullet

@item @code{halt} - Shutdown the system
@item @code{cpuuse} - print or reset per thread cpu usage
@item @code{stackuse} - print per thread stack usage
@item @code{perioduse} - print or reset per period usage
@item @code{wkspace} - Display information on Executive Workspace
@item @code{config} - Show the system configuration.
@item @code{itask} - List init tasks for the system
@item @code{extension} - Display information about extensions
@item @code{task} - Display information about tasks
@item @code{queue} - Display information about message queues
@item @code{sema} - display information about semaphores
@item @code{region} - display information about regions
@item @code{part} - display information about partitions
@item @code{object} - Display information about RTEMS objects
@item @code{driver} - Display the RTEMS device driver table
@item @code{dname} - Displays information about named drivers
@item @code{pthread} - Displays information about POSIX threads

@end itemize

@section Commands

This section details the RTEMS Specific Commands available.  A
subsection is dedicated to each of the commands and
describes the behavior and configuration of that
command as well as providing an example usage.

@c
@c
@c
@page
@subsection halt - Shutdown the system

@pgindex halt

@subheading SYNOPSYS:

@example
halt
@end example

@subheading DESCRIPTION:

This command is used to shutdown the RTEMS application.

@subheading EXIT STATUS:

This command does not return.

@subheading NOTES:


@subheading EXAMPLES:

The following is an example of how to use @code{halt}:

@example
SHLL [/] $ halt
@end example

The user will not see another prompt and the system will 
shutdown.

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_HALT
@findex CONFIGURE_SHELL_COMMAND_HALT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_HALT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_HALT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_halt

The @code{halt} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_halt(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{halt} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_HALT_Command;
@end example

@c
@c
@c
@page
@subsection cpuuse - print or reset per thread cpu usage

@pgindex cpuuse

@subheading SYNOPSYS:

@example
cpuuse [-r]
@end example

@subheading DESCRIPTION:

This command may be used to print a report on the per thread
cpu usage or to reset the per thread CPU usage statistics. When
invoked with the @code{-r} option, the CPU usage statistics
are reset.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

The granularity of the timing information reported is dependent
upon the BSP and the manner in which RTEMS was built.  In the
default RTEMS configuration, if the BSP supports nanosecond
granularity timestamps, then the information reported will be
highly accurate. Otherwise, the accuracy of the information
reported is limited by the clock tick quantum.

@subheading EXAMPLES:

The following is an example of how to use @code{cpuuse}:

@example
SHLL [/] $ cpuuse
CPU Usage by thread
   ID            NAME         SECONDS   PERCENT
0x09010001   IDLE            49.745393   98.953
0x0a010001   UI1              0.000000    0.000
0x0a010002   SHLL             0.525928    1.046
Time since last CPU Usage reset 50.271321 seconds
SHLL [/] $ cpuuse -r
Resetting CPU Usage information
SHLL [/] $ cpuuse
CPU Usage by thread
   ID            NAME         SECONDS   PERCENT
0x09010001   IDLE             0.000000    0.000
0x0a010001   UI1              0.000000    0.000
0x0a010002   SHLL             0.003092  100.000
Time since last CPU Usage reset 0.003092 seconds
@end example

In the above example, the system had set idle for nearly
a minute when the first report was generated.  The 
@code{cpuuse -r} and @code{cpuuse} commands were pasted
from another window so were executed with no gap between.
In the second report, only the @code{shell} thread has
run since the CPU Usage was reset.  It has consumed
approximately 3.092 milliseconds of CPU time processing
the two commands and generating the output.

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CPUUSE
@findex CONFIGURE_SHELL_COMMAND_CPUUSE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CPUUSE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CPUUSE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_cpuuse

The @code{cpuuse} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_cpuuse(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{cpuuse} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CPUUSE_Command;
@end example

@c
@c
@c
@page
@subsection stackuse - print per thread stack usage

@pgindex stackuse

@subheading SYNOPSYS:

@example
stackuse
@end example

@subheading DESCRIPTION:

This command prints a Stack Usage Report for all of the tasks
and threads in the system.  On systems which support it, the
usage of the interrupt stack is also included in the report.

@subheading EXIT STATUS:

This command always succeeds and returns 0.

@subheading NOTES:

The @code{CONFIGURE_STACK_CHECKER_ENABLED} @code{confdefs.h} constant
must be defined when the application is configured for this
command to have any information to report.

@subheading EXAMPLES:

The following is an example of how to use @code{stackuse}:

@smallexample
SHLL [/] $ stackuse
Stack usage by thread
    ID      NAME    LOW          HIGH     CURRENT     AVAILABLE     USED
0x09010001  IDLE 0x023d89a0 - 0x023d99af 0x023d9760      4096        608
0x0a010001  UI1  0x023d9f30 - 0x023daf3f 0x023dad18      4096       1804
0x0a010002  SHLL 0x023db4c0 - 0x023df4cf 0x023de9d0     16384       5116
0xffffffff  INTR 0x023d2760 - 0x023d375f 0x00000000      4080        316
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_STACKUSE
@findex CONFIGURE_SHELL_COMMAND_STACKUSE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_STACKUSE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_STACKUSE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_stackuse

The @code{stackuse} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_stackuse(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{stackuse} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_STACKUSE_Command;
@end example

@c
@c
@c
@page
@subsection perioduse - print or reset per period usage

@pgindex perioduse

@subheading SYNOPSYS:

@example
perioduse [-r]
@end example

@subheading DESCRIPTION:

This command may be used to print a statistics report on the rate
monotonic periods in the application or to reset the rate monotonic
period usage statistics. When invoked with the @code{-r} option, the
usage statistics are reset.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

The granularity of the timing information reported is dependent
upon the BSP and the manner in which RTEMS was built.  In the
default RTEMS configuration, if the BSP supports nanosecond
granularity timestamps, then the information reported will be
highly accurate. Otherwise, the accuracy of the information
reported is limited by the clock tick quantum.

@subheading EXAMPLES:

The following is an example of how to use @code{perioduse}:

@smallexample
SHLL [/] $ perioduse   
Period information by period
--- CPU times are in seconds ---
--- Wall times are in seconds ---
   ID     OWNER COUNT MISSED          CPU TIME                  WALL TIME
                                    MIN/MAX/AVG                MIN/MAX/AVG
0x42010001 TA1    502      0 0:000039/0:042650/0:004158 0:000039/0:020118/0:002848
0x42010002 TA2    502      0 0:000041/0:042657/0:004309 0:000041/0:020116/0:002848
0x42010003 TA3    501      0 0:000041/0:041564/0:003653 0:000041/0:020003/0:002814
0x42010004 TA4    501      0 0:000043/0:044075/0:004911 0:000043/0:020004/0:002814
0x42010005 TA5     10      0 0:000065/0:005413/0:002739 0:000065/1:000457/0:041058

                                    MIN/MAX/AVG                MIN/MAX/AVG
SHLL [/] $ perioduse -r
Resetting Period Usage information
SHLL [/] $ perioduse
--- CPU times are in seconds ---
--- Wall times are in seconds ---
   ID     OWNER COUNT MISSED          CPU TIME                  WALL TIME
                                    MIN/MAX/AVG                MIN/MAX/AVG
0x42010001 TA1      0      0
0x42010002 TA2      0      0
0x42010003 TA3      0      0
0x42010004 TA4      0      0
0x42010005 TA5      0      0
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_PERIODUSE
@findex CONFIGURE_SHELL_COMMAND_PERIODUSE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_PERIODUSE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_PERIODUSE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_perioduse

The @code{perioduse} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_perioduse(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{perioduse} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_PERIODUSE_Command;
@end example

@c
@c
@c
@page
@subsection wkspace - display information on executive workspace

@pgindex wkspace

@subheading SYNOPSYS:

@example
wkspace
@end example

@subheading DESCRIPTION:

This command prints information on the current state of
the RTEMS Executive Workspace reported.  This includes the
following information:

@itemize @bullet
@item Number of free blocks
@item Largest free block
@item Total bytes free
@item Number of used blocks
@item Largest used block
@item Total bytes used
@end itemize

@subheading EXIT STATUS:

This command always succeeds and returns 0.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{wkspace}:

@example
SHLL [/] $ wkspace
Number of free blocks: 1
Largest free block:    132336
Total bytes free:      132336
Number of used blocks: 36
Largest used block:    16408
Total bytes used:      55344
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_WKSPACE
@findex CONFIGURE_SHELL_COMMAND_WKSPACE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_WKSPACE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_WKSPACE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_wkspace

The @code{wkspace} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_wkspace(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{wkspace} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_WKSPACE_Command;
@end example

@c
@c
@c
@page
@subsection config - show the system configuration.

@pgindex config

@subheading SYNOPSYS:

@example
config
@end example

@subheading DESCRIPTION:

This command display information about the RTEMS Configuration.

@subheading EXIT STATUS:

This command always succeeds and returns 0.

@subheading NOTES:

At this time, it does not report every configuration parameter.
This is an area in which user submissions or sponsorship of
a developer would be appreciated.

@subheading EXAMPLES:

The following is an example of how to use @code{config}:

@smallexample
INITIAL (startup) Configuration Info
------------------------------------------------------------------------------
WORKSPACE      start: 0x23d22e0;  size: 0x2dd20
TIME           usec/tick: 10000;  tick/timeslice: 50;  tick/sec: 100
MAXIMUMS       tasks: 20;  timers: 0;  sems: 50;  que's: 20;  ext's: 1
               partitions: 0;  regions: 0;  ports: 0;  periods: 0
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_CONFIG
@findex CONFIGURE_SHELL_COMMAND_CONFIG

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_CONFIG} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_CONFIG} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_config

The @code{config} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_config(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{config} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_CONFIG_Command;
@end example

@c
@c
@c
@page
@subsection itask - list init tasks for the system

@pgindex itask

@subheading SYNOPSYS:

@example
itask
@end example

@subheading DESCRIPTION:

This command prints a report on the set of initialization
tasks and threads in the system.

@subheading EXIT STATUS:

This command always succeeds and returns 0.

@subheading NOTES:

At this time, it includes only Classic API Initialization Tasks.
This is an area in which user submissions or sponsorship of
a developer would be appreciated.

@subheading EXAMPLES:

The following is an example of how to use @code{itask}:

@smallexample
SHLL [/] $ itask
  #    NAME   ENTRY        ARGUMENT    PRIO   MODES  ATTRIBUTES   STACK SIZE
------------------------------------------------------------------------------
   0   UI1    [0x2002258] 0 [0x0]        1    nP      DEFAULT     4096 [0x1000]
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_ITASK
@findex CONFIGURE_SHELL_COMMAND_ITASK

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_ITASK} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_ITASK} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_itask

The @code{itask} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_itask(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{itask} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_ITASK_Command;
@end example

@c
@c
@c
@page
@subsection extension - display information about extensions

@pgindex extension

@subheading SYNOPSYS:

@example
extension [id [id ...] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of User Extensions currently active in the system.

If invoked with a set of ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of using the @code{extension} command
on a system with no user extensions.

@smallexample
SHLL [/] $ extension
  ID       NAME
------------------------------------------------------------------------------
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_EXTENSION
@findex CONFIGURE_SHELL_COMMAND_EXTENSION

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_EXTENSION} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_EXTENSION} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_extension

The @code{extension} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_extension(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{extension} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_EXTENSION_Command;
@end example

@c
@c
@c
@page
@subsection task - display information about tasks

@pgindex task

@subheading SYNOPSYS:

@example
task [id [id ...] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Classic API Tasks currently active in the system.

If invoked with a set of ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use the @code{task} on an
application with just two Classic API tasks:

@smallexample
SHLL [/] $ task
  ID       NAME   PRIO   STAT   MODES  EVENTS   WAITID  WAITARG  NOTES
------------------------------------------------------------------------------
0a010001   UI1      1   SUSP   P:T:nA  NONE                      
0a010002   SHLL   100   READY  P:T:nA  NONE                      
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_TASK
@findex CONFIGURE_SHELL_COMMAND_TASK

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_TASK} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_TASK} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_task

The @code{task} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_task(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{task} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_TASK_Command;
@end example

@c
@c
@c
@page
@subsection queue - display information about message queues

@pgindex queue

@subheading SYNOPSYS:

@example
queue [id [id ... ] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Classic API Message Queues currently active in the system.

If invoked with a set of ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of using the @code{queue} command
on a system with no Classic API Message Queues.

@smallexample
SHLL [/] $ queue
  ID       NAME   ATTRIBUTES   PEND   MAXPEND  MAXSIZE
------------------------------------------------------------------------------
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_QUEUE
@findex CONFIGURE_SHELL_COMMAND_QUEUE

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_QUEUE} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_QUEUE} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_queue

The @code{queue} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_queue(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{queue} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_QUEUE_Command;
@end example

@c
@c
@c
@page
@subsection sema - display information about semaphores

@pgindex sema

@subheading SYNOPSYS:

@example
sema [id [id ... ] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Classic API Semaphores currently active in the system.

If invoked with a set of objects ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{sema}:

@smallexample
SHLL [/] $ sema
  ID       NAME   ATTR        PRICEIL CURR_CNT HOLDID 
------------------------------------------------------------------------------
1a010001   LBIO   PR:BI:IN      0        1     00000000
1a010002   TRmi   PR:BI:IN      0        1     00000000
1a010003   LBI00  PR:BI:IN      0        1     00000000
1a010004   TRia   PR:BI:IN      0        1     00000000
1a010005   TRoa   PR:BI:IN      0        1     00000000
1a010006   TRxa   <assoc.c: BAD NAME>   0    0 09010001
1a010007   LBI01  PR:BI:IN      0        1     00000000
1a010008   LBI02  PR:BI:IN      0        1     00000000
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_SEMA
@findex CONFIGURE_SHELL_COMMAND_SEMA

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_SEMA} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_SEMA} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_sema

The @code{sema} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_sema(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{sema} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_SEMA_Command;
@end example

@c
@c
@c
@page
@subsection region - display information about regions

@pgindex region

@subheading SYNOPSYS:

@example
region [id [id ... ] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Classic API Regions currently active in the system.

If invoked with a set of object ids as arguments, then just 
those object are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of using the @code{region} command
on a system with no user extensions.

@smallexample
SHLL [/] $ region
  ID       NAME   ATTR        STARTADDR LENGTH    PAGE_SIZE USED_BLOCKS
------------------------------------------------------------------------------
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_REGION
@findex CONFIGURE_SHELL_COMMAND_REGION

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_REGION} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_REGION} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_region

The @code{region} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_region(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{region} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_REGION_Command;
@end example

@c
@c
@c
@page
@subsection part - display information about partitions

@pgindex part

@subheading SYNOPSYS:

@example
part [id [id ... ] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Classic API Partitions currently active in the system.

If invoked with a set of object ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of using the @code{part} command
on a system with no user extensions.

@smallexample
SHLL [/] $ part
  ID       NAME   ATTR        STARTADDR LENGTH    BUF_SIZE  USED_BLOCKS
------------------------------------------------------------------------------
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_PART
@findex CONFIGURE_SHELL_COMMAND_PART

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_PART} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_PART} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_part

The @code{part} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_part(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{part} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_PART_Command;
@end example

@c
@c
@c
@page
@subsection object - display information about rtems objects

@pgindex object

@subheading SYNOPSYS:

@example
object [id [id ...] ]
@end example

@subheading DESCRIPTION:

When invoked with a set of object ids as arguments, then 
a report on those objects is printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{object}:

@smallexample
SHLL [/] $ object 0a010001 1a010002
  ID       NAME   PRIO   STAT   MODES  EVENTS   WAITID  WAITARG  NOTES
------------------------------------------------------------------------------
0a010001   UI1      1   SUSP   P:T:nA  NONE                      
  ID       NAME   ATTR        PRICEIL CURR_CNT HOLDID 
------------------------------------------------------------------------------
1a010002   TRmi   PR:BI:IN      0        1     00000000
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_OBJECT
@findex CONFIGURE_SHELL_COMMAND_OBJECT

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_OBJECT} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_OBJECT} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_object

The @code{object} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_object(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{object} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_OBJECT_Command;
@end example

@c
@c
@c
@page
@subsection driver - display the rtems device driver table

@pgindex driver

@subheading SYNOPSYS:

@example
driver [ major [ major ... ] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of Device Drivers currently active in the system.

If invoked with a set of major numbers as arguments, then just 
those Device Drivers are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{driver}:

@smallexample
SHLL [/] $ driver
  Major      Entry points
------------------------------------------------------------------------------
  0          init: [0x200256c];  control: [0x20024c8]
             open: [0x2002518];  close: [0x2002504]
             read: [0x20024f0];  write: [0x20024dc]
  1          init: [0x20023fc];  control: [0x2002448]
             open: [0x0];  close: [0x0]
             read: [0x0];  write: [0x0]
SHLL [/] $ 
@end smallexample

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DRIVER
@findex CONFIGURE_SHELL_COMMAND_DRIVER

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DRIVER} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_DRIVER} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_driver

The @code{driver} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_driver(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{driver} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DRIVER_Command;
@end example

@c
@c
@c
@page
@subsection dname - displays information about named drivers

@pgindex dname

@subheading SYNOPSYS:

@example
dname
@end example

@subheading DESCRIPTION:

This command XXX

WARNING! XXX This command does not appear to work as of 27 February 2008.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

NONE

@subheading EXAMPLES:

The following is an example of how to use @code{dname}:

@example
EXAMPLE_TBD
@end example

@subheading CONFIGURATION:

@findex CONFIGURE_SHELL_NO_COMMAND_DNAME
@findex CONFIGURE_SHELL_COMMAND_DNAME

This command is included in the default shell command set.  
When building a custom command set, define
@code{CONFIGURE_SHELL_COMMAND_DNAME} to have this
command included.

This command can be excluded from the shell command set by
defining @code{CONFIGURE_SHELL_NO_COMMAND_DNAME} when all
shell commands have been configured.

@subheading PROGRAMMING INFORMATION:

@findex rtems_shell_rtems_main_dname

The @code{dname} is implemented by a C language function
which has the following prototype:

@example
int rtems_shell_rtems_main_dname(
  int    argc,
  char **argv
);
@end example

The configuration structure for the @code{dname} has the
following prototype:

@example
extern rtems_shell_cmd_t rtems_shell_DNAME_Command;
@end example

@c
@c
@c
@page
@subsection pthread - display information about POSIX threads

@pgindex pthread

@subheading SYNOPSYS:

@example
pthread [id [id ...] ]
@end example

@subheading DESCRIPTION:

When invoked with no arguments, this command prints information on
the set of POSIX API threads currently active in the system.

If invoked with a set of ids as arguments, then just 
those objects are included in the information printed.

@subheading EXIT STATUS:

This command returns 0 on success and non-zero if an error is encountered.

@subheading NOTES:

This command is only available when the POSIX API is configured.

@subheading EXAMPLES:

The following is an example of how to use the @code{task} on an
application with four POSIX threads:

@smallexample
SHLL [/] $ pthread
  ID       NAME           PRI  STATE MODES   EVENTS    WAITID  WAITARG  NOTES
------------------------------------------------------------------------------
0b010002   Main           133 READY  P:T:nA    NONE   43010001 0x7b1148 
0b010003   ISR            133 Wcvar  P:T:nA    NONE   43010003 0x7b1148 
0b01000c                  133 READY  P:T:nA    NONE   33010002 0x7b1148 
0b01000d                  133 Wmutex P:T:nA    NONE   33010002 0x7b1148 

@end smallexample

@subheading CONFIGURATION:

This command is part of the monitor commands which are always
available in the shell.

@subheading PROGRAMMING INFORMATION:

This command is not directly available for invocation.

