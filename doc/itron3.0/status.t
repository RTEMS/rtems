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

@section Task

@itemize @bullet

@item Implementation
@itemize @bullet
@item cre_tsk - Stub, Needs to be Fleshed Out
@item del_tsk - Stub, Needs to be Fleshed Out
@item sta_tsk - Stub, Needs to be Fleshed Out
@item ext_tsk - Stub, Needs to be Fleshed Out
@item exd_tsk - Stub, Needs to be Fleshed Out
@item ter_tsk - Stub, Needs to be Fleshed Out
@item dis_dsp - Stub, Needs to be Fleshed Out
@item ena_dsp - Stub, Needs to be Fleshed Out
@item chg_pri - Stub, Needs to be Fleshed Out
@item rot_rdq - Stub, Needs to be Fleshed Out
@item rel_wai - Stub, Needs to be Fleshed Out
@item get_tid - Stub, Needs to be Fleshed Out
@item ref_tsk - Stub, Needs to be Fleshed Out
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@c Task-Dependent Synchronization
@c

@section Task-Dependent Synchronization

@itemize @bullet

@item Implementation
@itemize @bullet
@item sus_tsk - Stub, Needs to be Fleshed Out
@item rsm_tsk - Stub, Needs to be Fleshed Out
@item frsm_tsk - Stub, Needs to be Fleshed Out
@item slp_tsk - Stub, Needs to be Fleshed Out
@item tslp_tsk - Stub, Needs to be Fleshed Out
@item wup_tsk - Stub, Needs to be Fleshed Out
@item can_wup - Stub, Needs to be Fleshed Out
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@item Implement Using SuperCore Message Queue Handler
@item Passes Addresses of Messages
@item FIFO or Priority Task Blocking
@item FIFO or Priority Message Ordering
@item Send Returns Error on Overflow
@end itemize

@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@item Functionality Completely Supported by SuperCore Message Queue Handler
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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
@end itemize

@item Executive Modifications
@itemize @bullet
@item None Expected
@end itemize

@item Testing
@itemize @bullet
@item No Tests Written
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

