TI Hercules TMS570
==================

TI Hercules TMS570 series development boards:

TMS570LS31:

http://www.ti.com/tool/tmds570ls31hdk

TMS570LC43:

https://www.ti.com/tool/TMDX570LC43HDK

Overview
--------

Drivers:

  o Console
  o Clock
  o Ethernet - external lwIP fork repository

BSP variants:

  TMS570LS3137:

    tms570ls3137_hdk_intram - place code and data into internal SRAM
    tms570ls3137_hdk_sdram - place code and data into external SDRAM
    tms570ls3137_hdk - variant for stand-alone RTEMS application stored
                    and running directly from flash..

  TMS570LC4357:

    tms570ls4357_hdk_sdram - place code and data into external SDRAM
    tms570lc4357_hdk - variant for stand-alone RTEMS application stored
                    and running directly from flash.

Toolchain used for development
-------------------------------

Example of RTEMS build configuration (config.ini) used for testing of self-contained applications
running directly from flash:

    [arm/tms570lc4357_hdk]

All patches required for Cortex-R and big-endian ARM support are already
integrated into the GCC and RTEMS mainline.

MCU-specific flags used during compilation are located in
`externs/rtems/spec/build/bsps/arm/tms570/abi.yml`

When linking an application to an RTEMs build, ensure the following flags are set
(arm-rtems6-gcc/g++):

    -mbe32
    -qrtems
    -T{RTEMS_BSP_VARIANT_LINKERSCRIPT}

RTEMS_BSP_VARIANT_LINKERSCRIPT can be found with the static libraries built for rtems. There
is one for each BSP variant type. For example `linkcmds.tms570lc4357_hdk`

Execution
---------

Application built by the above process can be directly programmed
into Flash and run.

The following features are implemented in the `_hdk` BSP variants:

 + Initial CPU and peripheral initialization
 + Cores Self-test

For test and debug purposes, TI's HalCoGen generated application
can be used to set up the board and then a RTEMS application
can be loaded using OpenOCD to internal SRAM or external SDRAM.
This prevents wear of Flash which has limited guaranteed
erase cycles count.

Setup application code is available there:
          https://github.com/hornmich/tms570ls3137-hdk-sdram

A branch that enables loading to Flash via openocd on the TMS570LC4357
can be found here:
    https://github.com/len0rd/openocd

TMDS570LS31HDK setup to use SDRAM to load and debug RTEMS applications
-----------------------------------------------------------------------

  o Program SDRAM_SCI_configuration-program or another boot loader
    (for example ETHERNET XCP is developed)
  o write BSP application either to sdram or intram and jump to RTEMS start code

ETHERNET
--------

For ETHERNET, the lwIP port for TMS570LS3137 has been developed
at Industrial Informatics Group of Czech Technical University
in Prague and development versions are available on SourceForge.

RTEMS and TMS570 support is included in uLAN project lwIP
repository

  https://sourceforge.net/p/ulan/lwip-omk/

This port has been consolidated with other RTEMS-LWIP ports here:

  https://gitlab.rtems.org/rtems/pkg/rtems-lwip/-/tree/main/rtemslwip/tms570

A port to the TMS570LC4357 based off this work is underway.

Adapt BSP for other TMS570 based hardware
-----------------------------------------

Complete pin multiplexer initialization according
to the list of individual pins functions is included.
Pins function definition can be found and altered
in a file

 `rtems/c/src/lib/libbsp/arm/tms570/hwinit/init_pinmux.c`

Complete "database" of all possible pin functions for
a chip is provided in a file

  `rtems/c/src/lib/libbsp/arm/tms570/include/tms570<MODEL>-pins.h`

If another package or chip is considered then tools found
in next repository can be used or extended to generate header
files and pins "database"

  https://github.com/AoLaD/rtems-tms570-utils

Links to additional information
-------------------------------

Additional information about the CPU can be found at
  https://www.ti.com/product/tms570ls3137
  https://www.ti.com/product/TMS570LC4357
