Monitor task
============

The monitor task is an optional task that knows about RTEMS
data structures and can print out information about them.
It is a work-in-progress and needs many more commands, but
is useful now.

The monitor works best when it is the highest priority task,
so all your other tasks should ideally be at some priority
greater than 1.

To use the monitor:
-------------------

    ```c
    #include <rtems/monitor.h>

        ...

    rtems_monitor_init(0);
    ```

    The parameter to rtems_monitor_init() tells the monitor whether
    to suspend itself on startup.  A value of 0 causes the monitor
    to immediately enter command mode; a non-zero value causes the
    monitor to suspend itself after creation and wait for explicit
    wakeup.


    rtems_monitor_wakeup();
    
    wakes up a suspended monitor and causes it to reenter command mode.

Monitor commands
----------------

    The monitor prompt is 'rtems> '.
    Can abbreviate commands to "uniquity"
    There is a 'help' command.  Here is the output from various
    help commands:

        Commands (may be abbreviated)

```
          help      -- get this message or command specific help
          task      -- show task information
          queue     -- show message queue information
          symbol    -- show entries from symbol table
          pause     -- pause monitor for a specified number of ticks
          fatal     -- invoke a fatal RTEMS error

        task [id [id ...] ]
          display information about the specified tasks.
          Default is to display information about all tasks on this node

        queue [id [id ... ] ]
          display information about the specified message queues
          Default is to display information about all queues on this node

        symbol [ symbolname [symbolname ... ] ]
          display value associated with specified symbol.
          Defaults to displaying all known symbols.

        pause [ticks]
          monitor goes to "sleep" for specified ticks (default is 1)
          monitor will resume at end of period or if explicitly awakened

        fatal [status]
          Invoke 'rtems_fatal_error_occurred' with 'status'
          (default is RTEMS_INTERNAL_ERROR)

        continue
          put the monitor to sleep waiting for an explicit wakeup from the
          program running.
```


Sample output from 'task' command
---------------------------------

```
    rtems> task
      ID       NAME   PRIO   STAT   MODES  EVENTS   WAITID  WAITARG  NOTES
    ------------------------------------------------------------------------
    00010001   UI1     2    READY    P:T:nA    NONE15: 0x40606348
    00010002   RMON    1    READY    nP    NONE15: 0x40604110

    'RMON' is the monitor itself, so we have 1 "user" task.
    Its modes are P:T:nA which translate to:

        preemptable
        timesliced
        no ASRS

    It has no events.
    (this is the libc thread state)
```
