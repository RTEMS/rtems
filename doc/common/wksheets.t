@c ******   This comment is here to remind you not to edit the wksheets.t
@c ******   in any directory but common.
@c
@c Figures ...
@c   RTEMS RAM Workspace Worksheet
@c   RTEMS Code Space Worksheet
@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Memory Requirements

@section Introduction

Memory is typically a limited resource in real-time
embedded systems, therefore, RTEMS can be configured  to utilize
the minimum amount of memory while meeting all of the
applications requirements.  Worksheets are provided which allow
the RTEMS application developer to determine the amount of RTEMS
code and RAM workspace which is required by the particular
configuration.  Also provided are the minimum code space,
maximum code space,  and the constant data space required by
RTEMS.

@section Data Space Requirements

RTEMS requires a small amount of memory for its
private variables.  This data area must be in RAM and is
separate from the RTEMS RAM Workspace.  The following
illustrates the data space required for all configurations of
RTEMS:

@itemize @bullet
@item Data Space: RTEMS_DATA_SPACE
@end itemize

@section Minimum and Maximum Code Space Requirements

A maximum configuration of RTEMS includes the core
and all managers, including the multiprocessing manager.
Conversely, a minimum configuration of RTEMS includes only the
core and the following managers: initialization, task, interrupt
and fatal error.  The following illustrates the code space
required by these configurations of RTEMS:

@itemize @bullet
@item Minimum Configuration: RTEMS_MINIMUM_CONFIGURATION
@item Maximum Configuration: RTEMS_MAXIMUM_CONFIGURATION
@end itemize

@section RTEMS Code Space Worksheet

The RTEMS Code Space Worksheet is a tool provided to
aid the RTEMS application designer to accurately calculate the
memory required by the RTEMS run-time environment.  RTEMS allows
the custom configuration of the executive by optionally
excluding managers which are not required by a particular
application.  This worksheet provides the included and excluded
size of each manager in tabular form allowing for the quick
calculation of any custom configuration of RTEMS.  The RTEMS
Code Space Worksheet is below:

@ifset use-ascii
@page
@end ifset
@ifset use-tex
@page
@end ifset

@page
@center @b{RTEMS Code Space Worksheet}
@sp 1 

@ifset use-ascii

The following is a list of the components of the RTEMS code space.  The first
number in parentheses is the size when the component is included,
while the second number indicates its size when not included.  If the second
number is "NA", then the component must always be included.

@itemize @bullet
@item Core (RTEMS_CORE_CODE_SIZE, NA)
@item Initialization (RTEMS_INITIALIZATION_CODE_SIZE, NA)
@item Task (RTEMS_TASK_CODE_SIZE, NA)
@item Interrupt (RTEMS_INTERRUPT_CODE_SIZE, NA)
@item Clock (RTEMS_CLOCK_CODE_SIZE, NA)
@item Timer (RTEMS_TIMER_CODE_SIZE, RTEMS_TIMER_CODE_OPTSIZE)
@item Semaphore (RTEMS_SEMAPHORE_CODE_SIZE, RTEMS_SEMAPHORE_CODE_OPTSIZE)
@item Message (RTEMS_MESSAGE_CODE_SIZE, RTEMS_MESSAGE_CODE_OPTSIZE)
@item Event (RTEMS_EVENT_CODE_SIZE, RTEMS_EVENT_CODE_OPTSIZE)
@item Signal (RTEMS_SIGNAL_CODE_SIZE, RTEMS_SIGNAL_CODE_OPTSIZE)
@item Partition (RTEMS_PARTITION_CODE_SIZE, RTEMS_PARTITION_CODE_OPTSIZE)
@item Region (RTEMS_REGION_CODE_SIZE, RTEMS_REGION_CODE_OPTSIZE)
@item Dual Ported Memory (RTEMS_DPMEM_CODE_SIZE, RTEMS_DPMEM_CODE_OPTSIZE)
@item I/O (RTEMS_IO_CODE_SIZE, RTEMS_IO_CODE_OPTSIZE)
@item Fatal Error (RTEMS_FATAL_ERROR_CODE_SIZE, NA)
@item Rate Monotonic (RTEMS_RATE_MONOTONIC_CODE_SIZE, RTEMS_RATE_MONOTONIC_CODE_OPTSIZE)
@item Multiprocessing (RTEMS_MULTIPROCESSING_CODE_SIZE, RTEMS_MULTIPROCESSING_CODE_OPTSIZE)
@end itemize
@end ifset

@ifset use-tex

@tex
\line{\hskip 0.50in\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 2.25in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.00in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
&\bf Component && \bf Included && \bf Not Included && \bf Size &\cr\noalign{\hrule}
&Core && RTEMS_CORE_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Initialization && RTEMS_INITIALIZATION_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Task && RTEMS_TASK_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Interrupt && RTEMS_INTERRUPT_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Clock && RTEMS_CLOCK_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Timer && RTEMS_TIMER_CODE_SIZE && RTEMS_TIMER_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Semaphore && RTEMS_SEMAPHORE_CODE_SIZE && RTEMS_SEMAPHORE_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Message && RTEMS_MESSAGE_CODE_SIZE && RTEMS_MESSAGE_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Event && RTEMS_EVENT_CODE_SIZE && RTEMS_EVENT_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Signal && RTEMS_SIGNAL_CODE_SIZE && RTEMS_SIGNAL_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Partition && RTEMS_PARTITION_CODE_SIZE && RTEMS_PARTITION_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Region && RTEMS_REGION_CODE_SIZE && RTEMS_REGION_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Dual Ported Memory && RTEMS_DPMEM_CODE_SIZE && RTEMS_DPMEM_CODE_OPTSIZE && &\cr\noalign{\hrule}
&I/O && RTEMS_IO_CODE_SIZE && RTEMS_IO_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Fatal Error && RTEMS_FATAL_ERROR_CODE_SIZE && NA && &\cr\noalign{\hrule}
&Rate Monotonic && RTEMS_RATE_MONOTONIC_CODE_SIZE && RTEMS_RATE_MONOTONIC_CODE_OPTSIZE && &\cr\noalign{\hrule}
&Multiprocessing && RTEMS_MULTIPROCESSING_CODE_SIZE && RTEMS_MULTIPROCESSING_CODE_OPTSIZE && &\cr\noalign{\hrule}
&\multispan 5 \bf\hfil Total Code Space Requirements\qquad\hfil&&&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=4 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Component</STRONG></TD>
    <TD ALIGN=center><STRONG>Included</STRONG></TD>
    <TD ALIGN=center><STRONG>Not Included</STRONG></TD>
    <TD ALIGN=center><STRONG>Size</STRONG></TD></TR>
<TR><TD ALIGN=center>Core</TD>
    <TD ALIGN=center>RTEMS_CORE_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Initialization</TD>
    <TD ALIGN=center>RTEMS_INITIALIZATION_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Task</TD>
    <TD ALIGN=center>RTEMS_TASK_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Interrupt</TD>
    <TD ALIGN=center>RTEMS_INTERRUPT_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Clock</TD>
    <TD ALIGN=center>RTEMS_CLOCK_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Timer</TD>
    <TD ALIGN=center>RTEMS_TIMER_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_TIMER_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Semaphore</TD>
    <TD ALIGN=center>RTEMS_SEMAPHORE_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_SEMAPHORE_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Message</TD>
    <TD ALIGN=center>RTEMS_MESSAGE_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_MESSAGE_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Event</TD>
    <TD ALIGN=center>RTEMS_EVENT_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_EVENT_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Signal</TD>
    <TD ALIGN=center>RTEMS_SIGNAL_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_SIGNAL_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Partition</TD>
    <TD ALIGN=center>RTEMS_PARTITION_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_PARTITION_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Region</TD>
    <TD ALIGN=center>RTEMS_REGION_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_REGION_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Dual Ported Memory</TD>
    <TD ALIGN=center>RTEMS_DPMEM_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_DPMEM_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>I/O</TD>
    <TD ALIGN=center>RTEMS_IO_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_IO_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Fatal Error</TD>
    <TD ALIGN=center>RTEMS_FATAL_ERROR_CODE_SIZE</TD>
    <TD ALIGN=center>NA</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Rate Monotonic</TD>
    <TD ALIGN=center>RTEMS_RATE_MONOTONIC_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_RATE_MONOTONIC_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center>Multiprocessing</TD>
    <TD ALIGN=center>RTEMS_MULTIPROCESSING_CODE_SIZE</TD>
    <TD ALIGN=center>RTEMS_MULTIPROCESSING_CODE_OPTSIZE</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=center COLSPAN=3>
      <STRONG>Total Code Space Requirements</STRONG></TD>
     <TD><BR></TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@page

@c ******   Next node is set by a sed script in the document Makefile.
@c ******   This comment is here to remind you not to edit the wksheets.t
@c ******   in any directory but common.

@section RTEMS RAM Workspace Worksheet

The RTEMS RAM Workspace Worksheet is a tool provided
to aid the RTEMS application designer to accurately calculate
the minimum memory block to be reserved for RTEMS use.  This
worksheet provides equations for calculating the amount of
memory required based upon the number of objects configured,
whether for single or multiple processor versions of the
executive.  This information is presented in tabular form, along
with the fixed system requirements, allowing for quick
calculation of any application defined configuration of RTEMS.
The RTEMS RAM Workspace Worksheet is provided below:

@ifset use-ascii
@page
@end ifset
@ifset use-tex
@sp 2 
@end ifset

@center @b{RTEMS RAM Workspace Worksheet}
@sp 2 

@ifset use-ascii
The total RTEMS RAM Workspace required is the sum of the following:

@itemize @bullet
@item maximum_tasks * RTEMS_BYTES_PER_TASK
@item maximum_timers * RTEMS_BYTES_PER_TIMER
@item maximum_semaphores * RTEMS_BYTES_PER_SEMAPHORE
@item maximum_message_queues * RTEMS_BYTES_PER_MESSAGE_QUEUE
@item maximum_regions * RTEMS_BYTES_PER_REGION
@item maximum_partitions * RTEMS_BYTES_PER_PARTITION
@item maximum_ports * RTEMS_BYTES_PER_PORT
@item maximum_periods * RTEMS_BYTES_PER_PERIOD
@item maximum_extensions * RTEMS_BYTES_PER_EXTENSION
@item Floating Point Tasks * RTEMS_BYTES_PER_FP_TASK
@item Task Stacks
@item maximum_nodes * RTEMS_BYTES_PER_NODE
@item maximum_global_objects * RTEMS_BYTES_PER_GLOBAL_OBJECT
@item maximum_proxies * RTEMS_BYTES_PER_PROXY
@item Fixed System Requirements of RTEMS_BYTES_OF_FIXED_SYSTEM_REQUIREMENTS
@end itemize
@end ifset

@ifset use-tex
@tex
\line{\hskip 0.75in\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 3.0in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
& \bf Description && \bf Equation && \bf Bytes Required &\cr\noalign{\hrule}
& maximum\_tasks &&  * RTEMS_BYTES_PER_TASK = &&&\cr\noalign{\hrule}
& maximum\_timers &&  * RTEMS_BYTES_PER_TIMER = &&&\cr\noalign{\hrule}
& maximum\_semaphores &&  * RTEMS_BYTES_PER_SEMAPHORE = &&&\cr\noalign{\hrule}
& maximum\_message\_queues &&  * RTEMS_BYTES_PER_MESSAGE_QUEUE = &&&\cr\noalign{\hrule}
& maximum\_regions &&  * RTEMS_BYTES_PER_REGION = &&&\cr\noalign{\hrule}
& maximum\_partitions &&  * RTEMS_BYTES_PER_PARTITION = &&&\cr\noalign{\hrule}
& maximum\_ports &&  * RTEMS_BYTES_PER_PORT = &&&\cr\noalign{\hrule}
& maximum\_periods &&  * RTEMS_BYTES_PER_PERIOD = &&&\cr\noalign{\hrule}
& maximum\_extensions &&  * RTEMS_BYTES_PER_EXTENSION = &&&\cr\noalign{\hrule}
& Floating Point Tasks &&  * RTEMS_BYTES_PER_FP_TASK = &&&\cr\noalign{\hrule}
& Task Stacks &&\hskip 2.3em=&&&\cr\noalign{\hrule}
& Total Single Processor Requirements &&&&&\cr\noalign{\hrule}
}}\hfil}

\line{\hskip 0.75in\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 3.0in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
& \bf Description && \bf Equation && \bf Bytes Required &\cr\noalign{\hrule}
& maximum\_nodes &&  * RTEMS_BYTES_PER_NODE = &&&\cr\noalign{\hrule}
& maximum\_global\_objects &&  * RTEMS_BYTES_PER_GLOBAL_OBJECT = &&&\cr\noalign{\hrule}
& maximum\_proxies &&  * RTEMS_BYTES_PER_PROXY = &&&\cr\noalign{\hrule}
}}\hfil}

\line{\hskip 0.75in\vbox{\offinterlineskip\halign{
\vrule\strut#&
\hbox to 3.0in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 0.75in{\enskip\hfil#\hfil}&
\vrule#&
\hbox to 1.25in{\enskip\hfil#\hfil}&
\vrule#\cr
\noalign{\hrule}
& Total Multiprocessing Requirements &&&&&\cr\noalign{\hrule}
& Fixed System Requirements && RTEMS_BYTES_OF_FIXED_SYSTEM_REQUIREMENTS &&&\cr\noalign{\hrule}
& Total Single Processor Requirements &&&&&\cr\noalign{\hrule}
& Total Multiprocessing Requirements &&&&&\cr\noalign{\hrule}
& Minimum Bytes for RTEMS Workspace &&&&&\cr\noalign{\hrule}
}}\hfil}
@end tex
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=3 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=center><STRONG>Description</STRONG></TD>
    <TD ALIGN=center><STRONG>Equation</STRONG></TD>
    <TD ALIGN=center><STRONG>Bytes Required</STRONG></TD></TR>
<TR><TD ALIGN=left>maximum_tasks</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_TASK =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_timers</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_TIMER =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_semaphores</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_SEMAPHORE =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_message_queues</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_MESSAGE_QUEUE =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_regions</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_REGION =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_partitions</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_PARTITION =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_ports</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_PORT =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_periods</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_PERIOD =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_extensions</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_EXTENSION =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>Floating Point Tasks</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_FP_TASK =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left COLSPAN=2>Task Stacks</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left COLSPAN=2>
      <STRONG>Total Single Processor Requirements</STRONG></TD>
     <TD><BR></TD></TR>
<TR></TR>
<TR><TD ALIGN=center><STRONG>Description</STRONG></TD>
    <TD ALIGN=center><STRONG>Equation</STRONG></TD>
    <TD ALIGN=center><STRONG>Bytes Required</STRONG></TD></TR>
<TR><TD ALIGN=left>maximum_nodes</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_NODE =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_global_objects</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_GLOBAL_OBJECT =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left>maximum_proxies</TD>
    <TD ALIGN=right>* RTEMS_BYTES_PER_PROXY =</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left COLSPAN=2>
      <STRONG>Total Multiprocessing Requirements</STRONG></TD>
     <TD><BR></TD></TR>
<TR></TR>
<TR><TD ALIGN=left COLSPAN=2>Fixed System Requirements</TD>
    <TD ALIGN=center>RTEMS_BYTES_OF_FIXED_SYSTEM_REQUIREMENTS</TD></TR>
<TR><TD ALIGN=left COLSPAN=2>Total Single Processor Requirements</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left COLSPAN=2>Total Multiprocessing Requirements</TD>
     <TD><BR></TD></TR>
<TR><TD ALIGN=left COLSPAN=2>
      <STRONG>Minimum Bytes for RTEMS Workspace</STRONG></TD>
     <TD><BR></TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

