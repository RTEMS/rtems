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

@chapter CVME961 Timing Data

NOTE: The CVME961 board used by the RTEMS Project to
obtain i960CA times is currently broken.  The information in
this chapter was obtained using Release 3.2.1.

@section Introduction

The timing data for the i960CA version of RTEMS is
provided along with the target dependent aspects concerning the
gathering of the timing data.  The hardware platform used to
gather the times is described to give the reader a better
understanding of each directive time provided.  Also, provided
is a description of the  interrupt latency and the context
switch times as they pertain to the i960CA version of RTEMS.

@section Hardware Platform

All times reported except for the maximum period
interrupts are disabled by RTEMS were measured using a Cyclone
Microsystems CVME961 board.  The CVME961 is a 33 Mhz board with
dynamic RAM which has two wait state dynamic memory (four CPU
cycles) for read accesses and one wait state (two CPU cycles)
for write accesses.  The Z8536 on a SQUALL SQSIO4 mezzanine
board was used to measure elapsed time with one-half microsecond
resolution.  All sources of hardware interrupts are disabled,
although the interrupt level of the i960CA allows all interrupts.

The maximum  interrupt disable period was measured by
summing the number of CPU cycles required by each assembly
language instruction executed while interrupts were disabled.
Zero wait state memory was assumed.  The total CPU cycles
executed with interrupts disabled, including the instructions to
disable and enable interrupts, was divided by 33 to simulate a
i960CA executing at 33 Mhz with zero wait states.

@section Interrupt Latency

The maximum period with interrupts disabled within
RTEMS is less than
2.5 microseconds including the instructions
which disable and re-enable interrupts.  The time required for
the i960CA to generate an interrupt using the sysctl
instruction, vectoring to an interrupt handler, and for the
RTEMS entry overhead before invoking the user's interrupt
handler are a total of 37
microseconds.  These combine to yield
a worst case interrupt latency of less than
2.5 + 37
microseconds.  [NOTE: The maximum period with interrupts
disabled within RTEMS was last calculated for Release
3.2.1.]

It should be noted again that the maximum period with
interrupts disabled within RTEMS is hand-timed.  The interrupt
vector and entry overhead time was generated on the Cyclone
CVME961 benchmark platform using the sysctl instruction as the
interrupt source.

@section Context Switch

The RTEMS processor context switch time is 1
microseconds on the Cyclone CVME961 benchmark platform.  This
time represents the raw context switch time with no user
extensions configured.  Additional execution time is required
when a TSWITCH user extension is configured.  The use of the
TSWITCH extension is application dependent.  Thus, its execution
time is not considered part of the base context switch time.

The CVME961 has no hardware floating point capability
and floating point tasks are not supported.

The following table summarizes the context switch
times for the CVME961 benchmark platform:

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
\rtemsonecase{No Floating Point Contexts}{1}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{2}
\rtemscase{save initialized, restore initialized}{3}
\rtemscase{save idle, restore initialized}{4}
\rtemscase{save idle, restore idle}{5}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: 1
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: 2
@item save initialized, restore initialized: 3
@item save idle, restore initialized: 4
@item save idle, restore idle: 4
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>1</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>2</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>3</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>4</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>5</TD>
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
\rtemsonecase{TASK\_CREATE}{6}
\rtemsonecase{TASK\_IDENT}{7}
\rtemsonecase{TASK\_START}{8}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{9}
\rtemscase{suspended task -- returns to caller}
              {9}
\rtemscase{blocked task -- returns to caller}
              {10}
\rtemscase{ready task -- returns to caller}
              {11}
\rtemscase{suspended task -- preempts caller}
              {12}
\rtemscase{blocked task -- preempts caller}
              {13}
\rtemscase{ready task -- preempts caller}
              {14}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{15}
\rtemscase{suspended task}{16}
\rtemscase{blocked task}{17}
\rtemscase{ready task}{18}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{19}
\rtemscase{returns to caller}{20}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {21}
\rtemscase{task readied -- preempts caller}
              {22}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {23}
\rtemscase{returns to caller}{24}
\rtemscase{preempts caller}{25}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{26}
\rtemscase{no reschedule}{27}
\rtemscase{reschedule -- returns to caller}
              {28}
\rtemscase{reschedule -- preempts caller}
              {29}
\rtemsonecase{TASK\_GET\_NOTE}{30}
\rtemsonecase{TASK\_SET\_NOTE}{31}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {32}
\rtemscase{yield -- preempts caller}
              {33}
\rtemsonecase{TASK\_WAKE\_WHEN}{34}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: 6
@end itemize

@item TASK_IDENT
@itemize -
@item only case: 7
@end itemize

@item TASK_START
@itemize -
@item only case: 8
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: 9
@item suspended task -- returns to caller: 9
@item blocked task -- returns to caller: 10
@item ready task -- returns to caller: 11
@item suspended task -- preempts caller: 12
@item blocked task -- preempts caller: 13
@item ready task -- preempts caller: 14
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: 15
@item suspended task: 16
@item blocked task: 17
@item ready task: 18
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: 19
@item returns to caller: 20
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: 21
@item task readied -- preempts caller: 22
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: 23
@item returns to caller: 24
@item preempts caller: 25
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: 26
@item no reschedule: 27
@item reschedule -- returns to caller: 28
@item reschedule -- preempts caller: 29
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: 30
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: 31
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: 32
@item yield -- preempts caller: 33
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: 34
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>6</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>7</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>8</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>9</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>10</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>11</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>12</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>13</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>14</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>15</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>16</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>17</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>18</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>19</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>20</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>21</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>22</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>23</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>24</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>25</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>26</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>27</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>28</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>29</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>30</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>31</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>32</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>33</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>34</TD></TR>
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
\rtemscase{returns to nested interrupt}{35}
\rtemscase{returns to interrupted task}
              {36}
\rtemscase{returns to preempting task}
              {37}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{38}
\rtemscase{returns to interrupted task}
              {39}
\rtemscase{returns to preempting task}
              {40}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: 35
@item returns to interrupted task: 36
@item returns to preempting task: 37
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: 38
@item returns to interrupted task: 39
@item returns to preempting task: 40
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
        <TD ALIGN=center>35</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>36</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>37</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>38</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>39</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>40</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{41}
\rtemsonecase{CLOCK\_GET}{42}
\rtemsonecase{CLOCK\_TICK}{43}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: 41
@end itemize

@item CLOCK_GET
@itemize -
@item only case: 42
@end itemize

@item CLOCK_TICK
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
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>41</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>42</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>43</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{44}
\rtemsonecase{TIMER\_IDENT}{45}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{46}
\rtemscase{active}{47}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{48}
\rtemscase{active}{49}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{50}
\rtemscase{active}{51}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{52}
\rtemscase{active}{53}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{54}
\rtemscase{active}{55}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: 44
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: 45
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: 46
@item active: 47
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: 48
@item active: 49
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
    <TD ALIGN=center>44</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>45</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>46</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>47</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>48</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>49</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>50</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>51</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>52</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>53</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>54</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>55</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{56}
\rtemsonecase{SEMAPHORE\_IDENT}{57}
\rtemsonecase{SEMAPHORE\_DELETE}{58}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{59}
\rtemscase{not available -- NO\_WAIT}
              {60}
\rtemscase{not available -- caller blocks}
              {61}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{62}
\rtemscase{task readied -- returns to caller}
              {63}
\rtemscase{task readied -- preempts caller}
              {64}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: 56
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: 57
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: 58
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: 59
@item not available -- NO_WAIT: 60
@item not available -- caller blocks: 61
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: 62
@item task readied -- returns to caller: 63
@item task readied -- preempts caller: 64
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>56</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>57</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>58</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>59</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>60</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>61</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>62</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>63</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>64</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{65}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{66}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{67}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {68}
\rtemscase{task readied -- returns to caller}
              {69}
\rtemscase{task readied -- preempts caller}
              {70}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{71}
\rtemscase{task readied -- returns to caller}
              {72}
\rtemscase{task readied -- preempts caller}
              {73}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{74}
\rtemscase{task readied -- returns to caller}
              {75}
\rtemscase{task readied -- preempts caller}
              {76}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{77}
\rtemscase{not available -- NO\_WAIT}
              {78}
\rtemscase{not available -- caller blocks}
              {79}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{80}
\rtemscase{messages flushed}{81}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: 65
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: 66
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: 67
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: 68
@item task readied -- returns to caller: 69
@item task readied -- preempts caller: 70
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: 71
@item task readied -- returns to caller: 72
@item task readied -- preempts caller: 73
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: 74
@item task readied -- returns to caller: 75
@item task readied -- preempts caller: 76
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: 77
@item not available -- NO_WAIT: 78
@item not available -- caller blocks: 79
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: 80
@item messages flushed: 81
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>65</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>66</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>67</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>68</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>69</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>70</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>71</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>72</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>73</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>74</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>75</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>76</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>77</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>78</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>79</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>80</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>81</TD></TR>

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
\rtemscase{no task readied}{82}
\rtemscase{task readied -- returns to caller}
              {83}
\rtemscase{task readied -- preempts caller}
              {84}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{85}
\rtemscase{available}{86}
\rtemscase{not available -- NO\_WAIT}{87}
\rtemscase{not available -- caller blocks}
              {88}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: 82
@item task readied -- returns to caller: 83
@item task readied -- preempts caller: 84
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: 85
@item available: 86
@item not available -- NO_WAIT: 87
@item not available -- caller blocks: 88
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
        <TD ALIGN=center>82</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>83</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>84</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>85</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>86</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>87</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>88</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{89}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{90}
\rtemscase{signal to self}{91}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {92}
\rtemscase{returns to preempting task}
              {93}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: 89
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: 90
@item signal to self: 91
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: 92
@item returns to preempting task: 93
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>89</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>90</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>91</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          92</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          93</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{94}
\rtemsonecase{PARTITION\_IDENT}{95}
\rtemsonecase{PARTITION\_DELETE}{96}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{97}
\rtemscase{not available}{98}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {98}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: 94
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: 95
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: 96
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: 97
@item not available: 98
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: 99
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>94</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>95</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>96</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>97</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>98</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>98</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{100}
\rtemsonecase{REGION\_IDENT}{101}
\rtemsonecase{REGION\_DELETE}{102}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{103}
\rtemscase{not available -- NO\_WAIT}
              {104}
\rtemscase{not available -- caller blocks}
              {105}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{106}
\rtemscase{task readied -- returns to caller}
              {107}
\rtemscase{task readied -- preempts caller}
              {108}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: 100
@end itemize

@item REGION_IDENT
@itemize -
@item only case: 101
@end itemize

@item REGION_DELETE
@itemize -
@item only case: 102
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: 103
@item not available -- NO_WAIT: 104
@item not available -- caller blocks: 105
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: 106
@item task readied -- returns to caller: 107
@item task readied -- preempts caller: 108
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>100</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>101</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>102</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>103</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          104</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          105</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>106</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          107</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          108</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{109}
\rtemsonecase{PORT\_IDENT}{110}
\rtemsonecase{PORT\_DELETE}{111}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {112}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {113}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: 109
@end itemize

@item PORT_IDENT
@itemize -
@item only case: 110
@end itemize

@item PORT_DELETE
@itemize -
@item only case: 111
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: 112
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: 113
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>109</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>110</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>111</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>112</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>113</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{114}
\rtemsonecase{IO\_OPEN}{115}
\rtemsonecase{IO\_CLOSE}{116}
\rtemsonecase{IO\_READ}{117}
\rtemsonecase{IO\_WRITE}{118}
\rtemsonecase{IO\_CONTROL}{119}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: 114
@end itemize

@item IO_OPEN
@itemize -
@item only case: 115
@end itemize

@item IO_CLOSE
@itemize -
@item only case: 116
@end itemize

@item IO_READ
@itemize -
@item only case: 117
@end itemize

@item IO_WRITE
@itemize -
@item only case: 118
@end itemize

@item IO_CONTROL
@itemize -
@item only case: 119
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>114</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>115</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>116</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>117</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>118</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>119</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{120}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{121}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{122}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{123}
\rtemscase{inactive}{124}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {125}
\rtemscase{conclude period -- caller blocks}
              {126}
\rtemscase{obtain status}{127}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: 120
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: 121
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: 122
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: 123
@item inactive: 124
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: 125
@item conclude period -- caller blocks: 126
@item obtain status: 127
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>120</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>121</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>122</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>123</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>124</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         125</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         126</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>127</TD></TR>
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
