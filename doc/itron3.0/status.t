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

@chapter ITRON Implementation Status

@section Introduction

This chapter describes the status of the implementation of each
manager in the RTEMS implementataion of the uITRON 3.0 API.   The
status of each manager is presented in terms of documentation and
status relative to the extended level (level 'E') of the uITRON 3.0
API specification.  The extended level of the specification is
the level at which dynamic object creation, deletion, and
reference services are available.  This level is more akin to the other
APIs supported by RTEMS.  This purpose of this chapter is
to make it clear what is required to bring the RTEMS
uITRON API implementation into compliance with the
specification.  The following description of the specification
levels is taken from the uITRON 3.0 API specification.


uITRON 3.0 specification is divided into fewer system call levels than was the
previous uITRON 2.0 specification.  There are now just three levels: Level R
(Required), Level S (Standard) and Level E (Extended).  In addition to these
three levels, there is also Level C for CPU-dependent system calls.
In addition, the uITRON 3.0 API, defines the network level ('N') which
represents system calls that support the connection function

@itemize @bullet
@item [level R] (Required)
The functions in this level are mandatory for all implementations of
uITRON 3.0 specification.  This includes basic functions for achieving
a real-time, multitasking OS.  These functions can be implemented even
without a hardware timer.  This level corresponds to Levels 1 and 2
of uITRON 2.0 specification.

@item [level S] (Standard)
This includes basic functions for achieving a real-time, multitasking
OS.  This level corresponds to Levels 3 and 4 of uITRON 2.0
specification.

@item [level E] (Extended)
This includes additional and extended functions.  This corresponds to
functions not included in uITRON 2.0 specification (functions of
ITRON2 specification).  Specifically, this level includes object
creation and deletion functions, rendezvous functions, memorypools
and the timer handler.

@item [level C] (CPU dependent)
This level provides implementation-dependent functions required due to
the CPU or hardware configuration.

@end itemize

The support level of the connection function is indicated by appending an 'N'
to the end of the level.  For example, connectivity supported at [level S]
would be referred to as [level SN].  The support level for functions which
can only send requests for operations on other nodes but offer no system call
processing on the issuing node itself are indicated by the lower case letter
's' or 'e'.

@c
@c Task
@c

@section Task Status

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_tsk - Complete, Pending Review
@item del_tsk - Complete, Pending Review
@item sta_tsk - Complete, Pending Review
@item ext_tsk - Complete, Pending Review
@item exd_tsk - Complete, Pending Review
@item ter_tsk - Complete, Pending Review
@item dis_dsp - Complete, Pending Review
@item ena_dsp - Complete, Pending Review
@item chg_pri - Complete, Pending Review
@item rot_rdq - Complete, Pending Review
@item rel_wai - Stub, Needs to be Fleshed Out
@item get_tid - Complete, Pending Review
@item ref_tsk - Complete, Pending Review

@item Notes:
@itemize @bullet
@item None
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item itron01    - Hello world
@item itron02    - Semaphore test
@item itron03    - directives: ex_init, ex_start, t_create,
t_start, tm_tick, i_return, t_ident, tm_set, tm_get, tm_wkafter
See .doc file, verify correct
@item itron04    - Doc file needed
@item itron05    - directives: ext_tsk, cre_tsk, sta_tsk, rot_rdq
ex_start, t_create, t_start, tm_tick, i_return, t_ident, t_delete,
tm_wkafter, t_setpri, t_suspend
See .doc file, verify correct
@item itron06    - Doc file needed
@item itron07    - Doc file needed
@item itron08    - Doc file needed
@item itron09    - Doc file needed
@item itron10    - Doc file needed
@item tmitron01  - Doc file needed
@item tm_include - Doc file needed.  Timing test for semaphores.
@end itemize

@item Documentation
@itemize @bullet
@item Complete, Pending Review
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize

@end itemize

@c
@c Task-Dependent Synchronization
@c

@section Task-Dependent Synchronization Status

@itemize @bullet

@item Implementation
@itemize @bullet
@item sus_tsk  - Complete, Pending Review
@item rsm_tsk  - Complete, Pending Review
@item frsm_tsk - Complete, Pending Review
@item slp_tsk  - Stub, Needs to be Fleshed Out
@item tslp_tsk - Stub, Needs to be Fleshed Out
@item wup_tsk  - Stub, Needs to be Fleshed Out
@item can_wup  - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item None
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item Functional tests for complete routines.
@item Yellow line testing needs to be verified.
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Complete, Pending Review
@item
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Semaphore
@c

@section Semaphore

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_sem - Complete, Pending Review
@item del_sem - Complete, Pending Review
@item sig_sem - Complete, Pending Review
@item wai_sem - Complete, Pending Review
@item preq_sem - Complete, Pending Review
@item twai_sem - Complete, Pending Review
@item ref_sem - Complete, Pending Review
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Required
@end itemize

@item Testing
@itemize @bullet
@item Yellow Lined BUT Timeout Cases Not Actually Executed
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Complete, Pending Review
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item Complete, Pending Review
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Eventflags
@c

@section Eventflags

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_flg - Stub, Needs to be Fleshed Out
@item del_flg - Stub, Needs to be Fleshed Out
@item set_flg - Stub, Needs to be Fleshed Out
@item clr_flg - Stub, Needs to be Fleshed Out
@item wai_flg - Stub, Needs to be Fleshed Out
@item pol_flg - Stub, Needs to be Fleshed Out
@item twai_flg - Stub, Needs to be Fleshed Out
@item ref_flg - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Similar in Functionality to Classic API Events Manager
@item Implement Using new SuperCore Event Handler
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item Add SuperCore Events Object Based on Classic Events
@item Redo Classic Events to use SuperCore Events
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Good First Draft.
@item No Information in Operations.
@item Should not use "standard-like" language.
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Mailbox
@c

@section Mailbox

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_mbx - Stub, Needs to be Fleshed Out
@item del_mbx - Stub, Needs to be Fleshed Out
@item snd_msg - Stub, Needs to be Fleshed Out
@item rcv_msg - Stub, Needs to be Fleshed Out
@item prcv_msg - Stub, Needs to be Fleshed Out
@item trcv_msg - Stub, Needs to be Fleshed Out
@item ref_mbx - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Passes Addresses of Messages
@item FIFO or Priority Task Blocking
@item FIFO or Priority Message Ordering
@item Send Returns Error on Overflow
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Needs More Text
@item No Information in Background or Operations.
@item Service Descriptions are Weak.
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Message Buffer
@c

@section Message Buffer

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_mbf - Stub, Needs to be Fleshed Out
@item del_mbf - Stub, Needs to be Fleshed Out
@item snd_mbf - Stub, Needs to be Fleshed Out
@item psnd_mbf - Stub, Needs to be Fleshed Out
@item tsnd_mbf - Stub, Needs to be Fleshed Out
@item rcv_mbf - Stub, Needs to be Fleshed Out
@item prcv_mbf - Stub, Needs to be Fleshed Out
@item trcv_mbf - Stub, Needs to be Fleshed Out
@item ref_mbf - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Implement Using SuperCore Message Queue Handler
@item Passes Full Bodies of Messages
@item FIFO or Priority Task Blocking
@item FIFO Message Ordering Only
@item Send (snd_mbf and tsnd_mbf) Blocks on Overflow
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item SuperCore Message Queue Handler Must Support Blocking Sends.  [NOTE:
This is required for POSIX Message Queues as well.]
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Good First Draft.
@item No Information in Operations
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Rendezvous
@c

@section Rendezvous

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_por - Stub, Needs to be Fleshed Out
@item del_por - Stub, Needs to be Fleshed Out
@item cal_por - Stub, Needs to be Fleshed Out
@item pcal_por - Stub, Needs to be Fleshed Out
@item tcal_por - Stub, Needs to be Fleshed Out
@item acp_por - Stub, Needs to be Fleshed Out
@item pacp_por - Stub, Needs to be Fleshed Out
@item tacp_por - Stub, Needs to be Fleshed Out
@item fwd_por - Stub, Needs to be Fleshed Out
@item rpl_rdv - Stub, Needs to be Fleshed Out
@item ref_por - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Hardest ITRON Manager to Implement
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item Doubtful, Probably Implement in Terms of Multiple SuperCore Objects.
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Shell, Needs to be Fleshed Out
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Interrupt
@c

@section Interrupt

@itemize @bullet

@item Implementation
@itemize @bullet
@item def_int - Stub, Needs to be Fleshed Out
@item ret_int - Stub, Needs to be Fleshed Out
@item ret_wup - Stub, Needs to be Fleshed Out
@item loc_cpu - Stub, Needs to be Fleshed Out
@item unl_cpu - Stub, Needs to be Fleshed Out
@item dis_int - Stub, Needs to be Fleshed Out
@item ena_int - Stub, Needs to be Fleshed Out
@item chg_iXX - Stub, Needs to be Fleshed Out
@item ref_iXX - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item This quote from the ITRON specification needs to be thought about:@*
@*@i{"When an interrupt is invoked, the interrupt handler defined with
this system call is started directly by the
interrupt processing mechanism of the CPU hardware.  Accordingly, code at the
beginning and end of an interrupt handler must save and restore any registers
used by the interrupt handler."}@*@*
Based on another comment, in the ret_int description, I think this means
that RTEMS will not support the TA_ASM style of interrupt handlers -- 
only the TA_HLNG style.@*@*
@i{When TA_HLNG is specified, a high-level language environment setting
program (a high-level language support routine) is called before branching
to the inthdr address.  The least significant bit (LSB) of the system
attribute bits is used for this specification.}

@item Specification allows special "interrupt-only" versions of system
calls named i???_??? (i.e. sig_sem and isig_sem).  This does not seem
to be something that would be implemented with RTEMS.  We could provide
macros mapping them onto the default versions if this is an issue.

@item How this operates versus the behavior of a true TRON chip is
up for discussion.

@item ret_wup is questionable in only high-level language ISRs.

@item dis_int and ena_int refer to a specific interrupt number.  These
may require hooking back out to the BSP.

@item for chg_iXX and reg_iXX, the XX should be replaced with something
that is meaningful on a particular CPU.
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Shell, Needs to be Fleshed Out
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Memory Pool
@c

@section Memory Pool

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_mpl - Stub, Needs to be Fleshed Out
@item del_mpl - Stub, Needs to be Fleshed Out
@item get_blk - Stub, Needs to be Fleshed Out
@item pget_blk - Stub, Needs to be Fleshed Out
@item tget_blk - Stub, Needs to be Fleshed Out
@item rel_blk - Stub, Needs to be Fleshed Out
@item ref_mpl - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Implement Using SuperCore Heap Handler
@item Similar to Region in Classic API with Blocking
@item FIFO or Priority Task Blocking
@item Specification Deliberately Open on Allocation Algorithm
@item Multiple Tasks Can be Unblocked by a single rel_blk
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Good First Draft.
@item No Information in Operations
@item Should not use "standard-like" language.
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Fixed Block
@c

@section Fixed Block

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_mpf - Stub, Needs to be Fleshed Out
@item del_mpf - Stub, Needs to be Fleshed Out
@item get_blf - Stub, Needs to be Fleshed Out
@item pget_blf - Stub, Needs to be Fleshed Out
@item tget_blf - Stub, Needs to be Fleshed Out
@item rel_blf - Stub, Needs to be Fleshed Out
@item ref_mpf - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Implement Using SuperCore Chain Handler
@item Similar to Partition in Classic API with Blocking
@item FIFO or Priority Task Blocking
@item Specification Deliberately Open on Allocation Algorithm
@item Should add Blocking to Classic API Partition at Same Time
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Good First Draft.
@item No Information in Background or Operations
@item Should not use "standard-like" language.
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Time
@c

@section Time

@itemize @bullet

@item Implementation
@itemize @bullet
@item get_tim - Stub, Needs to be Fleshed Out
@item set_tim - Stub, Needs to be Fleshed Out
@item dly_tsk - Stub, Needs to be Fleshed Out
@item def_cyc - Stub, Needs to be Fleshed Out
@item act_cyc - Stub, Needs to be Fleshed Out
@item ref_cyc - Stub, Needs to be Fleshed Out
@item def_alm - Stub, Needs to be Fleshed Out
@item ref_alm - Stub, Needs to be Fleshed Out
@item ret_tmr - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item Need to Implement Time Conversion Routines
@item Epoch is January 1, 1985, 00:00:00 am (GMT).
@item Cyclic and Alarm Handlers may be TA_ASM or TA_HLNG.
@item Alarms may be Absolute or Relative Time based.
@item May Want to Implement a Timer Server Task
@item Termination via ret_tmr is Not Consistent with Current RTEMS Timers.
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Have Version in Word
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c System
@c

@section System

@itemize @bullet

@item Implementation
@itemize @bullet
@item get_ver - Stub, Needs to be Fleshed Out
@item ref_sys - Stub, Needs to be Fleshed Out
@item ref_cfg - Stub, Needs to be Fleshed Out
@item def_svc - Stub, Needs to be Fleshed Out
@item def_exc - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item May Have to Obtain ITRON "OS Maker" Id
@item - def_svc seems to imply a trap handler interface
@item - def_exc needs to be examined.
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Shell, Needs to be Fleshed Out
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

@c
@c Network Support
@c

@section Network Support

@itemize @bullet

@item Implementation
@itemize @bullet
@item nrea_dat - Stub, Needs to be Fleshed Out
@item nwri_dat - Stub, Needs to be Fleshed Out
@item nget_nod - Stub, Needs to be Fleshed Out
@item nget_ver - Stub, Needs to be Fleshed Out

@item Notes:
@itemize @bullet
@item None of these are difficult to implement on top of MPCI
@item MP Packet formats are well-defined.
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
@item No Timing Tests
@end itemize

@item Documentation
@itemize @bullet
@item Shell, Needs to be Fleshed Out
@end itemize

@item ITRON 3.0 API Conformance
@itemize @bullet
@item Level E - Extended Functionality
@itemize @bullet
@item 
@end itemize

@item Level C - CPU Dependent Functionality
@itemize @bullet
@item NA
@end itemize

@item Level N - Connection Functionality
@itemize @bullet
@item Not implemented
@end itemize
@end itemize

@end itemize

