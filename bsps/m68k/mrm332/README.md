mrm322
======
```
CPU: MC68332 @16 or 25MHz
RAM: 32k or 512k
ROM: 512k flash
```
   The Mini RoboMind is a small board based on the 68332 microcontroller
designed and build by Mark Castelluccio.  For details, see:

        http://www.robominds.com

   This BSP was ported from the efi332 BSP by Matt Cross (profesor@gweep.net),
the efi332 BSP was written by John S Gwynne.


TODO
----
- integrate the interrupt driven stdin/stdout into RTEMS to (a) reduce
  the interrupt priority and (2) to prevent it from blocking.
- add a timer driver for the tmtest set.
