mcf5225x
========
Description: embed-it dpu

CPU: MCF52259, ??MHz
SRAM: 64K
FLASH: 512K

This is a embed-it board that uses the MCF52258 Coldfire CPU. 
This board is running at ??MHz scaled from the internal relocation 8MHz oscillator.



OLD-STUFF from MCF52235 EVB ... we have to change it ...

NOTES:

Currently this BSP must be configured with most RTEMS features turned
off as RAM usage is too high.

Configure as follows:

```shell
configure --target=m68k-rtems4.XXX --enable-rtemsbsp=mcf52235 ...
```
To get the tests to compile (but not run) change the linkcmds to specify
a larger sram memory region (256K works).  This of course will let you 
compile all tests, but many or most of them wont run.

See testsuites/samples/minumum for an example of what type of config flags
you need for this BSP!

In you project before you include confdefs.h, define some or all of the
following:

#define CONFIGURE_INIT_TASK_STACK_SIZE x
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE x
#define CONFIGURE_INTERRUPT_STACK_SIZE x

Note that the default stack size is 1K
Note that the default number of priorities is 15


TODO:

*) Add drivers for I2C, ADC, FEC
*) Support for LWIP
*) Recover the 1K stack space reserved in linkcmds used for board startup.



Interrupt map
-------------
```
+-----+-----------------------------------------------------------------------+
|     |                                PRIORITY                               |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|LEVEL|    7   |    6   |    5   |    4   |    3   |    2   |    1   |    0   |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  7  |        |        |        |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  6  |        |        |        |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  5  |        |        |        |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  4  |        |        |        |        |        |        |        |   PIT  |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  3  | UART 0 | UART 1 | UART 2 |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  2  |        |        |        |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+
|  1  |        |        |        |        |        |        |        |        |
+-----+--------+--------+--------+--------+--------+--------+--------+--------+ 
```


Timing tests
------------
```
*** TIME TEST 1 ***
rtems_semaphore_create 8
rtems_semaphore_delete 10
rtems_semaphore_obtain: available 3
rtems_semaphore_obtain: not available -- NO_WAIT 3
rtems_semaphore_release: no waiting tasks 7
*** END OF TEST 1 ***


*** TIME TEST OVERHEAD ***
rtems_shutdown_executive 0
rtems_task_create 0
rtems_task_ident 0
rtems_task_start 0
rtems_task_restart 0
rtems_task_delete 0
rtems_task_suspend 0
rtems_task_resume 0
rtems_task_set_priority 0
rtems_task_mode 0
rtems_task_wake_when 0
rtems_task_wake_after 0
rtems_interrupt_catch 0
rtems_clock_get 0
rtems_clock_set 0
rtems_clock_tick 0
<pause>
rtems_timer_create 0
rtems_timer_delete 0
rtems_timer_ident 0
rtems_timer_fire_after 0
rtems_timer_fire_when 1
rtems_timer_reset 0
rtems_timer_cancel 0
rtems_semaphore_create 0
rtems_semaphore_delete 0
rtems_semaphore_ident 0
rtems_semaphore_obtain 0
rtems_semaphore_release 0
rtems_message_queue_create 0
rtems_message_queue_ident 0
rtems_message_queue_delete 0
rtems_message_queue_send 0
rtems_message_queue_urgent 0
rtems_message_queue_broadcast 0
rtems_message_queue_receive 0
rtems_message_queue_flush 0
<pause>
rtems_event_send 0
rtems_event_receive 0
rtems_signal_catch 0
rtems_signal_send 0
rtems_partition_create 0
rtems_partition_ident 0
rtems_partition_delete 0
rtems_partition_get_buffer 0
rtems_partition_return_buffer 0
rtems_region_create 0
rtems_region_ident 0
rtems_region_delete 0
rtems_region_get_segment 0
rtems_region_return_segment 0
rtems_port_create 0
rtems_port_ident 0
rtems_port_delete 0
rtems_port_external_to_internal 0
rtems_port_internal_to_external 0
<pause>
rtems_io_initialize 0
rtems_io_open 0
rtems_io_close 0
rtems_io_read 0
rtems_io_write 0
rtems_io_control 0
rtems_fatal_error_occurred 0
rtems_rate_monotonic_create 0
rtems_rate_monotonic_ident 0
rtems_rate_monotonic_delete 0
rtems_rate_monotonic_cancel 0
rtems_rate_monotonic_period 0
rtems_multiprocessing_announce 0
*** END OF TIME OVERHEAD ***
```
