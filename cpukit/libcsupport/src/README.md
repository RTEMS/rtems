libcsupport
===========

Overview of newlib support (newlib is from CYGNUS)
    Each task can have its own libc state including:
        open stdio files
        strtok
        multi precision arithmetic state
        etc.

    This is implemented by a reentrancy data structure for each task.

    When a task is "started" (in RTEMS sense) the reentrancy structure
    is allocated.

    When task is switched to, the value of global variable _impure_ptr
    is changed to the value of the new tasks reentrancy structure.

    When a task is deleted
        atexit() processing (for that task) happens
        task's stdio buffers are flushed

    When exit(3) is called
        calling task's atexit processing done
        global libc state atexit processing done
            (this will include any atexit routines installed by drivers)
        executive is shutdown
            causes a context switch back to bsp land


NOTE:
    libc extension are installed by bsp_libc_init()
        iff we are using clock interrupts.
        This hack is necessary to allow the tmtests to avoid
        timing the extensions.


Cases
-----
This is a list of cases to consider when implementing a file system:


```
+ Given a tree of this form:

        a ----- b
               /
             c
```

  Where a and b are directories and c is a link to directory b.  Consider
  this sequence:

    - rmdir a/b
    - mknod c/b/x
    - unlink c


TODO
----
+ newlib 1.8.0 has the wrong prototype for at least read() and write().

+ There should be a "eat it" stub for all system calls which are
  available to make filling out an operations table easier.  
  See device_lseek() for an example of where this would be nice.

+ Fix strerror() so it prints all error numbers.

+ Check the node allocation coment in the fchdir call.
+ Add an interface somewhere for this call.
