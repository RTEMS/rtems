gen68360
========

This package requires a version of GCC that supports the `-mcpu32` option.

Copyright (c) 1996 Eric Norum <eric@norum.ca>

This board support package works with several different versions of
MC68360 systems.  See the conditional-compile tests in startup/init68360.c
for examples.

Decisions made at compile time include:
- If the CPU is a member of the 68040 family, the BSP is
  compiled for a generic 68040/68360 system as described
  in Chapter 9 of the MC68360 User's Manual.  This version
  can be used with the Arnewsh SBC360 card.
- If the preprocessor symbol M68360_ATLAS_HSB is defined,
  the BSP is compiled for an Atlas HSB card.
- If the preprocessor symbol M68360_IMD_PGH is defined,
  the BSP is compiled for an IMD PGH360 card.
- Otherwise, the BSP is compiled for a generic 68360 system
  as described in Chapter 9 of the MC68360 User's Manual.  This
  version works with the Atlas ACE360 card.

```
BSP NAME:	    gen68360 or gen68360_040
BOARD:              Generic 68360 as described in Motorola MC68360 User's Manual
BOARD:              Atlas Computer Equipment Inc. High Speed Bridge (HSB)
BOARD:              Atlas Computer Equipment Inc. Advanced Communication Engine (ACE)
BOARD:              Arnewsh SBC360 68040/68360 card
BOARD:              IMD PGH Board (custom)
BUS:                none
CPU FAMILY:         Motorola CPU32+, Motorola 68040
COPROCESSORS:       none
MODE:               not applicable

DEBUG MONITOR:      none (Hardware provides BDM)
```

PERIPHERALS
-----------
```
TIMERS:             PIT, Watchdog, 4 general purpose, 16 RISC
  RESOLUTION:       one microsecond
SERIAL PORTS:       4 SCC, 2 SMC, 1 SPI
REAL-TIME CLOCK:
DMA:                Each serial port, 2 general purpose
VIDEO:              none
SCSI:               none
NETWORKING:         Ethernet on SCC1.
```

DRIVER INFORMATION
------------------
```
CLOCK DRIVER:       Programmable Interval Timer
IOSUPP DRIVER:      Serial Management Controller 1
SHMSUPP:            none
TIMER DRIVER:       Timer 1
```

STDIO
-----
```
PORT:               SMC1
ELECTRICAL:         EIA-232 (if board supplies level shifter)
BAUD:               9600
BITS PER CHARACTER: 8
PARITY:             None
STOP BITS:          1
```

Board description
-----------------
clock rate:	25 MHz
bus width:	8-bit PROM/FLASH, 32-bit DRAM
ROM:		To 1 MByte, 180 nsec (3 wait states), chip select 0
RAM:		4 or 16 MBytes of 60 nsec parity DRAM (1Mx36) to RAS1*/CAS1*

Board description (IMD PGH)
---------------------------
clock rate:	25 MHz
bus width:	8-bit PROM/FLASH, 32-bit DRAM
ROM:		512KByte, 180 nsec (3 wait states), chip select 0
RAM:		16 MBytes of 60 nsec no-parity DRAM (1Mx32) to RAS1*/CAS1*

Host System
-----------
OPENSTEP 4.2 (Intel and Motorola), Solaris 2.5, Linux 2.0.29

Verification (Standalone 68360)
-------------------------------
```
Single processor tests:  Passed
Multi-processort tests:  not applicable
Timing tests:
	Context Switch
	
	context switch: self 10
	context switch: to another task 11
	context switch: no floating point contexts 38
	fp context switch: restore 1st FP task 39
	fp context switch: save initialized, restore initialized 14
	fp context switch: save idle, restore initialized 15
	fp context switch: save idle, restore idle 41
	
	Task Manager
	
	rtems_task_create 202
	rtems_task_ident 390
	rtems_task_start 71
	rtems_task_restart: calling task 99
	rtems_task_restart: suspended task -- returns to caller 86
	rtems_task_restart: blocked task -- returns to caller 116
	rtems_task_restart: ready task -- returns to caller 88
	rtems_task_restart: suspended task -- preempts caller 132
	rtems_task_restart: blocked task -- preempts caller 153
	rtems_task_restart: ready task -- preempts caller 149
	rtems_task_delete: calling task 236
	rtems_task_delete: suspended task 191
	rtems_task_delete: blocked task 195
	rtems_task_delete: ready task 198
	rtems_task_suspend: calling task 78
	rtems_task_suspend: returns to caller 36
	rtems_task_resume: task readied -- returns to caller 39
	rtems_task_resume: task readied -- preempts caller 67
	rtems_task_set_priority: obtain current priority 26
	rtems_task_set_priority: returns to caller 59
	rtems_task_set_priority: preempts caller 110
	rtems_task_mode: obtain current mode 13
	rtems_task_mode: no reschedule 15
	rtems_task_mode: reschedule -- returns to caller 20
	rtems_task_mode: reschedule -- preempts caller 67
	rtems_task_wake_after: yield -- returns to caller 16
	rtems_task_wake_after: yields -- preempts caller 65
	rtems_task_wake_when 116
	
	Interrupt Manager
	
	interrupt entry overhead: returns to nested interrupt 10
	interrupt entry overhead: returns to interrupted task 10
	interrupt entry overhead: returns to preempting task 10
	interrupt exit overhead: returns to nested interrupt 8
	interrupt exit overhead: returns to interrupted task 10
	interrupt exit overhead: returns to preempting task 59
	
	Clock Manager
	
	rtems_clock_set 73
	rtems_clock_get 1
	rtems_clock_tick 16
	
	Timer Manager
	
	rtems_timer_create 31
	rtems_timer_ident 380
	rtems_timer_delete: inactive 43
	rtems_timer_delete: active 46
	rtems_timer_fire_after: inactive 53
	rtems_timer_fire_after: active 56
	rtems_timer_fire_when: inactive 72
	rtems_timer_fire_when: active 72
	rtems_timer_reset: inactive 47
	rtems_timer_reset: active 51
	rtems_timer_cancel: inactive 25
	rtems_timer_cancel: active 28
	
	Semaphore Manager
	
	rtems_semaphore_create 59
	rtems_semaphore_ident 438
	rtems_semaphore_delete 57
	rtems_semaphore_obtain: available 31
	rtems_semaphore_obtain: not available -- NO_WAIT 31
	rtems_semaphore_obtain: not available -- caller blocks 108
	rtems_semaphore_release: no waiting tasks 40
	rtems_semaphore_release: task readied -- returns to caller 56
	rtems_semaphore_release: task readied -- preempts caller 83
	
	Message Queue Manager
	
	rtems_message_queue_create 241
	rtems_message_queue_ident 379
	rtems_message_queue_delete 75
	rtems_message_queue_send: no waiting tasks 72
	rtems_message_queue_send: task readied -- returns to caller 72
	rtems_message_queue_send: task readied -- preempts caller 99
	rtems_message_queue_urgent: no waiting tasks 72
	rtems_message_queue_urgent: task readied -- returns to caller 72
	rtems_message_queue_urgent: task readied -- preempts caller 99
	rtems_message_queue_broadcast: no waiting tasks 43
	rtems_message_queue_broadcast: task readied -- returns to caller 82
	rtems_message_queue_broadcast: task readied -- preempts caller 109
	rtems_message_queue_receive: available 52
	rtems_message_queue_receive: not available -- NO_WAIT 34
	rtems_message_queue_receive: not available -- caller blocks 111
	rtems_message_queue_flush: no messages flushed 25
	rtems_message_queue_flush: messages flushed 34
	
	Event Manager
	
	rtems_event_send: no task readied 22
	rtems_event_send: task readied -- returns to caller 50
	rtems_event_send: task readied -- preempts caller 80
	rtems_event_receive: obtain current events -1
	rtems_event_receive: available 26
	rtems_event_receive: not available -- NO_WAIT 22
	rtems_event_receive: not available -- caller blocks 89
	
	Signal Manager
	
	rtems_signal_catch 16
	rtems_signal_send: returns to caller 32
	rtems_signal_send: signal to self 51
	exit ASR overhead: returns to calling task 42
	exit ASR overhead: returns to preempting task 58
	
	Partition Manager
	
	rtems_partition_create 74
	rtems_partition_ident 379
	rtems_partition_delete 40
	rtems_partition_get_buffer: available 29
	rtems_partition_get_buffer: not available 27
	rtems_partition_return_buffer 34
	
	Region Manager
	
	rtems_region_create 63
	rtems_region_ident 388
	rtems_region_delete 40
	rtems_region_get_segment: available 43
	rtems_region_get_segment: not available -- NO_WAIT 40
	rtems_region_get_segment: not available -- caller blocks 120
	rtems_region_return_segment: no waiting tasks 48
	rtems_region_return_segment: task readied -- returns to caller 98
	rtems_region_return_segment: task readied -- preempts caller 125
	
	Dual-Ported Memory Manager
	
	rtems_port_create 38
	rtems_port_ident 380
	rtems_port_delete 40
	rtems_port_internal_to_external 22
	rtems_port_external_to_internal 22
	
	IO Manager
	
	rtems_io_initialize 4
	rtems_io_open 1
	rtems_io_close 1
	rtems_io_read 1
	rtems_io_write 1
	rtems_io_control 1
	
	Rate Monotonic Manager
	
	rtems_rate_monotonic_create 36
	rtems_rate_monotonic_ident 380
	rtems_rate_monotonic_cancel 34
	rtems_rate_monotonic_delete: active 51
	rtems_rate_monotonic_delete: inactive 47
	rtems_rate_monotonic_period: obtain status 27
	rtems_rate_monotonic_period: initiate period -- returns to caller 50
	rtems_rate_monotonic_period: conclude periods -- caller blocks 72

Network tests:
       TCP throughput (as measured by ttcp):
               Receive: 1081 kbytes/sec
               Transmit: 953 kbytes/sec
```

Porting
-------
This board support package is written for a 68360 system similar to that
described in chapter 9 of the Motorola MC68360 Quad Integrated Communication
Processor Users' Manual.  The salient features of this hardware are:

	25 MHz external clock
	DRAM address multiplexing provided by 68360
	8-bit 180nsec PROM to CS0*
	4 MBytes of 60 nsec parity DRAM (1Mx36) to RAS1*/CAS1*
	Console serial port on SMC1
	Ethernet interface on SCC1

The board support package has been tested with:
	A home-built 68360 board
	An ACE360A and an HSB board produced by:
		Atlas Computer Equipment
		703 Colina Lane
		Santa Barbara, CA 93103
	A 68040/68360 board (SBC360) produced by:
		Arnewsh Inc.
		P.O. Box 270352
		Fort Collins, CO 80527-0352
	A custom 68360 board (PGH360) produced by IMD
