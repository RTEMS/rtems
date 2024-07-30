Beagleboard
===========

BSP for beagleboard xm, beaglebone (original aka white), and beaglebone black.

original beagleboard isn't tested.


CONFIGURING
-----------
bsp-specific build options in the environment at build time:

CONSOLE_POLLED=1 use polled i/o for console, required to run testsuite
CONSOLE_BAUD=... override default console baud rate

BSPs recognized are:
beagleboardorig  original beagleboard
beagleboardxm    beagleboard xm
beaglebonewhite  original beaglebone
beagleboneblack  beaglebone black

Currently the only distinction in the BSP are between the beagleboards and
the beaglebones, but the 4 names are specified in case hardware-specific
distinctions are made in the future, so this can be done without changing the
usage.


BUILDING
--------
To build BSPs for the beaglebone white and beagleboard xm, starting from
a directory in which you have this source tree in rtems-src:

$ mkdir b-beagle
$ cd b-beagle
$ ../rtems-src/configure --target=arm-rtems4.11 --enable-rtemsbsp="beaglebonewhite beagleboardxm"
$ make all

This should give you .exes somewhere.

Then you need 'mkimage' to transform a .exe file to a u-boot image
file. first make a flat binary:

$ arm-rtems4.11-objcopy $exe -O binary $exe.bin
$ gzip -9 $exe.bin
$ mkimage -A arm -O rtems -T kernel -a 0x80000000 -e 0x80000000 -n RTEMS -d $exe.bin.gz rtems-app.img

All beagles have memory starting at 0x80000000 so the load & run syntax is the same.

BOOTING
-------
Then, boot the beaglebone with u-boot on an SD card and load rtems-app.img
from u-boot. Interrupt the u-boot boot to get a prompt.

Set up a tftp server and a network connection for netbooting. And to
copy rtems-app.img to the tftp dir. Otherwise copy the .img to the FAT
partition on the SD card and make uboot load & run that.


BEAGLEBONES
-----------
(tested on both beaglebones)

Beaglebone original (white) or beaglebone black netbooting:

uboot# setenv ipaddr 192.168.12.20
uboot# setenv serverip 192.168.12.10
uboot# echo starting from TFTP
uboot# tftp 0x80800000 rtems-app.img
uboot# dcache off ; icache off
uboot# bootm 0x80800000

Beaglebone original (white) or beaglebone black from a FAT partition:

uboot# fatload mmc :1 0x80800000 ticker.img
uboot# dcache off ; icache off
uboot# bootm 0x80800000


BEAGLEBOARD
-----------
(tested on xm)

For the beagleboard the necessary commands are a bit different because
of the ethernet over usb:

uboot# setenv serverip 192.168.12.10
uboot# setenv ipaddr 192.168.12.62
uboot# setenv usbnet_devaddr e8:03:9a:24:f9:10
uboot# setenv usbethaddr e8:03:9a:24:f9:11
uboot# usb start
uboot# echo starting from TFTP
uboot# tftp 0x80800000 rtems-app.img
uboot# dcache off ; icache off
uboot# bootm 0x80800000


SD CARD
-------
There is a script here that automatically writes an SD card for any of
the beagle targets.

Let's write one for the Beaglebone Black. Assuming your source tree is
at $HOME/development/rtems/rtems-src and your bsp is built and linked
with examples and installed at $HOME/development/rtems/4.11.

```shell
    % cd $HOME/development/rtems/rtems-src/c/src/lib/libbsp/arm/beagle/simscripts
    % sh sdcard.sh $HOME/development/rtems/4.11 $HOME/development/rtems/b-beagle/arm-rtems4.11/c/beagleboneblack/testsuites/samples/hello/hello.exe
```

The script should give you a whole bunch of output, ending in:

    Result is in bone_hello.exe-sdcard.img.

There you go. dd that to an SD card and boot!

The script needs to know whether it's for a Beagleboard xM or one of the
Beaglebones. This is to know which uboot to use. It will detect this
from the path the executable is in (in the above example, it contains
'beagleboneblack'), so you have to specify the full path.


Good luck & enjoy!

Ben Gras
beng@shrike-systems.com


JTAG
----
To run RTEMS from scratch (without any other bootcode) on the beagles,
you can comfortably load the executables over JTAG using gdb. This is
necessarily target-specific however.

1. BBXM

  - For access to JTAG using openocd, see simscripts/bbxm.cfg.
  - openocd then offers access to gdb using simscripts/gdbinit.bbxm.
  - start openocd using bbxm.cfg
  - copy your .exe to a new dir and that gdbinit file as .gdbinit in the same dir
  - go there and start gdb:
    $ arm-rtems4.11-gdb hello.exe
  - gdb will invoke the BBXM hardware initialization in the bbxm.cfg
    and load the ELF over JTAG. type 'c' (for continue) to run it.
  - breakpoints, C statement and single-instruction stepping work.

2. beaglebone white

This has been tested with openocd and works but not in as much detail as for
the BBXM yet (i.e. loading an executable from scratch).


Testing
-------
To build and run the tests for this BSP, use the RTEMS tester.
The necessary software can be built with the RTEMS source builder.

To build the BSP for testing:
	- set CONSOLE_POLLED=1 in the configure environment, some tests
	  assume console i/o is polled
	- add --enable-tests to the configure line

1. Qemu

Linaro Qemu can emulate the beagleboard xm and so run all regression
tests in software. Build the bbxm.bset from the RTEMS source builder and
you will get qemu linaro that can run them. There is a beagleboardxm_qemu
bsp in the RTEMS tester to invoke it with every test.

2. bbxm hardware

This requires JTAG, see README.JTAG. Use the beagleboardxm bsp in the
RTEMS tester. It starts gdb to connect to openocd to reset the target
and load the RTEMS executable for each test iteration.
