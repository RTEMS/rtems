Copyright and License
---------------------

For Copyright and License of the source code, see the header in
libi2c.c. 

Overview
--------

This directory contains a general I2C/SPI API library. It offers a
standard API to I2C or SPI based device drivers, abstracting the low
level driver (dealing with the I2C/SPI controller hardware of the
board) from the high-level drivers (dealing with devices connected to
the I2C or SPI bus).

In most cases throughout this document, i2c and spi devices are
handled in a similar way. Therefore spi will not be explicitly named
in every location.


Features
---------

  + supports multiple i2c and/or spi busses 
  + supports multiple devices connected to each i2c/spi bus
  + handles bus and device registration to the I/O manager

Structure
---------

This library defines a layered API to i2c and spi devices. The
layering is:

```
  +----------------------------------------+
 6|            Application                 |
  +----------------------------------------+
 5|         RTEMS I/O Manager              |
  +----------------------------------------+
 4|**      libi2c OS adaption layer      **|
  +----------------------------------------+
 3|     high level i2c device driver       |
  |          (EEPROM, RTC, ...)            |
  |     (e.g. in c/src/libchip/i2c)        |
  +----------------------------------------+
 2|** libi2c low level abstraction layer **|
  +----------------------------------------+
 1|      i2c controller driver             |
  |              (in BSP)                  |
  +----------------------------------------+
```

This document will describe the following interfaces in separate
sections:

  + the interface between the RTEMS I/O Manager and the libi2c OS
  interface (5<->4)

  + the interface between the libi2c OS interface and the high level
  i2c device driver (4<->3)

  + the interface between the high level i2c device driver and the
  libi2c low level abstraction layer (3<->2)

  + the interface between the libi2c low level abstraction layer and
  the i2c controller driver (2<->1)


Differences between i2c and spi bus
-----------------------------------
SPI and I2C has many similarities, but also some differences:

- I2C uses inband addressing (the first bits sent select, which slave
device is addressed) while SPI uses dedicated select lines to address
a slave device

- SPI supports combined full duplex read-write transactions while I2C
either sends or receives data from a slave device

- SPI supports a varity of per-slave options, which include:
  - number of bits per character to transfer
  - polarity and phase of clock wrt data
  - clock frequency

The libi2c API defines a superset of functions to handle both flavours
of serial data transmission, but care should be taken not to use
features dedicated to the wrong type of serial bus.


Library Initialization
----------------------
Before any libi2c API is used, the library must be initialized. This
is achived with a call to function

    rtems_libi2c_initialize (). 

It creates a global mutex to lock internal data structures and
registers the OS adaption layer to the RTEMS I/O manager.

Any subsequent call to this function will be silently ignored.

Typically the BSP startup code will perform this initialization.

A proper place for initializing the i2c layer and populating it
with busses and device drivers (see 'Bus Registration' and
'Device/Driver Registration' below) is the 'predriver_hook'
where most facilities (such as malloc, libio) are already
available. Note, however, that 'stdio' is not yet functional
at this point and all i2c bus and device drivers should carefully
avoid using stdio so that other drivers which may build on top
of i2c devices may be initialized properly (this may happen
just after 'predriver_hook' when stdio is still not available).
E.g., drivers listed in the configuration table are initialized
during this step.

Note that while 'libi2c' could be initialized from the rtems
configuration table like other drivers there is no easy
way of populating the i2c framework with bus- and device-
drivers at this point (unless a special 'i2c' configuration
table describing the bus layout is implemented in the future).

For the time being, we must rely on the BSP (predriver_hook)
to initialize the i2c system if it is used by other drivers
(e.g., the RTC driver may have to use a i2c device).


Bus Registration
----------------
Each i2c and/or spi bus available must be registerd with a call to

int rtems_libi2c_register_bus (char *name, 
			       rtems_libi2c_bus_t * bus)

It registers the bus to the libi2c internal data structures and
creates a device node in the RTEMS filesystem with the given name. If
no name is given (name==NULL), then the default name "/dev/i2c" is
used instead.

With the second calling parameter "rtems_libi2c_bus_t * bus" the
caller passes in a set of function pointers, which define the entries
into the i2c controller driver (defined in the BSP).

This call returns an integer bus number, which can be used in
subsequent calls to register devices attached to this bus (see below).

Typically the BSP startup code will perform this registration for each
bus available on the board.


Device/Driver Registration
--------------------------
Each device attached to an i2c or spi bus must be registered with a
call to

int
rtems_libi2c_register_drv (char *name, rtems_libi2c_drv_t * drvtbl,
                           unsigned bus, unsigned i2caddr);

With this call, libi2c is informed, that:

- a device is attached to the given "bus" number (which in fact is the
return value received from a previous rtems_libi2c_register_bus()
call) with the address "i2caddr"

- the device is managed by a driver, who's entry functions are listed
  in "drvtbl"

- the device should be registered with the given "name" in the device
  tree of the filesystem.

The call will create a proper minor device number, which has the bus
number and i2c_address encoded. This minor number is the return value
of the call and is also associated with the filesystem node created
for this device.

Note: If you have multiple devices of the same type, you must register
each of them through a separate call (with the same "drvtbl", but
different name/bus/i2caddr).


(5<->4) RTEMS I/O Manager and the libi2c OS adaption layer IF
-------------------------------------------------------------

The RTEMS I/O Manager regards the libi2c OS adaption layer as a normal
RTEMS Device Driver with one unique major number and a set of minor
numbers, one for each bus and one for each device attached to one of
the busses.

Therefore the libi2c OS adaption layer provides the standard calls:

```c
static rtems_driver_address_table libi2c_io_ops = {
  initialization_entry:  i2c_init,
  open_entry:            i2c_open,
  close_entry:           i2c_close,
  read_entry:            i2c_read,
  write_entry:           i2c_write,
  control_entry:         i2c_ioctl
};
```

These calls perform some parameter checking and then call the
appropriate high level i2c device driver function, if available,
according to the entries in the "drvtbl" passed in the
rtems_libi2c_register_drv() call.

There are two exceptions: when i2c_read or i2c_write is called with a
minor number specifying a bus (and not a device attached to the bus),
then the respective transfer is performed as a raw byte stream
transfer to the bus.

The main reason for the libi2c OS adaption layer is, that it
dispatches the RTEMS I/O Manager calls to the proper device driver
according to the minor number used.

libi2c OS adaption layer and the high level i2c device driver IF
----------------------------------------------------------------

Each high level i2c device driver provides a set of functions in the
rtems_libi2c_drv_t data structure passed the libi2c when the device is
registered (see "Device registration" above). These function directly match
the RTEMS I/O Mangers calls "open", "close", "read", "write",
"control", and they are passed the same arguments. Functions not
needed may be omited (and replaced by a NULL pointer in
rtems_libi2c_drv_t).

high level i2c device driver and libi2c low level abstraction layer IF
----------------------------------------------------------------------
libi2c provides a set of functions for the high level drivers. These
functions are:

```c
rtems_libi2c_send_start();
rtems_libi2c_send_stop();
rtems_libi2c_send_addr();
rtems_libi2c_read_bytes();
rtems_libi2c_write_bytes();
rtems_libi2c_start_read_bytes();
rtems_libi2c_start_write_bytes();
rtems_libi2c_ioctl();
```

Please look into libi2c.h for the proper parameters and return codes.

These functions perform the proper i2c operations when called. 

A typical access sequence for the I2C bus would be:

```c
rtems_libi2c_send_start();
rtems_libi2c_send_addr();
rtems_libi2c_write_bytes();
rtems_libi2c_send_stop();
```

Alternatively, the rtems_libi2c_write_bytes() call could be relpaced
with a 

```c
          rtems_libi2c_read_bytes() 
```
call or a sequence of multiple calls.

Note: rtems_libi2c_send_start() locks the i2c/spi bus used, so no other
device can use this i2c/spi bus, until rtems_libi2c_send_stop() function
is called for the same device.


Special provisions for SPI devices:
-----------------------------------
For SPI devices and their drivers, the libi2c interface is used
slightly differently:

rtems_libi2c_send_start() will lock access to the SPI bus, but has no
effect on the hardware bus interface.

rtems_libi2c_ioctl(...,RTEMS_LIBI2C_IOCTL_SET_TFRMODE,...) will set
the transfer mode (bit rate, clock phase and polaritiy, bits per
char...) according to the rtems_libi2c_tfr_mode_t structure passed in.

rtems_libi2c_send_addr() will activate the proper select line to
address a certain SPI device. The correspondance between an address
and the select line pulled is BSP specific.

rtems_libi2c_send_stop(); will deactivate the address line and unlock
the bus.

A typical access sequence for the SPI bus would be:

```c
rtems_libi2c_send_start();
rtems_libi2c_ioctl(...,RTEMS_LIBI2C_IOCTL_SET_TFRMODE,...);
rtems_libi2c_send_addr();
rtems_libi2c_write_bytes();
rtems_libi2c_send_stop();
```

Alternatively, the rtems_libi2c_write_bytes() call could be relpaced
with a 
```c
         rtems_libi2c_read_bytes() 
```
or a 
```c
         rtems_libi2c_ioctl(...,RTEMS_LIBI2C_IOCTL_READ_WRITE,...)
```
call or a sequence of multiple calls.


libi2c low level abstraction layer and i2c controller driver IF
---------------------------------------------------------------
Each low level i2c/spi driver must provide a set of bus_ops functions
as defined in the rtems_libi2c_bus_ops_t structure.

```c
typedef struct rtems_libi2c_bus_ops_
{
  /* Initialize the bus; might be called again to reset the bus driver */
  rtems_status_code (*init) (rtems_libi2c_bus_t * bushdl);
  /* Send start condition */
  rtems_status_code (*send_start) (rtems_libi2c_bus_t * bushdl);
  /* Send stop  condition */
  rtems_status_code (*send_stop) (rtems_libi2c_bus_t * bushdl);
  /* initiate transfer from (rw!=0) or to a device */
  rtems_status_code (*send_addr) (rtems_libi2c_bus_t * bushdl,
                                  uint32_t addr, int rw);
  /* read a number of bytes */
  int (*read_bytes) (rtems_libi2c_bus_t * bushdl, unsigned char *bytes,
                     int nbytes);
  /* write a number of bytes */
  int (*write_bytes) (rtems_libi2c_bus_t * bushdl, unsigned char *bytes,
                      int nbytes);
  /* ioctl misc functions */
  int (*ioctl) (rtems_libi2c_bus_t * bushdl, 
		int   cmd,
		void *buffer;
		);
} rtems_libi2c_bus_ops_t;
```

Each of these functions performs the corresponding function to the i2c
bus. 


Special provisions for SPI devices:
-----------------------------------
For SPI busses, special behaviour is required:

```c
(*send_start) (rtems_libi2c_bus_t * bushdl) 
	      normally is an empty function.

 (*send_addr) (rtems_libi2c_bus_t * bushdl, uint32_t addr, int rw)
              will activate the SPI select line matching to addr.

(*send_stop) (rtems_libi2c_bus_t * bushdl) 
              will deactivate the SPI select line 

(*ioctl(...,RTEMS_LIBI2C_IOCTL_SET_TFRMODE,...) 
             will set the transfer mode (bit rate, clock phase and
             polaritiy, bits per char...) according to the
             rtems_libi2c_tfr_mode_t structure passed in. 

(*ioctl(...,RTEMS_LIBI2C_IOCTL_READ_WRITE,...) 
             will send and receive data at the same time.
```

Note: 
- low-level I2C drivers normally are specific to the master
device, but independent from the board hardware. So in many cases they
can totally reside in libcpu or libchip.

- low-level SPI drivers are mostly board independent, but the
  addressing is board/BSP dependent. Therefore the (*send_start),
  (*send_addr) and (*send_stop) functions are typically defined in the
  BSP. The rest of the functions can reside in libcpu or libchip.
