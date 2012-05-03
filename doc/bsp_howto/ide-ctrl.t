@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter IDE Controller Driver

@section Introduction

The IDE Controller driver is responsible for providing an
interface to an IDE Controller.  The capabilities provided by this
driver are:

@itemize @bullet
@item Read IDE Controller register  
@item Write IDE Controller register 
@item Read data block through IDE Controller Data Register
@item Write data block through IDE Controller Data Register
@end itemize

The reference implementation for an IDE Controller driver can 
be found in @code{$RTEMS_SRC_ROOT/c/src/libchip/ide}. This driver
is based on the libchip concept and allows to work with any of the IDE 
Controller chips simply by appropriate configuration of BSP. Drivers for a
particular IDE Controller chips locate in the following directories: drivers
for well-known IDE Controller chips locate into
@code{$RTEMS_SRC_ROOT/c/src/libchip/ide}, drivers for IDE Controller chips
integrated with CPU locate into
@code{$RTEMS_SRC_ROOT/c/src/lib/libcpu/myCPU} and 
drivers for custom IDE Controller chips (for example, implemented on FPGA)
locate into @code{$RTEMS_SRC_ROOT/c/src/lib/libbsp/myBSP}. 
There is a README file in these directories for each supported
IDE Controller chip. Each of these README explains how to configure a BSP
for that particular IDE Controller chip.

@section Initialization

IDE Controller chips used by a BSP are statically configured into
@code{IDE_Controller_Table}. The @code{ide_controller_initialize} routine is 
responsible for initialization of all configured IDE controller chips.
Initialization order of the chips based on the order the chips are defined in 
the @code{IDE_Controller_Table}.  

The following actions are performed by the IDE Controller driver
initialization routine:

@example
@group
rtems_device_driver ide_controller_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
@{
   for each IDE Controller chip configured in IDE_Controller_Table
     if (BSP dependent probe(if exists) AND device probe for this IDE chip 
        indicates it is present)
       perform initialization of the particular chip
       register device with configured name for this chip
@}
@end group
@end example

@section Read IDE Controller Register

The @code{ide_controller_read_register} routine reads the content of the IDE
Controller chip register. IDE Controller chip is selected via the minor
number. This routine is not allowed to be called from an application.

@example
@group
void ide_controller_read_register(rtems_device_minor_number minor,
                                  unsigned32 reg, unsigned32 *value)
@{
  get IDE Controller chip configuration information from
  IDE_Controller_Table by minor number

  invoke read register routine for the chip
@}
@end group
@end example

@section Write IDE Controller Register

The @code{ide_controller_write_register} routine writes IDE Controller chip 
register with specified value. IDE Controller chip is selected via the minor
number. This routine is not allowed to be called from an application.

@example
@group
void ide_controller_write_register(rtems_device_minor_number minor,
                                   unsigned32 reg, unsigned32 value)
@{
  get IDE Controller chip configuration information from
  IDE_Controller_Table by minor number

  invoke write register routine for the chip
@}
@end group
@end example

@section Read Data Block Through IDE Controller Data Register

The @code{ide_controller_read_data_block} provides multiple consequent read
of the IDE Controller Data Register. IDE Controller chip is selected via the 
minor number. The same functionality may be achieved via separate multiple
calls of @code{ide_controller_read_register} routine but 
@code{ide_controller_read_data_block} allows to escape functions call
overhead. This routine is not allowed to be called from an application.

@example
@group
void ide_controller_read_data_block(
  rtems_device_minor_number  minor,
  unsigned16                 block_size,
  blkdev_sg_buffer          *bufs,
  uint32_t                  *cbuf,
  uint32_t                  *pos
)
@{
  get IDE Controller chip configuration information from
  IDE_Controller_Table by minor number

  invoke read data block routine for the chip  
@}
@end group
@end example

@section Write Data Block Through IDE Controller Data Register

The @code{ide_controller_write_data_block} provides multiple consequent write
into the IDE Controller Data Register. IDE Controller chip is selected via the 
minor number. The same functionality may be achieved via separate multiple
calls of @code{ide_controller_write_register} routine but 
@code{ide_controller_write_data_block} allows to escape functions call
overhead. This routine is not allowed to be called from an application.

@example
@group
void ide_controller_write_data_block(
  rtems_device_minor_number  minor,
  unsigned16                 block_size,
  blkdev_sg_buffer          *bufs,
  uint32_t                  *cbuf,
  uint32_t                  *pos
)
@{
  get IDE Controller chip configuration information from
  IDE_Controller_Table by minor number

  invoke write data block routine for the chip  
@}
@end group
@end example
