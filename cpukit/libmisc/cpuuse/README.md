cpuuse
======

This directory contains code to report and reset per-task CPU usage.
If the BSP supports nanosecond timestamp granularity, this this information
is very accurate.  Otherwise, it is dependendent on the tick granularity. 

It provides two primary features:

  + Generate a CPU Usage Report
  + Reset CPU Usage Information

NOTES:

1.  If configured for tick granularity, CPU usage is "docked" by a
    clock tick at each context switch.
2.  If configured for nanosecond granularity, no work is done at each
    clock tick.  All bookkeeping is done as part of a context switch.


