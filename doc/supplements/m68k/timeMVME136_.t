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

@chapter MVME136 Timing Data

@section Introduction

The timing data for the MC68020 version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the interrupt latency and the context switch
times as they pertain to the MC68020 version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Motorola
MVME135 CPU board.  The MVME135 is a 20Mhz board with one wait
state dynamic memory and a MC68881 numeric coprocessor.  The
Zilog 8036 countdown timer on this board was used to measure
elapsed time with a one-half microsecond resolution.  All
sources of hardware interrupts were disabled, although the
interrupt level of the MC68020 allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  The worst case times of the MC68020 microprocessor
were used for each instruction.  Zero wait state memory was
assumed.  The total CPU cycles executed with interrupts
disabled, including the instructions to disable and enable
interrupts, was divided by 20 to simulate a 20Mhz MC68020.  It
should be noted that the worst case instruction times for the
MC68020 assume that the internal cache is disabled and that no
instructions overlap.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than TBD 
microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the MC68020 to vector an interrupt and for the RTEMS entry
overhead before invoking the user's interrupt handler are a
total of 9 
microseconds.  These combine to yield a worst case
interrupt latency of less than 
TBD + 9 
microseconds at 20Mhz.  [NOTE:  The maximum period with interrupts
disabled was last determined for Release 
3.2.1.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed and based upon
worst case (i.e. CPU cache disabled and no instruction overlap)
times for a 20Mhz MC68020.  The interrupt vector and entry
overhead time was generated on an MVME135 benchmark platform
using the Multiprocessing Communications registers to generate
as the interrupt source.

@section Context Switch

The RTEMS processor context switch time is 35
microseconds on the MVME135 benchmark platform when no floating
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

The exact amount of time required to save and restore
floating point context is dependent on whether an MC68881 or
MC68882 is being used as well as the state of the numeric
coprocessor.  These numeric coprocessors define three operating
states: initialized, idle, and busy.  RTEMS places the
coprocessor in the initialized state when a task is started or
restarted.  Once the task has utilized the coprocessor, it is in
the idle state when floating point instructions are not
executing and the busy state when floating point instructions
are executing.  The state of the coprocessor is task specific.

The following table summarizes the context switch
times for the MVME135 benchmark platform:

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
\rtemsonecase{No Floating Point Contexts}{35}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{39}
\rtemscase{save initialized, restore initialized}{66}
\rtemscase{save idle, restore initialized}{66}
\rtemscase{save idle, restore idle}{68}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 35
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 39
@item save initialized, restore initialized: 66
@item save idle, restore initialized: 66
@item save idle, restore idle: 66
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>35</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>39</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>66</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>66</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>68</TD>
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
\rtemsonecase{TASK\_CREATE}{148}
\rtemsonecase{TASK\_IDENT}{350}
\rtemsonecase{TASK\_START}{76}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{95}
\rtemscase{suspended task -- returns to caller}
              {89}
\rtemscase{blocked task -- returns to caller}
              {124}
\rtemscase{ready task -- returns to caller}
              {92}
\rtemscase{suspended task -- preempts caller}
              {125}
\rtemscase{blocked task -- preempts caller}
              {149}
\rtemscase{ready task -- preempts caller}
              {142}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{170}
\rtemscase{suspended task}{138}
\rtemscase{blocked task}{143}
\rtemscase{ready task}{144}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{71}
\rtemscase{returns to caller}{43}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {45}
\rtemscase{task readied -- preempts caller}
              {67}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {31}
\rtemscase{returns to caller}{64}
\rtemscase{preempts caller}{106}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{14}
\rtemscase{no reschedule}{16}
\rtemscase{reschedule -- returns to caller}
              {23}
\rtemscase{reschedule -- preempts caller}
              {60}
\rtemsonecase{TASK\_GET\_NOTE}{33}
\rtemsonecase{TASK\_SET\_NOTE}{33}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {16}
\rtemscase{yield -- preempts caller}
              {56}
\rtemsonecase{TASK\_WAKE\_WHEN}{117}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 148
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 350
@end itemize

@item TASK_START
@itemize -
@item only case: 76
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 95
@item suspended task -- returns to caller: 89
@item blocked task -- returns to caller: 124
@item ready task -- returns to caller: 92
@item suspended task -- preempts caller: 125
@item blocked task -- preempts caller: 149
@item ready task -- preempts caller: 142
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 170
@item suspended task: 138
@item blocked task: 143
@item ready task: 144
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 71
@item returns to caller: 43
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 45
@item task readied -- preempts caller: 67
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 31
@item returns to caller: 64
@item preempts caller: 106
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 14
@item no reschedule: 16
@item reschedule -- returns to caller: 23
@item reschedule -- preempts caller: 60
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 33
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 33
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 16
@item yield -- preempts caller: 56
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 117
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>148</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>350</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>76</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>95</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>89</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>124</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>92</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>125</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>149</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>142</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>170</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>138</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>143</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>144</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>71</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>43</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>45</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>31</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>64</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>106</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>14</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>16</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>23</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>60</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>33</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>33</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>16</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>56</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>117</TD></TR>
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
\rtemscase{returns to nested interrupt}{12}
\rtemscase{returns to interrupted task}
              {9}
\rtemscase{returns to preempting task}
              {9}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{<1}
\rtemscase{returns to interrupted task}
              {8}
\rtemscase{returns to preempting task}
              {54}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: 12
@item returns to interrupted task: 9
@item returns to preempting task: 9
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: <1
@item returns to interrupted task: 8
@item returns to preempting task: 54
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
        <TD ALIGN=center>12</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>9</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center><1</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>8</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>54</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{86}
\rtemsonecase{CLOCK\_GET}{1}
\rtemsonecase{CLOCK\_TICK}{17}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 86
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 1
@end itemize

@item CLOCK_TICK
@itemize -
@item only case: 17
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>86</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>17</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{28}
\rtemsonecase{TIMER\_IDENT}{343}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{43}
\rtemscase{active}{47}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{58}
\rtemscase{active}{61}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{88}
\rtemscase{active}{88}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{54}
\rtemscase{active}{58}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{31}
\rtemscase{active}{34}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 28
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 343
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 43
@item active: 47
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 58
@item active: 61
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
    <TD ALIGN=center>28</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>343</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>43</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>47</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>58</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>61</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>88</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>88</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>54</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>58</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>31</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>34</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{60}
\rtemsonecase{SEMAPHORE\_IDENT}{367}
\rtemsonecase{SEMAPHORE\_DELETE}{58}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{38}
\rtemscase{not available -- NO\_WAIT}
              {38}
\rtemscase{not available -- caller blocks}
              {109}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{44}
\rtemscase{task readied -- returns to caller}
              {66}
\rtemscase{task readied -- preempts caller}
              {87}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 60
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 367
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 58
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 38
@item not available -- NO_WAIT: 38
@item not available -- caller blocks: 109
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 44
@item task readied -- returns to caller: 66
@item task readied -- preempts caller: 87
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>60</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>367</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>58</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>38</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>38</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>109</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>44</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>66</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>87</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{200}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{341}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{80}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {97}
\rtemscase{task readied -- returns to caller}
              {101}
\rtemscase{task readied -- preempts caller}
              {123}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{96}
\rtemscase{task readied -- returns to caller}
              {101}
\rtemscase{task readied -- preempts caller}
              {123}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{53}
\rtemscase{task readied -- returns to caller}
              {111}
\rtemscase{task readied -- preempts caller}
              {133}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{79}
\rtemscase{not available -- NO\_WAIT}
              {43}
\rtemscase{not available -- caller blocks}
              {114}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{29}
\rtemscase{messages flushed}{39}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 200
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 341
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 80
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 97
@item task readied -- returns to caller: 101
@item task readied -- preempts caller: 123
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 96
@item task readied -- returns to caller: 101
@item task readied -- preempts caller: 123
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 53
@item task readied -- returns to caller: 111
@item task readied -- preempts caller: 133
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 79
@item not available -- NO_WAIT: 43
@item not available -- caller blocks: 114
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 29
@item messages flushed: 39
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>200</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>341</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>80</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>97</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>101</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>123</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>96</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>101</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>123</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>53</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>111</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>133</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>79</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>43</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>114</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>29</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>39</TD></TR>

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
\rtemscase{no task readied}{24}
\rtemscase{task readied -- returns to caller}
              {60}
\rtemscase{task readied -- preempts caller}
              {84}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{1}
\rtemscase{available}{28}
\rtemscase{not available -- NO\_WAIT}{23}
\rtemscase{not available -- caller blocks}
              {84}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 24
@item task readied -- returns to caller: 60
@item task readied -- preempts caller: 84
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: 1
@item available: 28
@item not available -- NO_WAIT: 23
@item not available -- caller blocks: 84
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
        <TD ALIGN=center>24</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>60</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>84</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>1</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>28</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>23</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>84</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{15}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{37}
\rtemscase{signal to self}{55}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {37}
\rtemscase{returns to preempting task}
              {54}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 15
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 37
@item signal to self: 55
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 37
@item returns to preempting task: 54
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>15</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>37</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>55</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          37</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          54</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{70}
\rtemsonecase{PARTITION\_IDENT}{341}
\rtemsonecase{PARTITION\_DELETE}{42}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{35}
\rtemscase{not available}{33}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {33}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: 70
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 341
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 42
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 35
@item not available: 33
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 43
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>70</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>341</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>42</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>35</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>33</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>33</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{63}
\rtemsonecase{REGION\_IDENT}{348}
\rtemsonecase{REGION\_DELETE}{39}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{52}
\rtemscase{not available -- NO\_WAIT}
              {49}
\rtemscase{not available -- caller blocks}
              {123}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{54}
\rtemscase{task readied -- returns to caller}
              {114}
\rtemscase{task readied -- preempts caller}
              {136}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 63
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 348
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 39
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 52
@item not available -- NO_WAIT: 49
@item not available -- caller blocks: 123
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 54
@item task readied -- returns to caller: 114
@item task readied -- preempts caller: 136
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>63</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>348</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>52</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          49</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          123</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>54</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          114</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          136</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{35}
\rtemsonecase{PORT\_IDENT}{340}
\rtemsonecase{PORT\_DELETE}{39}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {26}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {27}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 35
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 340
@end itemize

@item PORT_DELETE
@itemize -
@item only case: 39
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: 26
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: 27
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>35</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>340</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>26</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>27</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{4}
\rtemsonecase{IO\_OPEN}{2}
\rtemsonecase{IO\_CLOSE}{1}
\rtemsonecase{IO\_READ}{2}
\rtemsonecase{IO\_WRITE}{3}
\rtemsonecase{IO\_CONTROL}{2}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: 4
@end itemize

@item IO_OPEN
@itemize -
@item only case: 2
@end itemize

@item IO_CLOSE
@itemize -
@item only case: 1
@end itemize

@item IO_READ
@itemize -
@item only case: 2
@end itemize

@item IO_WRITE
@itemize -
@item only case: 3
@end itemize

@item IO_CONTROL
@itemize -
@item only case: 2
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>4</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>3</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>2</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{32}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{341}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{39}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{51}
\rtemscase{inactive}{48}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {54}
\rtemscase{conclude period -- caller blocks}
              {74}
\rtemscase{obtain status}{31}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 32
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 341
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 39
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 51
@item inactive: 48
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 54
@item conclude period -- caller blocks: 74
@item obtain status: 31
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>32</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>341</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>51</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>48</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         54</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         74</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>31</TD></TR>
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
