@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the semaphore
@c  manager.
@c
@c  $Id$
@c

@chapter Semaphore Manager

@section Introduction

The semaphore manager provides functions to allocate, delete, and
control counting semaphores.

The services provided by the semaphore manager are:

@itemize @bullet
@item @code{cre_sem} - Create Semaphore
@item @code{del_sem} - Delete Semaphore
@item @code{sig_sem} - Signal Semaphore
@item @code{wai_sem} - Wait on Semaphore
@item @code{preq_sem} - Poll and Request Semaphore
@item @code{twai_sem} - Wait on Semaphore with Timeout
@item @code{ref_sem} - Reference Semaphore Status
@end itemize

@section Background

@subsection Theory

Semaphores are used for synchronization and mutual exclusion by indicating
the availability and number of resources.  The task (the task which is
returning resources) notifying other tasks of an event increases the number
of resources held by the semaphore by one.  The task (the task which
will obtain resources) waiting for the event decreases the number of
resources held by the semaphore by one.  If the number of resources held by a
semaphore is insufficient (namely 0), the task requiring resources will
wait until the next time resources are returned to the semaphore.  If there is
more than one task waiting for a semaphore, the tasks will be placed in the
queue.


@subsection T_CSEM Structure

The T_CSEM structure is used to define the characteristics of a semaphore
and passed an as argument to the @code{cre_sem} service.  The structure
is defined as follows:

@example
typedef struct t_csem @{
        VP    exinf;    /* extended information */
        ATR   sematr;   /* semaphore attributes */
    /* Following is the extended function for [level X]. */
        INT   isemcnt;   /* initial semaphore count */
        INT   maxsem;    /* maximum semaphore count */
                ...
    /* additional information may be included depending on the
       implementation */
                ...
@} T_CSEM;

sematr:
    TA_TFIFO   H'0...00   /* waiting tasks are handled by FIFO */
    TA_TPRI    H'0...01   /* waiting tasks are handled by priority */

@end example

where the meaning of each field is:

@table @b
@item exinf
is the extended information XXX

@item sematr
is the attributes for this semaphore.  The only attributed
which can be specified is whether tasks wait in FIFO (@code{TA_TFIFO})
or priority (@code{TA_TPRI}) order.

@item isemcnt
is the initial count of the semaphore.

@item maxsem
is the maximum count the semaphore may have.  It places an upper
limit on the value taken by the semaphore.

@end table

@subsection T_RSEM Structure

The T_RSEM structure is filled in by the @code{ref_sem} service with
status and state information on a semaphore.  The structure
is defined as follows:

@example
typedef struct t_rsem @{
        VP      exinf;    /* extended information */
        BOOL_ID wtsk;     /* indicates whether or not there is a
                             waiting task */
        INT     semcnt;   /* current semaphore count */
                ...
    /* additional information may be included depending on the
       implementation */
                ...
@} T_RSEM;
@end example

@table @b

@item exinf
is extended information.

@item wtsk
is TRUE when there is one or more task waiting on the semaphore.
It is FALSE if no tasks are currently waiting.

@item semcnt
is the current semaphore count.

@end table

@section Operations

@subsection Using as a Binary Semaphore

Creating a semaphore with a limit on the count of 1 effectively
restricts the semaphore to being a binary semaphore.  When the
binary semaphore is available, the count is 1.  When the binary
semaphore is unavailable, the count is 0.;

@section Directives

This section details the semaphore manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_sem
@c

@page
@subsection cre_sem - Create Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_sem(
  ID semid,
  T_CSEM *pk_csem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_NOMEM} - Insufficient memory (Memory for control block cannot be
allocated)

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_RSATR} - Reserved attribute (sematr was invalid or could not be
used)

@code{E_OBJ} - Invalid object state (a semaphore of the same ID already
exists)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (pk_csem is invalid and/or isemcnt or
maxsem is negative or invalid)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a task-
independent portion

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for exinf, sematr, isemcnt and/or
maxsem)

@subheading DESCRIPTION:



@subheading NOTES:

NONE


@c
@c  del_sem
@c

@page
@subsection del_sem - Delete Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@subheading DESCRIPTION:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion


@subheading NOTES:

NONE


@c
@c  sig_sem
@c

@page
@subsection sig_sem - Signal Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER sig_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_QOVR} - Queuing or nesting overflow (the queuing count given by
semcnt went over the maximum allowed)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@subheading DESCRIPTION:

@subheading NOTES:

NONE


@c
@c  wai_sem
@c

@page
@subsection wai_sem - Wait on Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER wai_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (tmout is -2 or less)

@code{E_DLT} - The object being waited for was deleted (the specified
semaphore was deleted while waiting)

@code{E_RLWAI} - Wait state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)


@subheading DESCRIPTION:

@subheading NOTES:

NONE


@c
@c  preq_sem
@c

@page
@subsection preq_sem - Poll and Request Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER preq_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (tmout is -2 or less)

@code{E_DLT} - The object being waited for was deleted (the specified
semaphore was deleted while waiting)

@code{E_RLWAI} - Wait state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)


@subheading DESCRIPTION:

@subheading NOTES:

NONE


@c
@c  twai_sem
@c

@page
@subsection twai_sem - Wait on Semaphore with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER twai_sem(
  ID semid,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (tmout is -2 or less)

@code{E_DLT} - The object being waited for was deleted (the specified
semaphore was deleted while waiting)

@code{E_RLWAI} - Wait state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)


@subheading DESCRIPTION:

@subheading NOTES:

NONE


@c
@c  ref_sem
@c

@page
@subsection ref_sem - Reference Semaphore Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_sem(
  T_RSEM *pk_rsem,
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (the packet address for the return
parameters could not be used)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_RPAR} - A value outside the range supported by the requesting
node and/or transmission packet format was returned as a parameter (a
value outside supported range was specified for exinf, wtsk or semcnt)

@subheading DESCRIPTION:

@subheading NOTES:

NONE

