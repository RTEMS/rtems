@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the fixed block
@c  manager.
@c
@c  $Id$
@c

@chapter Fixed Block Manager

@section Introduction

The fixed block manager provides functions for creating, deleting, getting, polling, getting with timeout, releasing, and referencing the fixed-sized memorypool. This manager is based on ITRON 3.0 standard.

The services provided by the fixed block manager are:

@itemize @bullet
@item @code{cre_mpf} - Create Fixed-Size Memorypool
@item @code{del_mpf} - Delete Fixed-Size Memorypool
@item @code{get_blf} - Get Fixed-Size Memory Block
@item @code{pget_blf} - Poll and Get Fixed-Size Memory Block
@item @code{tget_blf} - Get Fixed-Size Memory Block with Timeout
@item @code{rel_blf} - Release Fixed-Size Memory Block
@item @code{ref_mpf} - Reference Fixed-Size Memorypool Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the fixed block manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mpf
@c

@page
@subsection cre_mpf - Create Fixed-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_mpf(
  ID mpfid,
  T_CMPF *pk_cmpf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_NOMEM} - Insufficient memory (Memory for control block and/or for
memorypool cannot be allocated)
  
@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
  
@code{E_RSATR} - Reserved attribute (mpfatr was invalid or could not be used)
  
@code{E_OBJ} - Invalid object state (a memorypool of the same ID already exists)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (pk_cmpf is invalid or mpfsz and/or blfsz is
negative or invalid)

@subheading DESCRIPTION:

This system call creates a fixed-size memorypool having the ID number specified by mpfid.  Specifically, a memory area of a size based on values of mpfcnt and blfsz is reserved for use as a memorypool.  A control block for the memorypool being created is also allocated.  The get_blf system call specifying the memorypool created by this call can be issued to allocate memory blocks of the size given by blfsz (in bytes).

@subheading NOTES:

The memory area required for creating memorypools and for allocating control blocks for each object is allocated while system initialization. Associated parameters are therefore specified at system configuration.

@c
@c  del_mpf
@c

@page
@subsection del_mpf - Delete Fixed-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mpf(
  ID mpfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the memorypool specified by mpfid does not exist)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from a user task.  This is implementation dependent.)

@subheading DESCRIPTION:

This system call deletes the fixed-size memorypool specified by mpfid.  No check or error report is performed even if there are tasks using memory from the memorypool to be deleted.  This system call completes normally even if some of the memory blocks are not returned.  Issuing this system call causes memory used for the control block of the associated memorypool and the memory area making up the memorypool itself to be released.  After this system call is invoked, another memorypool having the same ID number can be created.  This system call will complete normally even if there are tasks waiting to get memory blocks from the memorypool.  In that case, an E_DLT error will be returned to each waiting task.

@subheading NOTES:

When a memorypool being waited for by more than one tasks is deleted, the order of tasks on the ready queue after the WAIT state is cleared is implementation dependent in the case of tasks having the same priority.


@c
@c  get_blf
@c

@page
@subsection get_blf - Get Fixed-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER get_blf(
  VP *p_blf,
  ID mpfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
 
@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mpfid does not exist)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less)
  
@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout exceeded
  
@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for pget_blf and tget_blf(tmout=TMO_POL))

@subheading DESCRIPTION:

A memory block is allocated from the fixed-size memorypool specified by mpfid.  The start address of the memory block allocated is returned to blf.  The size of the memory block allocated is specified by the blfsz parameter when the fixed-size memorypool was created.  The allocated memory block is not cleared to zero.  The contents of the allocated memory block are undefined.  If the memory block cannot be obtained from the specified memorypool when get_blf is issued, the task issuing get_blf will be placed on the memory allocation queue of the specified memorypool, and wait until it can get the memory it requires.  If the object being waited for is deleted (the specified memorypool is deleted while waiting), an E_DLT error will be returned.

@subheading NOTES:

NONE


@c
@c  pget_blf
@c

@page
@subsection pget_blf - Poll and Get Fixed-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =pget_blf(
  VP *p_blf,
  ID mpfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mpfid does not exist)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less)
  
@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout exceeded
  
@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for pget_blf and tget_blf(tmout=TMO_POL))

@subheading DESCRIPTION:

The pget_blf system call has the same function as get_blf except for the waiting feature.  Pget_blf polls whether or not the task should wait if get_blf is executed.  The meaning of parameters to pget_blf are the same with get_blf.  The specific operations by pget_blf are as follows.

  - If there is a free memory block available, processing is the same as
    get_blf: that is, the requested memory is allocated and the system call
    completes normally.

  - If there is no free memory block, an E_TMOUT error is returned to
    indicate polling failed and the system call finishes.  Unlike get_blf,
    the issuing task does not wait in this case.  Also, the issuing task
    does not get any memory.

@subheading NOTES:

NONE


@c
@c  tget_blf
@c

@page
@subsection tget_blf - Get Fixed-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tget_blf(
  VP *p_blf,
  ID mpfid,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mpfid does not exist)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less)
  
@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout exceeded
  
@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for pget_blf and tget_blf(tmout=TMO_POL))

@subheading DESCRIPTION:

The tget_blf system call has the same function as get_blf with an additional timeout feature.  A maximum wait time (timeout value) can be specified using the parameter tmout.  When a timeout is specified, a timeout error, E_TMOUT, will result and the system call will finish if the period specified by tmout elapses without conditions for releasing wait being satisfied (i.e. without free memory becoming available).

@subheading NOTES:

NONE

@c
@c  rel_blf
@c

@page
@subsection rel_blf - Release Fixed-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rel_blf(
  ID mpfid,
  VP blf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mpfid does not exist)
  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (blf is invalid or an attempt was made to return
the memory block to the wrong memorypool)

@subheading DESCRIPTION:

This system call releases the memory block specified by blf and returns it to the fixed-size memorypool specified by mpfid.  Executing rel_blf allows memory to be allocated to the next task waiting for memory allocation from the memorypool given by mpfid, thus releasing that task from its WAIT state.

@subheading NOTES:

The fixed-size memorypool to which the memory block is returned must be the same memorypool from which it was originally allocated.  An E_PAR error will result if an attempt is made to return a memory block to another memorypool than that from which it was originally allocated.

@c
@c  ref_mpf
@c

@page
@subsection ref_mpf - Reference Fixed-Size Memorypool Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mpf(
  T_RMPF *pk_rmpf,
  ID mpfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mpfid was invalid or could not be used)
 
@code{E_NOEXS} - Object does not exist \(the memorypool specified by mpfid does 
not exist.)
                                                                                                                                                  
@code{E_OACV} - Object access violation (A mpfid less than -4 was specified from
a user task.  This is implementation dependent.)
 
@code{E_PAR} - Parameter error (the packet address for the return parameters
could not be used)

@subheading DESCRIPTION:

This system call refers to the state of the fixed-size memorypool specified by mpfid, and returns the current number of free blocks (frbcnt), information of a task waiting to be allocated memory (wtsk), and its extended information (exinf).  Wtsk indicates whether or not there is a task waiting to be allocated memory from the fixed-size memorypool specified.  If there is no waiting task, wtsk is returned as FALSE = 0.  If there is a waiting task, wtsk is returned as a value other than 0.

@subheading NOTES:

While the frsz return parameter of ref_mpl returns the total size of free memory, the frbcnt return parameter of ref_mpf returns the number of free blocks.

Depending on the implementation, additional information besides wtsk and frbcnt (such as memorypool attributes, mpfatr) may also be referred.

