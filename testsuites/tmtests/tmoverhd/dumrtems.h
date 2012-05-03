/*  dumrtems.h
 *
 *  This include file contains phony macros to map
 *  executive calls to Empty_directive().
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __DUMRTEMS_h
#define __DUMRTEMS_h

/* RTEMS call redefinition macros */

#undef rtems_signal_catch
#undef rtems_signal_send
#undef rtems_io_close
#undef rtems_io_control
#undef rtems_io_initialize
#undef rtems_io_open
#undef rtems_io_read
#undef rtems_io_write
#undef rtems_port_internal_to_external
#undef rtems_port_external_to_internal
#undef rtems_port_create
#undef rtems_port_delete
#undef rtems_port_ident
#undef rtems_event_receive
#undef rtems_event_send
#undef rtems_initialize_data_structures
#undef rtems_shutdown_executive
#undef rtems_interrupt_catch
#undef rtems_partition_create
#undef rtems_partition_delete
#undef rtems_partition_get_buffer
#undef rtems_partition_ident
#undef rtems_partition_return_buffer
#undef rtems_message_queue_broadcast
#undef rtems_message_queue_create
#undef rtems_message_queue_delete
#undef rtems_message_queue_flush
#undef rtems_message_queue_ident
#undef rtems_message_queue_receive
#undef rtems_message_queue_send
#undef rtems_message_queue_urgent
#undef rtems_region_create
#undef rtems_region_delete
#undef rtems_region_get_segment
#undef rtems_region_ident
#undef rtems_region_return_segment
#undef rtems_semaphore_create
#undef rtems_semaphore_delete
#undef rtems_semaphore_ident
#undef rtems_semaphore_obtain
#undef rtems_semaphore_release
#undef rtems_task_create
#undef rtems_task_delete
#undef rtems_task_get_note
#undef rtems_task_ident
#undef rtems_task_mode
#undef rtems_task_restart
#undef rtems_task_resume
#undef rtems_task_set_note
#undef rtems_task_set_priority
#undef rtems_task_start
#undef rtems_task_suspend
#undef rtems_clock_get
#undef rtems_clock_set
#undef rtems_clock_tick
#undef rtems_task_wake_after
#undef rtems_task_wake_when
#undef rtems_fatal_error_occurred
#undef rtems_rate_monotonic_create
#undef rtems_rate_monotonic_ident
#undef rtems_rate_monotonic_delete
#undef rtems_rate_monotonic_cancel
#undef rtems_rate_monotonic_period
#undef rtems_multiprocessing_announce
#undef rtems_timer_create
#undef rtems_timer_ident
#undef rtems_timer_delete
#undef rtems_timer_cancel
#undef rtems_timer_fire_after
#undef rtems_timer_fire_when
#undef rtems_timer_reset

#define     rtems_signal_catch( asraddr, mode ) \
           Empty_directive()
#define     rtems_signal_send( tid, signal ) \
           Empty_directive()

#define     rtems_io_close( major, minor, argp, rval ) \
           Empty_directive()
#define     rtems_io_control( major, minor, argp, rval ) \
           Empty_directive()
#define     rtems_io_initialize( major, minor, argp, rval ) \
           Empty_directive()
#define     rtems_io_open( major, minor, argp, rval ) \
           Empty_directive()
#define     rtems_io_read( major, minor, argp, rval ) \
           Empty_directive()
#define     rtems_io_write( major, minor, argp, rval ) \
           Empty_directive()

#define     rtems_port_internal_to_external( dpid, internal, external ) \
           Empty_directive()
#define     rtems_port_external_to_internal( dpid, external, internal ) \
           Empty_directive()
#define     rtems_port_create( name, intaddr, extaddr, length, dpid ) \
           Empty_directive()
#define     rtems_port_delete( dpid ) \
           Empty_directive()
#define     rtems_port_ident( name, dpid ) \
           Empty_directive()

#define     rtems_event_receive( eventin, options, timeout, eventout ) \
           Empty_directive()
#define     rtems_event_send( tid, event ) \
           Empty_directive()

#define     rtems_initialize_executive( conftbl, cputbl ) \
           Empty_directive()
#define     rtems_shutdown_executive( the_error ) \
           Empty_directive()

#define     rtems_interrupt_catch( israddr, vector, oldisr ) \
           Empty_directive()

#define     rtems_partition_create( name, paddr, length, bsize, attr, ptid ) \
           Empty_directive()
#define     rtems_partition_delete( ptid ) \
           Empty_directive()
#define     rtems_partition_get_buffer( ptid, bufaddr ) \
           Empty_directive()
#define     rtems_partition_ident( name, node, ptid ) \
           Empty_directive()
#define     rtems_partition_return_buffer( ptid, bufaddr ) \
           Empty_directive()

#define     rtems_message_queue_broadcast( qid, buffer, count ) \
           Empty_directive()
#define     rtems_message_queue_create( name, count, attr, qid ) \
           Empty_directive()
#define     rtems_message_queue_delete( qid ) \
           Empty_directive()
#define     rtems_message_queue_flush( qid, count ) \
           Empty_directive()
#define     rtems_message_queue_ident( name, node, qid ) \
           Empty_directive()
#define     rtems_message_queue_receive( qid, buffer, options, timeout ) \
           Empty_directive()
#define     rtems_message_queue_send( qid, buffer ) \
           Empty_directive()
#define     rtems_message_queue_urgent( qid, buffer ) \
           Empty_directive()

#define     rtems_region_create( name, paddr, length, pagesize, attr, rnid ) \
           Empty_directive()
#define     rtems_region_delete( rnid ) \
           Empty_directive()
#define     rtems_region_get_segment( rnid, size, options, timeout, segaddr ) \
           Empty_directive()
#define     rtems_region_ident( name, rnid ) \
           Empty_directive()
#define     rtems_region_return_segment( rnid, segaddr ) \
           Empty_directive()

#define     rtems_semaphore_create( name, count, attr, priceil, smid ) \
           Empty_directive()
#define     rtems_semaphore_delete( smid ) \
           Empty_directive()
#define     rtems_semaphore_ident( name, node, smid ) \
           Empty_directive()
#define     rtems_semaphore_obtain( smid, options, timeout ) \
           Empty_directive()
#define     rtems_semaphore_release( smid ) \
           Empty_directive()

#define     rtems_task_create( name, priority, stack_size, mode, attr, tid ) \
           Empty_directive()
#define     rtems_task_delete( tid ) \
           Empty_directive()
#define     rtems_task_get_note( tid, notepad, note ) \
           Empty_directive()
#define     rtems_task_ident( name, node, tid ) \
           Empty_directive()
#define     rtems_task_mode( mode, mask, pmode ) \
           Empty_directive()
#define     rtems_task_restart( tid, arg ) \
           Empty_directive()
#define     rtems_task_resume( tid ) \
           Empty_directive()
#define     rtems_task_set_note( tid, notepad, note ) \
           Empty_directive()
#define     rtems_task_set_priority( tid, priority, ppriority ) \
           Empty_directive()
#define     rtems_task_start( tid, saddr, arg ) \
           Empty_directive()
#define     rtems_task_suspend( tid ) \
           Empty_directive()

#define     rtems_clock_get( options, time ) \
           Empty_directive()
#define     rtems_clock_set( time ) \
           Empty_directive()
#define     rtems_clock_tick() \
           Empty_directive()
#define     rtems_task_wake_after( ticks ) \
           Empty_directive()
#define     rtems_task_wake_when( time ) \
           Empty_directive()

#define     rtems_fatal_error_occurred( errcode ) \
           Empty_directive()

#define     rtems_rate_monotonic_create( name, id ) \
           Empty_directive()
#define     rtems_rate_monotonic_ident( name, id ) \
           Empty_directive()
#define     rtems_rate_monotonic_delete( id ) \
           Empty_directive()
#define     rtems_rate_monotonic_cancel( id ) \
           Empty_directive()
#define     rtems_rate_monotonic_period( id, period ) \
           Empty_directive()

#define     rtems_multiprocessing_announce() \
           Empty_directive()

#define     rtems_timer_create( name, tmid ) \
           Empty_directive()
#define     rtems_timer_delete( tmid ) \
           Empty_directive()
#define     rtems_timer_cancel( tmid ) \
           Empty_directive()
#define     rtems_timer_ident( name, tmid ) \
           Empty_directive()
#define     rtems_timer_fire_after( tmid, ticks, routine, user_data ) \
           Empty_directive()
#define     rtems_timer_fire_when( tmid, time, routine, user_data ) \
           Empty_directive()
#define     rtems_timer_reset( tmid ) \
           Empty_directive()

#if defined(__GNUC__)
#define RTEMS_GCC_NOWARN_UNUSED	__attribute__((unused))
#else
#define RTEMS_GCC_NOWARN_UNUSED
#endif

#endif
/* end of include file */
