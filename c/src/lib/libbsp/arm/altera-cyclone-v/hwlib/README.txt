HWLIB
=====
Hwlib is a collection of sources provided by Altera for the Cyclone-V.

As hwlib is third party software, please keep modifications and additions 
to the sources to a minimum for easy maintenance. Otherwise updating to a 
new version of hwlib released by Altera can become difficult.

The hwlib directory contains only those files from Alteras hwlib which are
required by the BSP (the whole hwlib was considered too big).
The directory structure within the hwlib directory is equivalent to Alteras
hwlib directory structure. For easy maintenance only whole files have been
left out.

Altera provides the hwlib with their SoC Embedded Design Suite (EDS).

HWLIB Version:
--------------
The files have been taken from the following hwlib versions:

|========================================
| Version | File
|         |
| 13.0SP1 | include/alt_address_space.h
| 13.0SP1 | include/alt_clock_group.h
| 13.0SP1 | include/alt_clock_manager.h
| 13.0SP1 | include/alt_generalpurpose_io.h
| 13.0SP1 | include/alt_hwlibs_ver.h
| 13.1    | include/alt_i2c.h
| 13.0SP1 | include/alt_interrupt_common.h
| 13.0SP1 | include/alt_mpu_registers.h
| 13.0SP1 | include/alt_reset_manager.h
| 13.0SP1 | include/hwlib.h
| 13.0SP1 | include/socal/alt_clkmgr.h
| 13.0SP1 | include/socal/alt_gpio.h
| 13.1    | include/socal/alt_i2c.h
| 13.0SP1 | include/socal/alt_l3.h
| 13.0SP1 | include/socal/alt_rstmgr.h
| 13.0SP1 | include/socal/alt_sdr.h
| 13.0SP1 | include/socal/alt_sysmgr.h
| 13.0SP1 | include/socal/alt_uart.h
| 13.0SP1 | include/socal/hps.h
| 13.0SP1 | include/socal/socal.h
| 13.0SP1 | src/hwmgr/alt_address_space.c
| 13.0SP1 | src/hwmgr/alt_clock_manager.c
| 13.0SP1 | src/hwmgr/alt_generalpurpose_io.c
| 13.1    | src/hwmgr/alt_i2c.c
| 13.0SP1 | src/hwmgr/alt_reset_manager.c
|========================================

hwlib 13.0SP1 is from SoC EDS 13.0.1.232
hwlib 13.1 is from SoC EDS 14.0.0.200
