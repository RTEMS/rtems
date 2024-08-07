CSB337
======
This is the BSP for Cogent Computer System's CSB337 and updated 
for CSB637, single board computers using the Atmel AT91RM9200 CPU.
The differences in the board are very slight but important:

```
 CSB337             CSB637
========           ========
16Mb RAM           64Mb RAM
??
```

Please check README.kit637_v6 for more explanation about the Cogent's
Development Kit that uses the CSB637 single board computer.


kit637_v6
---------
Fernando Nicodemos <fgnicodemos@terra.com.br>
from NCB - Sistemas Embarcados Ltda. (Brazil)

This is the BSP for Cogent Computer System's KIT637_V6. It implements an 
updated version of the CSB337 board with a number of optional peripherals.

This KIT is implemented by CSB637 single board computer using the 
Atmel AT91RM9200 CPU and the CSB937 target main board. It uses an 
Optrex LCD (T-51750AA, 640x480) and Touchscreen (not supported in this
BSP). The IDE and USB (host and device) interfaces are also not supported.
The SD and Compact Flash cards drivers are still under development. 

The differences in the CSB637 single board computer are very slight but 
important:

``` 
                     CSB337        CSB637
                   ==========    ==========
Clock speed        184MHz        184MHz
External memory    16MB SDRAM    64MB SDRAM
Flash memory       8MB Strata    8/16MB Strata (8MB used by default)
Video buffer       1MB           8MB
Video driver       S1D13706      S1D13506
PHY Layer          LXT971ALC     BCM5221
```

?? Some GPIO or interrupts moved around.
