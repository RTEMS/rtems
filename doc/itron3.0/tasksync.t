@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the task-dependent synchronization
@c  manager.
@c
@c  $Id$
@c

@chapter Task-Dependent Synchronization Manager

@section Introduction

The 
task-dependent synchronization manager is ...

The services provided by the task-dependent synchronization manager are:

@itemize @bullet
@item @code{sus_tsk} - Suspend Other Task
@item @code{rsm_tsk} - Forcibly Resume Suspended Task Resume Suspended Task
@item @code{frsm_tsk} - Forcibly Resume Suspended Task
@item @code{slp_tsk} - Sleep Task Sleep Task with Timeout
@item @code{tslp_tsk} - Sleep Task with Timeout
@item @code{wup_tsk} - Wakeup Other Task
@item @code{can_wup} - Cancel Wakeup Request
@end itemize

@section Background

@section Operations

@section System Calls

This section details the task-dependent synchronization manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  sus_tsk
@c

@page
@subsection sus_tsk - Suspend Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER sus_tsk (
 ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  rsm_tsk
@c

@page
@subsection rsm_tsk - Forcibly Resume Suspended Task Resume Suspended Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rsm_tsk (
 ID tskid ER ercd =frsm_tsk 
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  frsm_tsk
@c

@page
@subsection frsm_tsk - Forcibly Resume Suspended Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =frsm_tsk (
 ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  slp_tsk
@c

@page
@subsection slp_tsk - Sleep Task Sleep Task with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER slp_tsk (
 ER ercd =tslp_tsk 
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:
This system call puts the issuing task (which was in RUN state) into WAIT state, causing the issuing task to sleep until wup_tsk is invoked.

Since the slp_tsk system call causes the issuing task to enter WAIT state, slp_tsk calls may not be nested.  It is possible, however, for another task to execute a sus_tsk on a task which has put itself in WAIT state using slp_tsk.  If this happens, the task will enter the combined WAIT-SUSPEND state.
No polling function for slp_tsk is provided.  A similar function can be implemented if necessary using can_wup.

@c
@c  tslp_tsk
@c

@subsection tslp_tsk - Sleep Task with Timeout
@subsection tslp_tsk - Sleep Task with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
ER ercd =tslp_tsk (
 TMO tmout
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
@code{EXXX} - 
@code{E_CTX} - Context error (issued from task-independent portions or a task in dispatch disabled state)

The tslp_tsk system call is the same as slp_tsk but with an additional timeout feature.  If a wup_tsk is issued before the period of time specified by tmout elapses, tslp_tsk will complete normally.  An E_TMOUT error will result if no wup_tsk is issued before the time specified by tmout expires.  Specifying tmout = TMO_FEVR = -1 can be used to set the timeout period to forever (no timeout).  In this case, tslp_tsk will function exactly the same as slp_tsk causing the issuing task to wait forever for wup_tsk to be issued.

Since the tslp_tsk system call causes the issuing task to enter WAIT state, tslp_tsk calls may not be nested.  It is possible, however, for another task to execute a sus_tsk on a task which has put itself in WAIT state using tslp_tsk.  If this happens, the task will enter the combined WAIT-SUSPEND state. 
If you simply wish to delay a task (make it wait for a while), use dly_tsk rather than tslp_tsk.

@c
@c  wup_tsk
@c

@subsection wup_tsk - Wakeup Other Task
@subsection wup_tsk - Wakeup Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
ER wup_tsk (
 ID tskid
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
@code{EXXX} - 
@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

If the specified task is not in the WAIT state caused by a slp_tsk or tslp_tsk, the wakeup request based on the wup_tsk call will be queued. In other words, a record will be kept that a wup_tsk has been issued for the specified task and no WAIT state will result even if slp_tsk or tslp_tsk is executed by the task later.  This is called queuing for wakeup request.

An E_QOVR error will result if wup_tsk is issued more than the maximum value allowed for the wakeup request queuing count (wupcnt).


@c
@c  can_wup
@c

@subsection can_wup - Cancel Wakeup Request
@subsection can_wup - Cancel Wakeup Request

@subheading CALLING SEQUENCE:

@ifset is-C
ER can_wup (
 INT *p_wupcnt, ID tskid
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
@code{EXXX} - 
@code{EN_RPAR} - A value outside the range supported by the issuing node and/or transmission packet format was returned as a return parameter (a value outside supported range was returned for wupcnt)

A task may specify itself by specifying tskid = TSK_SELF = 0.  Note, however, that an E_ID error will result if tskid = TSK_SELF = 0 is specified when this system call is issued from a task-independent portion.



