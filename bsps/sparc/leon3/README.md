leon3
=====
```
BSP NAME:           leon3
BUS:                AMBA Plug & Play 
CPU FAMILY:         sparc
CPU:                LEON3
```

DRIVERS
=======
Timer Driver, Console Driver, Opencores Ethernet Driver


Notes
-----
This BSP supports single LEON3-processor system with minimum peripheral
configuration of one UART. BSP reads system configuration area to get
information such as memory mapping and usage of interrupt resources and
installs device drivers based on this information.

There are no restrictions on memory mapping of UARTS. Console driver
operates in polled mode.

Console driver uses timer 0 of General Purpose Timer and must be configured
to use separate interrupts for each timer. No restrictions on memory mapping.
Interrupt request signal can not be shared with other devices.  

Ethernet MAC core can be mapped in arbitrary memory address space and use
arbitrary interrupt request signal. Interrupt request signal can not be
shared with other devices.
