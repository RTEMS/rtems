@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the task-dependent synchronization
@c  manager.
@c
@c  $Id$
@c

@chapter Task-Dependent Synchronization Manager

@section Introduction

The task-dependent synchronization manager is designed to utilize those synchronization
functions already supported by tasks.  This includes functions that suspend tasks for a while and associated functions that release SUSPEND state, and synchronization functions which make tasks wait and wake them up.

The services provided by the task-dependent synchronization manager are:

@itemize @bullet
@item @code{sus_tsk} - Suspend Other Task
@item @code{rsm_tsk} - Resume Suspended Task
@item @code{frsm_tsk} - Forcibly Resume Suspended Task
@item @code{slp_tsk} - Sleep Task
@item @code{tslp_tsk} - Sleep Task with Timeout
@item @code{wup_tsk} - Wakeup Other Task
@item @code{can_wup} - Cancel Wakeup Request
@end itemize

@section Operations
@subsection Suspend Other Task
This call stops the execution of a task by putting it into a SUSPEND state.  This call is not able to specify itself, since this would end the flow of execution altogether.  If the task is already in a WAIT state, then SUSPEND is added to become WAIT-SUSPEND.  These modes are turned on and off separately, without affecting one another.  Furthermore, SUSPEND states can be nested, and tasks in a SUSPEND state are allocated resources as normal.

@subsection Resume Suspended Task
This operation restarts the execution of a task that was previously stopped by the SUSPEND OTHER TASK call.  Obviously, a task cannot specify itself using this call.  Since SUSPEND states can be nested, one call to RESUME releases only one SUSPEND.  Thus, it takes as many RESUMES as SUSPENDS to return the task to execution.

@subsection Forcibly Resume Suspended Task
This call has the same functionality as the previously mentioned Resume Suspended Task with one exception.  This call releases all nested SUSPENDS at once, which guarantees the task will return to execution.

@subsection Sleep Task
The Sleep Task operation causes the specified task to sleep until a Wakeup Task function is called.  This puts the task in a WAIT state.  WAIT states can not be nested, but can be combined with SUSPEND states as mentioned earlier.  

@subsection Sleep Task with Timeout
This function is identical to the Sleep Task function with an added timeout attribute.  If the timeout mark is reached before a Wakeup call is recieved, an error is generated.

@subsection Wakeup Other Task
The Wakeup Other Task call is used to release the WAIT state of a task.  These calls can be previously queued using the wupcnt value so that when the matching Sleep Task is executed, there will be no delay.

@subsection Cancel Wakeup Request
This function call resets the value of wupcnt to zero, thereby canceling all associated wakeup requests.  A call to self is acceptable for this operation, and may even be useful for monitoring certain situations.

@section System Calls

This section details the task-dependent synchronization manager's services. A subsection is dedicated to each of this manager's services and describes the calling sequence, related constants, usage, and status codes.


@c
@c  sus_tsk
@c

@page
@subsection sus_tsk - Suspend Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER sus_tsk(
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the task specified by tskid does not exist)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_OBJ} - Invalid object state (the specified task is in DORMANT state or the issuing task specified itself)

@code{E_QOVR} - Queuing or nesting overflow (the number of nesting levels given by suscnt went over the maximum allowed)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call suspends the execution of the task specified by tskid by putting it into SUSPEND state.

SUSPEND state is released by issuing the rsm_tsk or frsm_tsk system call. If the task specified to sus_tsk is already in WAIT state, it will be put in the combined WAIT-SUSPEND state by the execution of sus_tsk.  If wait conditions for the task are later fulfilled, it will enter SUSPEND state. If rsm_tsk is issued on the task, it will return to the WAIT state before the suspension.

Since SUSPEND state indicates the suspension of execution by a system call issued from another task, a task may not specify itself to this system call. An E_OBJ error will result if a task specifies itself.

If more than one sus_tsk call is issued to a task, that task will be put in multiple SUSPEND states.  This is called suspend request nesting.  When this is done, rsm_tsk must be issued the same number of times which sus_tsk was issued (suscnt) in order to return the task to its original state before the suspension.  This means it is possible to nest the pairs of sus_tsk and rsm_tsk.

The maximum number of times suspend requests may be nested, and even whether or not suspend request nesting (the ability to issue sus_tsk on the same task more than once) is even allowed, is implementation dependent.  Suspend request nesting is considered an extended function [level X] for which compatibility and connectivity are not guaranteed.

An E_QOVR error will result if sus_tsk is issued more than once on the same task on a system which does not support suspend request nesting or if it is issued more than the maximum number of times allowed.

@subheading NOTES:
A task which is suspended in addition to waiting for resources (such as waiting for a semaphore) can be allocated resources (such as semaphore counts) based on the same conditions as tasks which are not suspended.  Even when suspended, the allocation of resources is not delayed in any way.  Conditions concerning resource allocation and release of the wait state remain unchanged. In other words, SUSPEND state is completely independent of other processing and task states.  If it is desirable to delay the allocation of resources to a task which is suspended, the user should use chg_pri in conjunction with sus_tsk and rsm_tsk.

@c
@c  rsm_tsk
@c

@page
@subsection rsm_tsk - Resume Suspended Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rsm_tsk(
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the task specified by tskid does not exist)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_OBJ} - Invalid object state (the target task is not in SUSPEND state (including when it is DORMANT or when the issuing task specifies itself))

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call releases SUSPEND state of the task specified by tskid.  Specifically, it causes SUSPEND state to be released and the execution of the specified task to resume when the task has been suspended by the prior execution of sus_tsk.
If the specified task is in WAIT-SUSPEND state, the execution of rsm_tsk only releases the SUSPEND state, and the task will become WAIT state.

A task cannot specify itself to this system call.  An E_OBJ error will result if a task specifies itself.

Rsm_tsk only releases one suspend request from the suspend request nest (suscnt).  Accordingly, if more than one sus_tsk has been issued on the task in question (suscnt >= 2), that task will remain suspended even after the execution of rsm_tsk is completed.

@subheading NOTES:
It is implementation dependent which location in the ready queue a task returns to after the task which has been suspended from RUN or READY state is resumed by rsm_tsk.

@c
@c  frsm_tsk
@c

@page
@subsection frsm_tsk - Forcibly Resume Suspended Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =frsm_tsk(
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the task specified by tskid does not exist)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_OBJ} - Invalid object state (the target task is not in SUSPEND state (including when it is DORMANT or when the issuing task specifies itself))

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call releases SUSPEND state of the task specified by tskid.  Specifically, it causes SUSPEND state to be released and the execution of the specified task to resume when the task has been suspended by the prior execution of sus_tsk.  If the specified task is in WAIT-SUSPEND state, the execution of rsm_tsk only releases the SUSPEND state, and the task will become WAIT state.

A task cannot specify itself to this system call.  An E_OBJ error will result if a task specifies itself.

Frsm_tsk will clear all suspend requests (suscnt = 0) even if more than one sus_tsk has been issued (suscnt >= 2) on the same task.  In other words, SUSPEND state is guaranteed to be released, and execution will resume unless the task in question had been in combined WAIT-SUSPEND state.

@subheading NOTES:
It is implementation dependent which location in the ready queue a task returns to after the task which has been suspended from RUN or READY state is resumed by frsm_tsk.


@c
@c  slp_tsk
@c

@page
@subsection slp_tsk - Sleep Task Sleep Task with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER slp_tsk( void );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_PAR} - Parameter error (a timeout value -2 or less was specified)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while waiting)  

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a task in dispatch disabled state)

@subheading DESCRIPTION:

This system call puts the issuing task (which was in RUN state) into WAIT state, causing the issuing task to sleep until wup_tsk is invoked.

@subheading NOTES:
Since the slp_tsk system call causes the issuing task to enter WAIT state, slp_tsk calls may not be nested.  It is possible, however, for another task to execute a sus_tsk on a task which has put itself in WAIT state using slp_tsk.  If this happens, the task will enter the combined WAIT-SUSPEND state.

No polling function for slp_tsk is provided.  A similar function can be implemented if necessary using can_wup.

@c
@c  tslp_tsk
@c

@page
@subsection tslp_tsk - Sleep Task with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tslp_tsk(
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_PAR} - Parameter error (a timeout value -2 or less was specified)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while waiting)  

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a task in dispatch disabled state)

@subheading DESCRIPTION:
The tslp_tsk system call is the same as slp_tsk but with an additional timeout feature.  If a wup_tsk is issued before the period of time specified by tmout elapses, tslp_tsk will complete normally.  An E_TMOUT error will result if no wup_tsk is issued before the time specified by tmout expires.  Specifying tmout = TMO_FEVR = -1 can be used to set the timeout period to forever (no timeout).  In this case, tslp_tsk will function exactly the same as slp_tsk causing the issuing task to wait forever for wup_tsk to be issued.

@subheading NOTES:
Since the tslp_tsk system call causes the issuing task to enter WAIT state, tslp_tsk calls may not be nested.  It is possible, however, for another task to execute a sus_tsk on a task which has put itself in WAIT state using tslp_tsk.  If this happens, the task will enter the combined WAIT-SUSPEND state. 

If you simply wish to delay a task (make it wait for a while), use dly_tsk rather than tslp_tsk.

@c
@c  wup_tsk
@c

@page
@subsection wup_tsk - Wakeup Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER wup_tsk(
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the task specified by tskid does not exist)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_OBJ} - Invalid object state (the specified task is in DORMANT state or the issuing task specified itself)

@code{E_QOVR} - Queuing or nesting overflow (wakeup request queuing count will exceed the maximum value allowed for wupcnt)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call releases the WAIT state of the task specified by tskid caused by the execution of slp_tsk or tslp_tsk.

A task cannot specify itself in this system call.  An E_OBJ error will result if a task specifies itself.

If the specified task is not in the WAIT state caused by a slp_tsk or tslp_tsk, the wakeup request based on the wup_tsk call will be queued. In other words, a record will be kept that a wup_tsk has been issued for the specified task and no WAIT state will result even if slp_tsk or tslp_tsk is executed by the task later.  This is called queuing for wakeup request.

@subheading NOTES:
Wakeup requests are queued as follows.  A wakeup request queuing count (wupcnt) is kept in the TCB for each task.  Initially (when sta_tsk is executed) the value of wupcnt is 0.  Executing wup_tsk on a task which is not waiting for a wakeup increments the wakeup request queuing count by one for the specified task.  If slp_tsk or tslp_tsk is executed on that task, its wakeup request queuing count will be decremented by one.  If the task with wakeup request queuing count = 0 executes slp_tsk or tslp_tsk, that task will be put in WAIT state rather than decrementing the wakeup request queuing count.

It is always possible to queue at least one wup_tsk (wupcnt = 1); the maximum allowable number for the wakeup request queuing count (wupcnt) is implementation dependent, and may be any number higher than or equal to one. In other words, while the first wup_tsk issued to a task which is not waiting for a wakeup will not result in an error, it is implementation dependent whether or not any further wup_tsk calls on the same task will result in an error.  The ability to queue more than one wakeup request is considered an extended function [level X] for which compatibility and connectivity are not guaranteed.

An E_QOVR error will result if wup_tsk is issued more than the maximum value allowed for the wakeup request queuing count (wupcnt).


@c
@c  can_wup
@c

@page
@subsection can_wup - Cancel Wakeup Request

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER can_wup(
  INT *p_wupcnt,
  ID tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the task specified by tskid does not exist)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_OBJ} - Invalid object state (the target task is in DORMANT state)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@code{EN_RPAR} - A value outside the range supported by the issuing node and/or transmission packet format was returned as a return parameter (a value outside supported range was returned for wupcnt)

@subheading DESCRIPTION:
This system call returns the wakeup request queuing count (wupcnt) for the task specified by tskid while canceling all associated wakeup requests. Specifically, it resets the wakeup request queuing count (wupcnt) to 0. 

A task may specify itself by specifying tskid = TSK_SELF = 0.  Note, however, that an E_ID error will result if tskid = TSK_SELF = 0 is specified when this system call is issued from a task-independent portion.

@subheading NOTES:
An EN_RPAR error will result if the number of bits used on the target node is larger than that used on the requesting node, and if a value not supported by the requesting node is returned for wupcnt.

This system call can be used to determine whether or not processing has ended within a certain period when a task should periodically waken up by wup_tsk and do some processing.  In other words, if a task monitoring the progress of its processing issues can_wup before issuing a slp_tsk after finishing processing associated with a previous wakeup request, and if wupcnt, one of can_wup's return parameters, is equal to or greater than one, it indicates that the processing for the previous wakeup request does not complete within a required time.  This allows the monitoring task to take actions against processing delays.


