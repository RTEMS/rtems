@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the memory pool
@c  manager.
@c
@c  $Id$
@c

@chapter Memory Pool Manager

@section Introduction

The 
memory pool manager is ...

The services provided by the memory pool manager are:

@itemize @bullet
@item @code{cre_mpl} - Create Variable-Size Memorypool
@item @code{del_mpl} - Delete Variable-Size Memorypool
@item @code{get_blk} - Get Variable-Size Memory Block
@item @code{pget_blk} - Poll and Get Variable-Size Memory Block
@item @code{tget_blk} - Get Variable-Size Memory Block with Timeout
@item @code{rel_blk} - Release Variable-Size Memory Block
@item @code{ref_mpl} - Reference Variable-Size Memorypool Status
@end itemize

@section Background

Memorypool management functions manage memorypools and allocate memory blocks.
There are two types of memorypool: fixed-size memorypools and variable-size
memorypools.  Both are considered separate objects and require different
system calls for manipulation.  While the size of memory blocks allocated
from fixed-size memorypools are all fixed, blocks of any size may be
specified when allocating memory blocks from variable-size memorypools.

@section Operations

@section System Calls

This section details the memory pool manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mpl
@c

@page
@subsection cre_mpl - Create Variable-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_mpl(
  ID mplid,
  T_CMPL *pk_cmpl
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_NOMEM} - Insufficient memory (Memory for control block and/or for
memorypool cannot be allocated)

@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)

@code{E_RSATR} - Reserved attribute (mplatr was invalid or could not be used)

@code{E_OBJ} - Invalid object state (a memorypool of the same ID already exists)

@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (pk_cmpl is invalid or mplsz is negative or
invalid)

@subheading DESCRIPTION:

The cre_mpl directive creates a variable-size memorypool having the ID number specified by mplid.  Specifically, a memory area of the size determined by mplsz is allocated for use as a memorypool.  A control block for the memorypool being created is also allocated.  User memorypools have positive ID numbers, while system memorypools have negative ID numbers.  User tasks (tasks having positive task IDs) cannot access system memorypools (memorypools having negative ID numbers). 

@subheading NOTES:

The memory required for creating memorypools and for allocating control blocks for each object is reserved while system initialization. Associated parameters are therefore specified at system configuration.

@c
@c  del_mpl
@c

@page
@subsection del_mpl - Delete Variable-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mpl(
  ID mplid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
 
@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.)

@subheading DESCRIPTION:

This system call deletes the variable-size memorypool specified by mplid.  No check or error report is performed even if there are tasks using memory from the memorypool to be deleted.  This system call completes normally even if some of the memory blocks are not returned.  Issuing this system call causes memory used for the control block of the associated memorypool and the memory area making up the memorypool itself to be released.  After this system call is invoked, another memorypool having the same ID number can be created.

@subheading NOTES:

When a memorypool being waited for by more than one tasks is deleted, the order of tasks on the ready queue after the WAIT state is cleared is implementation dependent in the case of tasks having the same priority.

@c
@c  get_blk
@c

@page
@subsection get_blk - Get Variable-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER get_blk(
  VP *p_blk,
  ID mplid,
  INT blksz
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less, blksz is negative or invalid)
  
@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for
pget_blk and tget_blk(tmout=TMO_POL))

@subheading DESCRIPTION:

A memory block of the size in bytes given by blksz is allocated from the variable-size memorypool specified by mplid.  The start address of the memory block allocated is returned in blk.  The allocated memory block is not cleared to zero.  The contents of the memory block allocated are undefined.  If the memory block cannot be obtained from the specified memorypool when get_blk is issued, the task issuing get_blk will be placed on the memory allocation queue of the specified memorypool, and wait until it can get the memory it requires.

The order in which tasks wait on the queue when waiting to be allocated memory blocks is according to either FIFO or task priority.  The specification whereby tasks wait according to task priority is considered an extended function [level X] for which compatibility is not guaranteed.  Furthermore, when tasks form a memory allocation queue, it is implementation dependent whether priority is given to tasks requesting the smaller size of memory or those at the head of the queue.

@subheading NOTES:

Pget_blk and get_blk represent the same processing as specifying certain values (TMO_POL or TMO_FEVR) to tget_blk for tmout.  It is allowed that only tget_blk is implemented in the kernel and that pget_blk and get_blk are implemented as macros which call tget_blk.

@c
@c  pget_blk
@c

@page
@subsection pget_blk - Poll and Get Variable-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =pget_blk(
  VP *p_blk,
  ID mplid,
  INT blksz
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less, blksz is negative or invalid)

@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout
  
@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for
pget_blk and tget_blk(tmout=TMO_POL))

@subheading DESCRIPTION:

The pget_blk system call has the same function as get_blk except for the waiting feature.  Pget_blk polls whether or not the task should wait if get_blk is executed.  The meaning of parameters to pget_blk are the same with get_blk.  The specific operations by pget_blk are as follows.

  - If there is enough free memory to get the memory block of specified size
    immediately, processing is the same as get_blk: that is, the
    requested memory is allocated and the system call completes normally.

  - If there is not enough free memory, an E_TMOUT error is returned to
    indicate polling failed and the system call finishes.  Unlike get_blk,
    the issuing task does not wait in this case.  Also, the issuing task
    does not get any memory.

@subheading NOTES:

NONE

@c
@c  tget_blk
@c

@page
@subsection tget_blk - Get Variable-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tget_blk(
  VP *p_blk,
  ID mplid,
  INT blksz,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (tmout is -2 or less, blksz is negative or invalid)

@code{E_DLT} - The object being waited for was deleted (the specified memorypool
was deleted while waiting)
  
@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received while
waiting)
  
@code{E_TMOUT} - Polling failure or timeout
  
@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for
pget_blk and tget_blk(tmout=TMO_POL))


@subheading DESCRIPTION:

The tget_blk system call has the same function as get_blk with an additional timeout feature.  A maximum wait time (timeout value) can be specified using the parameter tmout.  When a timeout is specified, a timeout error, E_TMOUT, will result and the system call will finish if the period specified by tmout elapses without conditions for releasing wait being satisfied (i.e. without sufficient free memory becoming available).

@subheading NOTES:

NONE

@c
@c  rel_blk
@c

@page
@subsection rel_blk - Release Variable-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rel_blk(
  ID mplid,
  VP blk
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
 
@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
            a user task.  This is implementation dependent.)
  
@code{E_PAR} - Parameter error (blk is invalid or an attempt was made to return
the memory block to the wrong memorypool)

@subheading DESCRIPTION:

This system call releases the memory block specified by blk and returns it to the variable-size memorypool specified by mplid.  Executing rel_blk allows memory to be allocated to the next task waiting for memory allocation from the memorypool given by mplid, thus releasing that task from its WAIT state.  The variable-size memorypool to which the memory block is returned must be the same memorypool from which it was originally allocated.  An E_PAR error will result if an attempt is made to return a memory block to another memorypool than that from which it was originally allocated. 

@subheading NOTES:

When memory is returned to a variable-size memorypool for which more than one task is waiting, multiple tasks may be released from waiting at the same time depending on the number of bytes of memory.  The order of tasks on the ready queue among tasks having the same priority after being released from waiting for memory is implementation dependent.

@c
@c  ref_mpl
@c

@page
@subsection ref_mpl - Reference Variable-Size Memorypool Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mpl(
  T_RMPL *pk_rmpl,
  ID mplid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:
 
@code{E_OK} - Normal Completion
  
@code{E_ID} - Invalid ID number (mplid was invalid or could not be used)
  
@code{E_NOEXS} - Object does not exist (the memorypool specified by mplid does not exist)
  
@code{E_OACV} - Object access violation (A mplid less than -4 was specified from
a user task.  This is implementation dependent.) Note: User tasks can issue ref_mpl in order to reference memorypools of mplid = TMPL_OS = -4.  Whether or not memorypools of mplid = -3 or -2 may be specified to ref_mpl by user tasks is implementation dependent.
  
@code{E_PAR} - Parameter error (the packet address for the return parameters
could not be used)

@subheading DESCRIPTION:

This system call refers to the state of the variable-size memorypool specified by mplid, and returns the total free memory size currently available (frsz), the maximum continuous memory size of readily available free memory (maxsz), information of a task waiting to be allocated memory (wtsk), and its extended information (exinf).  Wtsk indicates, whether or not there is a task waiting to be allocated memory from the variable-size memorypool specified.  If there is no waiting task, wtsk is returned as FALSE = 0.  If there is a waiting task, wtsk is returned as a value other than 0.

@subheading NOTES:

In some implementations, memorypools having mplid = -3 or -2 may be referred with ref_mpl as memorypools used by implementation-dependent parts of the OS (such as those used for the stack only).  This system call can provide more detailed information regarding remaining memory if the usage of memorypools having mplid = -3 or -2 is more clearly defined.  Whether or not this feature is used and any details regarding information provided are implementation dependent.

