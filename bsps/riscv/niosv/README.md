# Intel NIOSV BSP

The Intel NIOSV BSP implementation provides the base components needed for any
NIOSV specific BSP: clock, console, irq, and timer. An example BSP for the Intel
Cyclone 10 LP Evaluation Board is included as a reference design and test
hardware. The base BSP assumes that the FPGA and NIOSV has the following
devices/IP attached to the processor.

 * Generic QUAD SPI Controlller II (EPCQ) - the serial flash memory device
 connected to the FPGA and contains the FPGA configuration, software image, and
 user data area.
 * On Chip ROM - an On-Chip Memory IP configured as ROM. Contains boot loader to
 load software image from the EPCQ device.
 * On Chip RAM - an On-Chip Memory IP configured as RAM. Used by the boot loader
 for data and bss. Can be used by software image as well.
 * External RAM Interface - an external memory device IP. In the case of the
 Cyclone 10 LP Evaluation Board, a hyperbus controller which is attached to a
 HyperRAM device.
 * Benchmark Timer - a modified Interval Timer IP which will be used as the BSPs
 benchmark timer.
 * Watchdog Timer - an Interval Timer IP configured as a watchdog which will be
 used to reset the NIOSV.
 * JTAG UART - an JTAG UART IP which will be used as the console port.

The following BSP parameters need to be set in the config.ini file when building
RTEMS in order to set up the memory map for the NIOSV:

 * NIOSV_EPCQ_ROM_REGION_BEGIN - the starting address of the EPCQ device
 attached to the NIOSV
 * NIOSV_EPCQ_ROM_REGION_SIZE - the size in bytes of the EPCQ device.
 * NIOSV_ONCHIP_ROM_REGION_BEGIN - the starting address of the onchip rom IP
 attached to the NIOSV
 * NIOSV_ONCHIP_ROM_REGION_SIZE - the size in bytes of the onchip rom IP device.
 * NIOSV_ONCHIP_RAM_REGION_BEGIN - the starting address of the onchip ram IP
 attached to the NIOSV
 * NIOSV_ONCHIP_RAM_REGION_SIZE - the size in bytes of the onchip ram IP device.
 * NIOSV_EXT_RAM_REGION_BEGIN - the starting address of the external ram
 interface IP attached to the NIOSV
 * NIOSV_EXT_RAM_REGION_SIZE - the size in bytes of the external ram interface
 IP device.
 * NIOSV_IS_NIOSVG - Whether or not the **NIOS V/g** processor is used.
 * NIOSV_HAS_FP - Whether or not the **NIOS V/g** processor has a FPU.

## Intel Cyclone 10 LP Evaluation Board Example

The **rtems_vm.jic** file included with this BSP is a Cyclone 10 LP Evaluation
Board programming file that can be downloaded to the board and has the following
IP:

* NIOS V/m Microcontroller
  * timer_sw_agent - Data Master: 0x10000100 - 0x1000013f
  * dm_agent - Instruction Master: 0x10030000 - 0x1003ffff,
  Data Master: 0x10030000 - 0x1003ffff
* Generic QUAD SPI Controlller II
  * avl_csr - Data Master: 0x10000140 - 0x1000017f
  * avl_mem - Data Master: 0x11000000 - 0x11ffffff
* On-Chip Memory (RAM or ROM) configured as ROM
  * s1 - Instruction Master: 0x10010000 - 0x10010fff,
  Data Master: 0x10010000 - 0x10010fff
* On-Chip Memory (RAM or ROM) configured as RAM
  * s1 - Instruction Master: 0x10020000 - 0x10021fff,
  Data Master: 0x10020000 - 0x10021fff
* HyperBus Controller (x8) - Infineon HyperBus Controller IP which can control
up to 2 HyperRAM devices. The Cyclone 10 LP Evaluation board has the HyperRAM
connected to the 2nd device (chip select 2). The boot loader in the On-Chip ROM
sets the HyperRAM base address to 0x01000000 in the slave register.
  * axi4_slave_memory - Instruction Master: 0x00000000 - 0x0fffffff,
  Data Master: 0x00000000 - 0x0fffffff
  * axi4_slave_register - Data Master: 0x10000000 - 0x100000ff
* Interval Timer configured for free running benchmark timer. Modified from
original Interval Timer to include a prescalar to adjust the clock rate.
  * s1 - Data Master: 0x10000180 - 0x1000019f
* Interval Timer configured as watchdog timer
  * s1 - Data Master: 0x100001c0 - 0x100001df
* System ID Peripheral
  * control_slave - Data Master: 0x10000200 - 0x10000207
* JTAG UART
  * jtag_slave - Data Master: 0x10000208 - 0x1000020f
* PIO (Parallel I/O) configured as outputs for turning on LEDs
  * s1 - Data Master: 0x100001a0 - 0x100001bf
* PIO (Parallel I/O) configured as inputs for reading dip switch settings
  * s1 - Data Master: 0x100001f0 - 0x100001ff
* PIO (Parallel I/O) configured as inputs for reading push buttons
  * s1 - Data Master: 0x100001e0 - 0x100001ef

Here are the associated config.ini entries:

[riscv/niosvc10lp]
NIOSV_EPCQ_ROM_REGION_BEGIN = 0x11000000
NIOSV_EPCQ_ROM_REGION_SIZE = 0x01000000
NIOSV_ONCHIP_ROM_REGION_BEGIN = 0x10010000
NIOSV_ONCHIP_ROM_REGION_SIZE = 4096
NIOSV_ONCHIP_RAM_REGION_BEGIN = 0x10020000
NIOSV_ONCHIP_RAM_REGION_SIZE = 8192
NIOSV_EXT_RAM_REGION_BEGIN = 0x01000000
NIOSV_EXT_RAM_REGION_SIZE = 0x00800000
NIOSV_IS_NIOSVG = False
NIOSV_HAS_FP = False

The **rtems_vg.jic** file included has the same peripheral IP but with a
**NIOS V/g** processor.

Here are the associated config.ini entries:

[riscv/niosvc10lp]
NIOSV_EPCQ_ROM_REGION_BEGIN = 0x11000000
NIOSV_EPCQ_ROM_REGION_SIZE = 0x01000000
NIOSV_ONCHIP_ROM_REGION_BEGIN = 0x10010000
NIOSV_ONCHIP_ROM_REGION_SIZE = 4096
NIOSV_ONCHIP_RAM_REGION_BEGIN = 0x10020000
NIOSV_ONCHIP_RAM_REGION_SIZE = 8192
NIOSV_EXT_RAM_REGION_BEGIN = 0x01000000
NIOSV_EXT_RAM_REGION_SIZE = 0x00800000
NIOSV_IS_NIOSVG = True
NIOSV_HAS_FP = False

The **rtems_vgfp.jic** file included has the same peripheral IP but with a
**NIOS V/g** processor with the FPU enabled.

Here are the associated config.ini entries:

[riscv/niosvc10lp]
NIOSV_EPCQ_ROM_REGION_BEGIN = 0x11000000
NIOSV_EPCQ_ROM_REGION_SIZE = 0x01000000
NIOSV_ONCHIP_ROM_REGION_BEGIN = 0x10010000
NIOSV_ONCHIP_ROM_REGION_SIZE = 4096
NIOSV_ONCHIP_RAM_REGION_BEGIN = 0x10020000
NIOSV_ONCHIP_RAM_REGION_SIZE = 8192
NIOSV_EXT_RAM_REGION_BEGIN = 0x01000000
NIOSV_EXT_RAM_REGION_SIZE = 0x00800000
NIOSV_IS_NIOSVG = True
NIOSV_HAS_FP = True

### Intel Cyclone 10 LP Evaluation Board Implementation Details

In order to make it easier to create a RTEMS BSP for a NIOSV project built in
Quartus Prime, this BSP used the **system.h** file that is generated by the
Quartus Prime's BSP editor. The file was renamed to **bsp_system.h**. The header
guard was modifed from **__SYSTEM_H_** to **__BSP_SYSTEM_H_** to reflect the new
name of the file. The **#include "linker.h"** line was also removed as that is
not needed.

This BSP also makes use of the device driver code generated from the Quartus
Prime's BSP editor with some minor changes to make the code easier to read. A
developer could use the same strategy or create something from scratch.

The hyperbus controller IP that comes with the Cyclone 10 LP Evaluation Board is
proprietary and requires a license. Instead, the BSP uses a hyperbus controller
IP that can be requested from Infineon (no link provided because it most likely
will change). Just use your favorite search engine and you should be able find
the request form. You may need to answer some marketing questions but I had no
trouble getting it. The IP is written for Xilinx/AMD parts so you will have to
write some Verilog code to connect it to a Cyclone 10 LP. I was a novice so it
took me a while to get it to work. The biggest revelation that took me forever
to figure out was that you have to use a Input Delay From Pin assignment to meet
RWDS timing on a read.  I was trying to use logic for this which was not the
right approach.

The On-Chip Memory IP configured as ROM needs to have a boot loader pre-loaded.
Follow the instructions outlined in the Quartus Prime software user guide to
initialize the ROM with the **bootloader_niosvc10lp_xx.hex** file include with
this BSP. This boot loader will initialize the hyperbus controller and load any
application image found at offset 0x100000 within the EPCQ device to the
HyperRAM (i.e. NIOSV_EXT_RAM_REGION_BEGIN). The boot loader will validate the
application image before loading it into the HyperRAM using a CRC32 algorithm.
If the image was loaded successfully in the HyperRAM, the first two LEDs will be
on; otherwise, the first, second, and fourth LEDs will be on indicating a load
failure.

The application image must have the following header information located at
offset 0x100000 within the EPCQ device in order for the boot loader to load the
application into the HyperRAM:

```
typedef struct
{
  uint32_t offset;  //The offset of binary code image relative to the start of
                    //the header information
  uint32_t size;    //The size of the binary code image in bytes
  uint32_t crc;     //The calculated CRC value for the binary code image
                    //(in accordance with crc.c/h)
  char version[11]; //A "C" string version of the binary code image
                    //(i.e. "1.00.0000")
}file_header_t;
```

This is the typical memory map within the EPCQ device.

0x000000: <FPGA configuration image>
0x100000: <file_header_t><binary code image>

### Intel Cyclone 10 LP Evaluation Board Build Environment

The following are the steps I performed to create the **rtems_vm.jic** file used
to program the Cyclone 10 LP Evaluation Board. The process is broken down to
three phases. The first phase is to generate BSP files from an initial
compilation (SOF file) of the system to get the **system.h** and any C device
driver code that could to be used in the RTEMS NIOSV BSP and boot loader. The
second phase is to compile a final SOF file that contains a FPGA configuration
with the On-Chip ROM loaded with the boot loader hex file. The third phase is to
combine the FPGA configuration SOF file and an application HEX file into a JIC
file that can be programmed on the FPGA EPCQ device.

#### Phase 1
Here is an outline of the first phase to generate BSP source files from a SOF
file (see the Cyclone 10 LP Evaluation Board project that comes with the board
for more details). The main outputs from this process that are useful for a
RTEMS BSP, is the **system.h** file and any C device driver source files that
can be used as a basis for RTEMS device drivers.

 * Create a new project in the Quartus Prime software (I used the 23.1std.0
 version)
 * Set the project device to the Cyclone 10 LP 10CL025YU256I7G (check you board
 to confirm)
 * Set the device and pins options (use the project that comes with the eval
 board for the correct settings).
 * Use Platform Designer within Quartus Prime to create a system that implemets
 the IP listed in the sections above.
   * I have placed pictures of my Platform Designer system in the BSP
   (see **Platform Designer X.png** files)
   * You will still need to setup each IP correctly (use the eval board
   reference design as guidance).
   * For the first compilation, the ROM memory will not be initialized as you
   will need to create a boot loader in the next phase.
 * After completing the design, generate the HDL.
 * Add a top level Verilog file to connect your generated system to the I/O pins
 (see **cl10lp_rtems.v** file in BSP for an example).
 * Add a SDC file to constrain the system (see **cl10lp_rtems.sdc** file in BSP
 for an example)
 * Compile the design.
 * In the Pin Planner, assign the location for each I/O pin (use the eval board
 reference design for the correct location and pin settings).
 * In the Assignment editor, add an "Input Delay From Pin" assignment
 (see **Assignment Editor.png** file in BSP).
 * Re-compile the design.
 * Use the **niosv-bsp-editor.exe** tool that comes with Quartus Prime to
 generate a software BSP using the compiled SOF file.
 * From the BSP generation process, the **system.h** and any C driver files can
 be used to make a RTEMS BSP and boot loader.


This is the RTEMS build environment structure I used to create the RTEMS NIOSV
BSP, boot loader, and application.

```
+--  home directory               // the home directory of the user
     +-- sandbox2                 // a sandbox directory for development
         +---+-- rtems            // the rtems base directory for all things
         |    |                    // RTEMS (source code and install directory)
         |    |-- 6                // the rtems install directory
         |    +-- src              // the rtems directory for gitlab repos
         |        |-- rsb          // the rtems source builder repo directory
         |        |-- rtems        // the rtems OS repo directory
         |        +-- rtems-tools  // the rtems tools repo directory
         +-- bin                  // the directory where the boot and app exe
         |                       // outputs will be placed
         +-- gdb                  // the directory where the boot and app
         |                        // debug ELF outputs will be placed
         +-- head_file            // an application that will place the
         |                        // <file_header_t> information in the binary
         |                        // output
         +-- boot                 // the NIOSV bootloader directory for On-Chip
         |                        // Memory (ROM)
         +-- app                  // the user application directory (RTEMS
                                  // console app)
```

In your home directory, add a .gdbinit file to add the "sandbox/gdb" directory
as a safe path for loading files with GDB.
 * add-auto-load-safe-path ~/sandbox/gdb

Please see the **boot.zip** file to see an example of a boot loader that can be
used in the On-Chip ROM. This boot loader uses the RTEMS BSP header files that
get installed after building RTEMS.

Please see the **app.zip** file that contains an application that is built using
the RTEMS WAF system. This application is just a console app that can control
some device driver features.

Please see the **head_file.zip** file that contains an application to add the
<file_header_t> information to the binary image.

This is the RTEMS BSP build environment structure I used.

```
+--  rtems src directory            // the rtems top level directory
     +-- bsps                       // the rtems bsps directory
         +-- riscv                  // the rtems riscv bsp top level directory
             +-- niosv              // the rtems niosv bsp top level directory
                 +-- cache          // the niosv cache implementation
                 +-- clock          // the niosv clock device implementation
                 +-- console        // the niosv console implementation using
                 |                  // the JTAG UART
                 +-- flash          // the niosv EPCQ device implementation
                 +-- include        // the niosv base bsp include files
                 +-- irq            // the niosv IRQ implementation
                 +-- niosvc10lp     // the Intel Cyclone 10 LP evaluation board
                 |                  // bsp example
                 +-- start          // the niosv bsp start up implementation
                 +-- README.md      // this file
                 +-- supporting.zip // a zip file containing all the supporting
                                    // code and files referenced in
                                    // this README.md
      +-- spec                      // the rtems spec directory
         +-- build                  // the rtems spec build directory
             +-- bsps               // the rtems spec bsps directory
                 +-- riscv          // the rtems spec riscv directory
                     +-- niosv      // the rtems spec niosv directory containing
                                    // the configuration for all niosv bsps. Add
                                    // new BSP configurations here.
```

#### Phase 2
Here is an outline of the second phase to generate a boot loader HEX file that
can be loaded into the On-Chip ROM memory.

  * Create the rtems src directory under ${HOME}/sandbox2
  (i.e. ${HOME}/sandbox2/rtems/src).
  * Follow the steps in the RTEMS documentation to download the RTEMS Resource
  Builder (RSB).
  * Use RSB to compile the riscv-rtems6 binaries.
  * Follow the steps in the RTEMS documentation to download the RTEMS RTOS.
  * Develop a BSP for the NIOSV by adding source files to the
  **bsps/riscv/niosv** and **spec/build/bsps/riscv/niosv** directories.
  * Add a **config.ini** to configure the NIOSV BSP.
  * Build the RTEMS NIOSV BSP.
  * Create the boot loader directory under ${HOME}/sandbox2
  (i.e. ${HOME}/sandbox2/boot).
  * Develop a boot loader that will fit in the On-Chip ROM memory that will load
  an application from the EPCQ device.
  * Build the boot loader HEX file output
  * Create the head_file directory under ${HOME}/sandbox2
  (i.e. ${HOME}/sandbox2/head_file).
  * Develop a Linux application that will put the **file_header_t** information
  in front of the binary code image.
  * Build the head_file Linux application.
  * Create the application directory under ${HOME}/sandbox2
  (i.e. ${HOME}/sandbox2/app).
  * Develop an application using the RTEMS WAF system that will fit in the EPCQ
  device (EPCQ size - maximum FPGA configuration size)
  and include the **file_header_t** information.
  * Build the application HEX file output.

Once the boot loader and application compiles succesfully, follow these steps to
complete phase 2.

 * Use the **elf2hex.exe** tool that comes with Quartus Prime to convert the
 boot loader output to a HEX file that can be loaded into the
 On-Chip ROM
```
elf2hex.exe --width=32 --base=0x10010000 --end=0x10010fff
 --input=<full path to boot loader hex file>
 --output=<full path to Quartus Prime project hex file>
```
 * In Platform designer, intialize the On-Chip ROM memory with the generated HEX
 file from **elf2hex.exe**
 * Save the Platform Designer project and re-generated the HDL.
 * In Quartus Prime, re-compile the project.
 * You now have an SOF file which contains the boot loader.

#### Phase 3

Follow these steps to generate a JIC file that can be programmed on the FPGA.

 * In Quartus Prime, select **File>Convert Programming Files** tool.
 * Change the Programming file type to .jic
 * Press the ellipse button next to the Configuration device and select the
 Cyclone 10 LP device family and the EPCQ128A device (verify on your own
 hardware).
 * Press OK.
 * Change the file name of the JIC file to **rtems_vm.jic**.
 * Select the **Flash Loader** input file and press the **Add Device** button.
 * Select **Cyclone 10 LP** and then **10CL025Y** (verify on your own hardware).
 * Press OK.
 * Select the **SOF Data** input file and press the **Add File** button.
 * Navigate to the generated SOF file from phase 2 and select it.
 * Press Open
 * Now, press the **Add Hex Data** button.
 * In the dialog, select **Relative addressing** and enter 0x100000 as the start
 address.
 * Press the ellipse button next to the Hex file box.
 * Navigate to the HEX file that was generated by the application build and
 select it.
 * Press OK.
 * Press the **Generate** button to produce the **rtems_vm.jic** file.

### Programming the JIC file on the Cyclone 10 LP Evaluation Board

Make sure the Cyclone 10 LP Evaluation Board is powered up through the USB cable
and connected to the computer running Quartus Prime.  Also, make sure SW.4 is
**On**.  If the virtual JTAG chain is enabled, the programmer will be unable to
program the EPCQ device.

 * In Quartus Prime, select **Tools>Programmer**.
 * Select the currently loaded SOF file and press **Delete**.
 * Press **Add File**.
 * Navigate to the **rtems_vm.jic**, select it, and press **Open**.
 * select the Program/Configure checkbox on the JIC file.
 * Press **Start** to program the file onto the device.

Enjoy!
