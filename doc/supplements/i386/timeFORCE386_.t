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
 
@chapter CPU386 Timing Data

@section Introduction

The timing data for the i386 version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the i386 version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Force
Computers CPU386 board.  The CPU386 is a 16 Mhz board with zero
wait state dynamic memory and an i80387 numeric coprocessor.
One of the count-down timers provided by a Motorola MC68901 was
used to measure elapsed time with one microsecond resolution.
All sources of hardware interrupts are disabled, although the
interrupt level of the i386 allows all interrupts.

The maximum period interrupts are disabled was
measured by summing the number of CPU cycles required by each
assembly language instruction executed while interrupts were
disabled.  Zero wait state memory was assumed.  The total CPU
cycles executed with interrupts disabled, including the
instructions to disable and enable interrupts, was divided by 16
to simulate a i386 executing at 16 Mhz.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than 13.0 microseconds 
including the instructions
which disable and re-enable interrupts.  The time required for
the i386 to generate an interrupt using the int instruction,
vectoring to an interrupt handler, and for the RTEMS entry
overhead before invoking the user's interrupt handler are a
total of 12 microseconds.  These combine to yield a worst case
interrupt latency of less 
13.0 + 12 
microseconds.  [NOTE:  The
maximum period with interrupts disabled within RTEMS was last
calculated for Release 3.1.0.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed.  The interrupt
vector and entry overhead time was generated on the Force
Computers CPU386 benchmark platform using the int instruction as
the interrupt source.

@section Context Switch

The RTEMS processor context switch time is 34
microseconds on the Force Computers CPU386 benchmark platform.
This time represents the raw context switch time with no user
extensions configured.  Additional execution time is required
when a TASK_SWITCH user extension is configured.  The use of the
TASK_SWITCH extension is application dependent.  Thus, its
execution time is not considered part of the base context switch
time.

Since RTEMS was designed specifically for embedded
missile applications which are floating point intensive, the
executive is optimized to avoid unnecessarily saving and
restoring the state of the numeric coprocessor.  The state of
the numeric coprocessor is only saved when a FLOATING_POINT task
is dispatched and that task was not the last task to utilize the
coprocessor.  In a system with only one FLOATING_POINT task, the
state of the numeric coprocessor will never be saved or
restored.  When the first FLOATING_POINT task is dispatched,
RTEMS does not need to save the current state of the numeric
coprocessor.

The exact amount of time required to save and restore
floating point context is dependent on the state of the numeric
coprocessor.  RTEMS places the coprocessor in the initialized
state when a task is started or restarted.  Once the task has
utilized the coprocessor, it is in the idle state when floating
point instructions are not executing and the busy state when
floating point instructions are executing.  The state of the
coprocessor is task specific.

The following table summarizes the context switch
times for the Force Computers CPU386 benchmark platform:

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
\rtemsonecase{No Floating Point Contexts}{34}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{57}
\rtemscase{save initialized, restore initialized}{59}
\rtemscase{save idle, restore initialized}{59}
\rtemscase{save idle, restore idle}{83}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 34
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 57
@item save initialized, restore initialized: 59
@item save idle, restore initialized: 59
@item save idle, restore idle: 59
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>34</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>57</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>59</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>59</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>83</TD>
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
\rtemsonecase{TASK\_CREATE}{157}
\rtemsonecase{TASK\_IDENT}{748}
\rtemsonecase{TASK\_START}{86}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{118}
\rtemscase{suspended task -- returns to caller}
              {45}
\rtemscase{blocked task -- returns to caller}
              {138}
\rtemscase{ready task -- returns to caller}
              {105}
\rtemscase{suspended task -- preempts caller}
              {149}
\rtemscase{blocked task -- preempts caller}
              {162}
\rtemscase{ready task -- preempts caller}
              {156}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{187}
\rtemscase{suspended task}{147}
\rtemscase{blocked task}{153}
\rtemscase{ready task}{157}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{81}
\rtemscase{returns to caller}{45}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {46}
\rtemscase{task readied -- preempts caller}
              {71}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {30}
\rtemscase{returns to caller}{67}
\rtemscase{preempts caller}{115}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{19}
\rtemscase{no reschedule}{21}
\rtemscase{reschedule -- returns to caller}
              {27}
\rtemscase{reschedule -- preempts caller}
              {66}
\rtemsonecase{TASK\_GET\_NOTE}{32}
\rtemsonecase{TASK\_SET\_NOTE}{32}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {18}
\rtemscase{yield -- preempts caller}
              {63}
\rtemsonecase{TASK\_WAKE\_WHEN}{128}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 157
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 748
@end itemize

@item TASK_START
@itemize -
@item only case: 86
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 118
@item suspended task -- returns to caller: 45
@item blocked task -- returns to caller: 138
@item ready task -- returns to caller: 105
@item suspended task -- preempts caller: 149
@item blocked task -- preempts caller: 162
@item ready task -- preempts caller: 156
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 187
@item suspended task: 147
@item blocked task: 153
@item ready task: 157
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 81
@item returns to caller: 45
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 46
@item task readied -- preempts caller: 71
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 30
@item returns to caller: 67
@item preempts caller: 115
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 19
@item no reschedule: 21
@item reschedule -- returns to caller: 27
@item reschedule -- preempts caller: 66
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 32
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 32
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 18
@item yield -- preempts caller: 63
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 128
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>157</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>748</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>86</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>118</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>45</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>138</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>105</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>149</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>162</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>156</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>187</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>147</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>153</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>157</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>81</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>45</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>46</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>71</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>30</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>67</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>115</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>19</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>21</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>27</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>66</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>32</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>32</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>18</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>63</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>128</TD></TR>
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
              {13}
\rtemscase{returns to preempting task}
              {12}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{10}
\rtemscase{returns to interrupted task}
              {13}
\rtemscase{returns to preempting task}
              {58}
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
@item returns to interrupted task: 13
@item returns to preempting task: 12
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: 10
@item returns to interrupted task: 13
@item returns to preempting task: 58
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
        <TD ALIGN=center>13</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>12</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>13</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>58</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{85}
\rtemsonecase{CLOCK\_GET}{2}
\rtemsonecase{CLOCK\_TICK}{16}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 85
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 2
@end itemize

@item CLOCK_TICK
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
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>85</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>2</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>16</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{34}
\rtemsonecase{TIMER\_IDENT}{729}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{48}
\rtemscase{active}{52}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{65}
\rtemscase{active}{69}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{92}
\rtemscase{active}{92}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{58}
\rtemscase{active}{63}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{32}
\rtemscase{active}{37}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 34
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 729
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 48
@item active: 52
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 65
@item active: 69
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
    <TD ALIGN=center>34</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>729</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>48</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>52</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>65</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>69</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>92</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>92</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>58</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>63</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>32</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>37</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{64}
\rtemsonecase{SEMAPHORE\_IDENT}{787}
\rtemsonecase{SEMAPHORE\_DELETE}{60}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{41}
\rtemscase{not available -- NO\_WAIT}
              {40}
\rtemscase{not available -- caller blocks}
              {123}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{47}
\rtemscase{task readied -- returns to caller}
              {70}
\rtemscase{task readied -- preempts caller}
              {95}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 64
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 787
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 60
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 41
@item not available -- NO_WAIT: 40
@item not available -- caller blocks: 123
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 47
@item task readied -- returns to caller: 70
@item task readied -- preempts caller: 95
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>64</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>787</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>60</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>41</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>40</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>123</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>47</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>70</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>95</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{294}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{730}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{81}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {117}
\rtemscase{task readied -- returns to caller}
              {118}
\rtemscase{task readied -- preempts caller}
              {144}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{117}
\rtemscase{task readied -- returns to caller}
              {116}
\rtemscase{task readied -- preempts caller}
              {144}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{53}
\rtemscase{task readied -- returns to caller}
              {122}
\rtemscase{task readied -- preempts caller}
              {146}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{93}
\rtemscase{not available -- NO\_WAIT}
              {45}
\rtemscase{not available -- caller blocks}
              {127}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{29}
\rtemscase{messages flushed}{41}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 294
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 730
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 81
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 117
@item task readied -- returns to caller: 118
@item task readied -- preempts caller: 144
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 117
@item task readied -- returns to caller: 116
@item task readied -- preempts caller: 144
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 53
@item task readied -- returns to caller: 122
@item task readied -- preempts caller: 146
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 93
@item not available -- NO_WAIT: 45
@item not available -- caller blocks: 127
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 29
@item messages flushed: 41
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>294</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>730</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>81</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>117</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>118</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>144</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>117</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>116</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>144</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>53</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>122</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>146</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>93</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>45</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>127</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>29</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>41</TD></TR>

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
\rtemscase{no task readied}{26}
\rtemscase{task readied -- returns to caller}
              {60}
\rtemscase{task readied -- preempts caller}
              {89}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{<1}
\rtemscase{available}{27}
\rtemscase{not available -- NO\_WAIT}{25}
\rtemscase{not available -- caller blocks}
              {94}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 26
@item task readied -- returns to caller: 60
@item task readied -- preempts caller: 89
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: <1
@item available: 27
@item not available -- NO_WAIT: 25
@item not available -- caller blocks: 94
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
        <TD ALIGN=center>26</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>60</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>89</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center><1</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>27</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>25</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>94</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{13}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{34}
\rtemscase{signal to self}{59}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {39}
\rtemscase{returns to preempting task}
              {60}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 13
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 34
@item signal to self: 59
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 39
@item returns to preempting task: 60
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>13</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>34</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>59</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          39</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          60</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{83}
\rtemsonecase{PARTITION\_IDENT}{730}
\rtemsonecase{PARTITION\_DELETE}{40}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{34}
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
@item only case: 83
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 730
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 40
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 34
@item not available: 33
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 40
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>83</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>730</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>40</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>34</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{68}
\rtemsonecase{REGION\_IDENT}{739}
\rtemsonecase{REGION\_DELETE}{39}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{49}
\rtemscase{not available -- NO\_WAIT}
              {45}
\rtemscase{not available -- caller blocks}
              {127}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{52}
\rtemscase{task readied -- returns to caller}
              {113}
\rtemscase{task readied -- preempts caller}
              {138}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 68
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 739
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 39
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 49
@item not available -- NO_WAIT: 45
@item not available -- caller blocks: 127
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 52
@item task readied -- returns to caller: 113
@item task readied -- preempts caller: 138
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>68</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>739</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>49</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          45</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          127</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>52</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          113</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          138</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{39}
\rtemsonecase{PORT\_IDENT}{728}
\rtemsonecase{PORT\_DELETE}{39}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {26}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {26}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 39
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 728
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
@item only case: 26
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>728</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>26</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>26</TD></TR>

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
\rtemsonecase{IO\_OPEN}{1}
\rtemsonecase{IO\_CLOSE}{1}
\rtemsonecase{IO\_READ}{<1}
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
@item only case: 4
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
@item only case: <1
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
    <TD ALIGN=center>4</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center><1</TD></TR>
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
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{36}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{725}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{39}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{53}
\rtemscase{inactive}{49}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {53}
\rtemscase{conclude period -- caller blocks}
              {82}
\rtemscase{obtain status}{30}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 36
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 725
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 39
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 53
@item inactive: 49
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 53
@item conclude period -- caller blocks: 82
@item obtain status: 30
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>36</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>725</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>39</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>53</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>49</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         53</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         82</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>30</TD></TR>
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
