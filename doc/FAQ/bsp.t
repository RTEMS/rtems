@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter BSP Questions

The items in this category provide answers to questions
commonly asked about BSPs.

@section What is a BSP?

BSP is an acronym for Board Support Package.

A BSP is a collection of device drivers, startup code, linker scripts,
and compiler support files (specs files) that tailor RTEMS for a 
particular target hardware environment.

@section What has to be in a BSP?

The basic set of items is the linker script, bsp_specs, and startup code.
If you want standard IO, then you need a console driver. This is needed
to run any of the RTEMS tests.  If you want to measure passage of time,
you need a clock tick driver.  This driver is needed for all RTEMS tests 
EXCEPT hello world and the timing tests.  The timer driver is a benchmark
timer and is needed for the tmtests (timing tests).  Sometimes you will
see a shmsupp directory which is for shared memory multiprocessing
systems.  The network driver and real-time clock drivers are optional
and not required by any RTEMS tests.



