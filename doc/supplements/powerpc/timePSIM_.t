@c
@c  Timing information for PSIM
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

@chapter PSIM Timing Data

@section Introduction

The timing data for RTEMS on the PSIM target 
is provided along with the target
dependent aspects concerning the gathering of the timing data.
The hardware platform used to gather the times is described to
give the reader a better understanding of each directive time
provided.  Also, provided is a description of the interrupt
latency and the context switch times as they pertain to the
PowerPC version of RTEMS.

@section Hardware Platform

All times reported in this chapter were measured using the PowerPC
Instruction Simulator (PSIM). PSIM simulates a variety of PowerPC
6xx models with the PPC603e being used as the basis for the measurements
reported in this chapter.

The PowerPC decrementer register was was used to gather
all timing information.  In real hardware implementations
of the PowerPC architecture, this register would typically
count something like CPU cycles or be a function of the clock
speed.  However, with PSIM each count of the decrementer register 
represents an instruction.  Thus all measurements in this 
chapter are reported as the actual number of instructions
executed.  All sources of hardware interrupts were disabled,
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
61
microseconds.  These combine to yield a worst case interrupt
latency of less than TBD +
61 microseconds at 
na Mhz.
[NOTE:  The maximum period with interrupts disabled was last
determined for Release 4.0.0-lmco.]

The maximum period with interrupts disabled within
RTEMS is hand-timed with some assistance from PSIM.  The maximum
period with interrupts disabled with RTEMS occurs was not measured
on this target.

The interrupt vector and entry overhead time was
generated on the PSIM benchmark platform using the PowerPC's
decrementer register.  This register was programmed to generate
an interrupt after one countdown.

@section Context Switch

The RTEMS processor context switch time is 214
instructions on the PSIM benchmark platform when no floating
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
times for the PSIM benchmark platform:
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
\rtemsonecase{No Floating Point Contexts}{214}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{255}
\rtemscase{save initialized, restore initialized}{140}
\rtemscase{save idle, restore initialized}{140}
\rtemscase{save idle, restore idle}{290}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 214
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 255
@item save initialized, restore initialized: 140
@item save idle, restore initialized: 140
@item save idle, restore idle: 140
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>214</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>255</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>140</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>140</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>290</TD>
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
\rtemsonecase{TASK\_CREATE}{1075}
\rtemsonecase{TASK\_IDENT}{1637}
\rtemsonecase{TASK\_START}{345}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{483}
\rtemscase{suspended task -- returns to caller}
              {396}
\rtemscase{blocked task -- returns to caller}
              {491}
\rtemscase{ready task -- returns to caller}
              {404}
\rtemscase{suspended task -- preempts caller}
              {644}
\rtemscase{blocked task -- preempts caller}
              {709}
\rtemscase{ready task -- preempts caller}
              {686}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{941}
\rtemscase{suspended task}{703}
\rtemscase{blocked task}{723}
\rtemscase{ready task}{729}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{403}
\rtemscase{returns to caller}{181}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {191}
\rtemscase{task readied -- preempts caller}
              {803}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {147}
\rtemscase{returns to caller}{264}
\rtemscase{preempts caller}{517}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{88}
\rtemscase{no reschedule}{110}
\rtemscase{reschedule -- returns to caller}
              {112}
\rtemscase{reschedule -- preempts caller}
              {386}
\rtemsonecase{TASK\_GET\_NOTE}{156}
\rtemsonecase{TASK\_SET\_NOTE}{155}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {92}
\rtemscase{yield -- preempts caller}
              {348}
\rtemsonecase{TASK\_WAKE\_WHEN}{546}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 1075
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 1637
@end itemize

@item TASK_START
@itemize -
@item only case: 345
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 483
@item suspended task -- returns to caller: 396
@item blocked task -- returns to caller: 491
@item ready task -- returns to caller: 404
@item suspended task -- preempts caller: 644
@item blocked task -- preempts caller: 709
@item ready task -- preempts caller: 686
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 941
@item suspended task: 703
@item blocked task: 723
@item ready task: 729
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 403
@item returns to caller: 181
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 191
@item task readied -- preempts caller: 803
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 147
@item returns to caller: 264
@item preempts caller: 517
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 88
@item no reschedule: 110
@item reschedule -- returns to caller: 112
@item reschedule -- preempts caller: 386
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 156
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 155
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 92
@item yield -- preempts caller: 348
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 546
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>1075</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>1637</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>345</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>483</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>396</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>491</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>404</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>644</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>709</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>686</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>941</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>703</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>723</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>729</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>403</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>181</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>191</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>803</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>147</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>264</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>517</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>88</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>110</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>112</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>386</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>156</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>155</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>92</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>348</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>546</TD></TR>
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
\rtemscase{returns to nested interrupt}{60}
\rtemscase{returns to interrupted task}
              {62}
\rtemscase{returns to preempting task}
              {61}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{55}
\rtemscase{returns to interrupted task}
              {67}
\rtemscase{returns to preempting task}
              {344}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: 60
@item returns to interrupted task: 62
@item returns to preempting task: 61
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: 55
@item returns to interrupted task: 67
@item returns to preempting task: 344
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
        <TD ALIGN=center>60</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>62</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>61</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>55</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>67</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>344</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{340}
\rtemsonecase{CLOCK\_GET}{29}
\rtemsonecase{CLOCK\_TICK}{81}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 340
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 29
@end itemize

@item CLOCK_TICK
@itemize -
@item only case: 81
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>340</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>29</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>81</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{144}
\rtemsonecase{TIMER\_IDENT}{1595}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{197}
\rtemscase{active}{181}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{252}
\rtemscase{active}{269}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{333}
\rtemscase{active}{334}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{233}
\rtemscase{active}{250}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{156}
\rtemscase{active}{140}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 144
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 1595
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 197
@item active: 181
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 252
@item active: 269
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
    <TD ALIGN=center>144</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>1595</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>197</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>181</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>252</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>269</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>333</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>334</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>233</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>250</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>156</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>140</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{223}
\rtemsonecase{SEMAPHORE\_IDENT}{1836}
\rtemsonecase{SEMAPHORE\_DELETE}{1836}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{175}
\rtemscase{not available -- NO\_WAIT}
              {175}
\rtemscase{not available -- caller blocks}
              {530}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{206}
\rtemscase{task readied -- returns to caller}
              {272}
\rtemscase{task readied -- preempts caller}
              {415}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 223
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 1836
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 1836
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 175
@item not available -- NO_WAIT: 175
@item not available -- caller blocks: 530
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 206
@item task readied -- returns to caller: 272
@item task readied -- preempts caller: 415
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>223</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>1836</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>1836</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>175</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>175</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>530</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>206</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>272</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>415</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{1022}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{1596}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{308}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {421}
\rtemscase{task readied -- returns to caller}
              {434}
\rtemscase{task readied -- preempts caller}
              {581}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{422}
\rtemscase{task readied -- returns to caller}
              {435}
\rtemscase{task readied -- preempts caller}
              {582}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{244}
\rtemscase{task readied -- returns to caller}
              {482}
\rtemscase{task readied -- preempts caller}
              {630}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{345}
\rtemscase{not available -- NO\_WAIT}
              {197}
\rtemscase{not available -- caller blocks}
              {542}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{142}
\rtemscase{messages flushed}{170}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 1022
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 1596
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 308
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 421
@item task readied -- returns to caller: 434
@item task readied -- preempts caller: 581
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 422
@item task readied -- returns to caller: 435
@item task readied -- preempts caller: 582
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 244
@item task readied -- returns to caller: 482
@item task readied -- preempts caller: 630
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 345
@item not available -- NO_WAIT: 197
@item not available -- caller blocks: 542
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 142
@item messages flushed: 170
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>1022</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>1596</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>308</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>421</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>434</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>581</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>422</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>435</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>582</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>244</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>482</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>630</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>345</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>197</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>542</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>142</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>170</TD></TR>

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
\rtemscase{no task readied}{145}
\rtemscase{task readied -- returns to caller}
              {250}
\rtemscase{task readied -- preempts caller}
              {407}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{17}
\rtemscase{available}{133}
\rtemscase{not available -- NO\_WAIT}{130}
\rtemscase{not available -- caller blocks}
              {442}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 145
@item task readied -- returns to caller: 250
@item task readied -- preempts caller: 407
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: 17
@item available: 133
@item not available -- NO_WAIT: 130
@item not available -- caller blocks: 442
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
        <TD ALIGN=center>145</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>250</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>407</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>17</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>133</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>130</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>442</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{95}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{165}
\rtemscase{signal to self}{275}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {216}
\rtemscase{returns to preempting task}
              {329}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 95
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 165
@item signal to self: 275
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 216
@item returns to preempting task: 329
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>95</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>165</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>275</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          216</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          329</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{320}
\rtemsonecase{PARTITION\_IDENT}{1596}
\rtemsonecase{PARTITION\_DELETE}{168}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{157}
\rtemscase{not available}{149}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {149}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: 320
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 1596
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 168
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 157
@item not available: 149
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 172
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>320</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>1596</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>168</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>157</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>149</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>149</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{239}
\rtemsonecase{REGION\_IDENT}{1625}
\rtemsonecase{REGION\_DELETE}{167}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{206}
\rtemscase{not available -- NO\_WAIT}
              {190}
\rtemscase{not available -- caller blocks}
              {556}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{230}
\rtemscase{task readied -- returns to caller}
              {412}
\rtemscase{task readied -- preempts caller}
              {562}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 239
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 1625
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 167
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 206
@item not available -- NO_WAIT: 190
@item not available -- caller blocks: 556
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 230
@item task readied -- returns to caller: 412
@item task readied -- preempts caller: 562
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>239</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>1625</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>167</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>206</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          190</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          556</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>230</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          412</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          562</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{167}
\rtemsonecase{PORT\_IDENT}{1594}
\rtemsonecase{PORT\_DELETE}{165}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {133}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {134}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 167
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 1594
@end itemize

@item PORT_DELETE
@itemize -
@item only case: 165
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: 133
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: 134
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>167</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>1594</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>165</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>133</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>134</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{23}
\rtemsonecase{IO\_OPEN}{18}
\rtemsonecase{IO\_CLOSE}{18}
\rtemsonecase{IO\_READ}{18}
\rtemsonecase{IO\_WRITE}{18}
\rtemsonecase{IO\_CONTROL}{18}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: 23
@end itemize

@item IO_OPEN
@itemize -
@item only case: 18
@end itemize

@item IO_CLOSE
@itemize -
@item only case: 18
@end itemize

@item IO_READ
@itemize -
@item only case: 18
@end itemize

@item IO_WRITE
@itemize -
@item only case: 18
@end itemize

@item IO_CONTROL
@itemize -
@item only case: 18
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>23</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>18</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>18</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>18</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>18</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>18</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{149}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{1595}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{169}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{212}
\rtemscase{inactive}{186}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {226}
\rtemscase{conclude period -- caller blocks}
              {362}
\rtemscase{obtain status}{142}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 149
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 1595
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 169
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 212
@item inactive: 186
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 226
@item conclude period -- caller blocks: 362
@item obtain status: 142
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>149</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>1595</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>169</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>212</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>186</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         226</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         362</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>142</TD></TR>
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
