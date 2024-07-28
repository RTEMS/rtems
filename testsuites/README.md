Testsuite
=========
This is the directory under which the RTEMS
test programs provided with the release are located.  The
following is a description of the contents of each file and
subdirectory directly in this directory:

NOTE: Other than the 'samples' directory these tests are intended
      only to exercise RTEMS features and are *not* good examples
      of programming for RTEMS.

    samples

       This directory contains a set of simple sample applications
       which can be used either to test a board support package
       or as the starting point for a custom application.

    mptests

       This directory contains the RTEMS Multiprocessor Test Suite.
       The tests in this directory provide near complete (98%+) test
       coverage of the multiprocessor specific code in RTEMS.

    psxtests

       This directory contains the RTEMS POSIX API Test Suite.
       The tests in this directory test the POSIX API support in RTEMS.

    sptests

       This directory contains the RTEMS Single Processor Test Suite.
       The tests in this directory provide near complete (98%+) test
       coverage of the non-multiprocessor code in RTEMS.

    tmtests

       This directory contains the RTEMS Timing Test Suite.
       The tests in this directory are used to measure the execution
       time of RTEMS directive and some critical internal functions.
       The results of these test are reported in the Fact Sheets
       and Supplental Manuals.



RTEMS Testsuite Configuration
-----------------------------
The RTEMS Testsuite lets you configure the tests for a BSP. Every BSP can have
a test configuration data file and this file is read when building the
tests. The test configuration data can control what tests are build and now the
tests are built.

The test configuration data files have a `.tcfg` file extension. You can
include other test configuration data files reducing repeated test
sequences. There is also a global test configuration data file for global
configurations.


Command
-------
The build system invokes the test check tool to determine what it does.

 rtems-test-check mode bsp testconfig includepaths test[s]

Mode (mode)
-----------

The check command modes are:

 1. `exclude`: The input list of tests is checked against the excluded tests
    and the tests that are not excluded are printed.

 2. `flags': The test build flags are returned. These flags can enumerate a
    test in a specific way.

BSP (bsp)
---------

The name of the BSP.


Test Configuration (testconfig)
-------------------------------
The path to the BSP's test configration file. If the file does not exist the
input list of tests is returned and there are no special build flags.


Include Paths (inputpaths)
--------------------------
A colon (`:`) separated list of paths test configuration files are search
for.


Tests (test[s])
---------------
If the mode is `exclude` this argument is a list of tests to be checked. If the
mode is `flags` a single test is required and the flags for the test are returned.


File Format
-----------
The file is an ASCII text file of lines. And text after and including the `#`
character is removed. Empty lines are ignored.

A line is either the `include` directive or a test state. The states are:

 exclude       : Exclude the test from being built.
 expected-fail : The test is built but expected to fail.
 user-input    : The test requires user input and may be aborted when running
                 the tests.
 indeterminate : The test result is indeterminate. This means the test may pass
                 or may fail therefore it cannot be included in the regression
                 results.
 benchmark     : The test is a benchmark and maybe is aborted when running the
                 tests because of the load and time the test may take to run.
                 Performance changes are currently not viewed as a regression.



Problems
--------
The tests were designed to operate in a very controlled environment.  If
one uses interrupt-driven console output, then tasks will block unexpectedly 
when a buffer filles and interrupts will perturb with the task execution order.

The tmtests should only be run with all interrupt sources disabled using
polled IO.

The following is a list of reported failures from running the tests on
a board with interrupt driven console IO.  These problems do not 
occur when using polled IO.

tm03, tm04, tm05, tm07, tm11, tm12, tm13, tm14, tm15, tm16, tm17,  
tm19, tm22, tm23, tm24 - All hit my breakpoint at  
_Internal_error_Occurred.

tm20, tm27 - hang in middle of tests.
tm26 - hangs at end of tests.

sp02 - Gets through some tests, then attempts to execute code at  
location 0.
sp04 - hangs at end of tests.
sp06 - rtems_task_restart of TA3 FAILED -- expected (0) got (4)
sp14 - hang in middle of tests
sp13, sp15 - terminate properly, but output at end is missing.
sp16 - _Internal_error_Occurred.
sp19 - rtems_clock_get FAILED -- expected (0) got (11), and hangs in  
middle of test.

cpuuse - hangs in middle of tests
malloctest - prints nothing

```
psx05 - Init: pthread_mutex_init - EINVAL (bad protocol)
../../../../../../../src/rtems-981001/c/src/tests/psxtests/psx05/../../../../../
../../src/rtems-981001/c/src/tests/psxtests/psx05/task.c:35:  
failed assertion `status == EBUSY'
N\: pthread_mutex_init - EINVAL (bad priority ceiling)
```

