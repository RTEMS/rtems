@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the task
@c  manager.
@c
@c  $Id$
@c

@chapter Task Manager

@section Introduction

The task manager is used to directly control and access the state of tasks.  Included among these are functions for creating, deleting, starting and terminating tasks, for releasing the WAIT state of tasks, for enabling/disabling task dispatching, for changing task priority levels, for rotatingtasks on the ready queue, and for accessing task state.

The services provided by the task manager are:

@itemize @bullet
@item @code{cre_tsk} - Create Task
@item @code{del_tsk} - Delete Task
@item @code{sta_tsk} - Start Task
@item @code{ext_tsk} - Exit Issuing Task
@item @code{exd_tsk} - Exit and Delete Issuing Task
@item @code{ter_tsk} - Terminate Other Task
@item @code{dis_dsp} - Disable Dispatch
@item @code{ena_dsp} - Enable Dispatch
@item @code{chg_pri} - Change Task Priority
@item @code{rot_rdq} - Rotate Tasks on the Ready Queue
@item @code{rel_wai} - Release Wait of Other Task
@item @code{get_tid} - Get Task Identifier
@item @code{ref_tsk} - Reference Task Status
@end itemize

@section Background

@subsection Task Definition
Many definitions of a task have been proposed in computer literature. Unfortunately, none of these definitions encompasses all facets of the concept in a manner which is operating system independent. Several of the more common definitions are provided to enable each user to select a definition which best matches their own experience and understanding of the task concept: 

@itemize @bullet
@item a "dispatchable" unit. 
@item an entity to which the processor is allocated. 
@item an atomic unit of a real-time, multiprocessor system. 
@item single threads of execution which concurrently compete for resources. 
@item a sequence of closely related computations which can execute concurrently with other computational sequences. 
@item From our implementation perspective, a task is the smallest thread of execution which can compete on its own for system resources. A task is manifested by the existence of a task control block (TCB). 
@end itemize

@subsection Task Manager Task Control Block
 The Task Control Block (TCB) is a defined data structure which contains all the information that is pertinent to the execution of a task. During system initialization, implementation reserves a TCB for each task configured. A TCB is allocated upon creation of the task and is returned to the TCB free list upon deletion of the task. 

The TCB's elements are modified as a result of system calls made by the application in response to external and internal stimuli. The TCB contains a task's name, ID, current priority, current and starting states, TCB user extension pointer, scheduling control structures, as well as data required by a blocked task. 

A task's context is stored in the TCB when a task switch occurs. When the task regains control of the processor, its context is restored from the TCB. When a task is restarted, the initial state of the task is restored from the starting context area in the task's TCB. 

@subsection T_CTSK Structure
The T_CTSK structure contains detailed information necessary to create the task. Such task attributes, start address, priority and stack size.  

@example
typedef struct t_ctsk @{
  VP    exinf;     /* extended information */
  ATR   tskatr;    /* task attributes */
  FP    task;      /* task start address */
  PRI   itskpri;   /* initial task priority */
  INT   stksz;     /* stack size */
  /* additional implementation dependent information may be included */
@} T_CTSK;

@end example

@subsection Task Manager Task States 
A task may exist in one of the following five states: 

@itemize @bullet
@item RUN - Currently scheduled to the CPU 
@item READY - May be scheduled to the CPU 
@item Wait - Unable to be scheduled to the CPU
@itemize @bullet
@item (Specific) WAIT - The task is issued a command to wait on a condition
@item SUSPEND - Another task suspended execution of the task
@item WAIT-SUSPEND - Both the WAIT and SUSPEND states apply
@end itemize
@item DORMANT - Created task that is not started 
@item NON-EXISTENT - Uncreated or deleted task 
@end itemize

An active task may occupy the RUN, READY, Wait or DORMANT state, otherwise the task is considered NON-EXISTENT. One or more tasks may be active in the system simultaneously. Multiple tasks communicate, synchronize, and compete for system resources with each other via system calls. The multiple tasks appear to execute in parallel, but actually each is dispatched to the CPU for periods of time determined by the scheduling algorithm. The scheduling of a task is based on its current state and priority. 

@subsection Task Manager Task Priority 
A task's priority determines its importance in relation to the other tasks executing on the same processor. Our implementation supports 255 levels of priority ranging from 1 to 255. Tasks of numerically smaller priority values are more important tasks than tasks of numerically larger priority values. For example, a task at priority level 5 is of higher privilege than a task at priority level 10. There is no limit to the number of tasks assigned to the same priority. 

Each task has a priority associated with it at all times. The initial value of this priority is assigned at task creation time. The priority of a task may be changed at any subsequent time. 

Priorities are used by the scheduler to determine which ready task will be allowed to execute. In general, the higher the logical priority of a task, the more likely it is to receive processor execution time. 

@section Operations

@subsection Task Manager Creating Tasks
The cre_tsk directive creates a task specified by tskid. Specifically, a TCB (Task Control Block) is allocated for the task to be created, and initialized according to accompanying parameter values of itskpri, task, stksz, etc.  A stack area is also allocated for the task based on the parameter stksz.

@subsection Task Manager Starting and Restarting Tasks 
The sta_tsk directive starts the task specified by tskid.  Specifically, it changes the state of the task specified by tskid from DORMANT into RUN/READY.  This enables the task to compete, based on its current priority, for the processor and other system resources. Any actions, such as suspension or change of priority, performed on a task prior to starting it are nullified when the task is started. 

Stacd can be used to specify parameters to be passed to the task when it is started.  This parameter can be read by the task being started, and may be used for transmitting simple messages.

The task priority on starting the task is given by the initial task priority parameter (itskpri) specified when the task was created.

Start request is not queued in this this system call.  In other words, if this system call is issued when the target task is not in DORMANT state, the system call will be ignored, and an E_OBJ error returned to the issuing task.

If cre_tsk [level EN] is not implemented on a system, tasks are created statically when the system is started.  Parameters required for creating a task, such as task starting address (task) and initial task priority (itskpri) are also specified statically at system startup.

@subsection Task Manager Suspending and Resuming Tasks 
The sus_tsk directive suspends the execution of the task specified by tskid by putting it into SUSPEND state. SUSPEND state is released by issuing the rsm_tsk or frsm_tsk system call.

If the task specified to sus_tsk is already in WAIT state, it will be put in the combined WAIT-SUSPEND state by the execution of sus_tsk.  If wait conditions for the task are later fulfilled, it will enter SUSPEND state. If rsm_tsk is issued on the task, it will return to the WAIT state before the suspension.

Both rsm_tsk and fsm_tsk system calls release SUSPEND state of the task specified by tskid.  Specifically, they cause SUSPEND state to be released and the execution of the specified task to resume when the task has been suspended by the prior execution of sus_tsk.

If the specified task is in WAIT-SUSPEND state, the execution of rsm_tsk only releases the SUSPEND state, and the task will become WAIT state.
 
@subsection Task Manager Changing Task Priority 
The chg_pri system call changes the current priority of the task specified by tskid to the value specified by tskpri.

A task may specify itself by specifying tskid = TSK_SELF = 0.  Note, however, that an E_ID error will result if tskid = TSK_SELF = 0 is specified to a system call issued from a task-independent portion.

The priority set by this system call remains in effect until the task exits.  Once a task enters DORMANT state, its priority prior to exiting is lost.  When a task which enters DORMANT state restarts, the initial task priority (itskpri) specified at task creation or at system startup will be used.

@subsection Task Manager Task Deletion 
The del_tsk system call deletes the task specified by tskid.  Specifically, it changes the state of the task specified by tskid from DORMANT into NON-EXISTENT (a virtual state not existing on the system), and then clears the TCB and releases stack. An E_OBJ error results if this system call is used on a task which is not DORMANT.

After deletion, another task having the same ID number can be created.

The exd_tsk system call causes the issuing task to exit and then delete itself.

When a task exits, that task does not automatically release all the resources (memory blocks, semaphores, etc.) which it had secured prior to the call.  It is the user's responsibility to see to it that all resources are released beforehand.

@section System Calls

This section details the task manager's services.
A subsection is dedicated to each of this manager's services and describes the calling sequence, related constants, usage, and status codes.


@c
@c  cre_tsk
@c

@page
@subsection cre_tsk - Create Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_tsk(
  ID tskid,
  T_CTSK *pk_ctsk
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_NOMEM} - Insufficient memory (Memory for control block and/or user stack cannot be allocated)

@code{E_ID} - Invalid ID Number (tskid was invalid or could not be used)

@code{E_RSATR} - Reserved attribute (tskatr was invalid or could not be used)

@code{E_OBJ} - Invalid object state (a task of the same ID already exists)

@code{E_OACV} - Object access violation (A tskid less than -4 was specified from a user task.  This is implementation dependent.)

@code{E_PAR} -  Parameter error (pk_ctsk, task, itskpri and/or stksz is invalid)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@code{EN_PAR} -  A value outside the range supported by the target node and/or transmission packet format was specified as a parameter (a value outside supported range was specified for exinf, tskatr, task, itskpri and/or stksz)

@subheading DESCRIPTION:
This system call creates the task specified by tskid.  Specifically, a TCB (Task Control Block) is allocated for the task to be created, and initialized according to accompanying parameter values of itskpri, task, tksz, etc.  A stack area is also allocated for the task based on the parameter stksz.

@subheading NOTES:
User tasks have positive ID numbers, while system tasks have negative ID numbers.  User tasks cannot access system objects (objects having negative ID numbers).
  
The new task created by this system call will be put in DORMANT state.

Extended information (exinf) has been added.  This allows the user to include additional information about task attributes.  If a larger region is desired for including user information, the user should allocate memory area and set the address of the memory packet to exinf.

Multiprocessing is not supported. Thus none of the "EN_" status codes will be returned. 


@c
@c  del_tsk
@c

@page
@subsection del_tsk - Delete Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_tsk(
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

@code{E_OBJ} -  Invalid object state (the target task is not in DORMANT state)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call deletes the task specified by tskid.  Specifically, it changes the state of the task specified by tskid from DORMANT into NON-EXISTENT (a virtual state not existing on the system), and then clears the TCB and releases stack. An E_OBJ error results if this system call is used on a task which is not DORMANT.

After deletion, another task having the same ID number can be created.

@subheading NOTES:
A task cannot delete itself by this system call.  An E_OBJ error will result if a task specifies itself, since such a task cannot be DORMANT. Use the exd_tsk system call rather than this one when a task needs to delete itself.


@c
@c  sta_tsk
@c

@page
@subsection sta_tsk - Start Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER sta_tsk(
  ID tskid,
  INT stacd
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

@code{E_OBJ} - Invalid object state (the target task is not in DORMANT state)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@code{EN_PAR} - A value outside the range supported by the target node and/or transmission packet format was specified as a parameter (a value outside supported range was specified for stacd)

@subheading DESCRIPTION:
This system call starts the task specified by tskid.  Specifically, it changes the state of the task specified by tskid from DORMANT into RUN/READY.

Stacd can be used to specify parameters to be passed to the task when it is started.  This parameter can be read by the task being started, and may be used for transmitting simple messages.

The task priority on starting the task is given by the initial task priority parameter (itskpri) specified when the task was created.

Start request is not queued in this this system call.  In other words, if this system call is issued when the target task is not in DORMANT state, the system call will be ignored, and an E_OBJ error returned to the issuing task.

If cre_tsk [level EN] is not implemented on a system, tasks are created statically when the system is started.  Parameters required for creating a task, such as task starting address (task) and initial task priority (itskpri) are also specified statically at system startup.

@subheading NOTES:


@c
@c  ext_tsk
@c

@page
@subsection ext_tsk - Exit Issuing Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void ext_tsk(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_CTX} - Context error (issued from task-independent portions or a task in dispatch disabled state)

 * System call may detect this error.  The error is not returned to the context issuing the system call.  Error codes therefore cannot be returned directly as a return parameter of the system call.  The behavior on error detection is implementation dependent.


@subheading DESCRIPTION:
This system call causes the issuing task to exit, changing the state of the task into the DORMANT state.

@subheading NOTES:
When a task exits due to ext_tsk, that task does not automatically release all the resources (memory blocks, semaphores, etc.) which it had obtained prior to the system call.  It is the user's responsibility that all resources are released beforehand.

Ext_tsk is a system call which does not return to the issuing context. Accordingly, even if an error code is returned on detection of some error, it is normal for tasks making this system call not to perform any error checking, and it is in fact possible that a program could run out of control.  For this reason, even if an error is detected upon issuing this system call, the error is not returned to the task which issued the system call.  If information on detected errors is required it should be left in a messagebuffer used as an error log.

In principle, information concerning a task recorded in the TCB, such as task priority, is reset whenever a task is placed in DORMANT state.  For example, its task priority after being restarted would be reset to the initial task priority (itskpri) specified by cre_tsk when it was first created, even if a task's priority was changed using chg_pri, then that task exits using ext_tsk, but later started by sta_tsk.  Task priority does not return to what it was when ext_tsk was executed.


@c
@c  exd_tsk
@c

@page
@subsection exd_tsk - Exit and Delete Issuing Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void exd_tsk(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_CTX} - Context error (issued from task-independent portions or a task in dispatch disabled state)

 * System call may detect the following error.  The error is not returned to the context issuing the system call even.  Error codes therefore cannot be returned directly as a return parameter of the system call.  The behavior on error detection is implementation dependent.

@subheading DESCRIPTION:
This system call causes the issuing task to exit and then delete itself.  In other words the state of the issuing task changes into the NON-EXISTENT (a virtual state not existing on the system).

@subheading NOTES:
When a task exits with exd_tsk, that task does not automatically release all the resources (memory blocks, semaphores, etc.) which it had secured prior to the call.  It is the user's responsibility to see to it that all resources are released beforehand.

Exd_tsk is a system call which does not return any parameters to the original issuing context.  Accordingly, even if an error code is returned on detection of some error, it is normal for tasks making this system call not to perform any error checking, and it is in fact possible that a program could run out of control.  For this reason, even if an error is detected upon making this system call, it is supposed that the error is not returned to the task which issued the system call.  If information on detected errors is required it should be left in a messagebuffer used as an error log.


@c
@c  ter_tsk
@c

@page
@subsection ter_tsk - Terminate Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ter_tsk(
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

@code{E_OBJ} - Invalid object state (the target task is already in DORMANT state or a task invalidly specified itself)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call forcibly terminates the task specified by tskid.  That is, it changes the state of the task specified by tskid into DORMANT.

Even if the target task is in wait state (including SUSPEND state), its wait state will be released and then it will be terminated.  If the target task is on a queue of some sort (such as waiting for a semaphore), it will be removed from that queue by ter_tsk.

A task cannot specify the issuing task in this system call.  An E_OBJ error will result if a task specifies itself.

There is an intermediate state waiting for the response (TR packet or TA packet) from the target node after executing the system call to access the other node and making a request (sending a TP packet) to the node. This state is called the "connection function response wait (TTW_NOD)" state. The ter_tsk system call may specify tasks which are in the connection function response wait state.  Tasks which are waiting for objects (such as a semaphore) on another node may also be specified to this system call. In such cases, ter_tsk will halt any system calls accessing other nodes which have been issued by the task to be terminated.

@subheading NOTES:
When a task is terminated by ter_tsk, that task does not automatically release all the resources (memory blocks, semaphores, etc.) which it had obtained prior to the call.  It is the user's responsibility to see to it that all resources are released beforehand.

In principle, information concerning a task recorded in the TCB, such as task priority, is reset whenever a task is placed in DORMANT state.  For example, its task priority after being restarted would be reset to the initial task priority (itskpri) specified by cre_tsk when it was first created, even if a task's priority was changed using chg_pri, then that task is terminated by ter_tsk, but later started by sta_tsk.  Task priority does not return to what it was when ter_tsk was executed.


@c
@c  dis_dsp
@c

@page
@subsection dis_dsp - Disable Dispatch

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER dis_dsp(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_CTX} - Context error (issued from task-independent portions or issued after execution of loc_cpu)

@subheading DESCRIPTION:
This system call disables task dispatching.  Dispatching will remain disabled after this call is issued until a subsequent call to ena_dsp is issued. The status of the issuing task will not be allowed to be changed to READY from the RUN.  It cannot be changed into WAIT, either.  However, since external interrupt is not disabled, interrupt handlers are allowed to run even when dispatching has been disabled.  While an executing task may be preempted by an interrupt handler with dispatching disabled, there is no possibility that it will be preempted by another task.

The following operations occur during the time dispatching is disabled.
@itemize @bullet
@item Even in a situation where normally a task issuing dis_dsp should be preempted by a system call issued by an interrupt handler or by the task issuing dis_dsp, the task that should normally be executed is not dispatched.  Instead, dispatching of this task is delayed until dispatch disabled state is cleared by ena_dsp.

@item If an interrupt handler invoked during dispatch disabled state issues sus_tsk for a running task (one that executed dis_dsp) to put it in SUSPEND state, or ter_tsk to put it in DORMANT state, the task transition is delayed until dispatch disabled state is cleared.

@item An E_CTX error will result if the task which has executed dis_dsp issues any system calls (such as slp_tsk or wai_sem) capable of putting an issuing task into WAIT state.

@item An EN_CTXID error will result if a task which has executed dis_dsp attempts to operate on objects on another node (that is, if the ID parameter of the system call issued refers to an object on another node).

@item TSS_DDSP will be returned as sysstat if system status is referenced using ref_sys.
@end itemize
No error will result if a task already in dispatch disable state issues dis_dsp.  It only keeps dispatch disabled state.  No matter how many times dis_dsp has been issued, a single ena_dsp enables dispatching again.  It is therefore for the user to determine what to do with nested pairs of dis_dsp and ena_dsp.

An E_CTX error will result if dis_dsp is issued when both interrupt and dispatching are disabled with loc_cpu.  (For details, see the description of loc_cpu.)

@subheading NOTES:
A running task cannot enter DORMANT or NON-EXISTENT state while dispatching is disabled.  An E_CTX error will result if an running task issues either ext_tsk or exd_tsk while interrupt and dispatching are disabled.  Note however that since both ext_tsk and exd_tsk are system calls which do not return to their original contexts, error notification using return parameters of these system calls is not possible.  If information on detected errors is required it should be left in a messagebuffer used as an error log.

Only if the system is not a multiprocessor configuration, system can take advantage of the dispatch disabled state for exclusive inter-task control.


@c
@c  ena_dsp
@c

@page
@subsection ena_dsp - Enable Dispatch

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ena_dsp(void);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_CTX} - Context error (issued from task-independent portions or issued after execution of loc_cpu)

@subheading DESCRIPTION:
This system call enables task dispatching, that is, it finishes dispatch disabled state caused by the execution of dis_dsp.

No error will result if a task which is not in dispatch disabled state issues ena_dsp.  In this case, dispatching will just remain enabled.

An E_CTX error will result if ena_dsp is issued when both interrupt and dispatching are disabled with loc_cpu.  (For details, see the description of loc_cpu.)

@subheading NOTES:


@c
@c  chg_pri
@c

@page
@subsection chg_pri - Change Task Priority

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER chg_pri(
  ID tskid,
  PRI tskpri
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

@code{E_PAR} - Parameter error (the value of tskpri is invalid or may not be used)

@code{E_OBJ} - Invalid object state (the target task is in DORMANT state)

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} = Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@code{EN_PAR} - A value outside the range supported by the target node and/or transmission packet format was specified as a parameter (a value outside supported range was specified for tskpri)

@subheading DESCRIPTION:
This system call changes the current priority of the task specified by tskid to the value specified by tskpri.

Under uITRON 3.0 specification, at least any value of 1 through 8 can be specified as task priority.  The smaller the value, the higher the priority.  Priority levels -4 through 0 are reserved, and they may not be used.  Priority levels outside this range (including negative values) may also be specified depending on the implementation; this is considered an extended function [level X] for which compatibility and connectivity are not guaranteed.  In general, negative priority levels are reserved for use by the system.

A task may specify itself by specifying tskid = TSK_SELF = 0.  Note, however, that an E_ID error will result if tskid = TSK_SELF = 0 is specified to a system call issued from a task-independent portion.  The priority set by this system call remains in effect until the task exits.  Once a task enters DORMANT state, its priority prior to exiting is lost.  When a task which enters DORMANT state restarts, the initial task priority (itskpri) specified at task creation or at system startup will be used.

If the target task is linked to ready queue or any other queue, this system call may result in the re-ordering of the queues.  If chg_pri is executed on a task waiting on the ready queue (including tasks in RUN state) or other priority-based queue, the target task will be moved to the end of the part of the queue for the associated priority.  If the priority specified is the same as the current priority, the task will still be moved behind other tasks of the same priority.  It is therefore possible for a task to relinquish its execution privileges using chg_pri on itself by specifying its current priority.

@subheading NOTES:
Depending on the implementation, specifying tskpri = TPRI_INI = 0 may cause a task's priority to be reset to the initial task priority (itskpri) which was defined when it was first created or when the system started.  This feature is used in some implementations in order to reset the task priority to its original value after setting it to a higher value for indivisible processing.  This feature is an extended function [level X] for which compatibility and connectivity are not guaranteed.


@c
@c  rot_rdq
@c

@page
@subsection rot_rdq - Rotate Tasks on the Ready Queue

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rot_rdq(
  PRI tskpri
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_PAR} - Parameter error (the value of tskpri is invalid)

@subheading DESCRIPTION:
This system call rotates tasks on the ready queue associated with the priority level specified by tskpri.  Specifically, the task at the head of the ready queue of the priority level in question is moved to the end of the ready queue, thus switching the execution of tasks having the same priority.  Round robin scheduling may be implemented by periodically issuing this system call in a given period of time.

When rot_rdq is issued by task portions with tskpri = TPRI_RUN = 0, the ready queue with the priority level of the issuing task is rotated.

When TPRI_RUN or a task's own priority level are specified for tskpri to rot_rdq, the task issuing the system call will be placed on the end of its ready queue.  In other words, task can issue rot_rdq to relinquishing its execution privileges.  The concept of "ready queue" envisioned in the description of this system call is one which includes the task in RUN state.

This system call does nothing if there are no tasks on the ready queue of the specified priority.  No error will result.

This system call cannot rotate ready queues on other nodes.

@subheading NOTES:
Depending on the implementation, it may be possible to issue rot_rdq(tskpri = TPRI_RUN) from task-independent portions, such as a cyclic handler.  In this case the ready queue including the running task, or the ready queue including the highest priority task, is rotated.  Normally these two are the same, but not always, as when task dispatching is delayed.  In that case it is implementation dependent whether to rotate the ready queue including the running task or the ready queue including the highest priority task.  Note that this is an extended function [Level X] for which compatibility and connectivity are not guaranteed.


@c
@c  rel_wai
@c

@page
@subsection rel_wai - Release Wait of Other Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rel_wai(
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

@code{E_OBJ} - Invalid object state (the target task is not in WAIT state (including when it is in DORMANT state or when the issuing task specifies itself))

@code{EN_OBJNO} - An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@subheading DESCRIPTION:
This system call forcibly releases WAIT state (not including SUSPEND state)
of the task specified by tskid.

An E_RLWAI error is returned to the task whose WAIT state has been released using rel_wai.  

Wait release requests by rel_wai are not queued.  In other words, if the task specified by tskid is already in WAIT state, the WAIT state is released, otherwise an E_OBJ error will be returned to the issuer.  An E_OBJ error will also result when a task specifies itself to this system call.

Rel_wai does not release SUSPEND state.  If rel_wai is issued on a task in WAIT-SUSPEND state, WAIT will be released but SUSPEND will continue for that task. When SUSPEND should also be released, the frsm_tsk system call must be issued separately.

@subheading NOTES:
A function similar to timeout can be implemented using an alarm handler which issues this system call on tasks specified time after they have entered WAIT state.

Rel_wai and wup_tsk differ in the following points.
@itemize @bullet
@item Wup_tsk can only release the WAIT state by slp_tsk or tslp_tsk, while rel_wai can release WAIT states caused by these and other calls (including wai_flg, wai_sem, rcv_msg, get_blk, etc.).
@item As seen from the target task, releasing WAIT state with wup_tsk results in a normal completion (E_OK), whereas releasing WAIT state with rel_wai results in an error (E_RLWAI).
@item When wup_tsk is used, a request is queued even if neither slp_tsk nor tslp_tsk have been executed on the target task yet.  When rel_wai is used to the task which is not in WAIT state, an E_OBJ error will result.
@end itemize


@c
@c  get_tid
@c

@page
@subsection get_tid - Get Task Identifier

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER get_tid(
  ID *p_tskid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@subheading DESCRIPTION:
This system call gets the ID of the issuing task.

If this system call is issued from a task-independent portion, tskid will be FALSE=0.

@subheading NOTES:


@c
@c  ref_tsk
@c

@page
@subsection ref_tsk - Reference Task Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_tsk(
  T_RTSK *pk_rtsk,
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

@code{E_PAR} - Parameter error (the packet address for return parameters cannot be used)

@code{EN_OBJNO} -  An object number which could not be accessed on the target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call was issued from a task in dispatch disabled state or from a task-independent portion

@code{EN_RPAR} - A value outside the range supported by the requesting node and/or transmission packet format was returned as a return parameter (a value outside supported range was returned for exinf, tskpri and/or tskstat)

@subheading DESCRIPTION:
This system call refers to the state of the task specified by tskid, and returns its current priority (tskpri), its task state (tskstat), and its extended information (exinf).

Tskstat may take the following values.

    tskstat:
@itemize @bullet
@item   TTS_RUN H'0...01        RUN state (currently running)
@item   TTS_RDY H'0...02        READY state (ready to run)
@item   TTS_WAI H'0...04        WAIT state (waiting for something)
@item   TTS_SUS H'0...08        SUSPEND state (forcibly made to wait)
@item   TTS_WAS H'0...0c        WAIT-SUSPEND state
@item   TTS_DMT H'0...10        DORMANT state
@end itemize

Since these task states are expressed by bit correspondences they are convenient when looking for OR conditions (such as whether a task is in RUN or READY state).  TTS_WAS is a combination of both TTS_SUS and TTS_WAI, TTS_SUS does not combine with any of the other states (TTS_RUN, TTS_RDY or TTS_DMT).

A task may specify itself by specifying tskid = TSK_SELF = 0.  Note, however, that an E_ID error will result if tskid = TSK_SELF = 0 is specified when this system call is issued from a task-independent portion.

An E_NOEXS error will result if the task specified to ref_tsk does not exist.

Tskstat will be TTS_RUN if ref_tsk is executed specifying a task which has been interrupted by an interrupt handler.

@subheading NOTES:
The values of TTS_RUN, TTS_RDY, TTS_WAI, etc. as return values for tskstat are not necessarily the same value to be entered in the TCB.  The way in which task state is represented in the TCB is implementation dependent.  When ref_tsk is executed, the internal representation of task state may simply be converted to the standard values TTS_RUN, TTS_RDY, TTS_WAI, etc.

Depending on the implementation, the following additional information can also be referenced in addition to exinf, tskpri and tskstat.

@itemize @bullet
@item tskwait Reason for wait
@item wid     Wait object ID
@item wupcnt  Number of queued wakeup requests
@item suscnt  Number of nested SUSPEND requests
@item tskatr  Task attributes
@item task    Task starting address
@item itskpri Initial task priority
@item stksz   Stack size
@end itemize





