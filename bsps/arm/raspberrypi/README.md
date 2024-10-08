Raspberry PI
============

BSP for the Raspberry Pi ARM board
This is a basic port that should work on either Model A or Model B.

It currently supports the following devices:
    o Console using the PL011 UART0
       The console driver only works with polled mode right now,
       the interrupt code is there, but it does not work yet.
       The console driver is currently hardcoded at 115k 8N1
    o Clock uses the internal ARM timer
       The Raspberry Pi can be overclocked through the config.txt file, this
       would affect the duration of the clock tick.
    o Benchmark timer reads the lower 32 bit GPU timer register

To run an RTEMS binary, it must be stripped and loaded on the SD card along with
the following files:

  bootcode.bin
  config.txt
  loader.bin
  start.elf
  kernel.img ( the RTEMS binary, you can change the name in config.txt )

These files can be obtained from a Linux installation image, or from here:
https://github.com/raspberrypi/firmware

I used an old 256MB SD card to boot RTEMS.
Much more information about the SD card file and bootloader can be found here:
http://elinux.org/RPi_Hub
http://www.raspberrypi.org

The linker script is set up for 128MB, so it can be used with a GPU/ARM split
of 128/128.
The bootloader that is used on the SD card determines the split of RAM between the
ARM and the GPU. It might make sense to adjust the GPU/ARM memory split to give
more memory to RTEMS, especially on a 512MB board.

To do:
    It would be nice to get support in the BSP for the following:
    o SD card
    o USB and USB 10/100 network chip on Model B
    o SPI
    o GPIO
    o ARM MMU
    o Graphics console
    o Sound

Credits and links:

  There is a wealth of code and information to reference on the raspberrypi.org bare metal forums:
  http://www.raspberrypi.org/phpBB3/viewforum.php?f=72

  I found information about how to program the timers, interrupts, and UART 0
  from the examples provided by:

  David Welch:
  https://github.com/dwelch67/raspberrypi
  The readme file at his github repository has valuable information about connecting a UART cable, JTAG etc.

  Steve Bate:
  http://www.stevebate.net/chibios-rpi/GettingStarted.html
  Steve provided a port of the Chibios RTOS to the raspberry Pi

  James Walmsley:
  http://www.raspberrypi.org/phpBB3/viewtopic.php?f=72&t=22423
  James ported FreeRTOS to the raspberry Pi.

