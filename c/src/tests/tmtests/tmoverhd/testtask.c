/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_unsigned8 Memory_area[ 2048 ];
rtems_unsigned8 Internal_port_area[ 256 ];
rtems_unsigned8 External_port_area[ 256 ];

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Init(
  rtems_task_argument argument
)
{ rtems_id id;
  rtems_status_code status;

  Set_find_average_overhead( TRUE );

  Print_Warning();

  puts( "\n\n*** TIME TEST OVERHEAD ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    254,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( id, Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

/* comment out the following include to verify type are correct */
#include "dumrtems.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_name                 name;
  rtems_unsigned32           index;
  rtems_cpu_table            cpu_table;
  rtems_id                   id;
  rtems_task_priority        in_priority;
  rtems_task_priority        out_priority;
  rtems_mode                 in_mode;
  rtems_mode                 mask;
  rtems_mode                 out_mode;
  rtems_unsigned32           note;
  rtems_time_of_day          time;
  rtems_interval             timeout;
  rtems_signal_set           signals;
  void                      *address_1;
  rtems_event_set            events;
  long                       buffer[ 4 ];
  rtems_unsigned32           count;
  rtems_device_major_number  major;
  rtems_device_minor_number  minor;
  rtems_unsigned32           io_result;
  rtems_unsigned32           error;
  rtems_clock_get_options    options;

  name        = rtems_build_name( 'N', 'A', 'M', 'E' );
  in_priority = 250;
  in_mode     = RTEMS_NO_PREEMPT;
  mask        = RTEMS_PREEMPT_MASK;
  note        = 8;
  timeout     = 100;
  signals     = RTEMS_SIGNAL_1 | RTEMS_SIGNAL_3;
  major       = 10;
  minor       = 0;
  error       = 100;
  options     = 0;

/* rtems_initialize_executive */
  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_initialize_executive( rtems_configuration_get_table(), &cpu_table );
  end_time = Read_timer();

  put_time(
    "rtems_initialize_executive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

/* rtems_shutdown_executive */

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_shutdown_executive( error );
  end_time = Read_timer();

  put_time(
    "rtems_shutdown_executive",
    end_time,
    OPERATION_COUNT,
    overhead,
    0
  );

/* rtems_task_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_create(
               name,
               in_priority,
               RTEMS_MINIMUM_STACK_SIZE,
               RTEMS_DEFAULT_MODES,
               RTEMS_DEFAULT_ATTRIBUTES,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_task_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_ident( name, RTEMS_SEARCH_ALL_NODES, id );
      end_time = Read_timer();

      put_time(
         "rtems_task_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_start */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_start( id, Task_1, 0 );
      end_time = Read_timer();

      put_time(
         "rtems_task_start",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_restart */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_restart( id, 0 );
      end_time = Read_timer();

      put_time(
         "rtems_task_restart",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_task_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_suspend */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_suspend( id );
      end_time = Read_timer();

      put_time(
         "rtems_task_suspend",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_resume */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_resume( id );
      end_time = Read_timer();

      put_time(
         "rtems_task_resume",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_set_priority */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_set_priority( id, in_priority, &out_priority );
      end_time = Read_timer();

      put_time(
         "rtems_task_set_priority",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_mode */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_mode( in_mode, mask, &out_mode );
      end_time = Read_timer();

      put_time(
         "rtems_task_mode",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_get_note */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_get_note( id, 1, note );
      end_time = Read_timer();

      put_time(
         "rtems_task_get_note",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_set_note */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_set_note( id, 1, note );
      end_time = Read_timer();

      put_time(
         "rtems_task_set_note",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_wake_when */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_wake_when( time );
      end_time = Read_timer();

      put_time(
         "rtems_task_wake_when",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_task_wake_after */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_task_wake_after( timeout );
      end_time = Read_timer();

      put_time(
         "rtems_task_wake_after",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_interrupt_catch */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_interrupt_catch( Isr_handler, 5, address_1 );
      end_time = Read_timer();

      put_time(
         "rtems_interrupt_catch",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_clock_get */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_clock_get( options, time );
      end_time = Read_timer();

      put_time(
         "rtems_clock_get",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_clock_set */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_clock_set( time );
      end_time = Read_timer();

      put_time(
         "rtems_clock_set",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_clock_tick */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
           (void) rtems_clock_tick();
      end_time = Read_timer();

      put_time(
         "rtems_clock_tick",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

rtems_test_pause();

/* rtems_timer_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_create( name, &id );
      end_time = Read_timer();

      put_time(
         "rtems_timer_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_timer_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_ident( name, id );
      end_time = Read_timer();

      put_time(
         "rtems_timer_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_fire_after */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_fire_after(
               id,
               timeout,
               Timer_handler,
               NULL
            );
      end_time = Read_timer();

      put_time(
         "rtems_timer_fire_after",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_fire_when */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_fire_when(
               id,
               time,
               Timer_handler,
               NULL
            );
      end_time = Read_timer();

      put_time(
         "rtems_timer_fire_when",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_reset */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_reset( id );
      end_time = Read_timer();

      put_time(
         "rtems_timer_reset",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_timer_cancel */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_timer_cancel( id );
      end_time = Read_timer();

      put_time(
         "rtems_timer_cancel",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_semaphore_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_semaphore_create(
               name,
               128,
               RTEMS_DEFAULT_ATTRIBUTES,
               RTEMS_NO_PRIORITY,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_semaphore_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_semaphore_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_semaphore_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_semaphore_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_semaphore_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_semaphore_ident( name, RTEMS_SEARCH_ALL_NODES, id );
      end_time = Read_timer();

      put_time(
         "rtems_semaphore_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_semaphore_obtain */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_semaphore_obtain( id, RTEMS_DEFAULT_OPTIONS, timeout );
      end_time = Read_timer();

      put_time(
         "rtems_semaphore_obtain",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_semaphore_release */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_semaphore_release( id );
      end_time = Read_timer();

      put_time(
         "rtems_semaphore_release",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_create(
               name,
               128,
               RTEMS_DEFAULT_ATTRIBUTES,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_ident(
              name,
              RTEMS_SEARCH_ALL_NODES,
              id
            );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_send */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_send( id, (long (*)[4])buffer );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_send",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_urgent */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_urgent( id, (long (*)[4])buffer );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_urgent",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_broadcast */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_broadcast(
               id,
               (long (*)[4])buffer,
               &count
            );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_broadcast",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_receive */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_receive(
               id,
               (long (*)[4])buffer,
               RTEMS_DEFAULT_OPTIONS,
               timeout
            );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_receive",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_message_queue_flush */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_message_queue_flush( id, &count );
      end_time = Read_timer();

      put_time(
         "rtems_message_queue_flush",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

rtems_test_pause();

/* rtems_event_send */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_event_send( id, events );
      end_time = Read_timer();

      put_time(
         "rtems_event_send",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_event_receive */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_event_receive(
               RTEMS_EVENT_16,
               RTEMS_DEFAULT_OPTIONS,
               timeout,
               &events
            );
      end_time = Read_timer();

      put_time(
         "rtems_event_receive",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_signal_catch */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_signal_catch( Asr_handler, RTEMS_DEFAULT_MODES );
      end_time = Read_timer();

      put_time(
         "rtems_signal_catch",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_signal_send */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_signal_send( id, signals );
      end_time = Read_timer();

      put_time(
         "rtems_signal_send",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_partition_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_partition_create(
               name,
               Memory_area,
               2048,
               128,
               RTEMS_DEFAULT_ATTRIBUTES,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_partition_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_partition_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_partition_ident( name, RTEMS_SEARCH_ALL_NODES, id );
      end_time = Read_timer();

      put_time(
         "rtems_partition_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_partition_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_partition_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_partition_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_partition_get_buffer */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_partition_get_buffer( id, address_1 );
      end_time = Read_timer();

      put_time(
         "rtems_partition_get_buffer",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_partition_return_buffer */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_partition_return_buffer( id, address_1 );
      end_time = Read_timer();

      put_time(
         "rtems_partition_return_buffer",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_region_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_region_create(
               name,
               Memory_area,
               2048,
               128,
               RTEMS_DEFAULT_ATTRIBUTES,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_region_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_region_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_region_ident( name, id );
      end_time = Read_timer();

      put_time(
         "rtems_region_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_region_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_region_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_region_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_region_get_segment */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_region_get_segment(
               id,
               243,
               RTEMS_DEFAULT_OPTIONS,
               timeout,
               &address_1
            );
      end_time = Read_timer();

      put_time(
         "rtems_region_get_segment",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_region_return_segment */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_region_return_segment( id, address_1 );
      end_time = Read_timer();

      put_time(
         "rtems_region_return_segment",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_port_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_port_create(
               name,
               Internal_port_area,
               External_port_area,
               0xff,
               &id
            );
      end_time = Read_timer();

      put_time(
         "rtems_port_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_port_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_port_ident( name, id );
      end_time = Read_timer();

      put_time(
         "rtems_port_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_port_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_port_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_port_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_port_external_to_internal */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_port_external_to_internal(
               id,
               &External_port_area[ 7 ],
               address_1
            );
      end_time = Read_timer();

      put_time(
         "rtems_port_external_to_internal",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_port_internal_to_external */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_port_internal_to_external(
               id,
               &Internal_port_area[ 7 ],
               address_1
            );
      end_time = Read_timer();

      put_time(
         "rtems_port_internal_to_external",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

rtems_test_pause();

/* rtems_io_initialize */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_initialize(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_initialize",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_io_open */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_open(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_open",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_io_close */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_close(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_close",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_io_read */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_read(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_read",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_io_write */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_write(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_write",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_io_control */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_io_control(
               major,
               minor,
               address_1,
               &io_result
            );
      end_time = Read_timer();

      put_time(
         "rtems_io_control",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_fatal_error_occurred */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_fatal_error_occurred( error );
      end_time = Read_timer();

      put_time(
         "rtems_fatal_error_occurred",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_rate_monotonic_create */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_rate_monotonic_create( name, &id );
      end_time = Read_timer();

      put_time(
         "rtems_rate_monotonic_create",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_rate_monotonic_ident */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_rate_monotonic_ident( name, id );
      end_time = Read_timer();

      put_time(
         "rtems_rate_monotonic_ident",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_rate_monotonic_delete */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_rate_monotonic_delete( id );
      end_time = Read_timer();

      put_time(
         "rtems_rate_monotonic_delete",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_rate_monotonic_cancel */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_rate_monotonic_cancel( id );
      end_time = Read_timer();

      put_time(
         "rtems_rate_monotonic_cancel",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_rate_monotonic_period */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_rate_monotonic_period( id, timeout );
      end_time = Read_timer();

      put_time(
         "rtems_rate_monotonic_period",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

/* rtems_multiprocessing_announce */

      Timer_initialize();
         for ( index = 1 ; index <= OPERATION_COUNT ; index ++ )
            (void) rtems_multiprocessing_announce();
      end_time = Read_timer();

      put_time(
         "rtems_multiprocessing_announce",
         end_time,
         OPERATION_COUNT,
         overhead,
         0
      );

  puts( "*** END OF TIME OVERHEAD ***" );

  rtems_test_exit( 0 );
}
