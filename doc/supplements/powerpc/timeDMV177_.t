@c
@c  Timing information for the DMV177
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

@chapter DMV177 Timing Data

@section Introduction

The timing data for RTEMS on the DY-4 DMV177 board
is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
PowerPC version of RTEMS.

@section Hardware Platform

All times reported in this chapter were measured using a DMV177 board.
All data and code caching was disabled.  This results in very deterministic
times which represent the worst possible performance.  Many embedded 
applications disable caching to insure that execution times are
repeatable.  Moreover, the JTAG port on certain revisions of the PowerPC
603e does not operate properly if caching is enabled.  Thus during 
development and debug, caching must be off.

The PowerPC decrementer register was was used to gather
all timing information.  In the PowerPC architecture,
this register typically counts
something like CPU cycles or is a function of the clock
speed.  On the PPC603e decrements once for every four (4) bus cycles.
On the DMV177, the bus operates at a clock speed of 
33 Mhz.  This result in a very accurate number since it is a function of the 
microprocessor itself.  Thus all measurements in this 
chapter are reported as the actual number of decrementer
clicks reported.  

To convert the numbers reported to microseconds, one should
divide the number reported by 8.650752.  This number was derived as
shown below:

@example
((33 * 1048576) / 1000000) / 4 = 8.650752
@end example

All sources of hardware interrupts were disabled,
although traps were enabled and the interrupt level of the
PowerPC allows all interrupts.

@section Interrupt Latency

The maximum period with traps disabled or the
processor interrupt level set to it's highest value inside RTEMS
is less than TBD
microseconds including the instructions which
disable and re-enable interrupts.  The time required for the
PowerPC to vector an interrupt and for the RTEMS entry overhead
before invoking the user's trap handler are a total of 
202
microseconds.  These combine to yield a worst case interrupt
latency of less than TBD +
202 microseconds at 
100.0 Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release 4.0.0-lmco.]

The maximum period with interrupts disabled within
RTEMS is hand-timed with some assistance from the PowerPC simulator.
The maximum period with interrupts disabled with RTEMS has not
been calculated on this target.

The interrupt vector and entry overhead time was
generated on the PSIM benchmark platform using the PowerPC's
decrementer register.  This register was programmed to generate
an interrupt after one countdown.

@section Context Switch

The RTEMS processor context switch time is 585
bus cycle on the DMV177 benchmark platform when no floating
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
times for the DMV177 benchmark platform:

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
\rtemsonecase{No Floating Point Contexts}{585}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{730}
\rtemscase{save initialized, restore initialized}{478}
\rtemscase{save idle, restore initialized}{825}
\rtemscase{save idle, restore idle}{478}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 585
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 730
@item save initialized, restore initialized: 478
@item save idle, restore initialized: 825
@item save idle, restore idle: 825
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>585</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>730</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>478</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>825</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>478</TD>
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
\rtemsonecase{TASK\_CREATE}{2301}
\rtemsonecase{TASK\_IDENT}{2900}
\rtemsonecase{TASK\_START}{794}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{1137}
\rtemscase{suspended task -- returns to caller}
              {906}
\rtemscase{blocked task -- returns to caller}
              {1102}
\rtemscase{ready task -- returns to caller}
              {928}
\rtemscase{suspended task -- preempts caller}
              {1483}
\rtemscase{blocked task -- preempts caller}
              {1640}
\rtemscase{ready task -- preempts caller}
              {1601}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{2117}
\rtemscase{suspended task}{1555}
\rtemscase{blocked task}{1609}
\rtemscase{ready task}{1620}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{960}
\rtemscase{returns to caller}{433}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {960}
\rtemscase{task readied -- preempts caller}
              {803}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {368}
\rtemscase{returns to caller}{633}
\rtemscase{preempts caller}{1211}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{184}
\rtemscase{no reschedule}{213}
\rtemscase{reschedule -- returns to caller}
              {247}
\rtemscase{reschedule -- preempts caller}
              {919}
\rtemsonecase{TASK\_GET\_NOTE}{382}
\rtemsonecase{TASK\_SET\_NOTE}{383}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {245}
\rtemscase{yield -- preempts caller}
              {851}
\rtemsonecase{TASK\_WAKE\_WHEN}{1275}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 2301
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 2900
@end itemize

@item TASK_START
@itemize -
@item only case: 794
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 1137
@item suspended task -- returns to caller: 906
@item blocked task -- returns to caller: 1102
@item ready task -- returns to caller: 928
@item suspended task -- preempts caller: 1483
@item blocked task -- preempts caller: 1640
@item ready task -- preempts caller: 1601
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 2117
@item suspended task: 1555
@item blocked task: 1609
@item ready task: 1620
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 960
@item returns to caller: 433
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 960
@item task readied -- preempts caller: 803
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 368
@item returns to caller: 633
@item preempts caller: 1211
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 184
@item no reschedule: 213
@item reschedule -- returns to caller: 247
@item reschedule -- preempts caller: 919
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 382
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 383
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 245
@item yield -- preempts caller: 851
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 1275
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>2301</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>2900</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>794</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>1137</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>906</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>1102</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>928</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>1483</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>1640</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>1601</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>2117</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>1555</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>1609</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>1620</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>960</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>433</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>960</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>803</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>368</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>633</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>1211</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>184</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>213</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>247</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>919</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>382</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>383</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>245</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>851</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>1275</TD></TR>
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
\rtemscase{returns to nested interrupt}{201}
\rtemscase{returns to interrupted task}
              {206}
\rtemscase{returns to preempting task}
              {202}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{201}
\rtemscase{returns to interrupted task}
              {213}
\rtemscase{returns to preempting task}
              {857}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: 201
@item returns to interrupted task: 206
@item returns to preempting task: 202
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: 201
@item returns to interrupted task: 213
@item returns to preempting task: 857
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
        <TD ALIGN=center>201</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>206</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>202</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>201</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>213</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>857</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{792}
\rtemsonecase{CLOCK\_GET}{78}
\rtemsonecase{CLOCK\_TICK}{214}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 792
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 78
@end itemize

@item CLOCK_TICK
@itemize -
@item only case: 214
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>792</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>78</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>214</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{357}
\rtemsonecase{TIMER\_IDENT}{2828}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{432}
\rtemscase{active}{471}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{607}
\rtemscase{active}{646}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{766}
\rtemscase{active}{764}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{552}
\rtemscase{active}{766}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{339}
\rtemscase{active}{378}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 357
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 2828
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 432
@item active: 471
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 607
@item active: 646
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
    <TD ALIGN=center>357</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>2828</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>432</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>471</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>607</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>646</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>766</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>764</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>552</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>766</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>339</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>378</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{571}
\rtemsonecase{SEMAPHORE\_IDENT}{3243}
\rtemsonecase{SEMAPHORE\_DELETE}{575}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{414}
\rtemscase{not available -- NO\_WAIT}
              {414}
\rtemscase{not available -- caller blocks}
              {1254}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{501}
\rtemscase{task readied -- returns to caller}
              {636}
\rtemscase{task readied -- preempts caller}
              {982}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 571
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 3243
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 575
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 414
@item not available -- NO_WAIT: 414
@item not available -- caller blocks: 1254
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 501
@item task readied -- returns to caller: 636
@item task readied -- preempts caller: 982
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>571</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>3243</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>575</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>414</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>414</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>1254</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>501</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>636</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>982</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{2270}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{2828}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{708}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {923}
\rtemscase{task readied -- returns to caller}
              {955}
\rtemscase{task readied -- preempts caller}
              {1322}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{919}
\rtemscase{task readied -- returns to caller}
              {955}
\rtemscase{task readied -- preempts caller}
              {1322}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{589}
\rtemscase{task readied -- returns to caller}
              {1079}
\rtemscase{task readied -- preempts caller}
              {1435}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{755}
\rtemscase{not available -- NO\_WAIT}
              {467}
\rtemscase{not available -- caller blocks}
              {1283}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{369}
\rtemscase{messages flushed}{431}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 2270
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 2828
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 708
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 923
@item task readied -- returns to caller: 955
@item task readied -- preempts caller: 1322
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 919
@item task readied -- returns to caller: 955
@item task readied -- preempts caller: 1322
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 589
@item task readied -- returns to caller: 1079
@item task readied -- preempts caller: 1435
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 755
@item not available -- NO_WAIT: 467
@item not available -- caller blocks: 1283
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 369
@item messages flushed: 431
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>2270</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>2828</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>708</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>923</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>955</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>1322</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>919</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>955</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>1322</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>589</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>1079</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>1435</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>755</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>467</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>1283</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>369</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>431</TD></TR>

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
\rtemscase{no task readied}{354}
\rtemscase{task readied -- returns to caller}
              {571}
\rtemscase{task readied -- preempts caller}
              {946}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{43}
\rtemscase{available}{357}
\rtemscase{not available -- NO\_WAIT}{331}
\rtemscase{not available -- caller blocks}
              {1043}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 354
@item task readied -- returns to caller: 571
@item task readied -- preempts caller: 946
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: 43
@item available: 357
@item not available -- NO_WAIT: 331
@item not available -- caller blocks: 1043
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
        <TD ALIGN=center>354</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>571</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>946</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>43</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>357</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>331</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>1043</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{267}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{408}
\rtemscase{signal to self}{607}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {464}
\rtemscase{returns to preempting task}
              {752}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 267
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 408
@item signal to self: 607
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 464
@item returns to preempting task: 752
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>267</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>408</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>607</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          464</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          752</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{762}
\rtemsonecase{PARTITION\_IDENT}{2828}
\rtemsonecase{PARTITION\_DELETE}{426}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{394}
\rtemscase{not available}{376}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {376}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: 762
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 2828
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 426
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 394
@item not available: 376
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 420
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>762</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>2828</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>426</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>394</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>376</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>376</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{614}
\rtemsonecase{REGION\_IDENT}{2878}
\rtemsonecase{REGION\_DELETE}{425}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{515}
\rtemscase{not available -- NO\_WAIT}
              {472}
\rtemscase{not available -- caller blocks}
              {1345}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{544}
\rtemscase{task readied -- returns to caller}
              {935}
\rtemscase{task readied -- preempts caller}
              {1296}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 614
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 2878
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 425
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 515
@item not available -- NO_WAIT: 472
@item not available -- caller blocks: 1345
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 544
@item task readied -- returns to caller: 935
@item task readied -- preempts caller: 1296
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>614</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>2878</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>425</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>515</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          472</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          1345</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>544</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          935</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          1296</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{428}
\rtemsonecase{PORT\_IDENT}{2828}
\rtemsonecase{PORT\_DELETE}{421}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {339}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {339}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 428
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 2828
@end itemize

@item PORT_DELETE
@itemize -
@item only case: 421
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: 339
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: 339
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>428</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>2828</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>421</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>339</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>339</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{52}
\rtemsonecase{IO\_OPEN}{42}
\rtemsonecase{IO\_CLOSE}{44}
\rtemsonecase{IO\_READ}{42}
\rtemsonecase{IO\_WRITE}{44}
\rtemsonecase{IO\_CONTROL}{42}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: 52
@end itemize

@item IO_OPEN
@itemize -
@item only case: 42
@end itemize

@item IO_CLOSE
@itemize -
@item only case: 44
@end itemize

@item IO_READ
@itemize -
@item only case: 42
@end itemize

@item IO_WRITE
@itemize -
@item only case: 44
@end itemize

@item IO_CONTROL
@itemize -
@item only case: 42
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>52</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>42</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>44</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>42</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>44</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>42</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{388}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{2826}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{427}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{519}
\rtemscase{inactive}{465}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {556}
\rtemscase{conclude period -- caller blocks}
              {842}
\rtemscase{obtain status}{377}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 388
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 2826
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 427
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 519
@item inactive: 465
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 556
@item conclude period -- caller blocks: 842
@item obtain status: 377
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>388</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>2826</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>427</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>519</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>465</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         556</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         842</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>377</TD></TR>
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
