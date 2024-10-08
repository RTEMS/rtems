jmr3904
=======
Simple BSP for the TX3904 simulator built into gdb.

Simulator Invocation
--------------------
The following is how the simulator is invoked.

target sim --board=jmr3904

GDB must be configured with a target like "tx39-rtems".  Otherwise,
the simulator will not be built for the correct instruction
and peripheral set.

Simulator Information
---------------------
The simulated system clock counts instructions.  Setting the clock
source to "clock" and the divider to 1 results in the timer directly
counting the number of instructions executed.

Status
------

+ hello.exe locks up while running the global destructors.  This almost
  has to be a linkcmds issue.

+ Workaround: bspclean.c actually explicits invokes _sys_exit() BEFORE
  letting the global destructors run.

+ There is a clock tick device driver which has not been calibrated.

+ There is no timer device driver.
