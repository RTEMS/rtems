@chapter =  The Board Support Package Structure

The BSP are all kept in the $RTEMS_ROOT/c/src/lib/libbsp directory. They
are filed under the processor family (m68k, powerpc, etc.). A given BSP
consists in the following directories: 

@itemize @bullet

@item clock : support for the realtime clock, which provides a regular
time basis to the kernel,

@item console : rather the serial driver than only a console driver, it
deals with the board UARTs (i.e. serial devices),

@item include : the include files,

@item startup : the board initialization code,

@item timer : support of timer devices,

@item shmsupp : support of shared memory in a multiprocessor system,

@item network : the KA9Q ethernet driver. 

@end itemize

Another important element are the makefiles, which have to be provided by
the user. 


Rem : You should have a copy of the gen68340 BSP
($RTEMS_ROOT/c/src/lib/libbsp/m68k/gen68340) in hand while reading this
piece of documentation. 

Further in this document we'll use the $BSP340_ROOT label for
$RTEMS_ROOT/c/src/lib/libbsp/m68k/gen68340. 

