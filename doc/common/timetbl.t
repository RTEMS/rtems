@c ******   This comment is here to remind you not to edit the timetbl.t
@c ******   in any directory but common.
@c 
@c Time Table Template
@c
@c  COPYRIGHT (c) 1988-2002.
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
\rtemsonecase{No Floating Point Contexts}{RTEMS_NO_FP_CONTEXTS}
\rtemsdirective{Floating Point Contexts}
\rtemscase{restore first FP task}{RTEMS_RESTORE_1ST_FP_TASK}
\rtemscase{save initialized, restore initialized}{RTEMS_SAVE_INIT_RESTORE_INIT}
\rtemscase{save idle, restore initialized}{RTEMS_SAVE_IDLE_RESTORE_INIT}
\rtemscase{save idle, restore idle}{RTEMS_SAVE_IDLE_RESTORE_IDLE}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item No Floating Point Contexts
@itemize -
@item only case: RTEMS_NO_FP_CONTEXTS
@end itemize
@item Floating Point Contexts
@itemize -
@item restore first FP task: RTEMS_RESTORE_1ST_FP_TASK
@item save initialized, restore initialized: RTEMS_SAVE_INIT_RESTORE_INIT
@item save idle, restore initialized: RTEMS_SAVE_IDLE_RESTORE_INIT
@item save idle, restore idle: RTEMS_SAVE_IDLE_RESTORE_INIT
@end itemize
@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>No Floating Point Contexts</STRONG></TD>
    <TD ALIGN=center>RTEMS_NO_FP_CONTEXTS</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Floating Point Contexts</TR>
    <TR><TD ALIGN=left><dd>restore first FP task</TD>
        <TD ALIGN=center>RTEMS_RESTORE_1ST_FP_TASK</TD>
    <TR><TD ALIGN=left><dd>save initialized, restore initialized</TD>
        <TD ALIGN=center>RTEMS_SAVE_INIT_RESTORE_INIT</TD>
    <TR><TD ALIGN=left><dd>save idle, restore initialized</TD>
        <TD ALIGN=center>RTEMS_SAVE_IDLE_RESTORE_INIT</TD>
    <TR><TD ALIGN=left><dd>save idle, restore idle</TD>
        <TD ALIGN=center>RTEMS_SAVE_IDLE_RESTORE_IDLE</TD>
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
\rtemsonecase{TASK\_CREATE}{RTEMS_TASK_CREATE_ONLY}
\rtemsonecase{TASK\_IDENT}{RTEMS_TASK_IDENT_ONLY}
\rtemsonecase{TASK\_START}{RTEMS_TASK_START_ONLY}
\rtemsdirective{TASK\_RESTART}
\rtemscase{calling task}{RTEMS_TASK_RESTART_CALLING_TASK}
\rtemscase{suspended task -- returns to caller}
              {RTEMS_TASK_RESTART_SUSPENDED_RETURNS_TO_CALLER}
\rtemscase{blocked task -- returns to caller}
              {RTEMS_TASK_RESTART_BLOCKED_RETURNS_TO_CALLER}
\rtemscase{ready task -- returns to caller}
              {RTEMS_TASK_RESTART_READY_RETURNS_TO_CALLER}
\rtemscase{suspended task -- preempts caller}
              {RTEMS_TASK_RESTART_SUSPENDED_PREEMPTS_CALLER}
\rtemscase{blocked task -- preempts caller}
              {RTEMS_TASK_RESTART_BLOCKED_PREEMPTS_CALLER}
\rtemscase{ready task -- preempts caller}
              {RTEMS_TASK_RESTART_READY_PREEMPTS_CALLER}
\rtemsdirective{TASK\_DELETE}
\rtemscase{calling task}{RTEMS_TASK_DELETE_CALLING_TASK}
\rtemscase{suspended task}{RTEMS_TASK_DELETE_SUSPENDED_TASK}
\rtemscase{blocked task}{RTEMS_TASK_DELETE_BLOCKED_TASK}
\rtemscase{ready task}{RTEMS_TASK_DELETE_READY_TASK}
\rtemsdirective{TASK\_SUSPEND}
\rtemscase{calling task}{RTEMS_TASK_SUSPEND_CALLING_TASK}
\rtemscase{returns to caller}{RTEMS_TASK_SUSPEND_RETURNS_TO_CALLER}
\rtemsdirective{TASK\_RESUME}
\rtemscase{task readied -- returns to caller}
              {RTEMS_TASK_RESUME_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_TASK_RESUME_TASK_READIED_PREEMPTS_CALLER}
\rtemsdirective{TASK\_SET\_PRIORITY}
\rtemscase{obtain current priority}
              {RTEMS_TASK_SET_PRIORITY_OBTAIN_CURRENT_PRIORITY}
\rtemscase{returns to caller}{RTEMS_TASK_SET_PRIORITY_RETURNS_TO_CALLER}
\rtemscase{preempts caller}{RTEMS_TASK_SET_PRIORITY_PREEMPTS_CALLER}
\rtemsdirective{TASK\_MODE}
\rtemscase{obtain current mode}{RTEMS_TASK_MODE_OBTAIN_CURRENT_MODE}
\rtemscase{no reschedule}{RTEMS_TASK_MODE_NO_RESCHEDULE}
\rtemscase{reschedule -- returns to caller}
              {RTEMS_TASK_MODE_RESCHEDULE_RETURNS_TO_CALLER}
\rtemscase{reschedule -- preempts caller}
              {RTEMS_TASK_MODE_RESCHEDULE_PREEMPTS_CALLER}
\rtemsonecase{TASK\_GET\_NOTE}{RTEMS_TASK_GET_NOTE_ONLY}
\rtemsonecase{TASK\_SET\_NOTE}{RTEMS_TASK_SET_NOTE_ONLY}
\rtemsdirective{TASK\_WAKE\_AFTER}
\rtemscase{yield -- returns to caller}
              {RTEMS_TASK_WAKE_AFTER_YIELD_RETURNS_TO_CALLER}
\rtemscase{yield -- preempts caller}
              {RTEMS_TASK_WAKE_AFTER_YIELD_PREEMPTS_CALLER}
\rtemsonecase{TASK\_WAKE\_WHEN}{RTEMS_TASK_WAKE_WHEN_ONLY}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset
 
@ifset use-ascii
@ifinfo
@itemize @bullet

@item TASK_CREATE
@itemize -
@item only case: RTEMS_TASK_CREATE_ONLY
@end itemize

@item TASK_IDENT
@itemize -
@item only case: RTEMS_TASK_IDENT_ONLY
@end itemize

@item TASK_START
@itemize -
@item only case: RTEMS_TASK_START_ONLY
@end itemize

@item TASK_RESTART
@itemize -
@item calling task: RTEMS_TASK_RESTART_CALLING_TASK
@item suspended task -- returns to caller: RTEMS_TASK_RESTART_SUSPENDED_RETURNS_TO_CALLER
@item blocked task -- returns to caller: RTEMS_TASK_RESTART_BLOCKED_RETURNS_TO_CALLER
@item ready task -- returns to caller: RTEMS_TASK_RESTART_READY_RETURNS_TO_CALLER
@item suspended task -- preempts caller: RTEMS_TASK_RESTART_SUSPENDED_PREEMPTS_CALLER
@item blocked task -- preempts caller: RTEMS_TASK_RESTART_BLOCKED_PREEMPTS_CALLER
@item ready task -- preempts caller: RTEMS_TASK_RESTART_READY_PREEMPTS_CALLER
@end itemize

@item TASK_DELETE
@itemize -
@item calling task: RTEMS_TASK_DELETE_CALLING_TASK
@item suspended task: RTEMS_TASK_DELETE_SUSPENDED_TASK
@item blocked task: RTEMS_TASK_DELETE_BLOCKED_TASK
@item ready task: RTEMS_TASK_DELETE_READY_TASK
@end itemize

@item TASK_SUSPEND
@itemize -
@item calling task: RTEMS_TASK_SUSPEND_CALLING_TASK
@item returns to caller: RTEMS_TASK_SUSPEND_RETURNS_TO_CALLER
@end itemize

@item TASK_RESUME
@itemize -
@item task readied -- returns to caller: RTEMS_TASK_RESUME_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_TASK_RESUME_TASK_READIED_PREEMPTS_CALLER
@end itemize

@item TASK_SET_PRIORITY
@itemize -
@item obtain current priority: RTEMS_TASK_SET_PRIORITY_OBTAIN_CURRENT_PRIORITY
@item returns to caller: RTEMS_TASK_SET_PRIORITY_RETURNS_TO_CALLER
@item preempts caller: RTEMS_TASK_SET_PRIORITY_PREEMPTS_CALLER
@end itemize

@item TASK_MODE
@itemize -
@item obtain current mode: RTEMS_TASK_MODE_OBTAIN_CURRENT_MODE
@item no reschedule: RTEMS_TASK_MODE_NO_RESCHEDULE
@item reschedule -- returns to caller: RTEMS_TASK_MODE_RESCHEDULE_RETURNS_TO_CALLER
@item reschedule -- preempts caller: RTEMS_TASK_MODE_RESCHEDULE_PREEMPTS_CALLER
@end itemize

@item TASK_GET_NOTE
@itemize -
@item only case: RTEMS_TASK_GET_NOTE_ONLY
@end itemize

@item TASK_SET_NOTE
@itemize -
@item only case: RTEMS_TASK_SET_NOTE_ONLY
@end itemize

@item TASK_WAKE_AFTER
@itemize -
@item yield -- returns to caller: RTEMS_TASK_WAKE_AFTER_YIELD_RETURNS_TO_CALLER
@item yield -- preempts caller: RTEMS_TASK_WAKE_AFTER_YIELD_PREEMPTS_CALLER
@end itemize

@item TASK_WAKE_WHEN
@itemize -
@item only case: RTEMS_TASK_WAKE_WHEN_ONLY
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>TASK_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_START</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_START_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESTART</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_CALLING_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_SUSPENDED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_BLOCKED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_READY_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_SUSPENDED_PREEMPTS_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_BLOCKED_PREEMPTS_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>ready task -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESTART_READY_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>RTEMS_TASK_DELETE_CALLING_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>suspended task</TD>
        <TD ALIGN=center>RTEMS_TASK_DELETE_SUSPENDED_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>blocked task</TD>
        <TD ALIGN=center>RTEMS_TASK_DELETE_BLOCKED_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>ready task</TD>
        <TD ALIGN=center>RTEMS_TASK_DELETE_READY_TASK</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SUSPEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>calling task</TD>
        <TD ALIGN=center>RTEMS_TASK_SUSPEND_CALLING_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_SUSPEND_RETURNS_TO_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_RESUME</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESUME_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_RESUME_TASK_READIED_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_SET_PRIORITY</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current priority</TD>
        <TD ALIGN=center>RTEMS_TASK_SET_PRIORITY_OBTAIN_CURRENT_PRIORITY</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_SET_PRIORITY_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_SET_PRIORITY_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_MODE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current mode</TD>
        <TD ALIGN=center>RTEMS_TASK_MODE_OBTAIN_CURRENT_MODE</TD></TR>
    <TR><TD ALIGN=left><dd>no reschedule</TD>
        <TD ALIGN=center>RTEMS_TASK_MODE_NO_RESCHEDULE</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_MODE_RESCHEDULE_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>reschedule -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_MODE_RESCHEDULE_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_GET_NOTE</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_GET_NOTE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_SET_NOTE</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_SET_NOTE_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TASK_WAKE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>yield -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_TASK_WAKE_AFTER_YIELD_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>yield -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_TASK_WAKE_AFTER_YIELD_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left><STRONG>TASK_WAKE_WHEN</STRONG></TD>
    <TD ALIGN=center>RTEMS_TASK_WAKE_WHEN_ONLY</TD></TR>
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
\rtemscase{returns to nested interrupt}{RTEMS_INTR_ENTRY_RETURNS_TO_NESTED}
\rtemscase{returns to interrupted task}
              {RTEMS_INTR_ENTRY_RETURNS_TO_INTERRUPTED_TASK}
\rtemscase{returns to preempting task}
              {RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK}
\rtemsdirective{Interrupt Exit Overhead}
\rtemscase{returns to nested interrupt}{RTEMS_INTR_EXIT_RETURNS_TO_NESTED}
\rtemscase{returns to interrupted task}
              {RTEMS_INTR_EXIT_RETURNS_TO_INTERRUPTED_TASK}
\rtemscase{returns to preempting task}
              {RTEMS_INTR_EXIT_RETURNS_TO_PREEMPTING_TASK}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item Interrupt Entry Overhead
@itemize -
@item returns to nested interrupt: RTEMS_INTR_ENTRY_RETURNS_TO_NESTED
@item returns to interrupted task: RTEMS_INTR_ENTRY_RETURNS_TO_INTERRUPTED_TASK
@item returns to preempting task: RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK
@end itemize

@item Interrupt Exit Overhead
@itemize -
@item returns to nested interrupt: RTEMS_INTR_EXIT_RETURNS_TO_NESTED
@item returns to interrupted task: RTEMS_INTR_EXIT_RETURNS_TO_INTERRUPTED_TASK
@item returns to preempting task: RTEMS_INTR_EXIT_RETURNS_TO_PREEMPTING_TASK
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
        <TD ALIGN=center>RTEMS_INTR_ENTRY_RETURNS_TO_NESTED</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>RTEMS_INTR_ENTRY_RETURNS_TO_INTERRUPTED_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>RTEMS_INTR_ENTRY_RETURNS_TO_PREEMPTING_TASK</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>Interrupt Exit Overhead</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>returns to nested interrupt</TD>
        <TD ALIGN=center>RTEMS_INTR_EXIT_RETURNS_TO_NESTED</TD></TR>
    <TR><TD ALIGN=left><dd>returns to interrupted task</TD>
        <TD ALIGN=center>RTEMS_INTR_EXIT_RETURNS_TO_INTERRUPTED_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>RTEMS_INTR_EXIT_RETURNS_TO_PREEMPTING_TASK</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset


@section Clock Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{CLOCK\_SET}{RTEMS_CLOCK_SET_ONLY}
\rtemsonecase{CLOCK\_GET}{RTEMS_CLOCK_GET_ONLY}
\rtemsonecase{CLOCK\_TICK}{RTEMS_CLOCK_TICK_ONLY}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item CLOCK_SET
@itemize -
@item only case: RTEMS_CLOCK_SET_ONLY
@end itemize

@item CLOCK_GET
@itemize -
@item only case: RTEMS_CLOCK_GET_ONLY
@end itemize

@item CLOCK_TICK
@itemize -
@item only case: RTEMS_CLOCK_TICK_ONLY
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>CLOCK_SET</STRONG></TD>
    <TD ALIGN=center>RTEMS_CLOCK_SET_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_GET</STRONG></TD>
    <TD ALIGN=center>RTEMS_CLOCK_GET_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>CLOCK_TICK</STRONG></TD>
    <TD ALIGN=center>RTEMS_CLOCK_TICK_ONLY</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Timer Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{TIMER\_CREATE}{RTEMS_TIMER_CREATE_ONLY}
\rtemsonecase{TIMER\_IDENT}{RTEMS_TIMER_IDENT_ONLY}
\rtemsdirective{TIMER\_DELETE}
\rtemscase{inactive}{RTEMS_TIMER_DELETE_INACTIVE}
\rtemscase{active}{RTEMS_TIMER_DELETE_ACTIVE}
\rtemsdirective{TIMER\_FIRE\_AFTER}
\rtemscase{inactive}{RTEMS_TIMER_FIRE_AFTER_INACTIVE}
\rtemscase{active}{RTEMS_TIMER_FIRE_AFTER_ACTIVE}
\rtemsdirective{TIMER\_FIRE\_WHEN}
\rtemscase{inactive}{RTEMS_TIMER_FIRE_WHEN_INACTIVE}
\rtemscase{active}{RTEMS_TIMER_FIRE_WHEN_ACTIVE}
\rtemsdirective{TIMER\_RESET}
\rtemscase{inactive}{RTEMS_TIMER_RESET_INACTIVE}
\rtemscase{active}{RTEMS_TIMER_RESET_ACTIVE}
\rtemsdirective{TIMER\_CANCEL}
\rtemscase{inactive}{RTEMS_TIMER_CANCEL_INACTIVE}
\rtemscase{active}{RTEMS_TIMER_CANCEL_ACTIVE}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item TIMER_CREATE
@itemize -
@item only case: RTEMS_TIMER_CREATE_ONLY
@end itemize

@item TIMER_IDENT
@itemize -
@item only case: RTEMS_TIMER_IDENT_ONLY
@end itemize

@item TIMER_DELETE
@itemize -
@item inactive: RTEMS_TIMER_DELETE_INACTIVE
@item active: RTEMS_TIMER_DELETE_ACTIVE
@end itemize

@item TIMER_FIRE_AFTER
@itemize -
@item inactive: RTEMS_TIMER_FIRE_AFTER_INACTIVE
@item active: RTEMS_TIMER_FIRE_AFTER_ACTIVE
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
    <TD ALIGN=center>RTEMS_TIMER_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>TIMER_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_TIMER_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_DELETE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_TIMER_DELETE_INACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_TIMER_DELETE_ACTIVE</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_AFTER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_TIMER_FIRE_AFTER_INACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_TIMER_FIRE_AFTER_ACTIVE</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_FIRE_WHEN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_TIMER_FIRE_WHEN_INACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_TIMER_FIRE_WHEN_ACTIVE</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_RESET</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_TIMER_RESET_INACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_TIMER_RESET_ACTIVE</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>TIMER_CANCEL</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_TIMER_CANCEL_INACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_TIMER_CANCEL_ACTIVE</TD></TR>
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
\rtemsonecase{SEMAPHORE\_CREATE}{RTEMS_SEMAPHORE_CREATE_ONLY}
\rtemsonecase{SEMAPHORE\_IDENT}{RTEMS_SEMAPHORE_IDENT_ONLY}
\rtemsonecase{SEMAPHORE\_DELETE}{RTEMS_SEMAPHORE_DELETE_ONLY}
\rtemsdirective{SEMAPHORE\_OBTAIN}
\rtemscase{available}{RTEMS_SEMAPHORE_OBTAIN_AVAILABLE}
\rtemscase{not available -- NO\_WAIT}
              {RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_NO_WAIT}
\rtemscase{not available -- caller blocks}
              {RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_CALLER_BLOCKS}
\rtemsdirective{SEMAPHORE\_RELEASE}
\rtemscase{no waiting tasks}{RTEMS_SEMAPHORE_RELEASE_NO_WAITING_TASKS}
\rtemscase{task readied -- returns to caller}
              {RTEMS_SEMAPHORE_RELEASE_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_SEMAPHORE_RELEASE_TASK_READIED_PREEMPTS_CALLER}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item SEMAPHORE_CREATE
@itemize -
@item only case: RTEMS_SEMAPHORE_CREATE_ONLY
@end itemize

@item SEMAPHORE_IDENT
@itemize -
@item only case: RTEMS_SEMAPHORE_IDENT_ONLY
@end itemize

@item SEMAPHORE_DELETE
@itemize -
@item only case: RTEMS_SEMAPHORE_DELETE_ONLY
@end itemize

@item SEMAPHORE_OBTAIN
@itemize -
@item available: RTEMS_SEMAPHORE_OBTAIN_AVAILABLE
@item not available -- NO_WAIT: RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_NO_WAIT
@item not available -- caller blocks: RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_CALLER_BLOCKS
@end itemize

@item SEMAPHORE_RELEASE
@itemize -
@item no waiting tasks: RTEMS_SEMAPHORE_RELEASE_NO_WAITING_TASKS
@item task readied -- returns to caller: RTEMS_SEMAPHORE_RELEASE_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_SEMAPHORE_RELEASE_TASK_READIED_PREEMPTS_CALLER
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_SEMAPHORE_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_SEMAPHORE_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>SEMAPHORE_DELETE</STRONG></TD>
    <TD ALIGN=center>RTEMS_SEMAPHORE_DELETE_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_OBTAIN</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_OBTAIN_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_NO_WAIT</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_OBTAIN_NOT_AVAILABLE_CALLER_BLOCKS</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SEMAPHORE_RELEASE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_RELEASE_NO_WAITING_TASKS</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_RELEASE_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_SEMAPHORE_RELEASE_TASK_READIED_PREEMPTS_CALLER</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Message Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{MESSAGE\_QUEUE\_CREATE}{RTEMS_MESSAGE_QUEUE_CREATE_ONLY}
\rtemsonecase{MESSAGE\_QUEUE\_IDENT}{RTEMS_MESSAGE_QUEUE_IDENT_ONLY}
\rtemsonecase{MESSAGE\_QUEUE\_DELETE}{RTEMS_MESSAGE_QUEUE_DELETE_ONLY}
\rtemsdirective{MESSAGE\_QUEUE\_SEND}
\rtemscase{no waiting tasks}
              {RTEMS_MESSAGE_QUEUE_SEND_NO_WAITING_TASKS}
\rtemscase{task readied -- returns to caller}
              {RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_PREEMPTS_CALLER}
\rtemsdirective{MESSAGE\_QUEUE\_URGENT}
\rtemscase{no waiting tasks}{RTEMS_MESSAGE_QUEUE_URGENT_NO_WAITING_TASKS}
\rtemscase{task readied -- returns to caller}
              {RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_PREEMPTS_CALLER}
\rtemsdirective{MESSAGE\_QUEUE\_BROADCAST}
\rtemscase{no waiting tasks}{RTEMS_MESSAGE_QUEUE_BROADCAST_NO_WAITING_TASKS}
\rtemscase{task readied -- returns to caller}
              {RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_PREEMPTS_CALLER}
\rtemsdirective{MESSAGE\_QUEUE\_RECEIVE}
\rtemscase{available}{RTEMS_MESSAGE_QUEUE_RECEIVE_AVAILABLE}
\rtemscase{not available -- NO\_WAIT}
              {RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_NO_WAIT}
\rtemscase{not available -- caller blocks}
              {RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS}
\rtemsdirective{MESSAGE\_QUEUE\_FLUSH}
\rtemscase{no messages flushed}{RTEMS_MESSAGE_QUEUE_FLUSH_NO_MESSAGES_FLUSHED}
\rtemscase{messages flushed}{RTEMS_MESSAGE_QUEUE_FLUSH_MESSAGES_FLUSHED}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item MESSAGE_QUEUE_CREATE
@itemize -
@item only case: RTEMS_MESSAGE_QUEUE_CREATE_ONLY
@end itemize

@item MESSAGE_QUEUE_IDENT
@itemize -
@item only case: RTEMS_MESSAGE_QUEUE_IDENT_ONLY
@end itemize

@item MESSAGE_QUEUE_DELETE
@itemize -
@item only case: RTEMS_MESSAGE_QUEUE_DELETE_ONLY
@end itemize

@item MESSAGE_QUEUE_SEND
@itemize -
@item no waiting tasks: RTEMS_MESSAGE_QUEUE_SEND_NO_WAITING_TASKS
@item task readied -- returns to caller: RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_PREEMPTS_CALLER
@end itemize

@item MESSAGE_QUEUE_URGENT
@itemize -
@item no waiting tasks: RTEMS_MESSAGE_QUEUE_URGENT_NO_WAITING_TASKS
@item task readied -- returns to caller: RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_PREEMPTS_CALLER
@end itemize

@item MESSAGE_QUEUE_BROADCAST
@itemize -
@item no waiting tasks: RTEMS_MESSAGE_QUEUE_BROADCAST_NO_WAITING_TASKS
@item task readied -- returns to caller: RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_PREEMPTS_CALLER
@end itemize

@item MESSAGE_QUEUE_RECEIVE
@itemize -
@item available: RTEMS_MESSAGE_QUEUE_RECEIVE_AVAILABLE
@item not available -- NO_WAIT: RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_NO_WAIT
@item not available -- caller blocks: RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS
@end itemize

@item MESSAGE_QUEUE_FLUSH
@itemize -
@item no messages flushed: RTEMS_MESSAGE_QUEUE_FLUSH_NO_MESSAGES_FLUSHED
@item messages flushed: RTEMS_MESSAGE_QUEUE_FLUSH_MESSAGES_FLUSHED
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>MESSAGE_QUEUE_DELETE</STRONG></TD>
    <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_DELETE_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_SEND</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_SEND_NO_WAITING_TASKS</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_SEND_TASK_READIED_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_URGENT</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_URGENT_NO_WAITING_TASKS</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_URGENT_TASK_READIED_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_BROADCAST</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_BROADCAST_NO_WAITING_TASKS</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_BROADCAST_TASK_READIED_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_RECEIVE_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_NO_WAIT</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>MESSAGE_QUEUE_FLUSH</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>no messages flushed</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_FLUSH_NO_MESSAGES_FLUSHED</TD></TR>
    <TR><TD ALIGN=left><dd>messages flushed</TD>
        <TD ALIGN=center>RTEMS_MESSAGE_QUEUE_FLUSH_MESSAGES_FLUSHED</TD></TR>

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
\rtemscase{no task readied}{RTEMS_EVENT_SEND_NO_TASK_READIED}
\rtemscase{task readied -- returns to caller}
              {RTEMS_EVENT_SEND_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_EVENT_SEND_TASK_READIED_PREEMPTS_CALLER}
\rtemsdirective{EVENT\_RECEIVE}
\rtemscase{obtain current events}{RTEMS_EVENT_RECEIVE_OBTAIN_CURRENT_EVENTS}
\rtemscase{available}{RTEMS_EVENT_RECEIVE_AVAILABLE}
\rtemscase{not available -- NO\_WAIT}{RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_NO_WAIT}
\rtemscase{not available -- caller blocks}
              {RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item EVENT_SEND
@itemize -
@item no task readied: RTEMS_EVENT_SEND_NO_TASK_READIED
@item task readied -- returns to caller: RTEMS_EVENT_SEND_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_EVENT_SEND_TASK_READIED_PREEMPTS_CALLER
@end itemize

@item EVENT_RECEIVE
@itemize -
@item obtain current events: RTEMS_EVENT_RECEIVE_OBTAIN_CURRENT_EVENTS
@item available: RTEMS_EVENT_RECEIVE_AVAILABLE
@item not available -- NO_WAIT: RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_NO_WAIT
@item not available -- caller blocks: RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS
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
        <TD ALIGN=center>RTEMS_EVENT_SEND_NO_TASK_READIED</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>RTEMS_EVENT_SEND_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>RTEMS_EVENT_SEND_TASK_READIED_PREEMPTS_CALLER</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EVENT_RECEIVE</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>obtain current events</TD>
        <TD ALIGN=center>RTEMS_EVENT_RECEIVE_OBTAIN_CURRENT_EVENTS</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>RTEMS_EVENT_RECEIVE_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_NO_WAIT</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>RTEMS_EVENT_RECEIVE_NOT_AVAILABLE_CALLER_BLOCKS</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset
@section Signal Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{SIGNAL\_CATCH}{RTEMS_SIGNAL_CATCH_ONLY}
\rtemsdirective{SIGNAL\_SEND}
\rtemscase{returns to caller}{RTEMS_SIGNAL_SEND_RETURNS_TO_CALLER}
\rtemscase{signal to self}{RTEMS_SIGNAL_SEND_SIGNAL_TO_SELF}
\rtemsdirective{EXIT ASR OVERHEAD}
\rtemscase{returns to calling task}
              {RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_CALLING_TASK}
\rtemscase{returns to preempting task}
              {RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_PREEMPTING_TASK}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet
@item SIGNAL_CATCH
@itemize -
@item only case: RTEMS_SIGNAL_CATCH_ONLY
@end itemize

@item SIGNAL_SEND
@itemize -
@item returns to caller: RTEMS_SIGNAL_SEND_RETURNS_TO_CALLER
@item signal to self: RTEMS_SIGNAL_SEND_SIGNAL_TO_SELF
@end itemize

@item EXIT ASR OVERHEAD
@itemize -
@item returns to calling task: RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_CALLING_TASK
@item returns to preempting task: RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_PREEMPTING_TASK
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>SIGNAL_CATCH</STRONG></TD>
    <TD ALIGN=center>RTEMS_SIGNAL_CATCH_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>SIGNAL_SEND</TD></TR>
    <TR><TD ALIGN=left><dd>returns to caller</TD>
        <TD ALIGN=center>RTEMS_SIGNAL_SEND_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>signal to self</TD>
        <TD ALIGN=center>RTEMS_SIGNAL_SEND_SIGNAL_TO_SELF</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>EXIT ASR OVERHEAD</TD></TR>
    <TR><TD ALIGN=left><dd>returns to calling task</TD>
        <TD ALIGN=center>
          RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_CALLING_TASK</TD></TR>
    <TR><TD ALIGN=left><dd>returns to preempting task</TD>
        <TD ALIGN=center>
          RTEMS_SIGNAL_EXIT_ASR_OVERHEAD_RETURNS_TO_PREEMPTING_TASK</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Partition Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PARTITION\_CREATE}{RTEMS_PARTITION_CREATE_ONLY}
\rtemsonecase{PARTITION\_IDENT}{RTEMS_PARTITION_IDENT_ONLY}
\rtemsonecase{PARTITION\_DELETE}{RTEMS_PARTITION_DELETE_ONLY}
\rtemsdirective{PARTITION\_GET\_BUFFER}
\rtemscase{available}{RTEMS_PARTITION_GET_BUFFER_AVAILABLE}
\rtemscase{not available}{RTEMS_PARTITION_GET_BUFFER_NOT_AVAILABLE}
\rtemsonecase{PARTITION\_RETURN\_BUFFER}
              {RTEMS_PARTITION_GET_BUFFER_NOT_AVAILABLE}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PARTITION_CREATE
@itemize -
@item only case: RTEMS_PARTITION_CREATE_ONLY
@end itemize

@item PARTITION_IDENT
@itemize -
@item only case: RTEMS_PARTITION_IDENT_ONLY
@end itemize

@item PARTITION_DELETE
@itemize -
@item only case: RTEMS_PARTITION_DELETE_ONLY
@end itemize

@item PARTITION_GET_BUFFER
@itemize -
@item available: RTEMS_PARTITION_GET_BUFFER_AVAILABLE
@item not available: RTEMS_PARTITION_GET_BUFFER_NOT_AVAILABLE
@end itemize

@item PARTITION_RETURN_BUFFER
@itemize -
@item only case: RTEMS_PARTITION_RETURN_BUFFER_ONLY
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PARTITION_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_PARTITION_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_PARTITION_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PARTITION_DELETE</STRONG></TD>
    <TD ALIGN=center>RTEMS_PARTITION_DELETE_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>PARTITION_GET_BUFFER</STRONG></TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>RTEMS_PARTITION_GET_BUFFER_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><dd>not available</TD>
        <TD ALIGN=center>RTEMS_PARTITION_GET_BUFFER_NOT_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><STRONG>PARTITION_RETURN_BUFFER</STRONG></TD>
    <TD ALIGN=center>RTEMS_PARTITION_GET_BUFFER_NOT_AVAILABLE</TD></TR>
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
\rtemsonecase{REGION\_CREATE}{RTEMS_REGION_CREATE_ONLY}
\rtemsonecase{REGION\_IDENT}{RTEMS_REGION_IDENT_ONLY}
\rtemsonecase{REGION\_DELETE}{RTEMS_REGION_DELETE_ONLY}
\rtemsdirective{REGION\_GET\_SEGMENT}
\rtemscase{available}{RTEMS_REGION_GET_SEGMENT_AVAILABLE}
\rtemscase{not available -- NO\_WAIT}
              {RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_NO_WAIT}
\rtemscase{not available -- caller blocks}
              {RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_CALLER_BLOCKS}
\rtemsdirective{REGION\_RETURN\_SEGMENT}
\rtemscase{no waiting tasks}{RTEMS_REGION_RETURN_SEGMENT_NO_WAITING_TASKS}
\rtemscase{task readied -- returns to caller}
              {RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_RETURNS_TO_CALLER}
\rtemscase{task readied -- preempts caller}
              {RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_PREEMPTS_CALLER}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item REGION_CREATE
@itemize -
@item only case: RTEMS_REGION_CREATE_ONLY
@end itemize

@item REGION_IDENT
@itemize -
@item only case: RTEMS_REGION_IDENT_ONLY
@end itemize

@item REGION_DELETE
@itemize -
@item only case: RTEMS_REGION_DELETE_ONLY
@end itemize

@item REGION_GET_SEGMENT
@itemize -
@item available: RTEMS_REGION_GET_SEGMENT_AVAILABLE
@item not available -- NO_WAIT: RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_NO_WAIT
@item not available -- caller blocks: RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_CALLER_BLOCKS
@end itemize

@item REGION_RETURN_SEGMENT
@itemize -
@item no waiting tasks: RTEMS_REGION_RETURN_SEGMENT_NO_WAITING_TASKS
@item task readied -- returns to caller: RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_RETURNS_TO_CALLER
@item task readied -- preempts caller: RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_PREEMPTS_CALLER
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>REGION_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_REGION_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_REGION_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>REGION_DELETE</STRONG></TD>
    <TD ALIGN=center>RTEMS_REGION_DELETE_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_GET_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>available</TD>
        <TD ALIGN=center>RTEMS_REGION_GET_SEGMENT_AVAILABLE</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- NO_WAIT</TD>
        <TD ALIGN=center>
          RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_NO_WAIT</TD></TR>
    <TR><TD ALIGN=left><dd>not available -- caller blocks</TD>
        <TD ALIGN=center>
          RTEMS_REGION_GET_SEGMENT_NOT_AVAILABLE_CALLER_BLOCKS</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>REGION_RETURN_SEGMENT</TD></TR>
    <TR><TD ALIGN=left><dd>no waiting tasks</TD>
        <TD ALIGN=center>RTEMS_REGION_RETURN_SEGMENT_NO_WAITING_TASKS</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- returns to caller</TD>
        <TD ALIGN=center>
          RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>task readied -- preempts caller</TD>
        <TD ALIGN=center>
          RTEMS_REGION_RETURN_SEGMENT_TASK_READIED_PREEMPTS_CALLER</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Dual-Ported Memory Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{PORT\_CREATE}{RTEMS_PORT_CREATE_ONLY}
\rtemsonecase{PORT\_IDENT}{RTEMS_PORT_IDENT_ONLY}
\rtemsonecase{PORT\_DELETE}{RTEMS_PORT_DELETE_ONLY}
\rtemsonecase{PORT\_INTERNAL\_TO\_EXTERNAL}
              {RTEMS_PORT_INTERNAL_TO_EXTERNAL_ONLY}
\rtemsonecase{PORT\_EXTERNAL\_TO\_INTERNAL}
              {RTEMS_PORT_EXTERNAL_TO_INTERNAL_ONLY}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item PORT_CREATE
@itemize -
@item only case: RTEMS_PORT_CREATE_ONLY
@end itemize

@item PORT_IDENT
@itemize -
@item only case: RTEMS_PORT_IDENT_ONLY
@end itemize

@item PORT_DELETE
@itemize -
@item only case: RTEMS_PORT_DELETE_ONLY
@end itemize

@item PORT_INTERNAL_TO_EXTERNAL
@itemize -
@item only case: RTEMS_PORT_INTERNAL_TO_EXTERNAL_ONLY
@end itemize

@item PORT_EXTERNAL_TO_INTERNAL
@itemize -
@item only case: RTEMS_PORT_EXTERNAL_TO_INTERNAL_ONLY
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>PORT_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_PORT_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_PORT_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_DELETE</STRONG></TD>
    <TD ALIGN=center>RTEMS_PORT_DELETE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_INTERNAL_TO_EXTERNAL</STRONG></TD>
    <TD ALIGN=center>RTEMS_PORT_INTERNAL_TO_EXTERNAL_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>PORT_EXTERNAL_TO_INTERNAL</STRONG></TD>
    <TD ALIGN=center>RTEMS_PORT_EXTERNAL_TO_INTERNAL_ONLY</TD></TR>

  </TABLE>
</CENTER>
@end html
@end ifset

@section I/O Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{IO\_INITIALIZE}{RTEMS_IO_INITIALIZE_ONLY}
\rtemsonecase{IO\_OPEN}{RTEMS_IO_OPEN_ONLY}
\rtemsonecase{IO\_CLOSE}{RTEMS_IO_CLOSE_ONLY}
\rtemsonecase{IO\_READ}{RTEMS_IO_READ_ONLY}
\rtemsonecase{IO\_WRITE}{RTEMS_IO_WRITE_ONLY}
\rtemsonecase{IO\_CONTROL}{RTEMS_IO_CONTROL_ONLY}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item IO_INITIALIZE
@itemize -
@item only case: RTEMS_IO_INITIALIZE_ONLY
@end itemize

@item IO_OPEN
@itemize -
@item only case: RTEMS_IO_OPEN_ONLY
@end itemize

@item IO_CLOSE
@itemize -
@item only case: RTEMS_IO_CLOSE_ONLY
@end itemize

@item IO_READ
@itemize -
@item only case: RTEMS_IO_READ_ONLY
@end itemize

@item IO_WRITE
@itemize -
@item only case: RTEMS_IO_WRITE_ONLY
@end itemize

@item IO_CONTROL
@itemize -
@item only case: RTEMS_IO_CONTROL_ONLY
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>IO_INITIALIZE</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_INITIALIZE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_OPEN</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_OPEN_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CLOSE</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_CLOSE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_READ</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_READ_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_WRITE</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_WRITE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>IO_CONTROL</STRONG></TD>
    <TD ALIGN=center>RTEMS_IO_CONTROL_ONLY</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@section Rate Monotonic Manager

@ifset use-tex
@tex
\centerline{\vbox{\offinterlineskip\halign{
\span\rtemstimetable
\rtemsonecase{RATE\_MONOTONIC\_CREATE}{RTEMS_RATE_MONOTONIC_CREATE_ONLY}
\rtemsonecase{RATE\_MONOTONIC\_IDENT}{RTEMS_RATE_MONOTONIC_IDENT_ONLY}
\rtemsonecase{RATE\_MONOTONIC\_CANCEL}{RTEMS_RATE_MONOTONIC_CANCEL_ONLY}
\rtemsdirective{RATE\_MONOTONIC\_DELETE}
\rtemscase{active}{RTEMS_RATE_MONOTONIC_DELETE_ACTIVE}
\rtemscase{inactive}{RTEMS_RATE_MONOTONIC_DELETE_INACTIVE}
\rtemsdirective{RATE\_MONOTONIC\_PERIOD}
\rtemscase{initiate period -- returns to caller}
              {RTEMS_RATE_MONOTONIC_PERIOD_INITIATE_PERIOD_RETURNS_TO_CALLER}
\rtemscase{conclude period -- caller blocks}
              {RTEMS_RATE_MONOTONIC_PERIOD_CONCLUDE_PERIOD_CALLER_BLOCKS}
\rtemscase{obtain status}{RTEMS_RATE_MONOTONIC_PERIOD_OBTAIN_STATUS}
\rtemsendtimetable
}}\hfil}
@end tex
@end ifset

@ifset use-ascii
@ifinfo
@itemize @bullet

@item RATE_MONOTONIC_CREATE
@itemize -
@item only case: RTEMS_RATE_MONOTONIC_CREATE_ONLY
@end itemize

@item RATE_MONOTONIC_IDENT
@itemize -
@item only case: RTEMS_RATE_MONOTONIC_IDENT_ONLY
@end itemize

@item RATE_MONOTONIC_CANCEL
@itemize -
@item only case: RTEMS_RATE_MONOTONIC_CANCEL_ONLY
@end itemize

@item RATE_MONOTONIC_DELETE
@itemize -
@item active: RTEMS_RATE_MONOTONIC_DELETE_ACTIVE
@item inactive: RTEMS_RATE_MONOTONIC_DELETE_INACTIVE
@end itemize

@item RATE_MONOTONIC_PERIOD
@itemize -
@item initiate period -- returns to caller: RTEMS_RATE_MONOTONIC_PERIOD_INITIATE_PERIOD_RETURNS_TO_CALLER
@item conclude period -- caller blocks: RTEMS_RATE_MONOTONIC_PERIOD_CONCLUDE_PERIOD_CALLER_BLOCKS
@item obtain status: RTEMS_RATE_MONOTONIC_PERIOD_OBTAIN_STATUS
@end itemize

@end itemize
@end ifinfo
@end ifset

@ifset use-html
@html
<CENTER>
  <TABLE COLS=2 WIDTH="80%" BORDER=2>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CREATE</STRONG></TD>
    <TD ALIGN=center>RTEMS_RATE_MONOTONIC_CREATE_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_IDENT</STRONG></TD>
    <TD ALIGN=center>RTEMS_RATE_MONOTONIC_IDENT_ONLY</TD></TR>
<TR><TD ALIGN=left><STRONG>RATE_MONOTONIC_CANCEL</STRONG></TD>
    <TD ALIGN=center>RTEMS_RATE_MONOTONIC_CANCEL_ONLY</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_DELETE</TD></TR>
    <TR><TD ALIGN=left><dd>active</TD>
        <TD ALIGN=center>RTEMS_RATE_MONOTONIC_DELETE_ACTIVE</TD></TR>
    <TR><TD ALIGN=left><dd>inactive</TD>
        <TD ALIGN=center>RTEMS_RATE_MONOTONIC_DELETE_INACTIVE</TD></TR>
<TR><TD ALIGN=left COLSPAN=2><STRONG>RATE_MONOTONIC_PERIOD</TD></TR>
    <TR><TD ALIGN=left><dd>initiate period -- returns to caller</TD>
        <TD ALIGN=center>
         RTEMS_RATE_MONOTONIC_PERIOD_INITIATE_PERIOD_RETURNS_TO_CALLER</TD></TR>
    <TR><TD ALIGN=left><dd>conclude period -- caller blocks</TD>
        <TD ALIGN=center>
         RTEMS_RATE_MONOTONIC_PERIOD_CONCLUDE_PERIOD_CALLER_BLOCKS</TD></TR>
    <TR><TD ALIGN=left><dd>obtain status</TD>
        <TD ALIGN=center>RTEMS_RATE_MONOTONIC_PERIOD_OBTAIN_STATUS</TD></TR>
  </TABLE>
</CENTER>
@end html
@end ifset

@tex
\global\advance \smallskipamount by 4pt
@end tex
