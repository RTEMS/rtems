@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@include ../../common/timemac.texi
@tex
\global\advance \smallskipamount by -4pt
@end tex

@chapter ERC32 Timing Data

@section Introduction

The timing data for RTEMS on the ERC32 implementation
of the SPARC architecture is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
SPARC version of RTEMS.

@section Hardware Platform

All times reported in this chapter were measured
using the SPARC Instruction Simulator (SIS) developed by the
European Space Agency.  SIS simulates the ERC32 -- a custom low
power implementation combining the Cypress 90C601 integer unit,
the Cypress 90C602 floating point unit, and a number of
peripherals such as counter timers, interrupt controller and a
memory controller.

For the RTEMS tests, SIS is configured with the
following characteristics:

@itemize @bullet
@item 15 Mhz clock speed

@item 0 wait states for PROM accesses

@item 0 wait states for RAM accesses
@end itemize

The ERC32's General Purpose Timer was used to gather
all timing information.  This timer was programmed to operate
with one microsecond accuracy.  All sources of hardware
interrupts were disabled, although traps were enabled and the
interrupt level of the SPARC allows all interrupts.

@section Interrupt Latency

The maximum period with traps disabled or the
processor interrupt level set to it's highest value inside RTEMS
is less than TBD
microseconds including the instructions which
disable and re-enable interrupts.  The time required for the
ERC32 to vector an interrupt and for the RTEMS entry overhead
before invoking the user's trap handler are a total of 
8
microseconds.  These combine to yield a worst case interrupt
latency of less than TBD +
8 microseconds at 
15.0 Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release 4.2.0-prerelease.]

The maximum period with interrupts disabled within
RTEMS is hand-timed with some assistance from SIS.  The maximum
period with interrupts disabled with RTEMS occurs during a
context switch when traps are disabled to flush all the register
windows to memory.  The length of time spent flushing the
register windows varies based on the number of windows which
must be flushed.  Based on the information reported by SIS, it
takes from 4.0 to 18.0 microseconds (37 to 122 instructions) to
flush the register windows.  It takes approximately 41 CPU
cycles (2.73 microseconds) to flush each register window set to
memory.  The register window flush operation is heavily memory
bound.

[NOTE: All traps are disabled during the register
window flush thus disabling both software generate traps and
external interrupts.  During a normal RTEMS critical section,
the processor interrupt level (pil) is raised to level 15 and
traps are left enabled.  The longest path for a normal critical
section within RTEMS is less than 50 instructions.]

The interrupt vector and entry overhead time was
generated on the SIS benchmark platform using the ERC32's
ability to forcibly generate an arbitrary interrupt as the
source of the "benchmark" interrupt.

@section Context Switch

The RTEMS processor context switch time is 10
microseconds on the SIS benchmark platform when no floating
point context is saved or restored.  Additional execution time
is required when a TASK_SWITCH user extension is configured.
The use of the TASK_SWITCH extension is application dependent.
Thus, its execution time is not considered part of the raw
context switch time.

Since RTEMS was designed specifically for embedded
missile applications which are floating point intensive, the
executive is optimized to avoid unnecessarily saving and
restoring the state of the numeric coprocessor.  The state of
the numeric coprocessor is only saved when an FLOATING_POINT
task is dispatched and that task was not the last task to
utilize the coprocessor.  In a system with only one
FLOATING_POINT task, the state of the numeric coprocessor will
never be saved or restored.  When the first FLOATING_POINT task
is dispatched, RTEMS does not need to save the current state of
the numeric coprocessor.

The following table summarizes the context switch
times for the ERC32 benchmark platform:

@c ******   This comment is here to remind you not to edit the timetbl.t
@c ******   in any directory but common.
@c 
@c Time Table Template
@c
@c  COPYRIGHT (c) 1988-1999.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{No Floating Point Contexts}{21}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{26}
\rtemscase{save initialized, restore initialized}{24}
\rtemscase{save idle, restore initialized}{23}
\rtemscase{save idle, restore idle}{33}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 21
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 26
@item save initialized, restore initialized: 24
@item save idle, restore initialized: 23
@item save idle, restore idle: 23
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>21</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>26</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>24</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>23</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>33</TD>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Directive Times

This sections is divided into a number of
subsections, each of which contains a table listing the
execution times of that manager's directives.

@page
@section Task Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TASK\_CREATE}{59}
\rtemsonecase{TASK\_IDENT}{163}
\rtemsonecase{TASK\_START}{30}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{64}
\rtemscase{suspended task -- returns to caller}
              {36}
\rtemscase{blocked task -- returns to caller}
              {47}
\rtemscase{ready task -- returns to caller}
              {37}
\rtemscase{suspended task -- preempts caller}
              {77}
\rtemscase{blocked task -- preempts caller}
              {84}
\rtemscase{ready task -- preempts caller}
              {75}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{91}
\rtemscase{suspended task}{47}
\rtemscase{blocked task}{50}
\rtemscase{ready task}{51}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{56}
\rtemscase{returns to caller}{16}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {17}
\rtemscase{task readied -- preempts caller}
              {52}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {10}
\rtemscase{returns to caller}{25}
\rtemscase{preempts caller}{67}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{5}
\rtemscase{no reschedule}{6}
\rtemscase{reschedule -- returns to caller}
              {9}
\rtemscase{reschedule -- preempts caller}
              {42}
\rtemsonecase{TASK\_GET\_NOTE}{10}
\rtemsonecase{TASK\_SET\_NOTE}{10}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {6}
\rtemscase{yield -- preempts caller}
              {49}
\rtemsonecase{TASK\_WAKE\_WHEN}{75}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 59
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 163
@end itemize

@item TASK_START
@itemize -
@item only case: 30
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 64
@item suspended task -- returns to caller: 36
@item blocked task -- returns to caller: 47
@item ready task -- returns to caller: 37
@item suspended task -- preempts caller: 77
@item blocked task -- preempts caller: 84
@item ready task -- preempts caller: 75
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 91
@item suspended task: 47
@item blocked task: 50
@item ready task: 51
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 56
@item returns to caller: 16
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 17
@item task readied -- preempts caller: 52
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 10
@item returns to caller: 25
@item preempts caller: 67
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 5
@item no reschedule: 6
@item reschedule -- returns to caller: 9
@item reschedule -- preempts caller: 42
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 10
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 10
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 6
@item yield -- preempts caller: 49
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 75
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>59</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>163</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>30</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>64</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>36</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>47</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>37</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>77</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>84</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>75</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>91</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>47</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>50</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>51</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>56</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>16</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>17</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>52</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>25</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>5</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>6</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>42</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>10</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>10</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>6</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>49</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>75</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@page
@section Interrupt Manager

It should be noted that the interrupt entry times
include vectoring the interrupt handler.

@ifset use-tex
@sp 1
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsdirective{Interrupt Entry Overhead}
\rtemscase{returns to nested interrupt}{7}
\rtemscase{returns to interrupted task}
              {8}
\rtemscase{returns to preempting task}
              {8}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{5}
\rtemscase{returns to interrupted task}
              {7}
\rtemscase{returns to preempting task}
              {14}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: 7
@item returns to interrupted task: 8
@item returns to preempting task: 8
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: 5
@item returns to interrupted task: 7
@item returns to preempting task: 14
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Entry Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>7</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>8</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>8</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>5</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>7</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>14</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{33}
\rtemsonecase{CLOCK\_GET}{4}
\rtemsonecase{CLOCK\_TICK}{6}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 33
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 4
@end itemize

@item CLOCK_TICK
@itemize -
@item only case: 6
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>33</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>4</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>6</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{11}
\rtemsonecase{TIMER\_IDENT}{159}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{15}
\rtemscase{active}{17}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{21}
\rtemscase{active}{23}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{34}
\rtemscase{active}{34}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{20}
\rtemscase{active}{22}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{10}
\rtemscase{active}{13}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 11
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 159
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 15
@item active: 17
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 21
@item active: 23
@end itemize

@item TIMER_FIRE_WHEN
@itemize -
@item inactive: TIMER_FIRE_WHEN_INACTIVE
@item active: TIMER_FIRE_WHEN_ACTIVE
@end itemize

@item TIMER_RESET
@itemize -
@item inactive: TIMER_RESET_INACTIVE
@item active: TIMER_RESET_ACTIVE
@end itemize

@item TIMER_CANCEL
@itemize -
@item inactive: TIMER_CANCEL_INACTIVE
@item active: TIMER_CANCEL_ACTIVE
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TIMER_CREATE</STRONG></TD>
    <TD ALIGN=center>11</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>159</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>15</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>17</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>21</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>23</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>34</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>34</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>20</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>22</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>13</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@page
@section Semaphore Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SEMAPHORE\_CREATE}{19}
\rtemsonecase{SEMAPHORE\_IDENT}{171}
\rtemsonecase{SEMAPHORE\_DELETE}{19}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{12}
\rtemscase{not available -- NO\_WAIT}
              {12}
\rtemscase{not available -- caller blocks}
              {67}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{14}
\rtemscase{task readied -- returns to caller}
              {23}
\rtemscase{task readied -- preempts caller}
              {57}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 19
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 171
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 19
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 12
@item not available -- NO_WAIT: 12
@item not available -- caller blocks: 67
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 14
@item task readied -- returns to caller: 23
@item task readied -- preempts caller: 57
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>19</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>171</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>19</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>12</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>12</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>14</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>23</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>57</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{114}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{159}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{25}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {36}
\rtemscase{task readied -- returns to caller}
              {38}
\rtemscase{task readied -- preempts caller}
              {76}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{36}
\rtemscase{task readied -- returns to caller}
              {38}
\rtemscase{task readied -- preempts caller}
              {76}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{15}
\rtemscase{task readied -- returns to caller}
              {42}
\rtemscase{task readied -- preempts caller}
              {83}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{30}
\rtemscase{not available -- NO\_WAIT}
              {13}
\rtemscase{not available -- caller blocks}
              {67}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{9}
\rtemscase{messages flushed}{13}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 114
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 159
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 25
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 36
@item task readied -- returns to caller: 38
@item task readied -- preempts caller: 76
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 36
@item task readied -- returns to caller: 38
@item task readied -- preempts caller: 76
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 15
@item task readied -- returns to caller: 42
@item task readied -- preempts caller: 83
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 30
@item not available -- NO_WAIT: 13
@item not available -- caller blocks: 67
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 9
@item messages flushed: 13
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>114</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>159</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>25</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>36</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>38</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>76</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>36</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>38</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>76</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>15</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>42</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>83</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>30</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>13</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>13</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@page
@section Event Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsdirective{EVENT\_SEND}
\rtemscase{no task readied}{9}
\rtemscase{task readied -- returns to caller}
              {22}
\rtemscase{task readied -- preempts caller}
              {58}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{1}
\rtemscase{available}{10}
\rtemscase{not available -- NO\_WAIT}{9}
\rtemscase{not available -- caller blocks}
              {60}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 9
@item task readied -- returns to caller: 22
@item task readied -- preempts caller: 58
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: 1
@item available: 10
@item not available -- NO_WAIT: 9
@item not available -- caller blocks: 60
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no task readied</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>22</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>58</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>1</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>60</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{6}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{14}
\rtemscase{signal to self}{22}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {27}
\rtemscase{returns to preempting task}
              {56}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 6
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 14
@item signal to self: 22
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 27
@item returns to preempting task: 56
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>6</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>14</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>22</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          27</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          56</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{34}
\rtemsonecase{PARTITION\_IDENT}{159}
\rtemsonecase{PARTITION\_DELETE}{14}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{12}
\rtemscase{not available}{10}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {10}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: 34
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 159
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 14
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 12
@item not available: 10
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 16
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>34</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>159</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>14</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>12</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>10</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@page
@section Region Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{REGION\_CREATE}{22}
\rtemsonecase{REGION\_IDENT}{162}
\rtemsonecase{REGION\_DELETE}{14}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{19}
\rtemscase{not available -- NO\_WAIT}
              {19}
\rtemscase{not available -- caller blocks}
              {67}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{17}
\rtemscase{task readied -- returns to caller}
              {44}
\rtemscase{task readied -- preempts caller}
              {77}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 22
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 162
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 14
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 19
@item not available -- NO_WAIT: 19
@item not available -- caller blocks: 67
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 17
@item task readied -- returns to caller: 44
@item task readied -- preempts caller: 77
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>22</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>162</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>14</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>19</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          19</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>17</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          44</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          77</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{14}
\rtemsonecase{PORT\_IDENT}{159}
\rtemsonecase{PORT\_DELETE}{13}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {9}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {9}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 14
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 159
@end itemize

@item PORT_DELETE
@itemize -
@item only case: 13
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: 9
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: 9
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>14</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>159</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>13</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>9</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>9</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{2}
\rtemsonecase{IO\_OPEN}{1}
\rtemsonecase{IO\_CLOSE}{1}
\rtemsonecase{IO\_READ}{1}
\rtemsonecase{IO\_WRITE}{1}
\rtemsonecase{IO\_CONTROL}{1}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: 2
@end itemize

@item IO_OPEN
@itemize -
@item only case: 1
@end itemize

@item IO_CLOSE
@itemize -
@item only case: 1
@end itemize

@item IO_READ
@itemize -
@item only case: 1
@end itemize

@item IO_WRITE
@itemize -
@item only case: 1
@end itemize

@item IO_CONTROL
@itemize -
@item only case: 1
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{12}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{159}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{14}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{19}
\rtemscase{inactive}{16}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {20}
\rtemscase{conclude period -- caller blocks}
              {55}
\rtemscase{obtain status}{9}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 12
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 159
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 14
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 19
@item inactive: 16
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 20
@item conclude period -- caller blocks: 55
@item obtain status: 9
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>12</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>159</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>14</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>19</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>16</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         20</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         55</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>9</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@tex
\global\advance \smallskipamount by 4pt
@end tex

@tex
\global\advance \smallskipamount by 4pt
@end tex
